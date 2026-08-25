# vt — OSI Virtual Terminal

Basic-class Virtual Terminal with a TELNET profile (ISO VT, DIS-level in this release — the rest of ISODE is IS-level). Provides `vt` (initiator) and `vtd` (responder).

Volume 3 (`doc/volume3.pdf`) covers the UNIX implementation. This is one of the remaining large users of `pepy` rather than `pepsy`.

`.py` files (`send_asq.py`, `rcv_text.py`, …) are PEPY ASN.1 inputs, **not Python**. `actions*.c` / `states*.c` are the VT protocol machine. `vt_telnet.c` maps the TELNET profile onto a local tty.

Built by the top-level `all-vt` target. Incoming associations go through `tsapd`.
