"""Portable unit tests for release selection, parsing, and archive behavior."""

from __future__ import annotations

import io
import json
import tempfile
import unittest
import zipfile
from pathlib import Path

from scripts.release.embree import _extract_zip, select_asset
from scripts.release.common import ReleaseError
from scripts.release.licenses import (
    copy_discovered,
    download_qt_licenses,
    find_qt_license_dir,
    find_qt_root,
    safe_license_name,
)
from scripts.release.linux import glibc_versions
from scripts.release.macos import dependency_is_relocatable, parse_otool_dependencies
from scripts.release.package import package_windows


class EmbreeTests(unittest.TestCase):
    """Verify release asset selection and archive extraction behavior."""

    def test_select_asset_is_case_insensitive(self) -> None:
        """Asset matching tolerates capitalization differences in upstream names."""
        assets = ["embree-4.4.1.x64.windows.zip", "embree-4.4.1.ARM64.macOS.zip"]
        self.assertEqual(
            select_asset(assets, r"arm64.*mac(os)?\.zip$"),
            "embree-4.4.1.ARM64.macOS.zip",
        )

    def test_select_asset_reports_failure(self) -> None:
        """A missing platform asset produces an explicit release failure."""
        with self.assertRaises(ReleaseError):
            select_asset(["linux.tar.gz"], r"windows.*\.zip$")

    def test_zip_extraction_preserves_symlinks(self) -> None:
        """Versioned macOS dylib symlink chains survive ZIP extraction."""
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
    """Verify license naming and recursive discovery."""

    def test_safe_name_preserves_origin_without_directories(self) -> None:
        """Flattened names retain enough source context to avoid collisions."""
        root = Path("deps")
        path = root / "library" / "LICENSE.txt"
        self.assertEqual(
            safe_license_name(root, path, "Dependencies"),
            "Dependencies__library__LICENSE.txt",
        )

    def test_discovery_copies_only_license_files(self) -> None:
        """Source files unrelated to licensing are not added to an artifact."""
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

    def test_qt_install_is_valid_without_local_license_directory(self) -> None:
        """An aqt installation can be located even when licenses were omitted."""
        with tempfile.TemporaryDirectory() as temporary:
            qt_root = Path(temporary) / "Qt" / "6.11.1" / "macos"
            qt_root.mkdir(parents=True)
            self.assertEqual(find_qt_root({"QT_ROOT_DIR": str(qt_root)}), qt_root)
            self.assertIsNone(find_qt_license_dir(qt_root))

    def test_download_qt_licenses_uses_pinned_tag_contents(self) -> None:
        """The fallback enumerates and downloads license files for the Qt version."""
        metadata = json.dumps(
            [
                {
                    "type": "file",
                    "name": "LGPL-3.0-only.txt",
                    "download_url": "https://example.test/LGPL-3.0-only.txt",
                }
            ]
        ).encode()

        class Response(io.BytesIO):
            """Minimal context-managed HTTP response used by the downloader."""

            def __enter__(self):
                """Return this in-memory response to the context manager."""
                return self

            def __exit__(self, *_args):
                """Close the in-memory response after the request is consumed."""
                self.close()

        requested: list[str] = []

        def opener(request):
            """Return deterministic API metadata and license content."""
            requested.append(request.full_url)
            return Response(metadata if "api.github.com" in request.full_url else b"LGPL text")

        with tempfile.TemporaryDirectory() as temporary:
            destination = Path(temporary)
            self.assertEqual(
                download_qt_licenses(destination, "6.11.1", opener=opener), 1
            )
            self.assertEqual(
                (destination / "LGPL-3.0-only.txt").read_text(), "LGPL text"
            )
        self.assertIn("ref=v6.11.1", requested[0])


class MacOSTests(unittest.TestCase):
    """Verify parsing and policy checks used by the macOS dependency audit."""

    def test_parse_dependencies(self) -> None:
        """otool metadata is removed while install names are retained."""
        output = """Binary:
\t@rpath/libembree.4.dylib (compatibility version 4.0.0)
\t/usr/lib/libc++.1.dylib (compatibility version 1.0.0)
"""
        self.assertEqual(
            parse_otool_dependencies(output),
            ["@rpath/libembree.4.dylib", "/usr/lib/libc++.1.dylib"],
        )

    def test_parse_dependencies_ignores_universal_architecture_headers(self) -> None:
        """Universal-binary section headers are not interpreted as dependencies."""
        binary = "/dist/SolTrace.app/Contents/Frameworks/QtConcurrent.framework/QtConcurrent"
        output = f"""{binary} (architecture x86_64):
\t@rpath/QtConcurrent.framework/Versions/A/QtConcurrent (compatibility version 6.0.0)
\t/usr/lib/libc++.1.dylib (compatibility version 1.0.0)
{binary} (architecture arm64):
\t@rpath/QtConcurrent.framework/Versions/A/QtConcurrent (compatibility version 6.0.0)
\t/usr/lib/libc++.1.dylib (compatibility version 1.0.0)
"""
        self.assertEqual(
            parse_otool_dependencies(output),
            [
                "@rpath/QtConcurrent.framework/Versions/A/QtConcurrent",
                "/usr/lib/libc++.1.dylib",
            ],
        )

    def test_dependency_policy(self) -> None:
        """Bundle-relative and system paths pass while build-machine paths fail."""
        self.assertTrue(dependency_is_relocatable("@rpath/QtCore.framework/QtCore"))
        self.assertTrue(
            dependency_is_relocatable(
                "/System/Library/Frameworks/AppKit.framework/AppKit"
            )
        )
        self.assertFalse(dependency_is_relocatable("/Users/runner/embree/libembree.dylib"))


class LinuxTests(unittest.TestCase):
    """Verify Linux runtime-baseline inspection helpers."""

    def test_extract_glibc_versions(self) -> None:
        """Duplicate GLIBC references collapse into a distinct version set."""
        data = b"prefix GLIBC_2.29 middle GLIBC_2.34 suffix GLIBC_2.29"
        self.assertEqual(glibc_versions(data), {"GLIBC_2.29", "GLIBC_2.34"})


class PackageTests(unittest.TestCase):
    """Verify final archive layout."""

    def test_windows_archive_uses_install_tree_as_root(self) -> None:
        """Windows ZIPs expose bin and license directories at archive root."""
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
