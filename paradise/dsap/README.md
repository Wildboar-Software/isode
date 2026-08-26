# paradise/dsap — PARADISE patches to libdsap

Two replacement sources that were overlaid onto ISODE 8.0 `dsap/`:

| Path | Role |
| --- | --- |
| [common/filter.c](common/README.md) | Filter handling used by DE/IDM searches |
| [net/dapbind.c](net/README.md) | Same DAP bind as stock; OID/error-copy cleanup |

Compare with `dsap/common/filter.c` and `dsap/net/dapbind.c` in the main tree to
see the overlay diffs.
