# psap — Presentation elements and ASN.1 encoding

`libpsap` is the *abstract syntax* library, not the connection-oriented
presentation protocol. It implements presentation streams, presentation elements
(`PE`), and the Basic Encoding Rules (BER) transformations used by every ISODE
encoder.

Volume 1, “Encoding of Data-Structures” (`doc/volume1.pdf`) documents this API.
The connection-oriented presentation *protocol* lives in `psap2/`.

## What lives here

- **Presentation streams (`PS`)** — wrap a file descriptor, memory buffer, or
  datagram so BER can be read and written.
- **Presentation elements (`PE`)** — in-memory trees of ASN.1 values
  (`pe_alloc`, `pe_cpy`, `pe_free`, …).
- **Primitive conversions** — integer, octet string, bit string, OID, time,
  BOOLEAN, SET/SEQUENCE helpers (`int2prim`, `oid2ode`, …).
- **Object registry** — `isobject.c`, `objectbyname.c`, `objectbyoid.c` for the
  ISO objects database.

PEPY, PEPSY, ROSY, DSAP, FTAM, and SNMP all encode through this library.
Programs typically `#include <isode/psap.h>` and link `-lisode` (which already
contains `libpsap`).
