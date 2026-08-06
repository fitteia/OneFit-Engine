# Model expressions

OneFit-Engine turns a compact expression into generated C fitting code.
Always quote the complete expression at the shell.

## General form

```text
OUTPUT(X[X-RANGE], PARAMETER, ...)[OUTPUT-RANGE]=EXPRESSION
```

The grammar (`lib/OneFit/Engine/Grammars.rakumod`) accepts `OUTPUT-RANGE`
in either position - attached directly to `OUTPUT` before the parameter
list, or after the closing parenthesis, right before `=`. Both are
equivalent; use whichever reads better for a given expression:

```text
OUTPUT[OUTPUT-RANGE](X[X-RANGE], PARAMETER, ...)=EXPRESSION
OUTPUT(X[X-RANGE], PARAMETER, ...)[OUTPUT-RANGE]=EXPRESSION
```

For example, both of these are equivalent:

```bash
onefite fit 'y[0<10](x[0<5],a:1[0<10],b=2)=a+b*x' data.dat
onefite fit 'y(x[0<5],a:1[0<10],b=2)[0<10]=a+b*x' data.dat
```

## Parameter declarations

| Syntax | Meaning |
| --- | --- |
| `a` | Free parameter with engine-selected initialization |
| `a:1.0` | Free parameter initialized to 1.0 |
| `a[0<10]` | Free parameter bounded from 0 through 10 |
| `a:1.0[0<10]` | Initialized and bounded parameter |
| `a=1.0` | Fixed parameter |
| `a=1.0[0<10]` | Fixed value carrying an allowed range |
| `a=1.0[0<10].MIXED=1` | Per-data-set parameter in a `--hybrid` fit (see [fitting](fitting.md)) |

The parser accepts `<` or `;` as range separators. The `<` form is used most
often in this project's examples, but it must remain inside quotes so the
shell does not treat it as input redirection.

## Axis ranges

Ranges after the output and independent variable control theoretical-curve
and plot ranges:

```text
y(x[0<100],a,b)[-1<1]=a+b*x
```

## Plotting model contributions

An escaped plus, `\+`, marks model contributions that should be plotted
separately:

```bash
onefite fit 'y(x,a,b,c)=a\+b*x+c*x*x' data.dat
```

The backslash is part of the model syntax, not merely shell escaping.

## Function aliases

Installed and local aliases can replace a complete expression:

```bash
onefite fit 'alias: one exponential' data.dat
onefite fit 'a: one exponential' data.dat
onefite fit '#1exp' data.dat
```

See [aliases](aliases.md).

## Dynamic overrides

`fit` accepts named arguments beginning with `#` to update an axis or
parameter declaration before fitting:

```bash
onefite fit 'y(x,a,b)=a+b*x' data.dat \
  '--#a=0.0[0<2]' '--#x[0<10]'
```

Quote every override - an unquoted `#` starts a shell comment in many
shells. Dynamic overrides are implemented for both forms of `fit`. `create`
accepts `#`-prefixed options without erroring, but silently ignores them
(only printing a "not defined" warning) rather than applying them - do not
rely on them there. `random` and `plot` have no catch-all for unrecognised
options at all, so passing one causes the command to fail to run rather
than being accepted and ignored.

## Individual, global, and mixed parameters

`--global` changes the objective from per-data-set fitting to a shared
global fit. `--hybrid` enables mixed fitting and implies global mode -
`--hybrid` is only available on `fit` (either form), not on `create` or
`random`.

A mixed parameter is marked with `.MIXED=1`:

```bash
onefite fit \
  'y(x,global:1[-1<1],individual_=1[-2<2].MIXED=1)=global+individual_*x' \
  sample-*.dat --hybrid --workers=2
```

The expression form of `fit` also detects `.MIXED=1` and activates global
mode automatically - but only that form; re-fitting a saved `INPUT-FILE`
does not re-scan its function text for `.MIXED=1`. Read
[fitting](fitting.md) before using mixed fits.

## Auxiliary C code

Use `--aux-code` when the expression calls a C function not already
available to generated model code (`create` and the expression form of
`fit` only):

```bash
onefite fit 'y(x,a)=a*custom(x)' data.dat \
  --aux-code='functions.c, double custom(double x)'
```

The value is comma-separated: the first element is the source filename and
the remaining elements are declarations. Generated native code is executed;
only use trusted source and expressions.

## Model library metadata

Inspect compiled model metadata and individual keys with:

```bash
onefite list models
onefite help MODEL
onefite help MODEL KEY
```

The grammar implementation lives in `lib/OneFit/Engine/Grammars.rakumod`.
When exact syntax is uncertain, use the repository tests and a small
disposable fit to verify it.
