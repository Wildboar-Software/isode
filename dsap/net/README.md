# dsap/net — DAP and DSP on the wire

Directory protocol machines: bind, unbind, invoke, result, error, reject, and abort for both the Directory Access Protocol (DUA ↔ DSA) and the Directory System Protocol (DSA ↔ DSA).

RO-BIND itself is implemented via `ronot/`. This directory issues the Directory operations on an already-bound ROS association.

| Prefix | Role |
| --- | --- |
| `dap*.c` | DAP: bind, read, search, list, compare, add, modify, modifyRDN, remove, abandon |
| `dsapbind*.c`, `dsapunbind*.c` | DSP bind/unbind |
| `dsapinvoke.c`, `dsapresult.c`, `dsaperror.c` | DSP operation PDUs |
| `dsapcontexts.c` | Presentation/application contexts |
| `dsapwait.c`, `dapwait.c` | Event wait |
| `ds_types.c`, `plog.c` | Shared types and protocol logging |

`dapbind.c` is one of the files the PARADISE overlay (`paradise/dsap/net/`) replaces.
