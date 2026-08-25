# rosap — Remote Operations Service

`librosap` implements OSI Remote Operations (ROS / X.219 / ISO 9072): invoke an operation, return a result or error, and reject malformed APDUs. It can run over presentation, reliable transfer, or (historically) session.

Volume 1 (`doc/volume1.pdf`) documents the “raw” ROS API. Most applications do not call RoSAP directly; they use the ROSY stub library (`rosy/`) or the Directory library (`dsap/`).

## Layout

| File / pattern | Role |
| --- | --- |
| `rosapinvoke.c`, `rosapresult.c`, `rosapuerror.c` | Invoke / result / user-error |
| `rosapureject.c`, `rosaperror.c` | Rejects and provider errors |
| `rosapwait.c`, `rosapasync.c`, `rosapselect.c` | Event wait and async handling |
| `ro2ps.c`, `ro2rts.c`, `ro2ss*.c` | Mapping onto presentation, RTS, or session |
| `ros.py` | ROS ASN.1 module (PEPY input, not Python) |

RO-BIND / RO-UNBIND onto ACSE are *not* in this library; they live in `ronot/`.
