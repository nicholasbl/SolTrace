"""Stage and statically validate Windows runtime dependencies."""

from __future__ import annotations

import os
import shutil
from pathlib import Path

from .common import ReleaseError, enabled, require_path, run


def _copy_matches(source: Path, pattern: str, destination: Path, description: str) -> None:
    """Copy all matching files, failing when a required runtime family is absent."""
    matches = sorted(path for path in source.glob(pattern) if path.is_file())
    if not matches:
        raise ReleaseError(f"No {description} matching {pattern!r} found in {source}")
    destination.mkdir(parents=True, exist_ok=True)
    for path in matches:
        shutil.copy2(path, destination)


def stage_runtime(*, install_dir: Path, optix_enabled: str | bool) -> None:
    """Copy Embree/TBB and, for OptiX builds, CUDA runtime DLLs beside the app."""
    runtime_value = os.environ.get("EMBREE_RUNTIME_DIR")
    if not runtime_value:
        raise ReleaseError("EMBREE_RUNTIME_DIR is not set")
    bin_dir = install_dir / "bin"
    _copy_matches(Path(runtime_value), "*.dll", bin_dir, "Embree runtime DLLs")

    if enabled(optix_enabled):
        cuda_value = os.environ.get("CUDA_PATH")
        if not cuda_value:
            raise ReleaseError("CUDA_PATH is not set for an OptiX build")
        _copy_matches(Path(cuda_value) / "bin", "cudart64_*.dll", bin_dir, "CUDA runtime DLLs")


def _require_match(directory: Path, pattern: str, description: str) -> None:
    """Require at least one staged file matching a dependency pattern."""
    if not any(path.is_file() for path in directory.glob(pattern)):
        raise ReleaseError(f"{description} is missing from {directory} (pattern {pattern})")


def _find_dumpbin() -> Path:
    """Locate the x64 ``dumpbin`` belonging to the latest installed MSVC toolset."""
    program_files = os.environ.get("ProgramFiles(x86)")
    if not program_files:
        raise ReleaseError("ProgramFiles(x86) is not set")
    vswhere = require_path(
        Path(program_files) / "Microsoft Visual Studio" / "Installer" / "vswhere.exe",
        "vswhere.exe",
    )
    result = run(
        [
            vswhere,
            "-latest",
            "-products",
            "*",
            "-requires",
            "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
            "-property",
            "installationPath",
        ],
        capture=True,
    )
    visual_studio = Path(result.stdout.strip())
    if not visual_studio:
        raise ReleaseError("Visual Studio with x64 C++ tools was not found")
    version_file = require_path(
        visual_studio / "VC" / "Auxiliary" / "Build" / "Microsoft.VCToolsVersion.default.txt",
        "MSVC tools version file",
    )
    tools_version = version_file.read_text(encoding="utf-8").strip()
    return require_path(
        visual_studio
        / "VC"
        / "Tools"
        / "MSVC"
        / tools_version
        / "bin"
        / "Hostx64"
        / "x64"
        / "dumpbin.exe",
        "dumpbin.exe",
    )


def validate(*, install_dir: Path, app_name: str, optix_enabled: str | bool) -> None:
    """Require key DLL families and print the executable's PE dependency table."""
    bin_dir = require_path(install_dir / "bin", "Windows binary directory")
    executable = require_path(bin_dir / f"{app_name}.exe", "SolTrace executable")
    _require_match(bin_dir, "Qt6Core.dll", "Qt6Core.dll")
    _require_match(bin_dir, "embree*.dll", "Embree runtime")
    _require_match(bin_dir, "tbb*.dll", "TBB runtime")
    if enabled(optix_enabled):
        _require_match(bin_dir, "cudart64_*.dll", "CUDA runtime")
    run([_find_dumpbin(), "/DEPENDENTS", executable])
