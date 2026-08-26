# psap2 — Presentation protocol (connection-oriented)

`libpsap2` implements the OSI presentation *service* (connection establishment,
context negotiation, data transfer) on top of session (`ssap`). It is distinct
from `psap/`, which only handles BER encoding of presentation elements.

Volume 2 (`doc/volume2.pdf`) documents the API. ACSE (`acsap`) is the usual
caller.

`ps.py` is the presentation ASN.1 module (a PEPY input, not Python). A drop-in
replacement that speaks RFC 1085 Lightweight Presentation over TCP/UDP lives in
`psap2-lpp/`.
