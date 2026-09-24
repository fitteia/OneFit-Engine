# Extending the model library

OneFit-Engine includes local C models under `C/local`, and older/core
models under `C/core/onefit-3.1/modelos`. Keep new site-specific work
isolated and tested, on a dedicated branch, so `onefite upgrade`'s `git
pull` never conflicts with or discards it.

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
