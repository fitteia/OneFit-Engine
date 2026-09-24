# Upgrading

## Before upgrading

1. Commit or back up local source changes.
2. Save local `aliases.json` and site-model changes (see
   [extending models](extending-models.md)).
3. Record the installed version and paths.
4. Stop active fitting jobs, and decide how the service will be restarted.
5. Back up scientific results that exist only on RAM-backed storage.

```bash
onefite --version
onefite path
git status --short --branch
```

## CLI upgrade

```bash
onefite upgrade
```

The current command can stop the service, run `git stash && git pull`,
invoke `./INSTALL`, and restart the service. Compilation, Git update, and
Debian package handling are all enabled by default.

Useful controls:

```bash
onefite upgrade --no-git --no-dpkg
onefite upgrade --systemd-daemon --test
onefite upgrade --merge-site=site
onefite upgrade --minuit=250
```

Use `onefite --help`/`onefite man` for the current signature.

## Manual upgrade

Manual control is preferable when local changes or a production service's
state need careful handling:

```bash
onefite service stop
git status --short --branch
git pull --ff-only
./INSTALL --no-git
onefite test --no-systemd-daemon --no-post-install
onefite service start --ip=127.0.0.1
```

Choose installer flags appropriate to the original installation - this
example does not preserve every systemd, platform, or user-space setting.

## Site branch

Keep custom models on a dedicated branch, and merge upstream changes into
it deliberately rather than editing a production checkout in place.
`--merge-site=BRANCH` asks the installer/upgrade path to merge a local
branch, but it is not a substitute for reviewing conflicts and running
model tests yourself. See [extending models](extending-models.md).

## Verify after upgrade

```bash
onefite --version
onefite list models >/dev/null
onefite test --no-systemd-daemon --no-post-install
```

Then validate one representative scientific fit, plot generation, format
conversion, and - if used - the service/API.

## Recovery

If an upgrade fails, preserve logs and the worktree state. Check the
stash list, since `git stash && git pull` may have created one:

```bash
git status
git stash list
```

**Don't blindly apply or drop stashes** - inspect each with `git stash show
-p stash@{N}` first; running `./INSTALL`/`upgrade` more than once without
resolving an earlier stash keeps adding new ones on top, and it's easy to
lose track of which one holds real work. Restore the previously-tested
commit and installation flags, rebuild, and validate. Keep versioned fit
descriptions so results can be reproduced across engine changes.
