# paradise/dsap/common — `filter.c` overlay

PARADISE replacement for `dsap/common/filter.c`. The package tarball
overwrote that file onto ISODE 8.0 (`isode-paradise.pdf`). This tree
keeps both copies; `./make` still compiles `dsap/common/filter.c`, not
this overlay.

The files are not identical. The only behavioural change is a bugfix in
`strfilter` (how a substring Filter item is built from a string with no
`*`). It is not Directory search relaxation, extra filter types, or a
size limit.

## What `strfilter` does

`strfilter(at, s, type)` is a `libdsap` helper used by User-Friendly
Naming (UFN; [IETF RFC 1484](https://www.rfc-editor.org/rfc/rfc1484.html)).
The callers are `dsap/common/ufn_parse.c` and PARADISE DE’s copy
`ufnp.c`:

```c
#define SUBSTRINGS() \
    ((ufn_flags & UFN_WILDHEAD) ? FILTERITEM_SUBSTRINGS \
                                : -FILTERITEM_SUBSTRINGS)
```

Default `ufn_flags` is `UFN_ALL`, which includes `UFN_WILDHEAD`. A
leading minus on the type is a local convention, not an X.500 filter
item: both values still produce `FILTERITEM_SUBSTRINGS` on the wire.
`UFN_WILDHEAD` means “may match inside the value” (`*s*`); without it
the string is an initial substring (`s*`).

Strings that already contain `*` are parsed the same in both copies
(initial / any / final pieces). The difference is the no-wildcard case.

## Stock ISODE 8.0 (dangling `else`)

Without `*`, `FILTERITEM_SUBSTRINGS` stored the same value in both
`fi_sub_any` *and* `fi_sub_initial` (`s*s*` — duplicate components).
`-FILTERITEM_SUBSTRINGS` stored neither (empty substring item). The
overlay’s original RCS log (later stripped) was:

```text
Revision 9.1  1992/06/24  fix UFN filters for duplicates
```

With the default `UFN_WILDHEAD`, that duplicate filter is too tight: the
value must start with `s` and then contain `s` again, so ordinary names
fail.

## PARADISE braces

The overlay adds braces so the two types stay distinct:

- `FILTERITEM_SUBSTRINGS` (`UFN_WILDHEAD`): any-substring only (`*s*`)
- `-FILTERITEM_SUBSTRINGS`: initial-substring only (`s*`)

That is the intended UFN wildcard-head vs initial-substring split, not
a new matching rule.

## Not search relaxation, not DE/IDM filters

This tree has no “relax” matching-rule control. Approximate match
(`FILTERITEM_APPROX`, Soundex in `dsap/common/soundex.c`) was already
in stock ISODE; UFN still ORs those items in when `UFN_APPROX` is set.
This overlay does not add that.

DE and IDM prompted searches build their own filters in
`paradise/others/quipu/uips/{de,idm}/filter.c` (`eqfilter`,
`subsfilter`), including the later `FILTERITEM_APPROX` pass when
`fuzzyMatching` is on. Those files are independent of this overlay.
PARADISE DE also vendors the same `strfilter` braces as `fixedfilt.c`
(“patch for ufn generated filters”) because that DUA links its own UFN
copy (`ufnp.c`). IDM does not call `strfilter`.

Other differences versus today’s `dsap/common/filter.c` are later
modernization (headers, prototypes), not extra filter behaviour.
