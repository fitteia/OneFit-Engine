# OneFit Engine

OneFit-Engine (`onefite`) is fitteia's internal fitting core: a Raku
command-line tool, and an optional small HTTP service, for importing
experimental data, defining model functions, and running individual, global,
or hybrid/mixed nonlinear least-squares fits with CERNLIB MINUIT. It produces
plots and portable JSON/SAV fit descriptions, and is intended to run in a VM
or container - locally or remotely - so fitteia's fitting core can be
embedded in user-defined environments and behind custom front-ends.

The current project version is **0.9.165**. The repository is distributed
under the [Artistic License 2.0](LICENSE).

> [!WARNING]
> The optional HTTP service has no authentication, authorization, or
> sandboxing, and it compiles and runs generated native code from client
> input. Do not expose it to an untrusted network. Read
> [the security guide](docs/security.md) before starting it.

## What it supports

- Fits defined directly on the command line, or loaded from saved JSON/SAV
  files.
- Individual, global, and hybrid/MIXED fitting.
- Plain-text, ZIP, Stelar SDF/SEF/HDF5, and OFE JSON/SAV inputs.
- MINUIT fitting methods, including `simp`, `scan`, `min`, and `minos`.
- Parallel mixed fits with configurable worker counts, optionally RAM-backed.
- Grace/PDF plots and optional MPEG4 output.
- Function aliases and locally extended C model libraries.
- A Cro-based HTTP upload-and-fit service (see the warning above).
- Linux, WSL2, macOS, and container-oriented installation paths.

## Quick start (Debian/Ubuntu)

```bash
sudo apt-get update && sudo apt-get install -y git raku
git clone https://github.com/fitteia/OneFit-Engine.git && cd OneFit-Engine
sudo ./pre-install-OFE-in-Linux.sh
./INSTALL --no-test --no-post-test && onefite service start && ./post-install.sh
```

See [Installation](docs/installation.md) for every other platform (WSL2,
macOS, other Linux distributions, Docker) and for exactly what those steps
do to your system - `./INSTALL` has real side effects (it runs `git stash &&
git pull`, can update system packages, and may invoke `sudo` repeatedly).

Verify it, then run one fit:

```bash
onefite --version
printf '1 3\n2 5\n3 7\n4 9\n' > line.dat
onefite fit 'y(x,a:0,b:1)=a+b*x' line.dat --autoxy --save-to=line-fit.json
onefite fit line-fit.json --fit-methods='simp scan min minos'   # refit the saved description
```

Continue with the [getting-started tutorial](docs/getting-started.md).

## Documentation

- **[Documentation index](docs/README.md)** - the full set, grouped by
  audience.
- **[Getting started](docs/getting-started.md)** - install verification and
  a complete first fit.
- **[Installation](docs/installation.md)** and
  **[platform guides](docs/platform/)** - every supported platform, and the
  installer's side effects.
- **[Model expressions](docs/model-expressions.md)**,
  **[data formats](docs/data-formats.md)**,
  **[fitting workflows](docs/fitting.md)**,
  **[aliases](docs/aliases.md)**,
  **[output and archives](docs/output-and-archives.md)**.
- **[Service operation](docs/service.md)**,
  **[HTTP API](docs/http-api.md)**, and
  **[security](docs/security.md)** (read before enabling the service).
- **[Testing](docs/testing.md)**, **[upgrading](docs/upgrading.md)**,
  **[extending the model library](docs/extending-models.md)**,
  **[troubleshooting](docs/troubleshooting.md)**,
  **[development](docs/development.md)**.

For the command-line reference itself (every sub-command and option, with
examples), don't look here - it lives with the tool and stays in sync with
it:

```bash
onefite --help      # one-line summary of every sub-command
onefite man          # the full manual
man onefite          # same manual, if the man page was installed
```

## Testing

```bash
onefite test list
onefite test --no-systemd-daemon --no-post-install
```

See [testing](docs/testing.md) for selecting individual tests and for the
multi-distro Docker sandbox, and [development](docs/development.md) for
contributor workflow.

## Repository layout

```text
bin/onefite                  CLI, Cro HTTP service, and embedded manual (POD)
INSTALL                      Raku installer invoked by the platform scripts
pre-install-OFE-in-Linux.sh  OS-level dependency setup (Debian/Fedora/Arch/SUSE)
pre-install-OFE-in-MacOS.sh  OS-level dependency setup (macOS)
post-install.sh              starts the service and runs `onefite test`
Dockerfile, Dockerfiles/*.dockerfile,
test-ofe-in-docker-sandbox.sh  containerized install/test workflows
C/core/                      the legacy C/Fortran fitting core
C/local/                     the local/site model function library
lib/                         the Raku `OneFit` module sources
etc/OFE/default/             installed configuration templates
t/                           the Raku test suite (`onefite test`)
examples/command-line/       runnable usage examples (`onefite path --examples`)
docs/                        this documentation
```

## License

[Artistic License 2.0](LICENSE) (a permanent license will be settled at the
end of the development process; `META6.json` and `LICENSE` should always
agree with this statement).
