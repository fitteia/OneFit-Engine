# Service operation

OneFit-Engine includes a small Cro HTTP application. It's intended for
trusted/local integration, not direct public exposure.

> [!WARNING]
> The service has no authentication or authorization, and it compiles and
> runs generated native model code from client input. Read
> [security](security.md) before starting it.

## Foreground operation

```bash
onefite start-web-engine --ip=127.0.0.1 --port=8142
```

Foreground mode is easiest to supervise and debug. When `--ip` is omitted,
the implementation tries to detect an IPv4 address and falls back to
`127.0.0.1`; pass the address explicitly in anything other than casual
local use.

Stop foreground operation with `Ctrl-C`.

## Background operation

```bash
onefite service start --ip=127.0.0.1 --port=8142
```

Without `-d`/`--systemd-daemon`, this command kills any existing process
matching `start-web-engine` and launches a new one with `nohup`, from
`$HOME/public_html`.

```bash
onefite service PID
onefite service log
onefite service stop
```

PID discovery scans `ps` output rather than using a dedicated PID file.
Don't run multiple independent instances under the same user without a
separate supervisor - a second `service start` will kill the first.

## Systemd operation

Install the unit first:

```bash
./INSTALL --systemd-daemon --ip=127.0.0.1 --port=8142
```

Then manage it through OneFit-Engine or systemd directly:

```bash
onefite service start --systemd-daemon --ip=127.0.0.1 --port=8142
sudo systemctl status onefite
sudo systemctl restart onefite
```

The CLI start command reloads systemd, enables the unit, and starts it.

## Logs

```bash
onefite path --log
onefite service log
```

The Cro application appends the peer address, route activity, upload name,
and elapsed time to the service log. Apply normal retention and permission
rules to it - request metadata may be sensitive.

## RAM-backed service work

```bash
onefite service start --use-ramdisk
```

This backs up `$HOME/public_html`, creates a RAM disk, replaces
`public_html` with a symlink to it, and copies the backup into the
RAM-backed storage. Check available memory and persistence expectations
first - a RAM disk's contents don't survive a reboot.

## Network deployment

The default service port is 8142. Prefer loopback (`--ip=127.0.0.1`), and
place any remote access behind a hardened reverse proxy providing TLS,
authentication, request limits, timeouts, and logging - the application
itself provides none of these. See [security](security.md) before doing
this at all.

Don't rely on the form's `username` field for identity - it only selects an
existing directory relative to the service's working directory; it is not
a login.

## Health checking

A minimal local check:

```bash
curl --fail --silent http://127.0.0.1:8142/list/models >/dev/null
```

This confirms routing and model-metadata access, not the compiler or
fitting pipeline. A full health check needs a small trusted fit (see
[HTTP API](http-api.md)) and appropriate resource limits.

See [HTTP API](http-api.md) for routes, and
[troubleshooting](troubleshooting.md) for startup failures.
