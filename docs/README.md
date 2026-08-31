# OneFit-Engine documentation

This documentation covers OneFit-Engine 0.9.165. The `multi MAIN` command
signatures and embedded POD in `bin/onefite` remain the authoritative CLI
reference - these guides explain workflows and link to that reference
rather than duplicating it.

## Start here

- [Getting started](getting-started.md) - install verification and a
  complete first fit.
- [Installation](installation.md) - installation modes, side effects, and
  platform selection.
- [Troubleshooting](troubleshooting.md) - diagnose common build, fit, plot,
  and service failures.

## Use OneFit-Engine

- [Model expressions](model-expressions.md) - parameters, bounds, aliases,
  contributions, mixed parameters, and auxiliary C code.
- [Data formats](data-formats.md) - text, ZIP, JSON, SAV, Stelar, and
  IST-FFC inputs.
- [Fitting](fitting.md) - `create`, `fit`, `random`, `plot`, global/hybrid
  fitting, parallelism, and quality-control workflows.
- [Aliases](aliases.md) - installed and local function aliases.
- [Output and archives](output-and-archives.md) - generated files, ZIP
  output, history, and replay.

## Operate the service

- [Service operation](service.md) - foreground, background, systemd, logs,
  ports, and deployment boundaries.
- [HTTP API](http-api.md) - routes, multipart request fields, responses,
  and `curl` examples.
- [Security](security.md) - required reading before enabling the
  unauthenticated, native-code-executing service.

## Install on a platform

- [Debian and Ubuntu](platform/debian-ubuntu.md)
- [Windows Subsystem for Linux](platform/wsl.md)
- [macOS](platform/macos.md)
- [Docker and container test environments](platform/docker.md)
- [Fedora, RHEL-family, Arch, and openSUSE](platform/other-linux.md)

## Maintain and extend

- [Testing](testing.md) - test selection, post-install checks, and
  containers.
- [Upgrading](upgrading.md) - upgrade behavior, local changes, and
  recovery.
- [Extending models](extending-models.md) - add local C fitting functions
  and metadata.
- [Development](development.md) - repository layout and contribution
  workflow.

## Reference commands

```bash
onefite --help
onefite man
onefite list models
onefite help MODEL
./INSTALL --help
```

## Documentation status

These guides describe observed implementation behavior, cross-checked
against `bin/onefite`, `INSTALL`, the pre-install scripts, and the Raku test
suite where practical. The HTTP interface is not
declared a stable public API. If a guide ever conflicts with current command
output, treat `onefite --help`, the embedded POD, and the implementation as
authoritative, and report the discrepancy.
