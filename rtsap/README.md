# rtsap — Reliable Transfer Service

`librtsap` implements the OSI Reliable Transfer Service Element (RTSE): an
application service that transfers a complete APDU on an association, using
session activities, minor-sync checkpoints, and a two-way-alternate “turn”.
That is not a more-reliable transport. TP0, TP4, and TCP already deliver octets
reliably on a live connection. RTSE sits above presentation (or, in X.410-1984
mode, session). Its job is APDU-level recovery after the association or session
fails — restart from a confirmed checkpoint — not “TCP, but more reliable”.

Historic RTS is
[CCITT X.410](https://www.itu.int/rec/T-REC-X.410/) (1984): *Message Handling
Systems: Remote Operations and Reliable Transfer Server*. In 1988 that was
split into
[ITU-T X.218](https://www.itu.int/rec/T-REC-X.218/) (model and service; ISO
twin
[ISO/IEC 9066-1:1989](https://www.iso.org/standard/16639.html)) and
[ITU-T X.228](https://www.itu.int/rec/T-REC-X.228/) (protocol; ISO twin
[ISO/IEC 9066-2:1989](https://www.iso.org/standard/16640.html)). `rts.py` is
lifted from ISO 9066-2. The later X.880 series is Remote Operations (ROS), not
RTS; see `rosap/`.

X.218’s “reliable” means: deliver the APDU exactly once, or tell the sender it
failed, and recover from communication or end-system failure with as little
retransmission as possible. X.228 maps that onto one session activity per APDU
and minor synchronization as checkpoints (`checkpointSize` in units of 1024
octets, plus a window of outstanding checkpoints). After an association drop,
the protocol recovers with A-ASSOCIATE carrying `ConnectionData recover`, then
activity-resume from the last confirmed checkpoint.

This tree does not write APDUs to disk. `RtTransferRequest` takes an in-memory
presentation element (`PE`). `rt2ps.c` / `rt2ss.c` encode it with `pe2ssdu`,
send it with `PDataRequest` / `SDataRequest`, and insert
`PMinSyncRequest` / `SMinSyncRequest` between chunks. The receiver grows
`acb_base` with `malloc` / `realloc` until activity-end, then `ssdu2pe`.
Optional `RtSetDownTrans` / `RtSetUpTrans` upcalls let the caller stream from
its own storage.
`others/rtf` reads and writes files that way, but `librtsap` itself never opens
a file for the APDU. Association recovery is refused (`rt2ssrespond.c` prints
"rejecting attempted recovery"). Activity resume is stubbed (`SV_RESUME` in
`doSSactivity` / `doPSactivity`) and raises a procedural exception. Provider
exception reports are treated as unrecoverable rather than restarting from a
checkpoint.

Volume 1 (`doc/volume1.pdf`) documents the API (`RtOpenRequest`,
`RtTransferRequest`, the upcalls). ROS (`rosap`) can sit on RTS via
`RoRtService` instead of presentation. Historic X.400 (1984) used RTS directly.

## Layout

| File / pattern | Role |
| --- | --- |
| `rtsaptrans.c`, `rtsappturn.c`, `rtsapgturn.c` | Transfer and turn |
| `rtsapwait.c`, `rtsapasync.c`, `rtsapselect.c` | Event wait and async |
| `rtsapdtrans.c`, `rtsaputrans.c` | Downtrans / uptrans upcalls |
| `rt2ps*.c` | Mapping onto ACSE/presentation |
| `rt2ss*.c` | Mapping onto session (X.410-1984 mode) |
| `rts.py` | RTSE ASN.1 module (PEPY input, not Python) |

`others/rtf` is a small file-transfer demo that exercises those upcalls.
`librtsap.3n` is the man page (X.410 names such as `RtBeginRequest` plus the
later `RtOpen` / `RtTransfer` entry points).
