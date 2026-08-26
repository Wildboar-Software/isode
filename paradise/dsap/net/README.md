# paradise/dsap/net — `dapbind.c` overlay

PARADISE replacement for `dsap/net/dapbind.c`. Compared with stock
`dsap/net/dapbind.c`, the bind semantics are the same. `ds_bind` still
forces `DBA_AUTH_SIMPLE` and calls `secure_ds_bind`; that helper still
parses the single global `dsa_address`. There is no anonymous bind, no
backup DSA list, and no extra tailoring in this overlay.

What differs is housekeeping around the association setup in
`DapAsynBindReqAux` and `DapBindDecode`:

- After a failed `encode_DAS_DirectoryBindArgument`, and again after
  `RoAsynBindRequest`, the overlay `oid_free`s the copied ACSE PCI,
  application context, DAP presentation-context ASN, and default
  context (when `USE_DEFAULT_CTX` is set). Stock leaves those
  `oid_cpy` results unfreed.
- Association-reject diagnostics are copied with `bcopy` instead of
  length-checked `bcopy_int`. Stock can return `NOTOK` from the copy
  without finishing the bind-error mapping.
- `DapBindDecode` is `static`. The overlay includes `<stdlib.h>` and
  `<strings.h>` and drops `"pepsycodec.h"`.

The pilot DUA bind policy is not here. Directory Enquiries
(`paradise/others/quipu/uips/de/bind.c`) calls `DapAsynBindRequest`
directly with a simple bind: `username` and optional `password` from
`detailor`. A second `dsa_address` line in `detailor` is stored as
`backup_dsa_address`; `tryBackup()` swaps it in if the first DSA is
unreachable. The shipped `username` is a Directory Enquiries DN, not
an anonymous bind. IDM's `bind.c` likewise fills simple credentials and
calls `DapAsynBindRequest` itself.
