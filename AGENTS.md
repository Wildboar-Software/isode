# ISODE / Quipu

Historic OSI stack plus the Quipu X.500 Directory System Agent. See [README.md](README.md) for the standard Linux build and Docker run instructions.

## Cursor Cloud specific instructions

Use the repo wrapper `./make`, not bare `make`. It always loads `config/CONFIG.make`.

Development VMs use `config/linux-dev.make` (stricter `-Werror=…` flags and GCC SARIF on stderr), not `config/linux.make`. Those copies, `h/config.h`, and `support/*.local` are gitignored. The VM update script re-applies:

```shell
cp config/linux.h h/config.h
cp config/linux-dev.make config/CONFIG.make
cp config/*.local support/
```

Only `./make everything` is a reliable compile. Subdirectory builds usually fail because earlier libraries and generated headers are missing. `YACC = bison -y` and `LEX = lex` (`flex` provides `/usr/bin/lex`). After changing `CONFIG.make`, run `./make clean` before rebuilding so objects compiled under `linux.make` are not reused.

`linux-dev.make` uses `CC = cc` and GCC-only `-fdiagnostics-format=sarif-stderr`. This image’s `/usr/bin/cc` is Clang unless switched; the snapshot sets `update-alternatives --set cc /usr/bin/gcc` (and `c++` to `g++`). If a rebuild dies immediately with `invalid value 'sarif-stderr'`, `cc` is Clang again.

As of this environment, `./make everything` with `linux-dev.make` does **not** complete: GCC `-Werror=sign-conversion` fails hundreds of historic conversions (for example `compat/logger.c`). `config/linux.make` is the fragment that currently produces a full tree. Keep `CONFIG.make` as `linux-dev.make` for development; copy `linux.make` over it only when you need a green `everything` build, then copy `linux-dev.make` back.

`./make inst-everything` installs under `/usr/local/{bin,sbin,etc/isode,lib,include/isode}`. It can exit non-zero at the end because historic Unix `lint1` is not installed; the Quipu/DISH/tsapd binaries still install. `./make lint` / `lint-everything` is not usable here for the same reason.

### What to run

Quipu is the shipped product. It listens on TCP 17003 (ITOT) itself and does **not** go through `tsapd`. Persistence is EDB files, not a SQL database. The DSA entry in the EDB must match `mydsaname` in `quiputailor`.

| Goal | Command |
| --- | --- |
| Quipu + DISH smoke test (no install) | `./quipu-e2e.sh` (DSA on `QUIPU_E2E_PORT`, default 21703) |
| FTAM smoke test (no install) | `./ftam-e2e.sh` (needs `bwrap`; tsapd on `FTAM_E2E_PORT`, default 21704) |
| Installed DSA | `/usr/local/sbin/ros.quipu -t /usr/local/etc/isode/quiputailor` |
| DISH (DAP client) | `quipu/dish/xdish` or `/usr/local/bin/dish` |
| ASN.1 encodings | `pepy/pepytest < pepy/test/test1.pe` |
| Classic OSI stack test | install, start `tsapd`, then `./make test` (`isode-test.sh`) |

`cd pepy && ./make tests` is broader than `pepytest` and currently fails compiling extra helpers such as `mpp`. `tsapd` (TCP 102) is only needed for FTAM, VT, IMISC, `ftam-e2e.sh`, and `isode-test.sh`.

`*.py` under `pepy/`, `vt/`, and `dsap/` are PEPY ASN.1 compiler inputs, not Python.
