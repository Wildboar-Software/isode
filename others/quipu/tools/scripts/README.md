# others/quipu/tools/scripts — Application entities in the DIT

Shell helpers that dump and reload Application Entity Information.

1. Bind in DISH as the DSA manager.
2. `moveto` the `local_DIT` location.
3. `dsa2aei` → `quipuentities` (host, application, context, address, exec
   vector).
4. Edit the file; `aei2dsa | sh` writes it back.

`support/bootsvc` loads the default service set first. See `READ-ME`.
