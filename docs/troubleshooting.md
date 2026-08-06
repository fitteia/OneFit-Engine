# Troubleshooting

## `onefite` is not found

```bash
command -v onefite
raku -e 'say $*REPO.repo-chain'
```

Confirm the install's binary directory and Raku's site `bin` directory are
on `PATH`. Use `./INSTALL --help` and repeat installation with an explicit
`--bindir` if necessary.

## Raku cannot find `OneFit`

A bare source checkout is not an installed module repository. Run the
installer, or, for development-only diagnosis, include the project library
explicitly:

```bash
raku -Ilib -e 'use OneFit; say "loaded"'
```

Generated placeholders such as `%OFE-PATH%` in source files mean running
some scripts directly from the checkout is not equivalent to running the
installed copy.

## Minuit build or checkout fails

The default compile path expects a sibling `../minuit` checkout and
updates it when Git handling is enabled.

```bash
ls -la ../minuit
git -C ../minuit status
command -v make gcc gfortran
```

Don't delete a customized Minuit tree without a backup. Review installer
output and CPU-architecture detection.

## Generated C compilation fails

Inspect `compile.log` in the generated work directory:

```bash
sed -n '1,240p' ofe-tmp/compile.log
```

Undeclared functions may need `--aux-code` declarations, or the model's
argument list may need correcting. Check expression quoting and grammar
first - see [model expressions](model-expressions.md).

## Fit does not converge

- inspect initialization values and bounds;
- simplify the model and data selection;
- try a deliberate `--fit-methods` sequence;
- verify uncertainties and units;
- disable outlier removal and quality rescaling while diagnosing;
- compare individual and global fits;
- disable parallelism (`--no-parallel`) to rule out workflow effects.

Scientific convergence problems aren't necessarily software errors.

## Plots or PDFs are missing

```bash
command -v grace xmgrace gs pdftk ffmpeg pdftoppm
```

Inspect the plot/conversion logs in the work directory. ImageMagick's
Ghostscript policy can block PDF/PS formats - the installer can loosen that
policy on some site installs; review rather than weakening it globally
without need (see [security](security.md#administrative-commands)).

## HDF5 or Stelar import fails

```bash
command -v h5dump
unzip -l input.zip
```

Check format-specific options, file integrity, expected zone/range
metadata, and whether the input already carries instrument errors - see
[data formats](data-formats.md).

## Service will not start

```bash
onefite service PID
onefite service log
ss -ltnp | grep 8142
```

Run in the foreground for direct diagnostics:

```bash
onefite start-web-engine --ip=127.0.0.1 --port=8142
```

For systemd:

```bash
sudo systemctl status onefite
sudo journalctl -u onefite --since today
```

## HTTP fit fails

Verify a simple `GET` route first, then a minimal trusted fit. Save
response headers and body - errors aren't consistently structured:

```bash
curl -v http://127.0.0.1:8142/list/models
```

Check upload filenames, service work-directory permissions, disk/memory
space, compiler availability, and the generated logs. Never "fix"
connectivity by exposing the unauthenticated service more broadly - see
[security](security.md).

## Alias not found

```bash
onefite list aliases
onefite path --aliases
test -f ./aliases.json && jq . ./aliases.json
```

Check spelling, whitespace, local-override precedence, and that the JSON is
valid. See [aliases](aliases.md).

## Parallel fit problems

```bash
onefite fit ... --no-parallel
ONEFITE_WORKERS=1 onefite fit ...
```

Compare results against serial execution, and check available CPU, memory,
and I/O. RAM-disk mode can improve I/O but can also exhaust memory.

## Collecting a useful report

Include: OFE version, OS/architecture, Raku version, install mode, the
exact command (with sensitive paths removed), input format, relevant logs,
and whether the problem reproduces without the HTTP service or
parallelism.
