# dsap/x500as — X.500 ASN.1 modules

PEPSY inputs for the Directory abstract syntaxes. Each `.py` file is an ASN.1
module (not Python) that `pepsy` compiles into `*_tables.c` and `*-types.h`.

| Module | Typical origin |
| --- | --- |
| `if.py` | Information Framework (X.501) |
| `af.py` | Authentication Framework (X.509) |
| `das.py` | Directory Abstract Service (X.511) |
| `do.py` | Directory Operational Binding / objects |
| `qu.py` | Quipu-specific types |
| `sa.py` | Selected attribute types |
| `th.py` | THORN extras |
| `nrs.py` | NRS information |

`asn_ext.c` is hand-written support the generated tables call. `*-cdefs.h` are
companion C definitions. Adding a protocol type usually means editing the
relevant `.py` and rebuilding this directory so `libx500as` (folded into
`libdsap`) picks up new tables.
