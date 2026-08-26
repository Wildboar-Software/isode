# ssap — Session Service Access Point

`libssap` implements the OSI session service on top of transport (`tsap`). It
handles connection negotiation, data transfer, tokens, synchronization points,
activities, and exception reporting.

Volume 2 (`doc/volume2.pdf`) is the reference. Presentation (`psap2`) is the
usual caller.

## Layout

Session primitives are split by name: `ssapinitiate.c` / `ssaprespond.c` for
setup, `ssaprelease*.c` for release, `ssapmajor*` / `ssapminor*` for
synchronization, `ssapactivity.c` for activities, `ssaptoken.c` for token
passing, and `ssapexpd.c` / `ssaptyped.c` for expedited and typed data.

Those differ from normal data (`SDataRequest` in `ssaprovider.c`, DT SPDU).
In half-duplex (`SR_HALFDUPLEX`) a DT send requires the data token
(`ST_DAT_TOKEN`). Expedited data (`SExpdRequest`) is an EX SPDU of at most
`SX_EXSIZE` (14) octets; it needs `SR_EXPEDITED`, skips the token check, and
is sent with `TExpdRequest` so it can overtake blocked normal data. Typed
data (`STypedRequest`) is a TD SPDU that needs `SR_TYPEDATA` but not the
data token, so it can be used in half-duplex without the token. See
[ITU-T X.215 / ISO 8326](https://www.itu.int/rec/T-REC-X.215/) §13.1–13.3
and [ITU-T X.225 / ISO 8327](https://www.itu.int/rec/T-REC-X.225/).

`*spkt*` files encode and decode session PDUs.
