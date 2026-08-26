# rosap — Remote Operations Service

`librosap` implements OSI Remote Operations (ROS / X.219 / ISO 9072): invoke an
operation, return a result or error, and reject malformed APDUs. It can run over
presentation, reliable transfer, or (historically) session.

This is classic ROSE as shipped, not the later X.880-series ASN.1 ROS.
`ros.py` is lifted from ISO 9072-2. The texts ISODE follows are
[ITU-T X.219](https://www.itu.int/rec/T-REC-X.219/) (1988): model, notation,
and ROSE service (ISO twin
[ISO/IEC 9072-1:1989](https://www.iso.org/standard/16648.html)); and
[ITU-T X.229](https://www.itu.int/rec/T-REC-X.229/) (1988): ROSE protocol
(ISO twin
[ISO/IEC 9072-2:1989](https://www.iso.org/standard/16649.html)).

ITU later republished Remote Operations as the X.880 series (1994). That
superseded X.219/X.229 rather than merely relocating the ROSE PDUs:
[X.880](https://www.itu.int/rec/T-REC-X.880/) is concepts, model, and
notation (ISO/IEC 13712-1);
[X.881](https://www.itu.int/rec/T-REC-X.881/) is the OSI ROSE *service*
(ISO/IEC 13712-2);
[X.882](https://www.itu.int/rec/T-REC-X.882/) is the OSI ROSE *protocol*
(ISO/IEC 13712-3). ITU still lists X.219 and X.229 as in force, but tells
readers to use X.880/X.881 for concepts and service, and X.882 for protocol.
The X.880 recs keep the X.229 ROSE APDUs unchanged while replacing
X.219's MACRO notation with ASN.1 information object classes.

Volume 1 (`doc/volume1.pdf`) documents the “raw” ROS API. Most applications do
not call RoSAP directly; they use the ROSY stub library (`rosy/`) or the
Directory library (`dsap/`).

## Layout

| File / pattern | Role |
| --- | --- |
| `rosapinvoke.c`, `rosapresult.c`, `rosapuerror.c` | Invoke / result / user-error |
| `rosapureject.c`, `rosaperror.c` | Rejects and provider errors |
| `rosapwait.c`, `rosapasync.c`, `rosapselect.c` | Event wait and async handling |
| `ro2ps.c`, `ro2rts.c`, `ro2ss*.c` | Mapping onto presentation, RTS, or session |
| `ros.py` | ROS ASN.1 module (PEPY input, not Python) |

RO-BIND / RO-UNBIND onto ACSE are *not* in this library; they live in `ronot/`.
