# Getting started

This tutorial assumes OneFit-Engine is installed and `onefite` is on `PATH`.
See [installation](installation.md) if it is not.

## 1. Verify the installation

```bash
onefite --version
onefite path --src
```

The first command reports the installed version and where `zef` resolves
the `OneFit-Engine` module to. The second prints the installed source path.

## 2. Create sample data

Create `line.dat` with two whitespace-separated columns, `x` and `y`:

```text
1 3
2 5
3 7
4 9
```

## 3. Run a fit

```bash
onefite fit 'y(x,a:0,b:1)=a+b*x' line.dat \
  --autoxy --save-to=line-fit.json
```

The expression defines:

- `y` as the dependent variable;
- `x` as the independent variable;
- `a` as a free parameter initialized to 0;
- `b` as a free parameter initialized to 1;
- `a+b*x` as the model.

The command imports the data, generates and compiles model code, performs
the fit, creates plots, and saves an OFE description to `line-fit.json`.

## 4. Inspect the result

Expression-based fits work under `ofe-tmp` unless `--zip-to` changes the
work name. Common artifacts include fit logs, generated source, parameter
files, plots, and `All.pdf`:

```bash
find ofe-tmp -maxdepth 1 -type f -print | sort
```

Don't build automation around every internal filename - some are
implementation details that can change between versions. Use `--save-to`,
`--zip-to`, and selected result columns (`--print-columns`) for stable,
user-facing output instead. See [output and archives](output-and-archives.md).

## 5. Refit the saved description

```bash
onefite fit line-fit.json --fit-methods='simp scan min minos'
```

When fitting an existing JSON or SAV description, `onefite` creates a named
work directory and a ZIP package by default. Use `--zip-to=FILE` to choose
the archive name.

## 6. Try bounds and fixed values

```bash
onefite fit 'y(x[0<5],a:0[-2<2],b=2)[0<12]=a+b*x' line.dat
```

Here `a` is initialized to 0 and bounded to `[-2,2]`; `b` is fixed to 2; the
plot ranges are set for both axes.

## 7. Get help

```bash
onefite --help
onefite man
onefite list models
onefite list aliases
onefite help BPP
```

Next, read [model expressions](model-expressions.md),
[data formats](data-formats.md), and [fitting workflows](fitting.md).
