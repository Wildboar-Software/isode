# paradise/support — `tsapd.c` overlay

PARADISE replacement for `support/tsapd.c`. The package tarball
overwrote that file onto ISODE 8.0. This tree keeps both copies;
`./make everything` still compiles `support/tsapd.c` (and `iaed` from
that same file with `-DIAE`). Nothing copies this overlay over it.

Both copies are the dual listener: without `-DIAE` this is `tsapd`
(`isoservices`); with `-DIAE` the Makefile links it as `iaed.c` and
it is `iaed`, which searches the Directory for
`iSODEApplicationEntity` and execs `execVector`. That dual mode,
`listenAddress` vs `presentationAddress`, and the multi-address
`TNetListen` loop are already in stock ISODE 8.0. Quipu itself still
does not go through `tsapd`.

The overlay's original RCS log (later stripped) was:

```text
Revision 9.1  1992/06/24  iaed service selection fix
Revision 9.2  1992/07/27  -T flags
```

## `iaed` matches the T-selector only

On an incoming T-CONNECT, stock `iaed` walks the Directory-derived
`iae[]` table with `tsap_addr_cmp`. That requires the same T-selector
and a matching NSAP (TCP host+port, X.25 DTE+PID, or NSAP bytes). The
called address from `TInit` often fails that NSAP check against the
Directory value (`Internet=hostname` from `bootsvc`, an empty listen
domain, and so on), so the service is not found.

The overlay `#if 0`s that call (`THIS doesn't work - JPO`) and
compares T-selector length and bytes only -- the same rule stock
`tsapd` already uses via `getisoserventbyselector`. Julian Onions
(JPO) is the comment author.

## `-T tailorfile`

Both the `tsapd` and `iaed` argument parsers gain `-T file`:
`isodesetailor(file)` and `setenv("ISOTAILOR", file)`. Stock has
neither switch. `tsapd.8c` / `iaed.8c` do not document it.

`arginit` calls `isodetailor()` first, and that function returns
immediately once `inited`. So `-T` does not reload this process's
own tailor (listen stacks, X.25 DTE, ...). It retargets the tailor
filename and exports `ISOTAILOR` for exec'd children. This tree's
`isodetailor()` does not `getenv("ISOTAILOR")`.

## Not COSINE listen addresses

The overlay does not add COSINE-specific listen addresses or extra
services. `iaed` already prefers `listenAddress` when present, else
`presentationAddress`, and warns if two AEs share a T-selector.
`support/bootsvc` still seeds those AE entries in the main tree.

Other differences versus today's `support/tsapd.c` are later
modernization (overflow helpers, `sysconf` `getdtablesize`, signal
prototypes), not extra listener behaviour.

Compare with `support/tsapd.c` if you need the exact hunks.
