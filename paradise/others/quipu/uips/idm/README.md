# paradise/others/quipu/uips/idm — Interactive Directory Manager

Prompted Directory User Agent for **organisation Directory accounts**.
An organisation manager binds over DAP, then lists, reads, adds,
modifies, and deletes people, roles, and rooms under that organisation
(or department). The org/OU header can be updated separately. With
tailor flags, a new organisation can be registered. Introduced in
PARADISE package release 2.

It is the same code family as DE (`de/`): interview-style prompts,
inverse video via termcap, per-object-class `.c` files, and `libdsap`
(not `libdish`). DE is public enquiry; IDM is the manager. It is
**not** the ISODE 5 **widget** / Screen Directory curses line
(`others/quipu/uips/sd/`). There is no `widget/` directory here.

`idm` is built from `dm.c` (the file comments it as `main.c`) plus
`people.c`, `org.c`, `ou.c`, `country.c`, and the people/role/room
helpers (`peopleoc.c`, `peoplerl.c`, `peoplerm.c`, `peoplecp.c`). See
`isode-paradise.pdf` Chapter 4.

## How it differs from DE, SD, and DISH

DE (`de/`, also `others/quipu/uips/de/`) is a **scripted interview**
for looking someone up: person / department / organisation / country,
then a numbered pick-list. It never adds, modifies, or deletes. IDM
reuses that prompt style and much of the same source layout (`bind.c`,
`video.c`, `pager.c`, `filter.c`, `atts.c`) for the opposite job:
maintain the organisation's entries once you have bound as a manager.

SD is a full-screen **curses** DIT browser (ISODE 5 widget library).
IDM never includes `<curses.h>` (the include in `dm.c` is commented
out) and has no widget templates for person / org / OU / role / DSA.

