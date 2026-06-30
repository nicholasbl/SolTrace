from __future__ import annotations

import tempfile
import unittest
import zipfile
from pathlib import Path

from scripts.release.embree import _extract_zip, select_asset
from scripts.release.common import ReleaseError
from scripts.release.licenses import copy_discovered, safe_license_name
from scripts.release.linux import glibc_versions
from scripts.release.macos import dependency_is_relocatable, parse_otool_dependencies
from scripts.release.package import package_windows


class EmbreeTests(unittest.TestCase):
    def test_select_asset_is_case_insensitive(self) -> None:
        assets = ["embree-4.4.1.x64.windows.zip", "embree-4.4.1.ARM64.macOS.zip"]
        self.assertEqual(
            select_asset(assets, r"arm64.*mac(os)?\.zip$"),
            "embree-4.4.1.ARM64.macOS.zip",
        )

    def test_select_asset_reports_failure(self) -> None:
        with self.assertRaises(ReleaseError):
            select_asset(["linux.tar.gz"], r"windows.*\.zip$")

    def test_zip_extraction_preserves_symlinks(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            archive = root / "embree.zip"
            link = zipfile.ZipInfo("lib/libembree.4.dylib")
            link.create_system = 3
            link.external_attr = 0o120777 << 16
            with zipfile.ZipFile(archive, "w") as bundle:
                bundle.writestr("lib/libembree.4.4.1.dylib", b"binary")
                bundle.writestr(link, "libembree.4.4.1.dylib")
            destination = root / "output"
            destination.mkdir()
            _extract_zip(archive, destination)
            self.assertTrue((destination / "lib" / "libembree.4.dylib").is_symlink())
            self.assertEqual(
                (destination / "lib" / "libembree.4.dylib").readlink(),
                Path("libembree.4.4.1.dylib"),
            )


class LicenseTests(unittest.TestCase):
    def test_safe_name_preserves_origin_without_directories(self) -> None:
        root = Path("deps")
        path = root / "library" / "LICENSE.txt"
        self.assertEqual(
            safe_license_name(root, path, "Dependencies"),
            "Dependencies__library__LICENSE.txt",
        )

    def test_discovery_copies_only_license_files(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary) / "source"
            destination = Path(temporary) / "destination"
            (root / "dependency").mkdir(parents=True)
            destination.mkdir()
            (root / "dependency" / "LICENSE").write_text("license", encoding="utf-8")
            (root / "dependency" / "code.cpp").write_text("code", encoding="utf-8")
            self.assertEqual(copy_discovered(root, destination, "Dep"), 1)
            self.assertEqual(
                [path.name for path in destination.iterdir()],
                ["Dep__dependency__LICENSE"],
            )


class MacOSTests(unittest.TestCase):
    def test_parse_dependencies(self) -> None:
        output = """Binary:
\t@rpath/libembree.4.dylib (compatibility version 4.0.0)
\t/usr/lib/libc++.1.dylib (compatibility version 1.0.0)
"""
        self.assertEqual(
            parse_otool_dependencies(output),
            ["@rpath/libembree.4.dylib", "/usr/lib/libc++.1.dylib"],
        )

    def test_dependency_policy(self) -> None:
        self.assertTrue(dependency_is_relocatable("@rpath/QtCore.framework/QtCore"))
        self.assertTrue(
            dependency_is_relocatable(
                "/System/Library/Frameworks/AppKit.framework/AppKit"
            )
        )
        self.assertFalse(dependency_is_relocatable("/Users/runner/embree/libembree.dylib"))


class LinuxTests(unittest.TestCase):
    def test_extract_glibc_versions(self) -> None:
        data = b"prefix GLIBC_2.29 middle GLIBC_2.34 suffix GLIBC_2.29"
        self.assertEqual(glibc_versions(data), {"GLIBC_2.29", "GLIBC_2.34"})


class PackageTests(unittest.TestCase):
    def test_windows_archive_uses_install_tree_as_root(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            install = root / "dist"
            (install / "bin").mkdir(parents=True)
            (install / "licenses").mkdir()
            (install / "bin" / "SolTrace.exe").write_bytes(b"executable")
            (install / "licenses" / "LICENSE.md").write_text(
                "license", encoding="utf-8"
            )
            asset = root / "SolTrace.zip"
            package_windows(install_dir=install, asset=asset, app_name="SolTrace")
            with zipfile.ZipFile(asset) as archive:
                self.assertEqual(
                    sorted(archive.namelist()),
                    ["bin/SolTrace.exe", "licenses/LICENSE.md"],
                )


if __name__ == "__main__":
    unittest.main()
