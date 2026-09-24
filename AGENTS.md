# Working with OneFit-Engine

Shared, canonical guidance for any coding agent working in this repository
(Claude Code, Codex, or otherwise) - `CLAUDE.md` just points here so
Claude Code loads it too. Human-facing docs live in `README.md` and
`docs/`; this file is for orienting quickly and not repeating mistakes
previous sessions already found and fixed.

## What this project is

OneFit-Engine (`onefite`) is **fitteia's core fitting engine** - a Raku
CLI and an optional small Cro-based HTTP service that imports experimental
data, parses model expressions, compiles them (with locally extended C
model libraries) and runs individual/global/hybrid nonlinear least-squares
fits with CERNLIB MINUIT. It produces Grace/PDF plots, optional MPEG4, and
portable JSON/SAV fit descriptions.

This is the **parent/core project** other repos in this same parent
directory build on top of or interoperate with:

- `onefite-gui` - a GUI front-end (own `AGENTS.md` if you need to work
  with a running instance of it).
- `onefite-native` - a Go/Rust port aiming for byte-for-byte-identical
  output to this engine (own `AGENTS.md` covers its parity-verification
  methodology).
- `onefite-ofe-assistant` - an LLM-driven assistant that drives `onefite`
  deterministically via CLI flags (prefers `-function`/explicit flags
  over natural-language planning for real work - see its own `AGENTS.md`).
- `onefite-c-code` - **not a consumer, a dependency**: this repo's own
  `C/core` (legacy C/Fortran fitting core) and `C/local` (site model
  function library) live there, not here. See "The C/ dependency" below.

When changing behavior here (CLI flags, HTTP routes, JSON/SAV schema,
model-expression syntax), assume at least one sibling repo depends on it
and check before renaming or removing anything public.

## The C/ dependency (read this before "fixing" a missing directory)

