# Fitting workflows

## Choose a command form

### Create without fitting

```bash
onefite create 'y(x,a,b)=a+b*x' data.dat --save-to=definition.json
```

`create` imports data, parses and compiles the model, and saves or prints
an engine description. It does not run MINUIT.

### Fit an expression and raw data

```bash
onefite fit 'y(x,a,b)=a+b*x' data.dat --save-to=result.json
```

This form supports data-import controls, auxiliary code, aliases, and
dynamic model overrides.

### Refit a saved description

```bash
onefite fit result.json --fit-methods='simp scan min minos'
```

This form supports `--export` and `--individual`, but raw-data import
options are already baked into the description. It creates a ZIP package by
default.

### Plot without fitting

```bash
onefite plot result.json --autoxy
```

### Randomize starting parameters

```bash
onefite random 'y(x,a:0[-10<10],b:0[-10<10])=a+b*x' data.dat --global
```

`onefite random` assigns a random value within each bounded parameter's
range, fixes those parameters, and runs the resulting model. It supports
fewer controls than `fit` - notably no `--fit-methods`, `--aux-code`,
`--archive`, or dynamic overrides, and its "skip plotting" flag is spelled
`--inp`/`--no-plot` rather than `fit`'s `--np`/`--no-plot`. Consult
`onefite --help`/`onefite man` for the exact, current option list.

### Which options apply to which form?

`create`, `random`, and both forms of `fit` share most of their
data-scaling and workspace options (`--autox`/`--logx`/etc.,
`--work-folder`), but diverge in real ways beyond what's listed above - for
example `create` and the expression form of `fit` share `--SymbSize`,
`--range`, `--gfilt`, and `--set-err`, none of which apply once you're
re-fitting a saved file. Don't assume an option that works on one form
works on all of them; `onefite man` documents each command's actual
signature.

## MINUIT methods

The default method sequence for new definitions is:

```text
simp scan min minos
```

Override it with a whitespace-separated list:

```bash
--fit-methods='simp min minos'
```

Methods run in the specified order. Dropping `minos` specifically loses
its (slower, but more reliable) asymmetric parameter-error estimates -
keep it in the sequence for anything but a quick exploratory fit. It's
sometimes dropped for large hybrid/MIXED fits to keep per-worker runtime
down; see [Hybrid/MIXED](#hybridmixed) below.

## Fitting modes

### Individual

Each data set is fitted independently. This is the default for ordinary
multi-file fitting. When loading a saved file, `--individual` forces this
mode.

### Global

```bash
onefite fit MODEL sample-*.dat --global
```

Global fitting minimizes a shared objective over all selected data sets.
Available on `create`, `random`, and both forms of `fit`.

### Hybrid/MIXED

```bash
onefite fit MODEL sample-*.dat --hybrid --workers=2 --fit-methods='simp scan min'
```

Hybrid/MIXED fits re-run the full method sequence per data set for the
per-block (`.MIXED=1`) parameters on top of the shared global fit, so
`minos`'s extra per-parameter re-minimization pass adds up fastest here -
dropping it (as above) is a reasonable way to keep exploratory hybrid runs
fast. Add it back (`--fit-methods='simp scan min minos'`, the default) once
you need real error estimates, e.g. for a final result.

Hybrid mode implies global fitting, while parameters marked `.MIXED=1` are
handled per data set. Only available on `fit` (either form) - `create` and
`random` don't support it. The expression form of `fit` also detects
`.MIXED=1` in the function text and turns on global mode by itself.
Validate a mixed model against a known result before increasing
parallelism. See [model expressions](model-expressions.md).

## Parallel execution

Parallel fitting of hybrid/mixed data blocks is enabled by default.

```bash
--workers=4
--no-parallel
```

A positive worker count sets `ONEFITE_WORKERS`; `--no-parallel` sets
`ONEFITE_NO_PARALLEL=1` for the current process. Both only affect `fit`.

## RAM-backed work

`--use-ramdisk` stages supported fits in a RAM-backed filesystem (`/dev/shm`
on non-macOS systems; a mounted RAM disk on macOS). This can improve
I/O-intensive fits but consumes memory and is not durable. Save or archive
required output before shutdown.

## Data and quality controls

- `--remove-outliers=2`, `2..5`, or `1,3,5` removes selected points.
- `--reduced-chi2` recalculates parameter errors for a reduced chi-square
  close to the number of degrees of freedom.
- `--error-bars` includes error bars in fit output and plots.
- `--pearson-correlation`/`--R2` calculates a Pearson quality coefficient.
- `--print-columns='2,a,a+1'` stores selected result columns.

These controls affect scientific interpretation. Record them with the fit
description or archive entry.

## Plot controls

```bash
--autox --autoy
--autoxy
--logx --logy
--logxy
--Num=200
--SymbSize=1.5
--no-plot
--mp4
```

`--mp4` converts `All.pdf` using the installed `pdf2mp4` helper and
requires the external PDF/video toolchain.

## Saving and packaging

```bash
--save-to=result.json
--zip-to=result.zip
--archive
```

`--save-to` writes a reusable engine description. `--zip-to` packages the
work directory. `--archive` records the command and associated data in
OFE's history. See [output and archives](output-and-archives.md).
