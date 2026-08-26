# tsap — Transport Service Access Point

`libtsap` presents the OSI transport service (ISO 8072 / X.214) to session
and above. It is a switch among compiled-in providers, not one protocol
machine.

ISODE's own user-space stack is mainly **TP0** over TCP
([RFC 1006](https://www.rfc-editor.org/rfc/rfc1006.html) / ITOT) and TP0
over X.25, plus CONS variants. The TP0 engine is `tp0ts.c`; TCP and X.25
glue are `ts2tcp.c` and `ts2x25.c`. Connection setup lives in
`tsapinitiate.c` and `tsaprespond.c`; listen/accept is `tsaplisten.c`.

ISODE **did** support TP4 as a TSAP provider when the host had a kernel
TP4 implementation. `libtsap` is not itself a TP4 protocol machine: with
`#define TP4` it shims to a vendor stack. There is no `tsaptp4.c` and no
`ISODE_TP4` flag. The shims (empty stubs unless the matching flag is set)
are:

| Flag | Kernel / vendor provider | File |
| --- | --- | --- |
| `SUN_TP4` | SunLink OSI on SunOS | `ts2sunlink.c` |
| `BSD_TP4` | 4.4BSD / UCB–UWisc TP4 | `ts2bsd.c` |
| `TLI_TP` | TLI TP4 (e.g. Concurrent RTU) | `ts2tli.c` |
| `XTI_TP` | XTI TP4 (e.g. AIX OSIMF) | `ts2xti.c` |

`config/OPTIONS` lists these. Typical SunLink configs (`config/sunlink7.h`)
define both `TP4` and `SUN_TP4`. The default Linux build
(`config/linux.h`) defines `TCP` and `SOCKETS` only — no `TP4`. On this
tree that means TP0/TCP, not SunOS TP4. Turning on the flags without a
kernel provider does not implement class 4.

This is the bottom of the OSI session/presentation stack. `tsapd` in
`support/` listens for incoming transport connections and execs the
matching server. Quipu can listen on TCP 17003 itself and does not need
`tsapd`.

Volume 2 (`doc/volume2.pdf`) documents the API. The transport *switch*
(which stacks are compiled in, OSI communities, TS-bridges) is also
described there.

## Layout

| File / pattern | Role |
| --- | --- |
| `tsapinitiate.c`, `tsaprespond.c`, `tsaplisten.c` | Connection setup and listen |
| `tsaprovider.c`, `tsapstate.c` | Provider state machine |
| `tp0ts.c` | User-space TP0 engine |
| `ts2tcp.c`, `ts2x25.c` | TP0 over TCP (RFC 1006) and X.25 |
| `ts2sunlink.c`, `ts2bsd.c`, `ts2tli.c`, `ts2xti.c` | TP4 shims to kernel providers |
| `*tpkt*` | TPDU encode/decode and debug dump |

## Quirks

- `tsapd` (TCP 102) is only needed for FTAM, VT, IMISC, `ftam-e2e.sh`, and
  `isode-test.sh`.
- Bridging TP0/TCP, TP0/X.25, and TP4 is the job of `others/tsbridge`, not this
  library.
