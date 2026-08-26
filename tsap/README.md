# tsap — Transport Service Access Point

`libtsap` implements an OSI transport service. In this tree that usually means
**TP0 over TCP** (RFC 1006 / ITOT), with historic backends for X.25, TLI/XTI,
and SunLink OSI TP4.

This is the bottom of the OSI session/presentation stack. `tsapd` in `support/`
listens for incoming transport connections and execs the matching server. Quipu
can listen on TCP 17003 itself and does not need `tsapd`.

Volume 2 (`doc/volume2.pdf`) documents the API. The transport *switch* (which
stacks are compiled in, OSI communities, TS-bridges) is also described there.

## Layout

| File / pattern | Role |
| --- | --- |
| `tsapinitiate.c`, `tsaprespond.c`, `tsaplisten.c` | Connection setup and listen |
| `tsaprovider.c`, `tsapstate.c` | Provider state machine |
| `tp0ts.c` | TP0 over a network service |
| `ts2tcp.c`, `ts2x25.c`, `ts2tli.c`, `ts2xti.c`, `ts2bsd.c`, `ts2sunlink.c` | Network backends |
| `*tpkt*` | TPDU encode/decode and debug dump |

## Quirks

- `tsapd` (TCP 102) is only needed for FTAM, VT, IMISC, `ftam-e2e.sh`, and
  `isode-test.sh`.
- Bridging TP0/TCP, TP0/X.25, and TP4 is the job of `others/tsbridge`, not this
  library.
