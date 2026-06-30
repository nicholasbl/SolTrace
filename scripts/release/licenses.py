"""Discover and bundle project and third-party license material."""

from __future__ import annotations

import os
import shutil
from pathlib import Path
from typing import Mapping

from .common import ReleaseError, enabled, require_path


LICENSE_PREFIXES = ("license", "copying", "notice", "eula", "third-party")


def is_license_file(path: Path) -> bool:
    """Return whether a file name uses one of the recognized license prefixes."""
    return path.is_file() and path.name.lower().startswith(LICENSE_PREFIXES)


def safe_license_name(root: Path, path: Path, prefix: str) -> str:
    """Flatten a source-relative license path into a collision-resistant file name."""
    relative = path.relative_to(root)
    flattened = "__".join(relative.parts)
    return f"{prefix}__{flattened}"


def copy_discovered(root: Path | None, destination: Path, prefix: str) -> int:
    """Copy recognized license files below ``root`` and return the copied count."""
    if root is None or not root.is_dir():
        return 0
    count = 0
    for source in sorted(root.rglob("*")):
        if not is_license_file(source):
            continue
        shutil.copy2(source, destination / safe_license_name(root, source, prefix))
        count += 1
    return count


def find_qt_root(environment: Mapping[str, str] = os.environ) -> Path:
    """Resolve the Qt installation root from variables set by install-qt-action."""
    configured = environment.get("QT_ROOT_DIR")
    if configured and (Path(configured) / "LICENSES").is_dir():
        return Path(configured)

    qt6_dir = environment.get("Qt6_DIR")
    if qt6_dir:
        path = Path(qt6_dir).resolve()
        if len(path.parents) >= 3:
            root = path.parents[2]
            if (root / "LICENSES").is_dir():
                return root
    raise ReleaseError("Could not locate Qt's LICENSES directory")


def destination_for(platform: str, install_dir: Path, app_name: str, appdir: Path | None) -> Path:
    """Choose the platform-native license directory inside a staged artifact."""
    if platform == "macos":
        return install_dir / f"{app_name}.app" / "Contents" / "Resources" / "licenses"
    if platform == "windows":
        return install_dir / "licenses"
    if platform == "linux" and appdir is not None:
        return appdir / "usr" / "share" / "licenses"
    raise ReleaseError(f"Unsupported license destination for platform {platform!r}")


def bundle(
    *,
    platform: str,
    workspace: Path,
    build_dir: Path,
    install_dir: Path,
    app_name: str,
    optix_enabled: str | bool,
    appdir: Path | None = None,
) -> Path:
    """Collect SolTrace, Qt, Embree, build dependency, CUDA, and OptiX licenses.

    Linux callers provide ``appdir`` because licenses are inserted before
    linuxdeploy creates the AppImage. macOS and Windows destinations are derived
    directly from the CMake installation tree.
    """
    destination = destination_for(platform, install_dir, app_name, appdir)
    soltrace_dir = destination / "SolTrace"
    soltrace_dir.mkdir(parents=True, exist_ok=True)
    shutil.copy2(require_path(workspace / "LICENSE.md", "SolTrace license"), soltrace_dir)

    qt_root = find_qt_root()
    shutil.copytree(qt_root / "LICENSES", destination / "Qt", dirs_exist_ok=True)

    sources: list[tuple[Path | None, str]] = [
        (_environment_path("EMBREE_INSTALL_DIR"), "Embree"),
        (build_dir / "_deps", "Dependencies"),
    ]
    if enabled(optix_enabled):
        sources.extend(
            [
                (workspace / "optix-dev", "OptiX"),
                (_environment_path("CUDA_PATH"), "CUDA"),
            ]
        )

    for root, prefix in sources:
        count = copy_discovered(root, soltrace_dir, prefix)
        print(f"Copied {count} {prefix} license file(s)")
    return destination


def _environment_path(name: str) -> Path | None:
    """Convert an optional environment variable into a path."""
    value = os.environ.get(name)
    return Path(value) if value else None
