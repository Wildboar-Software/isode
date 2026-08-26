# patches — Historic ISODE 8.0 overlay

`isode-8.0/` holds the Linux and compiler-generator patches that were applied to
stock ISODE 8.0: `config/linux.h`, `config/linux.make`, regenerated `lex.l` /
`yacc` bits for `pepy`, `pepsy`, and `rosy`, plus small Makefile and IMISC ASN.1
fixes.

This is archive material for understanding how the tree was ported, not a live
patch series. Current Linux configuration lives in `config/`.
