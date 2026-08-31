# Data formats

OneFit-Engine accepts raw experimental data and reusable engine
descriptions. Import behavior varies by format.

## Plain text

Files with `.dat` or `.txt` extensions contain whitespace-separated numeric
columns. The common forms are:

```text
x y
x y ey
```

The third column, when present, is the uncertainty in `y`. Fixtures under
`t/1/` and `t/3/` demonstrate accepted plain-text input.

Use `--data-labels` when imported blocks need explicit labels, and `--tags`
to attach data-set tags used by `--fit-if`/`--plot-if`.

## ZIP archives

ZIP files can contain supported raw data files. The import layer expands
and normalizes their contents into the fit work directory. Keep archive
member names simple and avoid unrelated files.

`--zip-to` is different: it packages fit *output*, not input - see
[output and archives](output-and-archives.md).

## JSON and SAV

`.json` and `.sav` files are engine descriptions containing data,
parameters, functions, and fit settings.

```bash
onefite convert input.sav output.json
onefite convert input.json output.sav
onefite fit saved-fit.json
onefite plot saved-fit.sav
```

Conversion direction is inferred from the filename extensions (whichever
side contains `json` is treated as JSON; anything else is treated as the
legacy CGI-encoded `.sav` format). `convert` uses `/tmp` as its work
directory unless `--work-folder` is given.

## Stelar formats

The import layer recognizes Stelar `.sdf`, `.sef`, and `.hdf5` inputs.
Relevant controls include:

- `--R1` to import R1 rather than Mz data, where supported;
- `--sef-R1-file=FILE` to supply frequencies for `.sef` import;
- `--range=TEXT` to select imported zones;
- `--gfilt=N` to smooth imported data with a Gaussian filter;
- `--fit-if`/`--plot-if` to select tagged blocks.

Instrument formats supply their own errors; `--set-err` does not apply to
IST-FFC, SEF, or HDF5 imports.

A compressed HDF5 example ships at
`examples/command-line/test-data/C12-60.zip`.

## IST-FFC

`create` supports `--ist-ffc-Mz`/`--ist-ffc` for importing Mz data - it's
the only command that has this option. `--R1` (available on `create` and
the expression form of `fit`) is used for field-cycling R1 imports instead.
None of these apply when re-fitting an existing JSON/SAV description, since
the import step has already happened.

## Setting or deriving uncertainties

For ordinary text inputs, `--set-err`/`--err` accepts several forms:

```text
1.0
1%
10x
x10/100
std
std split at 5
10% avg split at 10.5
```

Verify derived errors on a small data set before relying on them.

## Filtering data blocks

`--fit-if=CONDITION` controls which tagged blocks are fitted.
`--plot-if=CONDITION` controls which blocks and curves are plotted. The
exact tag expressions depend on imported metadata; inspect a generated JSON
file when building a conditional workflow.

## Format safety

Treat imported files as trusted input. Fit descriptions can carry embedded
model code (`AuxCode`), and both the CLI and the HTTP service write
uploaded/imported files to disk and invoke the fitting tools on them - see
[security](security.md).
