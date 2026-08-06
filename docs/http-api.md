# HTTP API

The Cro application started by `onefite start-web-engine` exposes a
multipart HTTP interface. It is not currently declared a stable public API.

> [!CAUTION]
> All routes are unauthenticated. `/fit` writes uploads, **assembles form
> values directly into a shell command**, compiles and runs generated
> native code, and can return arbitrary files from its work directory. Use
> only in a trusted, isolated deployment. See [security](security.md).

Examples use `http://127.0.0.1:8142`.

## `GET /`

Returns an HTML form for fitting, conversion, listing models, model help,
and the manual.

```bash
curl http://127.0.0.1:8142/
```

## `GET|POST /list`

Routes:

```text
/list
/list/models
/list/aliases
```

Returns `text/plain`, corresponding to `onefite list ... --quiet`.

```bash
curl http://127.0.0.1:8142/list/models
```

## `GET /help/MODEL[/KEY]`

Returns `text/plain` model metadata, or one metadata value.

```bash
curl http://127.0.0.1:8142/help/BPP
```

## `POST /help`

Multipart field `model` is required. One optional additional field may
carry the requested key as its *value* - the implementation inspects the
field's value, not a specific field name.

```bash
curl -F 'model=BPP' -F 'key=description' http://127.0.0.1:8142/help
```

## `GET|POST /man`

Returns `text/plain` containing the generated troff/man representation of
the embedded POD - not the normally-formatted terminal text you'd get from
`onefite man` at a shell.

```bash
curl http://127.0.0.1:8142/man
```

## `POST /convert`

Upload a JSON or SAV file as multipart field `file`. The service converts
to the other format and responds with `application/octet-stream`.

```bash
curl --fail --silent -F 'file=@fit.json' \
  http://127.0.0.1:8142/convert --output fit.sav
```

## `POST /fit`

Upload raw data or an engine description as multipart field `file`. All
other fields are optional and mirror `onefite fit FUNCTION ...`'s
command-line options under the same short or long names.

Minimal saved-description fit:

```bash
curl --fail --silent -F 'file=@fit.json' http://127.0.0.1:8142/fit
```

Fit raw data with an inline function, and download the whole result as a
zip:

```bash
curl --fail --silent \
  -F 'file=@line.dat' \
  -F 'function=y(x,a:0,b:1)=a+b*x' \
  -F 'autoxy=yes' \
  -F 'download=zip' \
  http://127.0.0.1:8142/fit \
  --output line.zip
```

...or just the fit-curve plot:

```bash
curl -F 'file=@line.dat' -F 'function=y(x,a:0,b:1)=a+b*x' -F 'autoxy=yes' \
  -F 'download=All.pdf' http://127.0.0.1:8142/fit -s --output All.pdf
```

...or the re-saved JSON, under a chosen work folder (two equivalent forms -
a `username` field, or an extra URL path segment):

```bash
curl -F 'file=@line.dat' -F 'function=y(x,a:0,b:1)=a+b*x' \
  -F 'download=result.json' -F 'username=ofe' -s http://127.0.0.1:8142/fit
curl -F 'file=@line.dat' -F 'function=y(x,a:0,b:1)=a+b*x' \
  -F 'download=result.json' -s http://127.0.0.1:8142/fit/ofe
```

Fitting a Stelar HDF5 file:

```bash
curl http://127.0.0.1:8142/fit \
  -F 'file=@C12-60.hdf5' \
  -F 'function=Mz[-1.5<1.5](t[1e-3<10],a,b,c=1[0.5<1],T11:0.05[0<4],T12:0.01[0<4])=a\+b*c*exp(-t/T11)\+b*(1-c)*exp(-t/T12)' \
  -F 'autox=yes' -F 'autoy=yes' -F 'logx=yes' \
  --silent
```

### Common fields

| Field | Meaning |
| --- | --- |
| `file` | Required upload |
| `function` | Model expression for raw data |
| `fit-methods` / `fm` | MINUIT method sequence |
| `Num` / `npts` | Theoretical curve point count |
| `SymbSize` / `ssz` | Symbol size |
| `global` / `g` | Global fit when value is `yes` or `1` |
| `hybrid` / `mixed` | Hybrid fit when value is `yes` or `1` |
| `autox`, `autoy`, `autoxy` | Automatic axes |
| `logx`, `logy`, `logxy` | Logarithmic axes |
| `R1`, `R2` | Import/quality controls |
| `err` / `set-err` | Error expression |
| `range`, `gfilt` | Import controls |
| `fit-if`, `plot-if` | Tag conditions |
| `remove-outliers` | Outlier selection |
| `reduced-chi2` | Error rescaling when `yes` or `1` |
| `print-cols` | Selected result columns |
| `username` | Existing work-directory selector; not authentication |
| `download` | Select response artifact |

Other CLI aliases (e.g. `pi` for `plot-if`, `sf` for `sef-R1-file`) are
accepted by the current route code. A multipart field whose name begins
with `#` is forwarded as a dynamic model override (see
[model expressions](model-expressions.md)).

### Download selection

- `download=zip` returns the whole work directory as a ZIP.
- `download=json` returns the saved JSON description
  (`FOLDER/BASENAME.json`, derived from the uploaded file's own name).
- any other non-empty value is treated as a path relative to that same
  per-upload folder, and streamed back verbatim.
- no `download` field returns a `text/plain` fit log instead.

Binary downloads use `application/octet-stream`. The temporary,
randomly-named work directory for the request is deleted after the
response is constructed.

### Error behavior

The application does not consistently map failures to structured HTTP
status codes or JSON error bodies. Check HTTP status, response content
type, output size, and expected artifacts. Don't treat a `text/plain`
response as a successful scientific fit without parsing its actual content.

## Compatibility

Until a versioned schema and route tests exist, treat route names, fields,
response formats, and generated filenames as implementation details. Pin
the OFE version in integrations, and test again after every upgrade.
