# Function aliases

Aliases give reusable names to complete model expressions.

## Alias sources

OneFit-Engine reads:

1. the installed `aliases.json` under the installation configuration path;
2. an optional `./aliases.json` in the current directory.

Local entries are merged over installed entries and take precedence, so
your own aliases survive an `onefite upgrade` (which only touches the
installed copy). Discover the installed configuration directory with:

```bash
onefite path --aliases
```

## Inspect aliases

```bash
onefite list aliases
```

The output is JSON. Use `jq` when available:

```bash
onefite list aliases --quiet | jq
```

## Use an alias

Equivalent accepted forms include:

```bash
onefite fit 'alias: one exponential' data.dat
onefite fit 'a: one exponential' data.dat
onefite fit '#1exp' data.dat
```

Quote aliases containing spaces or beginning with `#`.

## Define an alias from a fit

```bash
onefite fit 'y(x,a,b)=a+b*x' data.dat --define-alias=line
```

`--define-alias` (`-a`/`--da`/`--dali`; also `--alias` on the expression
form of `fit`) creates or replaces the named local alias, using the fitted
expression with its parameters substituted in. This writes to
`./aliases.json` in the current directory - review it before committing or
copying it into your installed set.

## Preserve custom aliases

The installer never overwrites your local project aliases. Compare an
installed alias file with the repository's shipped defaults - this works
from any directory, since both paths are resolved through `onefite path`:

```bash
diff "$(onefite path --aliases)/aliases.json" \
  "$(onefite path --src)/etc/OFE/default/aliases.json"
```
