# others/quipu/uips/doog/query — Doog query engine

Asynchronous Directory query library (`libQuery.a`) used by `doog`. The
parent `doog/` directory is the interactive shell around it; this
directory is the interesting `libdsap` async example. See the historic
`README` here for the original build notes.

## What UFN is

X.500 identifies every entry by a **distinguished name** (DN): a
sequence of typed attributes such as
`c=GB@o=Brunel University@ou=…@cn=…`. That is awkward to type.
**UFN** (User-Friendly Naming) is Steve Kille's scheme for the
opposite: people type an approximate, human name, and the Directory is
searched until a DN is found.

The paper is *Using the OSI Directory to achieve User Friendly Naming*
(`doc/READ-ME` lists it under `ufn/`; Volume 5 §17.3.1 describes the
`libdsap` routines and cites it as SKill90). A UFN is typically a
comma-separated string of name parts, written like a postal address —
person first, country last. Doog's own help gives:

```text
damanjit, manufacturing, brunel, gb
```

Volume 5 also mentions a tiny example `ufn` interface that takes a UFN
and prints a DN. That demo, and the paper itself, are not in this tree.

## How this engine uses it

`ufname.c` implements UFN resolution **asynchronously** on `libdsap`.
It does not call `ufn_match()` in `dsap/common/ufn_parse.c` (that
synchronous resolver is what DISH/FRED use). `str2ufname()` splits the
typed string on commas (quotes allowed). `do_ufn_resolve()` then walks
the parts, issuing exact and approximate DAP searches as they complete
(`directory_wait`). A part that contains `=` is treated as a typed
attribute comparison.

How many parts you typed selects a **search path** of base objects
(local organisation, country, root, …), configured in `doogrc` as
`searchpath` — the same idea as the system `ufnrc` file used by
DISH/FRED. Matches are classified good vs poor; poor matches can be
handed back so the user can pick which ones to follow
(`continue_ufn_search` in Doog's `interact.c`).

`ufsearch.c` is a second, form-oriented “user friendly search”:
attribute type/value pairs rather than a comma-separated UFN. Doog's
shell does not call it. The historic `README` says much of the code
here is not required by Doog, and that the intended `libQuery` would
offer both UFN and form-filled searching to other DUA writers (it also
names XLookUp, an X DUA not present in this tree). Bind, read, and
modify live here too; the library is still marked development /
pre-beta.

## Doog vs DISH and DE

| Program | Relationship to UFN |
| --- | --- |
| Doog | Dedicated UFN DUA. Type a friendly name at `:-` (or `find …`). Pre-beta; async `libdsap`. |
| This `query/` engine | Async UFN (and unused form-search) library behind Doog. |
| DISH | Expert DAP shell (`search`, `moveto`, …). Optional `fred -ufn` uses **synchronous** `libdsap` `ufn_match`. |
| FRED | Whois White Pages; `ufnrc` + `ufn_match`. |
| DE | Kiosk interview (person / dept / org / country). Not a UFN client in this copy; PARADISE DE can call `ufn_match`. DE hides names behind prompts; Doog asks for a UFN. |

See the parent [`doog/README.md`](../README.md) and
[`others/quipu/uips/de/README.md`](../../de/README.md).
