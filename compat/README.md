# compat — Portability and common utilities

`libicompat` is the lowest-level ISODE library. It papers over BSD vs System V differences and holds shared helpers used by every other layer: logging, ISO addressing, the tailor file, host/service lookup, and small string/number utilities.

Volume 2 (`doc/volume2.pdf`) covers the tailor file, logging, and addressing. The man page is `libicompat.3`.

## Layout

| Area | Typical files | Role |
| --- | --- | --- |
| Logging | `logger.c`, `log_tai.c` | `LLog` facility used throughout the stack |
| Tailoring | `isotailor.c` | Runtime `isotailor` configuration |
| Addresses | `isoaddrs.c`, `na2norm.c`, `norm2na.c`, `pa2str.c` | Network / presentation address conversion |
| Hosts & services | `isohost.c`, `isoservent.c`, `isofiles.c` | ISO host, service, and file lookup |
| Networking | `internet.c`, `dgram.c`, `*x25.c` | TCP, datagram, and historic X.25 glue |
| Small helpers | `explode.c`, `implode.c`, `lexequ.c`, `cmd_srch.c` | String splitting, case-blind compare, table search |

## Quirks

- Historic conversions in this directory (for example `logger.c`) are a common source of `-Werror=sign-conversion` failures under `config/linux-dev.make`.
- Network backends are selected by compile-time `#define`s in `h/config.h` (`TCP`, `X25`, `TP4`, and so on). See `config/OPTIONS`.
