# quipu — X.500 Directory System Agent

The shipped product: an OSI Directory System Agent (DSA). Persistence is
**EDB files**, not a SQL database. The installed binary is `ros.quipu`. DISH,
the DAP client, lives in `dish/`.

Volume 5 (`doc/volume5.pdf`) is the full user, administrator, and programmer
guide. `quipu.8c` is the short man page.

Quipu listens on **TCP 17003** (ITOT) itself and does **not** go through
`tsapd`. The DSA entry in the EDB must match `mydsaname` in `quiputailor`.

Smoke test without installing: `./quipu-e2e.sh` (DSA on `QUIPU_E2E_PORT`,
default 21703).

## Subdirectories

| Directory | Role |
| --- | --- |
| [dish/](dish/README.md) | DISH DAP client and `libdish` |
| [turbo/](turbo/README.md) | GDBM helpers for large EDBs (`edb2dbm`, `edbcat`, …) |

## Code map

| Area | Files |
| --- | --- |
| Process & tailor | `dsa.c`, `sys_init.c`, `tai_*.c`, `control.c` |
| Connections | `conn*.c`, `net_init.c` |
| DAP operations | `ds_read.c`, `ds_search.c`, `ds_list.c`, `ds_add.c`, `ds_modify*.c`, `ds_compare.c`, `ds_remove.c`, `ds_abandon.c`, `ds_bind.c` |
| Chaining / DSP | `dsa_chain.c`, `referral.c`, `oper_*.c`, `task_*.c` |
| Database | `entry_load.c`, `entry_dump.c`, `schema.c`, `parse2.c`, `shadow.c`, `update.c` |
| Access control | `checkacl.c`, `acl_info.c`, `eis_select.c` |
| TURBO search | `turbo_disk.c`, `turbo_search.c`, `turbo_debug.c` |
| Allocator | `malloc.c` — Quipu's optional fast allocator (disable via the Makefile `MALLOC_*` variables) |

## Configuration

- `quiputailor` — DSA name, database directory, logging, replication, caching
- EDB tree — typically under `others/quipu/quipu-db/` as examples; production
  data is installed beside the tailor file

GDBM is required for TURBO features (`libgdbm-dev` on Debian/Ubuntu).
