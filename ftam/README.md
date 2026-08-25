# ftam — FTAM protocol library

`libftam` implements OSI File Transfer, Access and Management (ISO 8571). It is the protocol engine only: association, file selection, grouped transfer, FADU access, attributes, and access control.

Volume 3 (`doc/volume3.pdf`) documents the user library. The UNIX initiator (`ftam`) and responder (`ftamd`) live in `ftam2/`. Gateways to Internet FTP live in `ftam-ftp/` and `ftp-ftam/`.

## Layout

| Pattern | Role |
| --- | --- |
| `ftaminitiate.c`, `ftamrespond.c` | Association setup |
| `ftamselect.c`, `ftamaccess*.c` | File select and access |
| `ftambulk*.c`, `ftamgroup*.c` | Bulk transfer and grouped operations |
| `ftamattr.c`, `ftamacl.c`, `ftamdocument.c` | Attributes, ACL, document types |
| `ftam.py`, `fadu.py`, `cont.py` | FTAM ASN.1 (PEPY inputs, not Python) |
| `isodocuments` | Document-type registry |

This is an IS-level implementation but a *minimal* service: text files, binary files, directory listings, and file management. `./ftam-e2e.sh` is the no-install smoke test (needs `bwrap` and `tsapd`).
