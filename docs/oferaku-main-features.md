# OFE-Raku features

OFE-Raku (OneFit-Engine's `onefite`) is the original Raku command line of the
OneFit engine. It parses a model expression with a Raku grammar, generates
and compiles C code for it, runs MINUIT through the native OFE engine, and
collects the results and plots. It can also run as an HTTP service. Its model
syntax, fit modes and saved-fit files are the same as OFE-Go's.

Each section below matches a box on the chart. Click a box on the chart to
jump to its section. For the full reference, run `onefite --help` or
`onefite man`. The guides in `docs/` (`onefite path --src`) explain each
workflow in depth.

## 01 · Access

### Command line

```bash
onefite fit 'y(x,a:0,b:1)=a+b*x' line.dat --autoxy --save-to=line-fit.json
onefite fit line-fit.json --fit-methods='simp scan min minos'
```

- The main commands are `fit`, `create`, `plot`, `random`, `convert`,
  `archive`, `list`, `help`, `path` and `man`.
- `./INSTALL --alias=ofe` also installs the command under a short name, such
  as `ofe`.

### OFE-GUI

```bash
onefite-gui -onefite=$(command -v onefite)
```

OFE-GUI is a separate browser front end that runs this `onefite` for you. It
gives you a form for every option, live output, a parameters table, plots,
scans and History. Run it on the same machine as the engine, or on a server
through `onefite-gui -ssh user@server`. Batches of parallel fits need the
engine on the same machine as the GUI. Open the OFE-GUI tab of this window to
learn more.

### HTTP service

```bash
onefite service start --ip=127.0.0.1 --port=8142
curl -F 'file=@line.dat' -F 'function=y(x,a:0,b:1)=a+b*x' \
  -F 'download=zip' http://127.0.0.1:8142/fit -o line.zip
```

- This is an upload-and-fit HTTP API built on Cro. You `POST /fit` a data file
  or saved fit together with options as form fields, and get the result
  back.
- Other routes: `/plot`, `/convert`, `/list`, `/help` and `/man`.
- `onefite service stop`, `onefite service log` and `onefite service PID`
  manage the service. `--systemd-daemon` runs it under systemd instead.
- **No authentication:** the service compiles and runs code sent by its
  clients. Use it only on loopback or a trusted network. See
  `docs/security.md`.

### VM or container

- The engine is meant to run on the machine where the data and compute are:
  a VM, a remote server, or a Docker container.
- `./INSTALL --docker` (detected automatically inside a container) and the
  `Dockerfile` cover container installs.
- Custom front ends can drive the engine from outside through the command
  line or the HTTP service.

## 02 · Choose input

### Fresh model + raw data

```bash
onefite fit 'y(x,a:1[0<10],b=2)=a+b*x' d1.dat d2.dat
onefite fit '#2exp' d1.dat
```

- The model syntax is `OUTPUT(X[range], PARAM, ...)[range]=EXPRESSION`. Each
  parameter is written in one of these forms:
  - `a`: free;
  - `a:1`: free, starting at 1;
  - `a[0<10]`: free, with bounds;
  - `a=1`: fixed;
  - `a_`: one value per block in a hybrid fit.
- `\+` splits the model into terms that are plotted separately.
- An alias (`'#NAME'`, `'alias: NAME'` or `'a: NAME'`) replaces the whole
  expression.
- `--aux-code='funcs.c, double f(double x)'` compiles your own C functions
  into the model.
- Supported data:
  - plain text columns: `x y`, or `x y error`;
  - ZIP files of data files;
  - Stelar SDF, SEF and HDF5 files;
  - IST-FFC files.
- Options for importing data:
  - `--R1` imports R1 instead of Mz;
  - `--zone-window` and `--gfilt` control how Stelar SDF data is averaged and
    smoothed;
  - `--sef-R1-file` gives the frequencies for a SEF file;
  - `--set-err` sets or derives the error column of text data, in forms such
    as `1%`, `10x`, `std` or `10% avg split at 10.5`.

### Resume a saved fit

```bash
onefite fit saved.json
onefite fit saved.json '--#b=0.5[0<1]' --individual
```

- A saved `.json` or `.sav` file holds the data, model, parameters and fit
  settings. Fitting it again starts from its saved values. The data import
  options are already part of the saved file.
- `'--#name=value[min<max]'` changes a parameter or an axis range for this
  run only. Always quote it, because `#` starts a comment in the shell.
- `--export` first writes the embedded data, and any AuxCode, to plain files.
- A resumed fit is zipped by default, under the input file's name.

### Batch of complete fits

```bash
onefite fit run1.json run2.json --hybrid
onefite fit '#1exp,d1.dat' '#2exp,d1.dat' --save-to={name}.json
onefite fit @fits.txt --jobs=2 --wf=results
```

- When every argument is a complete fit, the fits run in parallel. A complete
  fit is a saved fit, a packed `'#alias,data...'`, or `@FILE`.
- A jobs file (`@FILE`) has one fit per line, with TAB-separated fields:
  first a model and its data files (or a saved fit), then that fit's own
  options. A line's own fit mode (`--hybrid`, `--global` or `--individual`)
  replaces the batch's, so a single batch can compare modes.
- `--jobs` sets how many fits run at once. The default is the number of
  CPUs.
- Each fit's output goes to its own folder in a new `batch-YYYYmmdd-HHMMSS/`,
  and `batch.json` records each fit's state, chi2 and files.
- If one fit fails, the others keep running.

## 03 · Configure

### Fit strategy

- **Individual** fits each data block on its own. This is the default for
  several files, and `--individual` forces it for a saved fit.
- **Global** (`--global`) minimises one shared chi2 over all blocks.
- **Hybrid/MIXED** (`--hybrid`) works like a global fit, except that each
  parameter whose name ends in `_` (`Minf_`, `T11_`) is fitted separately per
  block, while the other parameters stay shared. Hybrid implies global, and
  it is only available with `fit`.
- Hand-written AuxCode that uses `mixed.h` can get the same per-block
  behaviour under plain `--global`, through a fixed last parameter named
  `MIXED`.

### Parameters and data

- **Starting values and bounds** come from the model or the saved fit, and
  `--#` overrides change them for one run.
- **Selected data sets**: `--selected-dataset='20kHz_1,#1-#4,#7'` (also
  `--sds`) fits only those blocks. Select by TAG, by `#N` position, or by
  range.
- `--fit-if='c1<0.1,1'` and `--plot-if` keep only the data rows that
  satisfy a condition on the row's columns (`c1` = x, `c2` = y, `c3` =
  error), then a step (`,1` every row, `,2` every second). They apply to
  Stelar and IST-FFC imports; `--selected-dataset` is what selects blocks.
