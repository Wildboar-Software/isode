# snmp/gawk — GNU awk and SNMP helper scripts

Vendored historic GNU awk sources plus a small “SNMP-capable gawk” layer used to
pretty-print MIB data from `netstat`/`traceroute`-style tools.

| Directory | Role |
| --- | --- |
| `gawk-2.11/`, `gawk-2.13/` | Upstream gawk snapshots from the original ISODE SNMP release |
| `s-gawk/` | Installable scripts `s-netstat` and `s-traceroute`, plus `mib.*` awk modules |

You do not need this directory to run `snmpd`. It is optional manager-side
tooling from the 4BSD/ISODE SNMP package.
