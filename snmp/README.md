# snmp — 4BSD/ISODE SNMP

SNMP agent, tools, and SMUX API contributed by NYSERNet. Uses ISODE's ASN.1 stack (`pepsy` + `libpsap`) rather than a standalone SNMP library. Historic MIBs are shipped as MOSY sources (`*.my`) compiled to `*.defs`.

See `doc/READ-ME` (the `snmp/` talk) and the man pages `snmpd.8c`, `snmpc.8c`, `snmpt.8c`, `unixd.8c`. Built by the top-level `all-snmp` target.

## Programs

| Binary | Role |
| --- | --- |
| `snmpd` | SNMP agent (UDP/161 and OSI transports when compiled in) |
| `snmpc` | SNMP agent compiled as a SMUX *peer* instead of a standalone agent |
| `snmpt` | Trap sink (UDP/162) |
| `snmpi` | Minimal SNMP initiator (manager CLI) |
| `unixd` | SMUX peer that exports UNIX-specific MIB objects (users, print, filesystem) |

`libisnmp` holds object/syntax/value handling (`objects.c`, `syntax.c`, `values.c`) plus SMUX (`smux.c`). MIB instrumentation is split per group: `system.c`, `interfaces.c`, `ip.c`, `icmp.c`, `tcp.c`, `udp.c`, `routes.c`, `clns.c`.

## MIB sources

`*.my` files are MOSY (SMI) modules; `others/mosy` compiles them. Matching `*.defs` files are the generated object lists `snmpd` loads (see `snmpd.defs` / `loadobjects.c`).

Configuration: `snmpd.rc` (communities, views, SMUX peers), `snmpd.defs` (objects).

## Subdirectories

| Directory | Role |
| --- | --- |
| [gawk/](gawk/README.md) | Bundled gawk plus SNMP-aware awk scripts (`s-netstat`, `s-traceroute`) |
