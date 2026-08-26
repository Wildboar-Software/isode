# rosy — Remote-operations stub generator

`rosy` reads a Remote Operations specification (a `.ry` file) and emits C
stubs plus lookup tables. `.ry` is rosy input: ASN.1 Remote Operations
Notation (OPERATION and ERROR macros). It is not Python. Rosy also writes a
`.py` presentation module so `pepy` or `pepsy` can encode the argument and
result types.

`librosy` is the runtime: invoke an operation, wait for the reply, dispatch
an incoming call, and return a result, error, or reject. The manuals call
this the “cooked” interface — you pass C values and table entries instead
of building ROS APDUs by hand with the “raw” RoSAP API in `rosap/`.

Volume 4 (`doc/volume4.pdf`) is the compiler and cookbook. `imisc/imisc.ry`
is the classic example. The password-lookup demo in `others/lookup` is the
sample from that volume.

Pass `-pepsy` when the generated `.py` will be compiled by `pepsy` rather
than the older `posy`/`pepy` pair. `rosy(1)` records that BIND and UNBIND
are unrecognized; association setup and release live in `ronot/`, not here.

## Layout

| File / pattern | Role |
| --- | --- |
| `rosy.c` | Compiler |
| `ryoper.c`, `ryopinvoke.c`, `rydispatch.c` | Operation invoke and server dispatch |
| `rydsresult.c`, `rydserror.c`, `rydsurej.c` | Result / error / user-reject |
| `rywait.c`, `ryopblock.c`, `rydsblock.c` | Event wait and blocking helpers |
| `rystub.c`, `ryfind.c`, `rygenid.c` | Stub tables and invoke-id allocation |

Output for a module `IMISC` is typically `IMISC-ops.h`, `IMISC-ops.c`,
`IMISC-stubs.c`, and `IMISC.py`. The ops files define
`table_IMISC_Operations` and `table_IMISC_Errors` — arrays of
`struct RyOperation` and `struct RyError` that name each operation, its
code, and how to encode arguments, results, and errors.

Once `librosy` is driving a connection, do not also call RoSAP invoke,
result, or error routines on that same association. `librosy` already
tracks outstanding invokes; mixing the two interfaces desynchronizes that
state. Use cooked (`librosy`) or raw (`librosap`), not both.
