"""Stage and validate relocatable dependencies in the macOS application bundle."""

from __future__ import annotations

import os
import shutil
from pathlib import Path

from .common import ReleaseError, require_path, run


EXPECTED_RPATH = "@executable_path/../Frameworks"


def parse_otool_dependencies(output: str) -> list[str]:
    """Parse dependency install names from ``otool -L`` output."""
    dependencies: list[str] = []
    for line in output.splitlines()[1:]:
        stripped = line.strip()
        if stripped:
            dependencies.append(stripped.split(" (", 1)[0])
    return dependencies


def dependency_is_relocatable(dependency: str) -> bool:
    """Allow bundle-relative references and Apple-provided system libraries."""
    return dependency.startswith(
        ("@rpath/", "@loader_path/", "@executable_path/", "/System/Library/", "/usr/lib/")
    )


def _copy_entry(source: Path, destination: Path) -> None:
    """Copy a regular file or recreate a relative dylib symlink without dereferencing it."""
    target = destination / source.name
    if target.is_symlink() or target.exists():
        target.unlink()
    if source.is_symlink():
        target.symlink_to(os.readlink(source))
    else:
        shutil.copy2(source, target)


def stage_runtime(*, install_dir: Path, app_name: str) -> None:
    """Copy Embree/TBB dylib chains into the bundle and ensure its runtime RPATH."""
    runtime_value = os.environ.get("EMBREE_RUNTIME_DIR")
    if not runtime_value:
        raise ReleaseError("EMBREE_RUNTIME_DIR is not set")
    runtime_dir = require_path(Path(runtime_value), "Embree runtime directory")
    app = require_path(install_dir / f"{app_name}.app", "macOS application bundle")
    executable = require_path(app / "Contents" / "MacOS" / app_name, "application executable")
    frameworks = app / "Contents" / "Frameworks"
    frameworks.mkdir(parents=True, exist_ok=True)

    libraries = sorted(
        path
        for path in runtime_dir.iterdir()
        if path.name.startswith(("libembree", "libtbb")) and path.name.endswith(".dylib")
    )
    if not libraries:
        raise ReleaseError(f"No Embree/TBB dylibs were found in {runtime_dir}")
    for library in libraries:
        _copy_entry(library, frameworks)

    load_commands = run(["otool", "-l", executable], capture=True).stdout
    if EXPECTED_RPATH not in load_commands:
        run(["install_name_tool", "-add_rpath", EXPECTED_RPATH, executable])


def validate(*, install_dir: Path, app_name: str, architecture: str) -> None:
    """Audit architecture, RPATHs, install names, and required Embree/TBB files."""
    app = require_path(install_dir / f"{app_name}.app", "macOS application bundle")
    executable = require_path(app / "Contents" / "MacOS" / app_name, "application executable")
    frameworks = require_path(app / "Contents" / "Frameworks", "Frameworks directory")

    file_output = run(["file", executable], capture=True).stdout
    if architecture not in file_output:
        raise ReleaseError(f"Expected {architecture} executable, got: {file_output.strip()}")
    if EXPECTED_RPATH not in run(["otool", "-l", executable], capture=True).stdout:
        raise ReleaseError(f"Application does not contain required RPATH {EXPECTED_RPATH}")

    macho_count = 0
    for candidate in sorted(path for path in app.rglob("*") if path.is_file()):
        if "Mach-O" not in run(["file", candidate], capture=True).stdout:
            continue
        macho_count += 1
        output = run(["otool", "-L", candidate], capture=True).stdout
        print(f"Dependencies for {candidate}:\n{output}")
        for dependency in parse_otool_dependencies(output):
            if not dependency_is_relocatable(dependency):
                raise ReleaseError(
                    f"Non-relocatable dependency in {candidate}: {dependency}"
                )
            name = Path(dependency).name
            if name.startswith(("libembree", "libtbb")) and not (frameworks / name).exists():
                raise ReleaseError(f"Bundled dependency is missing: {name}")

    if macho_count == 0:
        raise ReleaseError(f"No Mach-O files found in {app}")
    run(["vtool", "-show-build", executable])
