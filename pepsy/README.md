# pepsy — Table-driven ASN.1 compiler

`pepsy` is the replacement for `pepy` and `posy`. It reads an ASN.1 presentation
module and emits C type definitions plus encoding/decoding *tables* driven by
`libpepsy`. Quipu's Directory ASN.1 (`dsap/x500as`) and SNMP (`snmp/snmp.py`) go
through pepsy.

Volume 4 (`doc/volume4.pdf`) and `pepsy(1)` document the compiler. `libpepsy(3)`
documents the runtime.

It is meant to be backwards-compatible with posy and
**ignores pepy-style augmentations**[^1] in the input.

## Layout

| File / pattern | Role |
| --- | --- |
| `pepsy.c`, `pass2.c`, `main.c` | Compiler front/back end |
| `enc.c`, `dec.c`, `prnt.c`, `fre.c` | Table-driven encode / decode / print / free |
| `etabs.c`, `dtabs.c`, `ptabs.c` | Table interpretation |
| `UNIV.py` | Universal ASN.1 types |
| `t1.py` … `t3.py` | Compiler self-tests |
| `doc/` | Additional compiler notes |

`YACC = bison -y` and `LEX = lex`. Output for a module `FOO.py` is typically
`FOO_tables.c` plus headers consumed as `FOO-types.h`.

[^1]: ISODE extensions to ASN.1 in a `.py` presentation module that `pepy`
    weaves into generated C. They include `%{ ... %}` action blocks
    (verbatim C at the start and end of the module and on types),
    type/value handlers such as `[[i expr]]`, `[[s expr]]`, `[[p expr]]`,
    and `[[P type]]`, `SET OF`/`SEQUENCE OF` controls `<<expr>>`, and
    `ENCODER`/`DECODER`/`PRINTER`/`SECTIONS` directives. Volume 4
    Chapter 6 and `pepy(1)` document them; `posy(1)` keeps only the
    module-level verbatim actions, and `pepsy` ignores them all.
