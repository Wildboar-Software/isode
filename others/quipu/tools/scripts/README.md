# others/quipu/tools/scripts — AE entries in the Directory

ISODE originally resolved application names through the stub file
`isoentities` (see `support/isoentities.5`). The preferred replacement is
to store the same information as X.500 **Application Entity** entries in
Quipu, so ACSE can look them up in the Directory.

These three shell scripts dump that part of the DIT to a text file, and
load edits back. They are not compilers; they emit and consume DISH
commands (`search`, `add`, `modify`, `showname`, `showentry`).

## What gets stored

Under whatever Directory node you are sitting on, the scripts assume this
shape:

```text
cn=<process>                          applicationProcess
    cn=<process>@cn=<service>         applicationEntity
                                      (+ isodeapplicationEntity if
                                         an execVector is present)
```

`<process>` is usually a hostname (or a bucket such as `services`).
`<service>` is the AE common name (`filestore`, `file service`, …).

Each AE entry carries:

| Attribute | Meaning |
| --- | --- |
| `presentationAddress` | Where to connect (ISODE string form) |
| `supportedApplicationContext` | e.g. `iso ftam`, `iso vt` |
| `execVector` | Program `tsapd` should exec (optional) |

`support/bootsvc` seeds a default `cn=services` tree (FTAM, VT, echo,
IMISC, …). Run that once as manager if the subtree is empty. These
scripts then maintain whatever is already there.

## Prerequisites

The scripts call DISH as if its commands were on your `PATH`. Either:

- run them from a `dish` session, or
- use “DISH from the shell” (Volume 5 §4.7) with `DISHPROC` set so each
  command talks to one bound DISH process instead of starting a new one.

Before `dsa2aei` / `aei2dsa` / `bootsvc`:

1. Bind as someone who can write the subtree (typically the DSA manager):

   ```text
   bind -u "<DN of DSA manager>" -simple
   ```

2. `moveto` the parent under which AE entries should live. That is usually
   `local_DIT` in `dsaptailor`. Data is added **below the current
   position**, not at a hard-coded DN.

The Makefile here is a no-op; invoke the scripts from this directory (or
copy them onto `PATH`).

## `dsa2aei` — Directory → `quipuentities`

Subtree-searches for `objectClass=applicationEntity` from the current
position, then writes **`quipuentities`** in the working directory.
Colon-separated fields, one AE per line:

```text
host : common name : application context : address : exec vector
```

Example from the original notes:

```text
myhost: filestore: iso ftam: '0103'H/Int-X25(80)=23421920030013: iso.ftam
```

`dsa2aei` overwrites `quipuentities`. Lines starting with `#` are
comments (the script writes a short header of its own).

## Editing and loading

Edit `quipuentities` to add or correct rows. Grouping is by the first
field: `aei2dsa` ensures `cn=<host>` exists as an `applicationProcess`,
then add/modify `cn=<host>@cn=<name>` for each line.

**Deletions are not done from the file.** Remove an AE with DISH
`delete`.

`aei2dsa` prints a `/bin/sh` script of DISH commands to stdout; it does
not update the Directory itself:

```text
aei2dsa | sh
```

It assumes you are still bound and still sitting at the same DIT node as
when you ran `dsa2aei`.

## `ent2aei` — stub `isoentities` → triples

Reads `isoentities`-style lines on stdin (host, service qualifier,
address; `default` rows skipped). Maps a few historic qualifiers
(`filestore` → `iso ftam`, `terminal` → `iso vt`, and similar) and
prints space-separated `host`, context name, and address. That is a
migration aid toward `quipuentities`, not something `aei2dsa` can read
until you add colons and the remaining fields.

## See also

- `READ-ME` in this directory (original 1990 notes)
- [`../README.md`](../README.md) for the other Quipu admin tools
- Volume 1, “The ISODE Entities Database”; Volume 5, DISH
