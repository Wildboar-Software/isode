# ssap — Session Service Access Point

`libssap` implements the OSI session service on top of transport (`tsap`). It handles connection negotiation, data transfer, tokens, synchronization points, activities, and exception reporting.

Volume 2 (`doc/volume2.pdf`) is the reference. Presentation (`psap2`) is the usual caller.

## Layout

Session primitives are split by name: `ssapinitiate.c` / `ssaprespond.c` for setup, `ssaprelease*.c` for release, `ssapmajor*` / `ssapminor*` for synchronization, `ssapactivity.c` for activities, `ssaptoken.c` for token passing, and `ssapexpd.c` / `ssaptyped.c` for expedited and typed data.

`*spkt*` files encode and decode session PDUs.
