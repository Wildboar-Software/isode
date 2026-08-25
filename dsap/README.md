# dsap — Directory Service Access Point (`libdsap`)

X.500 client and shared Directory library: DAP/DSP encode/decode, attribute syntaxes, distinguished names, filters, OID tables, and DUA tailoring. Quipu (`quipu/`) links this library; DISH and the other DUAs do too.

Volume 5 (`doc/volume5.pdf`) is the Quipu/Directory manual. The programmer's guide there describes how to add attribute syntaxes (almost always in `common/`).

Built as part of `all-quipu` (after `h/quipu` headers). The result is `libdsap`, installed with `dsaptailor` and the `oidtable.*` files.

`.py` files under `x500as/` are PEPSY ASN.1 modules, **not Python**.

## Subdirectories

| Directory | Role |
| --- | --- |
| [x500as/](x500as/README.md) | X.500 ASN.1 modules compiled by `pepsy` into encode/decode tables |
| [common/](common/README.md) | DNs, attributes, syntaxes, filters, EDB parse, UFN, DUA tailor |
| [net/](net/README.md) | DAP and DSP association, invoke, result, and error over ROS |

## Files in this directory

| File | Role |
| --- | --- |
| `dsaptailor` | DUA tailor file: initial DSA address, logging, photo helpers, sizelimit |
| `oidtable.at` / `oidtable.oc` / `oidtable.gen` | Attribute types, object classes, and generic OIDs |
| `oidtable.*.local` | Site-local OID additions |
| `dsaptailor.root-dsas`, `root-dsas.sh` | Known root DSA list |

The DSA named in `dsa_address` must match `mydsaname` in `quipu/quiputailor` and an entry in the EDB.

## Building

From the repo root, after the OSI stack libraries exist:

```shell
./make all-quipu
```

A subdirectory `make` here will fail if `libisode` and `pepsy` have not already been built.
