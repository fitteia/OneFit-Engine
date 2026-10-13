# Extending the model library

OneFit-Engine includes local C models under `C/local`, and older/core
models under `C/core/onefit-3.1/modelos`. Keep new site-specific work
isolated and tested, on a dedicated branch, so `onefite upgrade`'s `git
pull` never conflicts with or discards it.

This page covers your own site-specific models. For optional bundles such as
Florence, use the separate [extensions
mechanism](installation.md#extensions) instead. The public
Florence bundle is NAG-free; a separate private NAG-derived repository is
available only to users with the relevant NAG license.

**Prefer an extension when you can.** A model that lives in its own repository
as an *extension* needs no site branch and never touches `META-C.json`: it is
cloned to `../C/extensions/<name>`, built by `make extensions` and added to the
model catalog automatically, and it survives `onefite upgrade`. See
[Extensions](installation.md#extensions) to install one and
`extensions/README.md` in `onefite-c-code` to write one. The site-branch
workflow below is for changing the base library itself.

## Create a site branch

From the repository root:

```bash
git switch -c site
git config user.email "your email" && git config user.name "your name"
```

Commit only intentional model, metadata, and test changes. Merge upstream
into this branch rather than editing a production checkout without
history.

## Add a model

Use an existing `C/local/NAME.c`/`NAME.h` pair as a structural reference. A
typical addition requires:

1. A C implementation and header under `C/local`.
2. A declaration in `C/local/userlib.h`.
3. A declaration in `C/local/UserLib.i` - **in both of the two places that
   file declares functions**: once inside its `%inline %{ ... %}` block,
   and again as a bare `extern` statement below it. Both copies must match;
   compare against an existing entry rather than writing them from memory.
4. Metadata in `C/META-C.json` (drives `onefite list models` and `onefite
   help MODEL [KEY]` - provide accurate parameter meaning, units,
   references, and constraints, and keep the JSON valid):

   ```bash
   jq empty C/META-C.json
   ```

5. A test fixture and automated test for compilation and numerical
   behavior.

```bash
git checkout site
# ... make your edits ...
git commit -a -m "add YOURMODEL"
```

## Build and test

```bash
./INSTALL --no-git --no-dpkg --no-web-server --no-post-test
```

Then verify discovery and a numerical fixture:

```bash
onefite help YOURMODEL
onefite test --no-systemd-daemon --no-post-install
```

Don't accept a clean compile alone as validation - test expected values,
parameter bounds, fitting convergence, and architecture/compiler
sensitivity.

## Rebuilding with upstream changes merged in

Either merge by hand and reinstall:

```bash
git checkout main && git pull && git merge -m "merge site" site
onefite upgrade -d --test
```

or let `onefite upgrade` do the merge for you in one step (still worth a
manual `git diff`/test pass afterward for anything non-trivial):

```bash
onefite upgrade --systemd-daemon --merge-site=site
```

Resolve conflicts in the registration and metadata files carefully,
rebuild, and rerun tests.

## Auxiliary code alternative

For a fit-specific helper that doesn't belong in the shared library, use
`--aux-code` on `create`/`fit` instead of adding a permanent model. This
still compiles and executes native code and must be treated as trusted
input - see [security](security.md).

## Raw parameter arrays in auxiliary C code

Named parameters in the model expression always get the current data block's
value. Auxiliary C code that reads the raw `par` array does not: in a
`--hybrid` fit, a parameter whose name ends in `_` has its own copy for each
block, and `par[k]` always reads block 1's. Read it with `HPAR(k)` instead:

```c
D = HPAR(18);   /* not par[18] */
```

`HPAR(k)` is `par[k]` for the current block - the block's own copy of a
hybrid parameter, `par[k]` itself for a shared parameter or outside
`--hybrid`. `k` is the parameter's index in `par` (its number in a parameter
file, minus one). It needs `par` and the data-set variable (for example `N`)
as arguments of the function, as in `double f(double x, double N, double *par)`:
the block number is `N_0`.

Both Raku and Go generate it in `ndata.h`, included by `AuxCode.c`, with two
counts:

```c
#define OFE_USER_NPARS 21
#define OFE_HYBRID_NPARS 1
```

`OFE_USER_NPARS` counts the user's parameters, excluding the engine's `par[0]`
bookkeeping entry and the appended hybrid copies. `OFE_HYBRID_NPARS` counts the
parameters whose names end in `_` while `--hybrid` is active (per block, not in
total); it is zero outside hybrid mode. Block `b`'s copy of the `j`-th such
parameter (counting from 0, in declaration order) is
`par[OFE_USER_NPARS + 1 + (b - 2) * OFE_HYBRID_NPARS + j]` for `b > 1`; block 1
uses the original `par[k]`. `HPAR` does this lookup. Engines older than these
definitions do not provide them.
