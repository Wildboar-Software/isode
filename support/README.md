# support — Daemons, tailor files, and ISO databases

Runtime support that is not a protocol library: the transport listener, ISODE databases, and the combined `libisode` archive.

Volume 1 (aliases, entities, macros, objects) and Volume 2 (services database, `isotailor`, logging) document the files this directory installs.

## Programs

| Binary | Role |
| --- | --- |
| `tsapd` | OSI transport listener (RFC 1006 on TCP, plus X.25/TP4 when compiled in). Exec's the server named in `isoservices`. |
| `iaed` | Application-entity daemon used with Quipu |
| `isod` / `isoc` | Generic ISODE initiator/responder helpers |
| `isore` | ISODE “re” utility |
| `lppd` | Lightweight-presentation listener (LPP build) |

## Installed databases

Copied from `*.db` / `*.local` (the `.local` files originate in `config/`):

| File | Role |
| --- | --- |
| `isotailor` | Logging, local hostname, transport switch, Directory pointers |
| `isoaliases` | Short names for distinguished names / hosts |
| `isoentities` | Stub application-entity directory (deprecated in favour of X.500) |
| `isoservices` | Service name → program to exec + selectors |
| `isobjects` | Object identifier names |
| `isomacros` | Address macros |

`bootsvc` loads default services into a running Quipu DSA.

Quipu does **not** go through `tsapd`; it listens on TCP 17003 (ITOT) itself.
