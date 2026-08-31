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
| `a_` | A per-data-set parameter in a `--hybrid` fit - see [Individual, global, and mixed parameters](#individual-global-and-mixed-parameters) below |

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

Parameters aren't individually tagged as "global" or "individual" -
whether one varies per data set or stays shared is decided by its
**name**, not a per-parameter flag: any parameter whose name ends in an
underscore (`Minf_`, `T11_`, `individual_`, ...) is a per-data-set
candidate; every other parameter is shared across all data sets.

For that naming convention to actually take effect, pass `--hybrid`:

```bash
onefite fit \
  'y(x,global:1[-1<1],individual_:1[-2<2])=global+individual_*x' \
  sample-*.dat --hybrid --workers=2
```

Without `--hybrid`, underscore-suffixed parameters are fitted as ordinary
*shared* parameters instead - every data set ends up with the identical
value. The project's own `t/12/p96.json` test fixture (`Parameters =
M0_,Mi_,c,T11_,T12_`) demonstrates this directly: `onefite fit p96.json
--hybrid` fits `M0_`/`Mi_`/`T11_`/`T12_` independently per data set; the
same file without `--hybrid` gives every data set identical values for
those parameters and a dramatically worse chi-square.

### Advanced: driving a mixed fit from custom C code

If you write your own auxiliary C code (`--aux-code`) and
`#include "mixed.h"` (see `include/mixed.h` in the repository), that
header implements a separate, lower-level mechanism: at runtime it reads
the fitting function's *last* parameter, and if it is a parameter
literally named `MIXED`, fixed to a nonzero value (`MIXED=1`), switches to
a per-data-set parameter array via its `PAR()` macro. This works with
plain `--global` alone, without `--hybrid`, because the check happens
inside your compiled C code rather than in `onefite` itself. It's only
relevant if you're hand-writing AuxCode against `mixed.h`'s own
conventions - ordinary model expressions should use the `--hybrid` plus
trailing-underscore approach above instead. See
[fitting](fitting.md) before using either form of mixed fit.

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
