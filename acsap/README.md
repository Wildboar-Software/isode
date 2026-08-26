# acsap — Association Control Service

`libacsap` implements the OSI Association Control Service Element (ACSE).
Applications use it to establish, release, and abort associations, and to
resolve application-entity titles into presentation addresses.

This is the top of the OSI stack that most ISODE applications actually call. It
sits on presentation (`psap2`) and is used by remote operations (`rosap`),
reliable transfer (`rtsap`), and the Directory (`dsap`).

Volume 1 of the User's Manual (`doc/volume1.pdf`) documents the API.

## Layout

| File / pattern | Role |
| --- | --- |
| `acsapinitiat.c`, `acsaprespond.c` | Client (initiator) and server (responder) association setup |
| `acsapreleas*.c`, `acsapabort*.c` | Orderly release and abort |
| `acsapaddr.c`, `oid2aei.c`, `isoentity.c` | Presentation-address and AE-title lookup |
| `acsold.py`, `acs.py`, `dase.py` | ACSE ASN.1 modules (PEPY/PEPSY inputs, not Python) |
| `dased.c` | Directory Assistance Service Entity daemon, built with Quipu |
| `acserver*.c`, `acsaptest.c` | Example server / test programs |

The library is asymmetric: it follows a client/server model even though the ACSE
standard is symmetric.

## Building

Built as part of `./make everything` (or the core `all` target). The installed
library is `libacsap`; headers live under `h/acsap.h`
(`#include <isode/acsap.h>`).
