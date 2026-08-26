# paradise/quipu — DSA source overlay

Replacement Quipu DSA sources from the COSINE PARADISE tarball. That
package unpacked over ISODE 8.0 and overwrote the same paths under
`quipu/`. This tree keeps both copies. `./make everything` and
`./make paradise` both compile `quipu/*.c` (`cd quipu; $(MAKE) all`).
Nothing copies these files over the live sources.

The five `.c` files exist in both trees. Stock is still ISODE 8.0 RCS
9.0 (June 1992). The overlay is later 9.1–9.3 patches (July–August
1992). There is no `#ifdef PARADISE` or `#ifdef COSINE`. Schema,
`quiputailor`, photo, sound, turbo, ACL, search, and dsaconfig are not
in this directory; those stay in stock `quipu/`. Today's `quipu/`
files also have later modernization (prototypes, overflow helpers)
that this overlay never received.

## What actually differs

- **DSP cache completeness** (`dsp_cache.c`, `get_dsa_info.c`,
  `ds_read.c`). Stock `cache_dsp_entry` always sets `e_complete =
  TRUE`, including Search results that only returned selected
  attributes. XT-DUA then treated those partial entries as complete
  (wrong “long”/non-leaf objects; add-entry against a remote non-leaf
  failed). The overlay adds a `complete` argument: Reads are complete;
  Search is complete only if all attributes and values were requested.
  `cant_use_cache` then returns true when `e_complete` is false, so
  the DSA will not answer a Read from a partial cache.

- **get-EDB initiator crash** (`update.c`). `dsa_wait()` is not
  re-entrant: if the connection being served fails, it frees the
  `oper` currently in use. The overlay wraps every `dsa_wait(0)` in
  the shadow/get-EDB path with `#ifdef USE_DSA_WAIT`. That macro is
  not defined anywhere, so the calls are compiled out. EDB updates
  block longer; they no longer core-dump. Stock still calls
  `dsa_wait(0)` at those sites. (`ds_search.c` still has `dsa_wait`
  calls; this overlay does not touch it.)

- **First EDB write for a mapped RDN** (`entry_load.c`). Names longer
  than `MAXFILENAMELEN` (15) go through `write_mapped_rdn`. After
  `tempnam`/`mkstemp`, stock leaves `aps->ps_ptr` in the middle of the
  generated path (`TEMPNAM` is set on Linux). Later path construction
  then overwrites the unique suffix. The overlay sets `ps_ptr` to the
  end of `aps->ps_base`.

- **`version.info`** is PARADISE-only; contents are `PARADISE`.
  `util/version.sh` can take that as a second argument so
  `quipuversion` becomes `quipuPARADISE …`. The live `quipu/Makefile`
  runs `version.sh quipu` without it, so the running DSA still reports
  the stock string.

Compare with
`quipu/{entry_load,ds_read,update,get_dsa_info,dsp_cache}.c` if you
need the exact hunks. RCS logs (later stripped) were: “first edb
update bug fix”; “Add entry problem with remote non-leaf objects”;
“oops wrong long entry … XT-DUA exposed caching BUG”; “complete
caching problem -> reverse logic”; “Cache search resutls if all attrs
requested”; “Random core dump during get_edb (initiator)”.
