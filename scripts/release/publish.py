from __future__ import annotations

from pathlib import Path

from .common import ReleaseError, require_path, run


def publish(*, tag: str, asset: Path) -> None:
    require_path(asset, "release asset")
    existing = run(["gh", "release", "view", tag], check=False, capture=True)
    if existing.returncode != 0:
        created = run(
            [
                "gh",
                "release",
                "create",
                tag,
                "--draft",
                "--generate-notes",
                "--title",
                tag,
            ],
            check=False,
            capture=True,
        )
        if created.returncode != 0:
            # Matrix jobs can race while creating the shared draft release.
            raced = run(["gh", "release", "view", tag], check=False, capture=True)
            if raced.returncode != 0:
                raise ReleaseError(
                    f"Could not create or find release {tag}: {created.stderr.strip()}"
                )
    run(["gh", "release", "upload", tag, asset, "--clobber"])
