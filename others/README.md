# others — Optional demos, gateways, and Quipu extras

Programs that are **not required** to run the OSI stack or Quipu. The historic
`READ-ME` says none of this need be installed unless you want to try it. The
default `others` Makefile only builds `quipu/photo`, `quipu/image`, `mosy`, and
`tsbridge`; `everything` in this directory adds the demos.

Volume 4's cookbook example is `lookup/`. SNMP's SMI compiler is `mosy/`. Quipu
user interfaces and sample databases are under `quipu/`.

## Subdirectories

| Directory | Role |
| --- | --- |
| [mosy/](mosy/README.md) | Managed Object Syntax compiler (SMI / OBJECT-TYPE) |
| [tsbridge/](tsbridge/README.md) | Transport-service bridge (TP0/TCP ↔ X.25 ↔ TP4) |
| [quipu/](quipu/README.md) | Photo/image helpers, sample EDBs, DSAconfig, DUAs (FRED, POD, DE, …) |
| [lookup/](lookup/README.md) | ROSY password-lookup demo (Volume 4) |
| [callback/](callback/README.md) | Dual-session callback demo |
| [listen/](listen/README.md) | Transport listen demo |
| [pingpong/](pingpong/README.md) | Asynchronous connect test |
| [idist/](idist/README.md) | `rdist` analogue over OSI |
| [ntp/](ntp/README.md) | NTP mapped onto ROS |
| [rfa/](rfa/README.md) | Remote File Access (master/slave trees) |
| [rtf/](rtf/README.md) | RTS file-transfer demo |
| [osilookup/](osilookup/README.md) | SunLink `osi.hosts` → `isoentities` |
| [X/](X/README.md) | Experimental X11 over ISO transport |
| [cl/](cl/README.md) | Connectionless ISODE (does not compile) |

Several of these still use `pepy` rather than `pepsy`.
