# config — Per-platform build skeletons

This directory is not a library. It holds the Makefile fragment and C header that describe the host OS, compiler flags, and install paths. The generation procedure is `isode-gen(8)` (`doc/isode-gen.8.md`).

Before building, the usual Linux setup is:

```shell
cp config/linux.h h/config.h
cp config/linux.make config/CONFIG.make
cp config/*.local support/
```

`CONFIG.make` is what every subdirectory Makefile includes (via the `./make` wrapper). `h/config.h` is what C sources include.

## Files

| Pattern | Role |
| --- | --- |
| `*.h` | C `#define`s for the OS and network stacks (`linux.h`, `sunos4.h`, `sys54.h`, …) |
| `*.make` | Make variables: `CC`, `OPTIONS`, install dirs, `YACC`, `LEX` |
| `linux-dev.make` | Development fragment: stricter `-Werror=…` and GCC SARIF diagnostics |
| `*.local` | Site overlays copied into `support/` (`aliases.local`, `services.local`, `entities.local`, `macros.local`, `objects.local`) |
| `OPTIONS` | Catalogue of compile-time `#define`s (`BSD42`, `TCP`, `X25`, `TP4`, …) |

## Conventions

- All install directories in the `.make` file must be absolute and end with `/`.
- After changing `CONFIG.make`, run `./make clean` before rebuilding so objects compiled under a different fragment are not reused.
- `linux.make` is the fragment that currently produces a full `everything` tree. `linux-dev.make` is stricter and does not complete under `-Werror=sign-conversion`.
