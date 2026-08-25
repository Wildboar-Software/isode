# pepsy — Table-driven ASN.1 compiler

`pepsy` is the replacement for `pepy` and `posy`. It reads an ASN.1 presentation module and emits C type definitions plus encoding/decoding *tables* driven by `libpepsy`. Quipu's Directory ASN.1 (`dsap/x500as`) and SNMP (`snmp/snmp.py`) go through pepsy.

Volume 4 (`doc/volume4.pdf`) and `pepsy(1)` document the compiler. `libpepsy(3)` documents the runtime.

It is meant to be backwards-compatible with posy and **ignores pepy-style augmentations** in the input.

## Layout

| File / pattern | Role |
| --- | --- |
| `pepsy.c`, `pass2.c`, `main.c` | Compiler front/back end |
| `enc.c`, `dec.c`, `prnt.c`, `fre.c` | Table-driven encode / decode / print / free |
| `etabs.c`, `dtabs.c`, `ptabs.c` | Table interpretation |
| `UNIV.py` | Universal ASN.1 types |
| `t1.py` … `t3.py` | Compiler self-tests |
| `doc/` | Additional compiler notes |

`YACC = bison -y` and `LEX = lex`. Output for a module `FOO.py` is typically `FOO_tables.c` plus headers consumed as `FOO-types.h`.
