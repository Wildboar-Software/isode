# ftam2 — FTAM initiator and responder

UNIX programs that use `libftam`:

| Binary | Role |
| --- | --- |
| `ftam` | Initiator (client): get, put, ls, directory management |
| `ftamd` | Responder (server): maps FTAM onto the local filesystem |

Volume 3 (`doc/volume3.pdf`) describes the UNIX implementation. `ftamusers.5` controls who may use the responder. `docs.py` is additional ASN.1 (PEPY input, not Python). `eurosinet/` holds historic profile notes.

Build via the top-level `all-ftam` target after the core libraries exist. Incoming associations are typically dispatched by `tsapd` from `isoservices`.
