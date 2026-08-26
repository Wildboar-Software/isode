# paradise/others/quipu/uips — PARADISE directory user interfaces

| Directory | Program |
| --- | --- |
| [de/](de/README.md) | Directory Enquiries — public-access DUA |
| [idm/](idm/README.md) | Interactive Directory Manager |

The live top-level Make variable `PARADISEDUA` points at
`others/quipu/uips/de` in the main tree (the ISODE 8.0 / stock Quipu
copy). This overlay is the PARADISE-packaged tree: a later DE, plus
IDM. It is **not** copied over the stock files at build time.

## Build (overlay is not applied)

Same pattern as `paradise/dsap/common/filter.c`: this tree keeps both
copies. `./make` does not overlay these sources onto
`others/quipu/uips/`.

- `./make paradise-dua` (and `paradise`) builds `PARADISEDUA` =
  `others/quipu/uips/de` — stock DE, not this directory.
- The snapshot `paradise/Makefile` sets
  `PARADISEDUA = others/quipu/uips/de others/quipu/uips/idm`, as if the
  tarball had already replaced stock DE and dropped IDM into the main
  tree. The live root `Makefile` still lists only stock DE.
- `./make everything` does not compile DE at all: `all-quipu` builds
  `dsap/` and `quipu/` (the DSA plus DISH). Stock DE lives under
  `others/quipu/uips/de/`; the top-level Makefile only *cleans*
  `others/`, it does not build it. Compile overlay DE or IDM by `cd`
  into this directory (or a child) and `./make`.

There is no `#ifdef PARADISE` in either DUA.

## DE versus stock `others/quipu/uips/de/`

Same program: prompted public enquiry, `libdsap`, `-ltermcap`, no
`widget/` tree (that is SD). The overlay `TAILORING` is dated 18 August
1992; stock is 3 March 1992. Diff of the trees (not a rewrite):

**Only in this overlay**

| File | Role |
| --- | --- |
| `loc.c`, `etc/delocality` | Locality search if the organisation is not under the country |
| `deufn.c`, `ufnp.c`, `etc/deufn` | User-Friendly Naming (UFN; [IETF RFC 1484](https://www.rfc-editor.org/rfc/rfc1484.html)): a comma in the person name skips the other prompts and calls `ufn_match` |
| `fixedfilt.c` | Vendored `strfilter` braces (“patch for ufn generated filters”); same bugfix as `paradise/dsap/common/filter.c` because this DUA links its own `ufnp.c` |
| `pbutil.c` | Present in the tree, **not** listed in the Makefile (not linked) |

**Behaviour in shared sources**

- `bind.c`: optional `password` from `detailor` (stock always binds
  with an empty password). Backup DSA: overlay calls `tryBackup()`;
  stock compares the function pointer to `OK`, so the second
  `dsa_address` line is never used.
- `init.c` / `detailor`: `deptQ` (skip the department prompt) and
  `fuzzyMatching` (stock always runs the `FILTERITEM_APPROX` last
  pass). Shipped overlay `username` is a full Directory Enquiries DN;
  stock `detailor` has the truncated line `uiries`.
- `de.c`: locality fallback (`searchOrgUnderLocality`); UFN branch;
  “Searching ” progress dots.
- Help: `LOCALITY` and `UFN` topics. Pager counts backspace when
  wrapping.

## IDM has no stock twin

There is no `others/quipu/uips/idm/` (and no other IDM) in the main
tree. “The other IDM” does not exist here. IDM was added in PARADISE
package release 2 as a **manager** DUA (add/modify/delete): same
prompt/termcap family as DE, not a second copy of DE. See
[idm/](idm/README.md).

The snapshot `paradise/others/quipu/uips/Makefile` lists `idm` in
`OTHERS`; the live `others/quipu/uips/Makefile` does not.
