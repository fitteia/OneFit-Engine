# Output and archives

## Work directories

Expression-based `fit` and `create` use an `ofe-tmp` work name by default.
`--zip-to=NAME.zip` changes the expression-fit work name to `NAME`.
Saved-file `fit` and `plot` derive their work name from the input filename
instead.

`--work-folder=DIR` selects the parent directory. Target directories may be
removed and recreated by these commands, so use a dedicated work parent
rather than one holding unrelated files.

## Common artifacts

Depending on command and options, output can include:

- generated C source and the compiled fitting executable;
- data and parameter files;
- compilation and fit logs (`compile.log`);
- per-block fit results and residuals;
- Grace and PDF plots;
- `All.pdf` and, with `--mp4`, `All.mp4`;
- a JSON or SAV engine description;
- a ZIP package of the whole work directory.

Internal filenames may change between versions. Prefer explicitly-named
`--save-to`/`--zip-to` outputs, and `--print-columns`-selected result
tables, in automation.

## Save versus ZIP

```bash
onefite fit MODEL data.dat --save-to=fit.json --zip-to=fit.zip
```

`--save-to` creates a reusable engine description you can later `fit`/`plot`
again. `--zip-to` packages the broader work environment (data, generated
code, plots, logs). Fitting an existing JSON/SAV file creates a ZIP by
default even without `--zip-to`; its default name is derived from the input
file's basename.

## Export from a saved description

```bash
onefite fit fit.json --export
```

This writes the embedded data to `data-fit.txt` and, when present, the
auxiliary C source to `AuxCode-fit.c`, before fitting.

## Select result columns

```bash
onefite fit MODEL data.dat --print-columns='2,a,a+1'
```

Selections may use numeric indices, parameter names, and relative suffixes
such as `a+1`/`a-1`. Selected tables are added to the engine's result hash,
which is useful for downstream parsing.

## Archive a fit

```bash
onefite fit MODEL data.dat --archive
```

This records the exact command line, and the data files it referenced, in
the local fit history/archive:

```bash
onefite path --archive     # the archive-log directory
onefite archive            # print the history
```

`archive`, `ar`, and `log` are three names for the same command.

## Replay an entry

```bash
onefite archive --fit=last
onefite ar --fit=last-1 --rchi2
onefite log --fit=2 --remove-outliers=1
```

An entry can be selected by its printed index, or by `last`/`last-N`
(counting back from the newest). Any text that isn't a bare number or
`last-N` falls back to the newest entry in the current implementation.
Additional named options are appended to the replayed command, letting you
re-run a stored fit with extra flags applied.

Archive replay unzips the entry's data and **re-executes the stored command
as a shell command**. Treat archive contents as trusted, and don't import
or replay an archive from a source you don't trust - see
[security](security.md).
