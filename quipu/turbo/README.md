# quipu/turbo — GDBM tools for large EDBs

Utilities used when Quipu is compiled with TURBO options (Volume 5 §12.2): store Entry Data Blocks in GDBM instead of (or in addition to) flat EDB text.

| Program | Role |
| --- | --- |
| `edb2dbm` | Load an EDB into a GDBM database |
| `edbcat` | Dump a GDBM EDB |
| `file2dn` | Map a filesystem path to a distinguished name |
| `tdirname` | Directory-name helper for TURBO trees |

Several Makefile targets (`tree2dbm`, `syncedb`, `synctree`) are stubs. Requires `libgdbm`. The DSA-side TURBO search/index code is `quipu/turbo_*.c` and `dsap/common/turbo_*.c`, not this directory.
