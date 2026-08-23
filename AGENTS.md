# ISODE / Quipu

Historic OSI stack plus the Quipu X.500 Directory System Agent. See [README.md](README.md) for the standard Linux build and Docker run instructions.

## Cursor Cloud specific instructions

Use the repo wrapper `./make`, not bare `make`. It always loads `config/CONFIG.make`.

There is no `config/linux-dev.make`. Linux builds use `config/linux.make` (copied to `config/CONFIG.make`). Those copies, `h/config.h`, and `support/*.local` are gitignored. The VM update script re-applies:

```shell
cp config/linux.h h/config.h
cp config/linux.make config/CONFIG.make
cp config/*.local support/
```

Only `./make everything` is a reliable compile. Subdirectory builds usually fail because earlier libraries and generated headers are missing. `config/linux.make` sets `YACC = bison -y` and `LEX = lex` (`flex` provides `/usr/bin/lex`).

`./make inst-everything` installs under `/usr/local/{bin,sbin,etc/isode,lib,include/isode}`. It can exit non-zero at the end because historic Unix `lint1` is not installed; the Quipu/DISH/tsapd binaries still install. `./make lint` / `lint-everything` is not usable here for the same reason.

### What to run

Quipu is the shipped product. It listens on TCP 17003 (ITOT) itself and does **not** go through `tsapd`. Persistence is EDB files, not a SQL database. The DSA entry in the EDB must match `mydsaname` in `quiputailor`.

| Goal | Command |
| --- | --- |
| Quipu + DISH smoke test (no install) | `./quipu-e2e.sh` (DSA on `QUIPU_E2E_PORT`, default 21703) |
| Installed DSA | `/usr/local/sbin/ros.quipu -t /usr/local/etc/isode/quiputailor` |
| DISH (DAP client) | `quipu/dish/xdish` or `/usr/local/bin/dish` |
| ASN.1 encodings | `pepy/pepytest < pepy/test/test1.pe` |
| Classic OSI stack test | install, start `tsapd`, then `./make test` (`isode-test.sh`) |

`cd pepy && ./make tests` is broader than `pepytest` and currently fails compiling extra helpers such as `mpp`. `tsapd` (TCP 102) is only needed for FTAM, VT, IMISC, and `isode-test.sh`.

`*.py` under `pepy/`, `vt/`, and `dsap/` are PEPY ASN.1 compiler inputs, not Python.
