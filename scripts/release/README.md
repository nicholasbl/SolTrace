# Release tooling

`release_tools.py` contains the platform packaging behavior used by
`.github/workflows/gui-release.yml`. The workflow owns the build matrix and
third-party setup; these modules own archive construction, runtime staging,
license collection, and static artifact validation.

Run the CLI from the repository root:

```console
python -m scripts.release.release_tools --help
```

The commands use only the Python standard library. Platform commands such as
`otool`, `dumpbin`, `ditto`, `linuxdeploy`, and `gh` are invoked as subprocesses
and must be available for the relevant command.

Run the portable unit tests with:

```console
python -m unittest discover -s scripts/release/tests -v
```

Keep GitHub-specific expressions and matrix decisions in the workflow. Put
branching, filesystem traversal, output parsing, and artifact validation here
so those behaviors remain readable and testable outside GitHub Actions.

## Why installers package a staged directory

The project install tree serves both application users and developers, so it
contains runtime files as well as headers, libraries, and CMake package files.
Official releases also use pinned binary dependencies and collect licenses that
may not exist in a developer's local dependency installation. Those concerns
must not become prerequisites for a normal local build or `cmake --install`.

The release pipeline therefore has two explicit phases:

1. CMake installs normally into `dist`, after which the Python tooling adds the
   pinned runtime libraries, licenses, and performs dependency validation.
2. Windows MSI packaging copies a consumer subset into `installer-root`,
   excluding the top-level `include` and `lib` development trees. A standalone
   CPack configuration packages that directory with WiX. It does not include the
   project's generated CPack configuration and does not execute install rules.

The Windows ZIP still packages the complete `dist` tree. This keeps a portable,
developer-friendly artifact while the MSI contains the consumer runtime. The
initial MSI is produced for the standard Embree build only; defining upgrade
and side-by-side installation behavior for the OptiX variant is a separate
product decision.

The MSI command expects a validated Windows `dist` tree, CMake 3.30 or newer,
and WiX 4 with its matching UI extension. CI pins and installs those tools. The
generated installer is x64, per-machine, installs under Program Files, and
therefore requests elevation. Building or running SolTrace locally does not
invoke this command.
