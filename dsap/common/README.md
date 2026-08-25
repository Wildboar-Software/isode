# dsap/common — Directory information model

The bulk of `libdsap`: in-memory entries, attribute types/values, distinguished names, RDNs, filters, schema, and every attribute syntax Quipu knows about (strings, certificates, photos, ACLs, mailboxes, …).

Volume 5 §17 is the programmer's guide. New syntaxes used by both the DSA and DUAs are registered in `quipu_sntx.c` (standard syntaxes in `std_sntx.c`). `make test` in this directory builds a small program that round-trips string-encoded attributes.

## Clusters of files

| Prefix / area | Role |
| --- | --- |
| `dn_*.c`, `rdn_*.c` | Distinguished names and RDNs |
| `as_*.c`, `avs_*.c`, `attrt_*.c`, `attrv.c` | Attribute sets, value sequences, types |
| `filter.c`, `soundex.c` | Search filters and approximate match |
| `entry.c`, `parse.c`, `file_*.c` | Entry objects and EDB text I/O |
| `acl.c`, `searchacl.c`, `authpolicy.c`, `policy.c` | Access control |
| `certificate.c`, `cpair.c`, `protected.c` | X.509-style strong authentication |
| `photo_*.c`, `audio.c`, `picture.c` | Photo/audio syntaxes |
| `ufn_*.c`, `aetufn.c`, `aetdap.c` | User-friendly naming and AE-title lookup |
| `turbo_avl.c`, `turbo_index.c` | AVL indexes used when TURBO is enabled |
| `sys_tai.c`, `tai_*.c`, `conf.c` | DUA-side tailor parsing |

`cstrings.c` is relevant to the modern `UTF8String` work mentioned in the root README.
