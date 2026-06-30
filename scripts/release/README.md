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
