# rosy — Remote-operations stub generator

`rosy` compiles a Remote Operations specification (`.ry`) into C stubs and dispatch tables. `librosy` is the runtime: invoke, wait, dispatch, result, error, reject.

Volume 4 (`doc/volume4.pdf`) is the “cooked” applications interface. `imisc/imisc.ry` is the classic example. The password-lookup demo in `others/lookup` is the cookbook sample from that volume.

## Layout

| File / pattern | Role |
| --- | --- |
| `rosy.c` | Compiler |
| `ryoper.c`, `ryopinvoke.c`, `rydispatch.c` | Operation invoke and server dispatch |
| `rydsresult.c`, `rydserror.c`, `rydsurej.c` | Result / error / user-reject |
| `rywait.c`, `ryopblock.c`, `rydsblock.c` | Event wait and blocking helpers |
| `rystub.c`, `ryfind.c`, `rygenid.c` | Stub tables and invoke-id allocation |

Do not mix raw RoSAP calls with `librosy` on the same association. Generated tables are named from the module, e.g. `RyOperation` / `RyError` arrays for `MODULE`.