DISH is the general-purpose DAP shell. IDM hides DNs and filters behind
menus. It does not implement User-Friendly Naming
([IETF RFC 1484](https://www.rfc-editor.org/rfc/rfc1484.html)); that is
Doog, and PARADISE DE's optional `ufnp.c` path. IDM searches with its
own `eqfilter` / `subsfilter` in `filter.c`. `atts.c` `de_dn_print`
calls `ufn_dn_print` only when the print format is `UFNOUT`.

| Program | Relationship to IDM |
| --- | --- |
| DE | Same prompt / termcap family; read-only enquiry. |
| SD | Curses widget DIT browser (`-lcurses -ltermcap`). No add/delete. |
| DISH | Full DAP shell. IDM hides that protocol. |
| `dish_manage` | DISH plus alias add/delete. Still DISH. |
| POD | X11 Athena-widget DUA; some modify, no add/rename. |
| Doog | Incomplete UFN client, not a manager UI. |

## Termcap, not libcurses

The Makefile link line is:

```text
$(LIBDSAP) $(LIBISODE) $(LSOCKET) -ltermcap
```

There is no `CURSES=` variable and no `-lcurses`. That matches DE and
differs from SD (`CURSES = -lcurses -ltermcap`).

What `-ltermcap` is for (`video.c`): `tgetent` / `tgetstr` / `tgetnum`
/ `tgoto` / `tputs` so prompts can use inverse video (`mr` / `me`),
the pager can clear the prompt line (`cm` / `ce`), and the bell can
sound (`bl`). Screen size comes from termcap `li` / `co`, defaulting
to 80x24. `rawinput.c` uses `ioctl` termio for the pager's single-key
input, not curses `initscr`.

On Linux that historic `-ltermcap` is usually a stub from the ncurses
package (`libncurses-dev`); IDM still does not call the curses API.

## Display is opinionated (PARADISE schema)

IDM does not dump arbitrary X.500 entries. It knows a small set of
object classes and a PARADISE-shaped attribute list.

Hard-coded classes (`destrings.h`, `add.c`):

| Kind | objectClass when adding |
| --- | --- |
| Person | `person` `organizationalPerson` `newPilotPerson` `quipuobject` |
| Role | `organizationalRole` `quipuobject` |
| Room | `room` `quipuobject` |
| Organisation | `organization` `quipuNonLeafObject` `quipuobject` |
| Org manager | `organizationalRole` `simpleSecurityObject` `quipuobject` |

An X.400 address adds `mhsUser` as well. Extra classes can be appended
from `dmtailor` (`prrOC`, `rlOC`, `rmOC`).

A person add always asks **surname**, **first name**, then **full name**
(`cn`, default `first last`). There is no photo attribute in this tree.

Which attributes are **listed, read, and prompted** is tailor-driven
(`commonatt`, `orgatt`, `ouatt`, `prratt`, `rlatt`, `rmatt` in
`dmtailor`). The shipped file is:

| Token | Attributes |
| --- | --- |
| `commonatt` | `telephoneNumber` (every class above) |
| `orgatt` / `ouatt` | `telexNumber` |
| `prratt` | `rfc822Mailbox`, `roomNumber` |
| `rlatt` | `telephoneNumber` |
| `rmatt` | `roomNumber`, `telephoneNumber`, `description` |

Friendly labels (`mapattname`) replace OID names on screen, for
example `rfc822Mailbox` -> "Electronic mail", `facsimileTelephoneNumber`
-> "Fax", `favouriteDrink` -> "Favourite drink". `printDetails` pads
those names to 22 columns. A people **list** is a one-liner: number,
common name (19 columns), telephone, mailbox (`printPersonOneLiner`).
A **modify** screen numbers the attributes; `etc/dmattr_numbers` shows
the intended look:

```text
         David Goodman
           1  Telephone number   +44 81 450 3399
                                 +44 71-387-7050 x3695
           2  Electronic mail    D.Goodman@cs.ucl.ac.uk
           3  Favourite drink    Rose Pouchong tea
           4  Room number        201
           5  Personal title     Mr
```

Postal address is six lines; telex is number / country / answerback;
phone and fax have a 32-character limit. UK grey-book mailbox order is
optional (`greyBook` in the tailor). Help files under `etc/dm*` match
those PARADISE attributes (`dmcn`, `dmtelephoneNumber`,
`dmfavouriteDrink`, `dmpersonalTitle`, ...).

## Commands and keys

Not F-keys, arrows, or Ctrl-L (those are SD widgets). Keys are the DE
prompt set, plus manager menus.

At any prompt except yes/no:

| Input | Action |
| --- | --- |
| number | Menu choice, or pick a numbered hit |
| Return | Accept the default in `[brackets]` |
| `q` | Leave this operation; `q` at the first menu exits |
| `?` | Help for the current prompt (`etc/dm*`) |
| `??` / `?help` | Help about help |
| `*` | List entries at this prompt |
| `xxx*` / `*xxx*` / `*xxx` / `xx*xx` | Wildcards (`displayValidWildCards`) |
| `-` | Clear / remove a value (modify and some add prompts) |
| Ctrl-C | Back to the initial menu (`onint2`); from there `q` exits |

Pager (`pager.c`; inverse-video prompt):

| Key | Action |
| --- | --- |
| Space | Next screen |
| Return | One more line |
| `q` | Leave the pager |
| digit then Return | Select that list number (when numbering is on) |

Slow searches print "Control-C, if you wish to abandon" and keep
dotting (`onalarm`).

### Top menu (`menu5*`, help `etc/dmoption5`)

```text
             1  Maintain organisation entries
             2  Update organisation entry
             3  Change password
             4  Help overview
```

Default is `[1  Maintain organisation entries]`. Option 2 becomes
"Update department entry" when `posdit` is an OU, or "Not available
(at this position)" otherwise. It edits the org/OU **header** (fax,
postal address, ...), not a person.

### Maintain menu (`menu2*`, help `etc/dmoption2`)

```text
             1  List all entries
             2  Read an entry
             3  Add a new entry
             4  Modify an existing entry
             5  Delete an existing entry
             6  View/Change directory position
```

Option 6 is shown only if `change_posdit` is on. Default is
`[1  List all entries]` (or `[3  Add a new entry]` just after
registering a new organisation). Add then asks person / role / room
(`etc/dmoption3`). List prints every person, role, and room under the
current position (`posdit`).

Login is an interview: country, organisation, optional department, then
your name. IDM looks for a `cn=Directory Manager` entry first
(`search_mgr`). Bind is simple credentials (`bind.c`). Without a
tailor `username`, it starts as `"interactive"` and prompts
(`ids.c` `check_credentials`).

## Configuration

Edit `etc/dmtailor` **before** install (copied to `$(ETCDIR)idm/`;
`~/.dmtailor` is tried first). Useful tokens:

- `dsa_address` — ITOT address of the access DSA
- `username` / `password` — skip the interactive bind if both are set
- `posdit` — starting Directory position
- `default_country` / `default_org` / `default_dept`
- `inverseVideo` — `on` / `off` (shipped `off`)
- `commonatt` / `prratt` / ... — attributes to show and edit
- `mapattname` / `mapphone` / `country` — labels, phone rewrite, country
  names
- `addorg`, `init_user`, `init_pswd`, `masterDSA` — registering a new
  organisation

`./make help` / `inst-idm` installs the `etc/dm*` help screens next to
the tailor file.

## Build and run

From this directory, after a tree-wide ISODE build so `libdsap.a` and
`libisode.a` exist:

```text
./make
# edit etc/dmtailor
./make inst-all
idm
```

`xdm` is the build name; `inst-idm` copies it to `$(BINDIR)idm`. The
welcome line is "Welcome to PARADISE - the COSINE Interactive Directory
Manager". Historic contact in `etc/dmbyebye`: PARADISE HelpDesk,
`helpdesk@paradise.ulcc.ac.uk`.
