# pepy — ASN.1 encoder/decoder compiler (yacc-based)

`pepy` reads a presentation module (a `.py` file written in ASN.1 plus ISODE
augmentations) and emits C that builds or recognizes the corresponding APDUs.
`posy` in this same directory is the older structure-generator for ASN.1 → C
structs.

Volume 4 (`doc/volume4.pdf`) is the reference.
**PEPSY (`pepsy/`) is the intended replacement** for both `pepy` and `posy`; the
root README still lists removing pepy/posy as future work. `vt/` and several
programs under `others/` still compile through pepy.

`.py` files in this tree are PEPY compiler inputs, **not Python**.

## Programs

| Binary | Role |
| --- | --- |
| `pepy` | Encoder/decoder/pretty-printer generator |
| `posy` | C structure generator (historic) |
| `pepytest` | Small encoding smoke test (`pepytest < pepy/test/test1.pe`) |

`YACC = bison -y` and `LEX = lex` (flex provides `/usr/bin/lex`). Generated
parsers live alongside `yacc.y.gnrc` and `lex.l.gnrc`.

`cd pepy && ./make tests` is broader than `pepytest` and currently fails
compiling extra helpers such as `mpp`.

## Conventions

- Generated encode/decode routines call `advise` / `PY_advise` on error.
- External modules are found via `$PEPYPATH` (colon-separated) looking for
  `MODULE.ph`.
- `PEPYPATH=-DPEPYPATH` in Makefiles is a compile flag, not the search path.
