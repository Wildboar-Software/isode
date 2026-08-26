# paradise — COSINE PARADISE DSA/DUA overlay

PARADISE was the early-1990s European X.500 pilot (COSINE). This directory is
**not a second copy of ISODE**. It is a small overlay that was applied on top of
ISODE 8.0: a few Quipu/DSAP/tsapd patches, plus the DE and IDM user interfaces.

The package PDF is `isode-paradise.pdf`. Install instructions are
`paradise-gen.8` (also formatted as `doc/COSINE`). Top-level Make targets
`paradise`, `paradise-dsa`, and `paradise-dua` build this subset.

The operational core was a central DSA and the DE public-access DUA. This second
release also includes IDM (Interactive Directory Manager).

## Subdirectories

| Directory | What the overlay replaces or adds |
| --- | --- |
| [dsap/](dsap/README.md) | Patched `filter.c` and `dapbind.c` |
| [quipu/](quipu/README.md) | Patched DSA files (`entry_load.c`, `ds_read.c`, …) |
| [support/](support/README.md) | Patched `tsapd.c` |
| [util/](util/README.md) | Patched `make-lib.sh` |
| [others/](others/README.md) | DE and IDM DUAs |

`Makefile` here is a snapshot of the ISODE top-level Makefile from the PARADISE
release; the live build rules are the root `Makefile`.
