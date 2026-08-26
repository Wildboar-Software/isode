# paradise/dsap — PARADISE patches to libdsap

Two replacement sources that were overlaid onto ISODE 8.0 `dsap/`:

| Path | Role |
| --- | --- |
| [common/filter.c](common/README.md) | UFN `strfilter` substring-construction bugfix |
| [net/dapbind.c](net/README.md) | Same DAP bind as stock; OID/error-copy cleanup |

Compare with `dsap/common/filter.c` and `dsap/net/dapbind.c` in the main
tree. This tree does not copy the overlay over those files at build
time.
