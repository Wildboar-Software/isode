# ronot — RO-BIND and RO-UNBIND on ACSE

`libronot` maps the Remote Operations Notation BIND and UNBIND macros onto
ACSE (A-ASSOCIATE / A-RELEASE). The code was pulled out of the Directory
library so Quipu and other application-service elements (ASEs) could
establish and release a ROS association through `RoBind` / `RoUnBind`
instead of calling ACSE A-ASSOCIATE / A-RELEASE themselves
(`AcAsynAssocRequest`, `AcRelRequest`, and the rest of `acsap`).

See the original design notes in `READ-ME`. BIND/UNBIND onto *RTSE* are not
provided.

## Remote Operations Notation

Remote Operations Notation (RO-notation) is the ASN.1 MACRO language used
to specify ROSE applications: a bind-operation to open an association, a
set of operations and errors to invoke on it, and an unbind-operation to
close it. ISODE takes that notation as a `.ry` module and compiles it with
`rosy` (`rosy/`) into C stubs and dispatch tables. At run time `librosy`
issues the operations over ROSE (`rosap/`).

Volume 4 (`doc/volume4.pdf`) is the compiler and cookbook. The in-tree
examples — `imisc/imisc.ry`, `others/lookup/lookup.ry` — use OPERATION and
ERROR only. `rosy(1)` records that BIND and UNBIND are unrecognized;
`ronot/` supplies the bind/unbind mapping in C instead.

`.ry` is rosy input. `.py` files here (`ronot.py`, `ry.py`) are PEPY ASN.1
modules, not Python.

## BIND and UNBIND macros

[X.219](https://www.itu.int/rec/T-REC-X.219/) §9 defines BIND as the
operation that establishes an application-association and UNBIND as the
one that releases it. Optional ARGUMENT, RESULT, and BIND-ERROR /
UNBIND-ERROR clauses describe user data carried on association setup and
release. X.219 §11 maps BIND onto ACSE A-ASSOCIATE (or RTSE RT-OPEN) and
UNBIND onto A-RELEASE (or RT-CLOSE).

No `.ry` file in this tree uses those macros. Annex B of X.219 shows the
form:

```
BindExample1 ::= BIND
	ARGUMENT BindArgumentType1
	RESULT BindResultType1
	BIND-ERROR BindErrorType1

UnbindExample3 ::= UNBIND
```

`ronot.py` encodes the corresponding values with the tags X.219 assigns
(`[16]` bind-argument through `[21]` unbind-error):

```
BindArgumentValue ::=
	[16] ANY
```

## What this library does

Typical entry points: `RoAsynBindRequest`, `RoBindInit`, `RoBindResult`,
`RoBindError`, `RoUnBindRequest`, and the retry variants. They wrap bind
arguments as presentation elements, call `acsap`, and return
`RoNOTindication`s rather than raw `RoSAPindication`s.

There is no `ronot(3)` man page in this tree. The C API is `h/ronot.h`
and `llib-lronot`; Volume 1 documents the ACSE calls underneath.

The long-term intent recorded in `READ-ME` was to fold this into `rosap`
or ROSY; that never happened here.

## Specifications

ROSE is not an RFC. The texts ISODE follows are:

- [ITU-T X.219](https://www.itu.int/rec/T-REC-X.219/) (1988): Remote
  Operations — model, notation, and service (RO-notation, BIND, UNBIND)
- [ITU-T X.229](https://www.itu.int/rec/T-REC-X.229/) (1988): ROSE
  protocol
- [ISO/IEC 9072-1:1989](https://www.iso.org/standard/16648.html): ISO
  text aligned with X.219
- [ISO/IEC 9072-2:1989](https://www.iso.org/standard/16649.html): ISO
  text aligned with X.229
- [ITU-T X.217](https://www.itu.int/rec/T-REC-X.217/) (ACSE service) and
  [ITU-T X.227](https://www.itu.int/rec/T-REC-X.227/) (ACSE protocol):
  A-ASSOCIATE / A-RELEASE

`rosy(1)` also cites ECMA TR/31 and the older X.410 (1984) Remote
Operations text that X.219 / X.229 replaced.

## Layout

| File | Role |
| --- | --- |
| `ronotbind1.c`, `ronotbind2.c` | BIND onto A-ASSOCIATE request/response |
| `ronotunbind1.c`, `ronotunbind2.c` | UNBIND onto A-RELEASE request/response |
| `ronotabort.c`, `ronotlose.c` | Abort and error mapping |
| `ronot.py`, `ry.py` | BIND/UNBIND value ASN.1 (PEPY input, not Python) |
| `READ-ME` | Original design notes |

## Building

Built as part of `./make everything` (core `all` target). The result is
`libronot`; the header is `h/ronot.h` (`#include <isode/ronot.h>`).