A fresh clone of this repository has **no `C/` directory** - that's
expected, not broken. `./INSTALL` clones
[`onefite-c-code`](https://github.com/fitteia/onefite-c-code) to `../C`
(a sibling of this checkout) on first install and `git pull`s it on later
ones, then compiles it there. `bin/onefite`'s `c-code-root()` resolves, in
order: `OFE-PATH/../C`, `OFE-PATH/C`, then a dev-only `OFE-PATH/src/c-code`
fallback (only used by `onefite-go`'s `doctor --install` dev layout - never
reached on a real install).

Practical implications:

- Adding/editing a model function (`C/local/*.c`/`.h`, `UserLib.i`,
  `META-C.json`) means committing in the **`onefite-c-code` checkout**
  (`../C`), not in this repo. See `docs/extending-models.md`.
- `git log`/`git blame` in this repo won't show C-model history.
- Extra models can also be **extensions** (`../C/extensions/<name>`, built by
  `make extensions` in the `onefite-c-code` checkout, written up in its
  `extensions/README.md`): `INSTALL --extension=...` / `--/default-extensions`
  fetch and build them, and `etc/extensions.mk` + `../C/META-CATALOG.json` are
  the only two things this repo reads from them (`catalog-file()` in
  `bin/onefite`, the `-include` in `etc/OFE/default/makefile`). Extension
  logic lives in `onefite-c-code/tools/extensions.pl`, not here.
- `jq empty C/META-C.json` and similar checks run against the sibling
  checkout's path, resolved relative to wherever `OFE-PATH` points.

## Bootstrap checklist for a fresh session

1. `git status` here (and any sibling repo you're about to touch) before
   assuming a clean tree.
2. Confirm whether `../C` (the `onefite-c-code` sibling) is present and
   built before assuming compilation/fit tests can run - see "The C/
   dependency" above.
3. Run the quick sanity checks below (syntax/module-load/POD render)
   before a full install or test run, especially against an environment
   you haven't touched before.
4. Read "Do not commit without being asked" below - it applies from the
   start of the session, not just at the end.

## Setup / build / install

This is a Raku (`rakudo`) project distributed as a `zef`-installable
package (`META6.json`), but **`zef install` alone will not give you a
working `onefite`** - native compilation (MINUIT, the C core, SWIG
bindings) is deliberately done by `./INSTALL` before `zef` is invoked;
`Build.rakumod`'s `zef` build hook only records already-staged native files
into `MANIFEST.site`, it doesn't compile anything itself.

Real install path (see `INSTALL`, `docs/installation.md`,
`docs/platform/*.md` for full detail and side effects):

```bash
sudo ./pre-install-OFE-in-Linux.sh   # or pre-install-OFE-in-MacOS.sh
./INSTALL --no-test --no-post-test && onefite service start && ./post-install.sh
```

`./INSTALL` is itself a Raku script (`multi MAIN` options, run directly
with `raku`/as an executable) with real side effects: it can run
`git stash && git pull`, install OS packages via `sudo`, clone/update the
`onefite-c-code` sibling, compile native code, install a systemd unit, and
modify ImageMagick's `policy.xml`. Read
`docs/installation.md#important-side-effects` before running it against a
non-disposable environment. For iterative development, a narrower,
non-destructive install is:

```bash
./INSTALL --to-user --no-dpkg --no-web-server --no-enable-gs --no-git
```

Docker: `Dockerfile` plus `Dockerfiles/*.dockerfile` (debian, ubuntu,
fedora, centos, alma, arch, suse) give base images with the language
toolchain but *not* the full dependency set - run the platform pre-install
script inside the container before `./INSTALL`, same as bare metal (see
`docs/testing.md`'s multi-distro sandbox section and
`test-ofe-in-docker-sandbox.sh`).

Quick sanity checks without a full install:

```bash
raku -c bin/onefite                       # syntax check
raku -Ilib -e 'use OneFit; say "loaded"'  # module loads
raku --doc=Text bin/onefite               # render the embedded POD as text
raku --doc=Man bin/onefite                # render it as a man page
jq empty META6.json
```

## Development workflow

Full checklist lives in `docs/development.md`; the parts most likely to
bite an agent:

- **`bin/onefite` is the whole CLI and HTTP service in one file** (~2100+
  lines), dispatched through `multi MAIN(...)` signatures. A subcommand's
  help text, usage, and behavior all come from that one signature - `#=`
  trailing comments become `--help` text, and the embedded `=pod` block
  (rendered via `Pod::To::Man`/`Pod::To::Text`) is a **second, separate**
  place the same behavior must be kept in sync by hand. When you change a
  `multi MAIN` signature: update its `#=` comments, update the POD, update
  only the workflow guides actually affected, re-render the POD
  (`raku --doc=Text`/`--doc=Man bin/onefite`), and test the positive,
  negated-Boolean (`--no-...`), short-alias, and invalid-option paths.
- **HTTP routes are defined inline in `start-web-engine`'s
  `Cro::HTTP::Router` block** (same file, currently `GET /`, `GET|POST
  /list[/models|/aliases]`, `GET|POST /help/MODEL[/KEY]`, `GET|POST /man`,
  `POST /fit`, `POST /plot`, `POST /convert`). A route's real contract is
  its Raku code, not `docs/http-api.md` - the two can and have drifted
  (see "known documentation gaps" below, now fixed, but assume it can
  happen again). Update `http-api.md`, `service.md`, and `security.md`
  together when a route changes.
- **`/fit`, `/plot`, and `/convert` build the underlying `onefite`
  invocation as a fixed argument array** (Raku `run 'onefite', |@args`),
  not a shell string - this was a deliberate fix for a real
  command-injection bug (see `git log --oneline -S'Pass request values as
  literal process arguments'`). Never reintroduce `shell("onefite $txt")`
  style construction from request data. The `download` field on `/fit` is
  still an unsanitized relative path, though - don't assume path handling
  elsewhere in the file is safe by association.
- **`onefite archive`/`ar`/`log` replay is CLI-only and still shells out**
  with an interpolated stored command (`OneFit::Engine::Archive`'s
  `HistoryLog.fit`) - treat a `.zip` archive as an executable, same as
  documented in `docs/security.md`.
- Version and license source of truth: `META6.json` (currently reports the
  live version via `onefite --version`) and `LICENSE` (Artistic License
  2.0, explicitly called provisional in `README.md`). Don't hardcode a
  version number into prose documentation - it goes stale (this happened
  in `docs/README.md` before this pass; it now points at `META6.json`
  instead).

## Coding conventions actually observed

There isn't a formal style guide, and the codebase is not internally
consistent - match the surrounding code rather than importing a "clean"
convention that isn't actually used nearby:

- `lib/OneFit*.rakumod`: `unit module`/`unit class` declarations, 4-space
  indentation, fluent-ish methods that mutate private (`has %!x`/`has
  @!x`) state and `self`-return (e.g. `Engine.new.path(...).read(...)`).
- `bin/onefite`: tab-based indentation that is **not** consistently
  applied (mixed tab widths and occasional spaces even within one `multi
  MAIN` body) - don't "fix" whitespace as a drive-by change; it's noise
  against a large diff.
- kebab-case for multi-word identifiers almost everywhere (variables,
  subs, named parameters: `$fit-methods`, `$work-folder`,
  `create-web-request-directory`), including named CLI options, which is
  why flags are things like `--fit-methods`, `--work-folder`, `--autoxy`.
- CLI options are declared with Raku's multiple-name binding
  (`Str :wf(:$work-folder)`) purely to get short aliases; the long,
  kebab-case name is the canonical one used in docs and examples.
- Model/engine internals favor terse, single-letter or domain-abbreviated
  names (`$R1`, `$R2`, `$ssz`, `%!arch`) inherited from the original
  Fortran/C-era naming - don't rename these opportunistically, other repos
  and saved JSON/SAV schemas depend on the exact field names.

## Testing approach

```bash
onefite test list                              # list available .rakutest files
onefite test --no-systemd-daemon --no-post-install   # isolated unit-test run
onefite test 00-use 04-convert --no-post-install     # selected tests, run concurrently
prove6 --lib t                                 # repository test runner directly
```

- Tests are `.rakutest` files under `t/`, using Raku's `Test` module (see
  `t/00-use.rakutest` for the plain style, `t/06-fit.rakutest` for one that
  drives a real fit end to end with timing instrumentation).
- `onefite test`'s `--post-install` (on by default) always runs a built-in
  CLI fit, and additionally POSTs the same fit to `127.0.0.1:8142` if a
  systemd-managed or background service is detected - pass
  `--no-systemd-daemon --no-post-install` for a fast, side-effect-free run
  while iterating.
- Compilation/fit tests need the `onefite-c-code` sibling already cloned
  and built (see "The C/ dependency" above) plus Minuit; plotting needs
  Grace/PDF tools; MPEG4 needs Poppler/FFmpeg; HDF5 import needs HDF5
  tools; service tests need Cro modules and a reachable instance. A green
  run on one platform does not imply every optional feature path was
  exercised - see `docs/testing.md`'s feature-prerequisites table.
- `test-ofe-in-docker-sandbox.sh build|run|shell <distro>` gives a
  disposable multi-distro environment (`debian ubuntu fedora centos alma
  arch suse`, or `all`) - remember to run the platform pre-install script
  inside the container before `./INSTALL`; the Dockerfiles alone don't
  provide the full dependency set.
- Validate any embedded-POD change the same way the tool renders it:
  `raku -c bin/onefite`, `raku --doc=Text bin/onefite`,
  `raku --doc=Man bin/onefite`.

## Do not commit without being asked

Per this project's working norm, treat documentation and code edits as
uncommitted working-tree changes unless explicitly told to commit - the
user reviews diffs before they become history.