- **Quality options**:
  - `--remove-outliers` drops points;
  - `--reduced-chi2` rescales the errors;
  - `--error-bars` shows the data's errors in the plots;
  - `--R2` adds the Pearson correlation;
  - `--print-columns` stores chosen result columns.

### Methods and parallelism

- **MINUIT methods** run in the given order. The default is
  `simp scan min minos`:
  - `simp` is a robust first search;
  - `scan` plots chi2 against each parameter;
  - `min` (MIGRAD) finds the minimum and the errors;
  - `minos` gives asymmetric errors. It is slow in hybrid fits, so you can
    leave it out of exploratory runs.
- **Workers**: hybrid blocks are fitted in parallel. `--workers=N` sets the
  number of workers, and `--no-parallel` turns them off. `--jobs` sets how
  many batch fits run at once.
- `--use-ramdisk` stages the work in RAM (`/dev/shm`, or a RAM disk on
  macOS). It is faster for fits that read and write many files, but the files
  are lost on reboot.

## OFE-Raku orchestrates → native OFE / MINUIT executes

For each fit, OFE-Raku:

1. parses the model with its Raku grammar;
2. imports the data;
3. writes the C code, data and parameter files;
4. compiles them against the native OFE library and MINUIT;
5. runs the optimizer;
6. collects the logs, plots and saved description.

A run is one of three kinds: a single fit, a hybrid fit whose blocks run on
parallel workers, or a batch of independent fits. A batch puts each fit in
its own folder and records them all in `batch.json`. `--work-folder` chooses
where the work goes, and the work folder may be deleted and created again, so
keep it free of unrelated files.

## Results

### Fit results

- The output shows the fitted values, errors, chi2 and MINUIT's status.
- `fit.log` holds MINUIT's full log.
- The results tables in the saved description list each block's chi2 and
  each parameter with its error.
- `--print-columns='2,a,a+1'` also stores just the columns you choose.

### Plots and SCAN

- The plots are drawn by Grace: one `fit-curves-N.pdf` per block, combined
  into `All.pdf`.
- `--mp4` also makes `All.mp4`.
- `--logx`, `--logy`, `--autox`, `--autoy` and `--Num` control how the plots
  look, and `--no-plot` skips them.
- With `scan` in the fit methods, `fit.log` has MINUIT's chi2 scans. OFE-GUI
  draws these scans.

### Save and reuse

- `--save-to=fit.json` (or `.sav`) writes a description you can fit, plot or
  convert again later.
- `--zip-to=fit.zip` packages the whole work folder.
- `onefite plot fit.json` redraws the plots at the saved values without
  fitting.
- `--archive` records the command and its data. `onefite archive` lists the
  archived runs, and `onefite archive --fit=last` replays one; options you
  add after it are passed on to the replayed run.

### Service responses

- A `/fit` request gets its result in the response.
- The `download` field picks what comes back:
  - `zip`: the whole work folder;
  - `json`: the saved description;
  - a file name (for example `All.pdf`): that one file;
  - no `download` field: the fit log, as text.
- Each request runs in its own retained folder.

## Supporting features

- `onefite list models`, `onefite list aliases` and `onefite help MODEL [KEY]`
  show the model catalogue and the aliases.
- `onefite --help` lists every command, and `onefite man` shows the full
  manual.
- `onefite path` shows the engine's folders (`--aliases`, `--log`, `--src`,
  …), and `onefite --version` shows its version.
- `onefite convert in.sav out.json` converts in either direction.
- `onefite random MODEL data.dat` sets each bounded parameter to a random
  value inside its bounds and evaluates the model, which makes synthetic
  data.
- **Install** with `./INSTALL`. It installs site-wide by default, or with
  `--to-user`, `--wsl`, `--macos` or `--docker`. `./INSTALL --help` lists the
  options. It can update system packages and run `git stash && git pull`, so
  read `docs/installation.md` first.
- **Maintain** with these commands:
  - `onefite upgrade` keeps the extensions and the MINUIT limit recorded in
    `etc/engine.json`, and puts the previous engine back if the build fails;
  - `onefite uninstall` removes the engine;
  - `onefite test` runs the test suite;
  - `onefite service start`, `stop`, `log` and `PID` manage the HTTP service.
- **Extend the engine** in three ways:
  - `--define-alias=NAME` saves a fitted model as a local alias in
    `./aliases.json`, which takes precedence over the installed aliases;
  - local C model libraries add model functions;
  - extensions such as Florence add model functions, and `./INSTALL` or
    `upgrade --extension=...` installs them.
