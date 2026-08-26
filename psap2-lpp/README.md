# psap2-lpp — Lightweight Presentation Protocol

Replacement for `libpsap2` that implements RFC 1085 *ISO Presentation Services
on top of TCP/IP-based internets*. Same presentation API
(`#include <isode/psap2.h>`), different stack: TCP or UDP instead of full
session/transport.

Contributed by The Wollongong Group. Built by the top-level `all-lpp` /
`inst-lpp` targets, which also rebuild a subset of the stack (`h`, `compat`,
`psap`, `pepsy`, `pepy`, `acsap`, `rosap`, `ronot`, `rosy`) plus `support` and
`imisc` LPP variants (`lppd`).

`ps2tcp.c` and `ps2udp.c` are the network mappings. Use this when you want
ROS/ACSE applications without a full OSI lower layer.
