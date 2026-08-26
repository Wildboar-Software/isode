# others/mosy — Managed Object Syntax compiler

Yacc-based ASN.1 compiler that understands the Internet SMI `OBJECT-TYPE` macro.
It compiles `*.my` MIB modules into `*.defs` object lists consumed by `snmp/`.

This is how `snmp/snmpd` learns its MIB: run `mosy` on the `.my` files in
`snmp/`. Not related to PEPY despite the similar “compiler reading a text
module” shape.
