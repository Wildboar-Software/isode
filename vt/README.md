# vt — OSI Virtual Terminal

Basic-class Virtual Terminal with a TELNET profile (ISO VT, DIS-level in
this tree — the rest of ISODE is IS-level). Provides `vt` (initiator) and
`vtd` (responder).

The service is ISO DIS 9040 (*Virtual Terminal Basic Class Service*); the
protocol is ISO DIS 9041 (*Virtual Terminal Basic Class Protocol*).
`vt.1c` and `vtd.8c` cite those DIS numbers. Protocol-machine comments
follow the July 1987 DIS 9041 tables. The man pages note that VT PDU
encodings may still need updating for the International Standard, so
DIS-level in this tree is still accurate. Later IS texts are
[ISO/IEC 9040:1997](https://www.iso.org/standard/28785.html)
(service; first IS [ISO 9040:1990](https://www.iso.org/standard/16601.html))
and [ISO/IEC 9041-1:1997](https://www.iso.org/standard/28786.html)
(protocol; first IS
[ISO 9041-1:1990](https://www.iso.org/standard/16606.html)).
There is no ITU-T X.3xx or T.541 twin of this protocol.

The TELNET profile is the NIST OSI Workshop Implementor's Agreements
profile (`vt.1c`, Volume 3 chapter 6), not
[ISO/IEC 9041-2](https://www.iso.org/standard/28787.html)
(a PICS proforma). See [NBS IR 88-3823](https://doi.org/10.6028/nbs.ir.88-3823).

Volume 3 (`doc/volume3.pdf`) covers the UNIX implementation. This is one of
the remaining large users of `pepy` rather than `pepsy`.

`.py` files (`send_asq.py`, `rcv_text.py`, …) are PEPY ASN.1 inputs,
**not Python**. `actions*.c` / `states*.c` are the VT protocol machine.
`vt_telnet.c` maps the TELNET profile onto a local tty.

Built by the top-level `all-vt` target. Incoming associations go through
`tsapd`.
