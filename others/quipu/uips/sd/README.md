# others/quipu/uips/sd — Screen Directory (curses DUA)

A full-screen, character-driven Directory User Agent for cursor-addressable
terminals. You sit at a DIT node (the **Search Area**), **List** children or
**Search** by type and value, then type a list number to read an entry or
move into it. Derived from the ISODE 5.0 **widget** interface (Paul Sharpe,
GEC Hirst; curses windows meant to look like X11 buttons/dialogs). Brunel
rewrote that line as SD. No add, delete, or modify.

It talks DAP through `libdsap` (same library as DISH) and links
`-lcurses -ltermcap`. It does **not** link `libdish` and does not offer
DISH commands.

Volume 5 §3.3 lists `sd` as “Screen Directory / A Curses based DUA.”
Chapter 5 is **SID** (“Steve’s Interface to Dish”), a different program:
DISH aliases/scripts, not this curses UI. Chapter 6 is DSC (DE’s ancestor);
chapter 8 is POD. Historic contact: `x500@brunel.ac.uk`.

## How it differs from DE, DISH, and SID

DISH (`quipu/dish/`) is the general-purpose DAP shell: bind, `moveto` a DN,
type protocol operations. Configuration is per-user (`.quipurc`).

DE (`de/`) is a **scripted interview** (`:-` prompts for person / dept / org /
country). The caller never sees a DN or a filter. SD is a **screen DIT
browser**: the Search Area, Type, and Search-for fields stay on screen, and
you pick numbered hits. Both are read-only kiosk-style DUAs; DE hides
navigation, SD *is* navigation.

SID (Volume 5 chapter 5) is easier DISH, not a full-screen program.

| Program | Relationship to SD |
| --- | --- |
| DISH | Full DAP shell. SD hides the protocol behind widgets. |
| DE | Four-prompt interview; no DIT walk. SD lists/searches relative to Search Area. |
| SID | DISH aliases (“Steve’s Interface to Dish”). Not curses; not this tree. |
| FRED | Whois-style command language, not a full-screen UI. |
| POD | X11 Athena-widget DUA (same config *idea* as SD: `filterTypes`, `readTypes`). Needs X11R4. |
| DSC | Volume 5 enquiry UI; DE replaces it. |

## Curses libraries (not ncurses by name)

“Curses” here is the historic Unix **curses** API, as the sources say:

- `READ-ME`: “full screen style interface based on the curses library.”
- `widget.h` / `main.c`: `#include <curses.h>` (`WINDOW`, `initscr`,
  `newwin`, `mvwaddch`, …). No `#include <ncurses.h>` or `<termcap.h>`.
- `Makefile`: `CURSES = -lcurses -ltermcap` (link line and `lint` both
  use `-lcurses`). Nothing named `-lncurses`.
- `widget.c`: BSD curses vs System V. `doupdate()` is compiled only for
  `SYS5` or `_AIX` (“not in BSD curses”). Linux ISODE is `BSD42` +
  `LINUX`, so the BSD `wrefresh` path is used. `crmode()` is the BSD
  name (`cbreak` on System V; ncurses still provides the macro).
- `calls.c`: “hack to get isode/curses compatability” (`WINDOW` vs the
  ISODE type). Comments also note SYS5 symbol clashes with curses.

**Termcap vs terminfo:** SD never calls `tgetent` / `tgoto`. Capability
lookup is inside libcurses. 1980s BSD curses used **termcap**
(`/etc/termcap`), which is why the extra `-ltermcap` is there. System V
curses used **terminfo**. ncurses is a terminfo implementation that still
ships a libcurses / libtermcap compatibility layer.

On Ubuntu / Debian, install the ncurses development package so those
historic names resolve:

```text
sudo apt install libncurses-dev
```

That package provides `<curses.h>`, `-lcurses` (usually a linker script
to libncurses), and a `-ltermcap` stub so this Makefile links. Runtime
already has `libncursesw6` and `ncurses-base` (terminfo) on a typical
desktop image; extra terminal entries are in `ncurses-term` if needed.
`TERM` must name a real type (`vt100`, `xterm`, …). PAD logins often
arrive as `TERM=network` — that is why `X29/startup` runs `tset` before
`xsd`.

You do not need a separate `libtermcap-dev` for a normal Linux build.
You do need `libdsap.a` / `libisode.a` from a tree-wide
`./make everything`, and `bison` (`YACC = bison -y`) for `conf_read.y`.

## What the screen looks like

There are **no screenshots** in this directory (including `X29/`). The
frames below are a reconstruction from `wdgtdefs.h` (widgets and labels),
`widget.c` `boxwdgt()` (`.` / `-` / `` ` `` / `'` boxes, height 3),
`posnwidgets()` on **80×24**, `printbar()` (`]` top, `[` bottom, `*`
thumb), `scrollbar()` numbering, `as_print(..., READOUT)`
(`%-21s - value`), and the worked example in `sd.1c`. They are not a
captured session; curses will reflow if `COLS`/`LINES` differ, and
adjacent widgets keep both borders (`||`).

Chrome is four rows of 3-line widgets (title, commands, Search Area, type
+ search). A 3-column scrollbar occupies the remaining left strip. The
text pane is `newwin(LINES-1-lowy, COLS-3, lowy, 3)` — on 80×24 that is
11 lines starting at row 12, column 3. Search Area is display-only
(`callch` is `'\0'`); you change it by List / Widen / History / a list
number, not by typing into the field.

List after Search Area `GB, Brunel University.`, Type `Person`, Search
for `Plum` (`sd.1c`):

```text
.------------------------------------------------------------------------------.
|                        QUIPU X.500 Screen Directory.                         |
`------------------------------------------------------------------------------'
.-------..-------..-------..-------------------..----------..------------------.
|q Quit ||h Help ||l List ||w Widen Search Area||b History ||Go To Number:     |
`-------'`-------'`-------'`-------------------'`----------'`------------------'
.------------------------------------------------------------------------------.
|Search Area:  GB, Brunel University.                                          |
`------------------------------------------------------------------------------'
.--------------------..--------------------------------------------------------.
|t Type: Person      ||s Search for:  Plum                                     |
`--------------------'`--------------------------------------------------------'
.-. 1 Computer Science, Polonius Plum
|]| 2 Law, Plumley Farquarson
|*|<Total of 2 entries>
|*|
|*|
|*|
|*|
|*|
|*|
|*|
|[|
`-'
```

Typing `1` (then Return) reads that entry. Leaf nodes keep Search Area
where it was; non-leaves become the new area. Attribute lines are
`as_print` READOUT (`sd.1c` text, 21-column names):

```text
Result of look up ...
commonName            - Polonius Plum
surname               - Plum
userClass             - staff
rfc822Mailbox         - P.Plum@brunel.ac.uk
description           - "ruddy faced and gat toothed"
```

`h` or `?` replaces the command row with help buttons and loads
`sd/helpdir/*` into the text pane (prefix from `help.c`: “SD X.500
Directory Agent - Concise Help”). Help chrome:

```text
.------------------------------------------------------------------------------.
|                      QUIPU X.500 Screen Directory: Help                      |
`------------------------------------------------------------------------------'
.-----------..------------..----------..-------------------..-------..---------.
|q QUIT Help||s Search for||b History ||w Widen Search Area||l List ||n Number |
`-----------'`------------'`----------'`-------------------'`-------'`---------'
```

### Keys (`sd/helpdir/help`, `interact()`)

| Key | Widget | Action |
| --- | --- | --- |
| `q` | Quit | Unbind and `endwin`. |
| `h` or `?` | Help | Help widget set + `helpdir` files. |
| `l` | List | One-level children of Search Area (DSA entries filtered out). |
| `w` | Widen | Strip one RDN from Search Area (`GB, Brunel…` → `GB`). |
| `b` | History | Numbered list of nodes visited (`history` cap, default 20). |
| `t` | Type | Cycle types allowed at this DIT level (`typeDefaults`). |
| `s` | Search for | Prompt for a value, then search. |
| Return | (dummy) | Search; if the value is empty, List instead. |
| `1`–`9` | Go To Number | Finish the number in the dialog; read/move to that hit. |
| `[` / `]` | Scrollbar | Page the text pane (`[` toward later lines, `]` toward earlier). |
| Ctrl-L | — | Full redraw. |
| Ctrl-C | — | Quit at the idle prompt; “Interrupted” during a callback. |

In a dialog: backspace/DEL deletes a character; Ctrl-U clears the field.
A leading `<` means the string is scrolled.

## Configuration

Per-user `$(HOME)/.duaconfig/` else `$(ETCDIR)/sd/duaconfig/` (`sd.5`).
`-test` forces `./sd/duaconfig/` from the source directory.

| File | Role |
| --- | --- |
| `readTypes` | Attribute OIDs to fetch on Read (quoted name ignored). |
| `filterTypes/Type_*` | Named search types (Person, Place, Department, Organization). Lisp-like filters; `*` is the value typed at search time. |
| `typeDefaults` | Per-RDN-OID which types are offered and which is default. `@` is the root (“The World”). |

`$(HOME)/.duarc` (optional): `username`, `password`, `prefergreybook`,
`dsap`, `isode`, `service`, `history`. Initial Search Area is
`local_dit` from the DSAP tailor, or `The World`. Bind identity is that
username (or `-u` / `-p`), not whoever sits at the terminal — same idea
as DE’s public-access bind, but the file is `.duarc` not `detailor`.

Default Person filter in this tree (`Type_person`) is objectClass=person
AND (cn or title approximately equal the typed value). `sd.5`’s Person
example also mentions surname; the shipped file does not.

## Build and run

From this directory, after a tree-wide ISODE build so `libdsap.a` and
`libisode.a` exist:

```text
./make
./xsd -test          # must be run from this directory; still binds a DSA
./make inst-all      # or ./make install (inst-all + clean)
sd
```

`xsd` is the build name; `inst-sd` copies it to `$(BINDIR)sd`. Flags
actually parsed in `read_args()` (the man page only documents `-c`, and
that blurb still says “Pod”):

| Flag | Meaning |
| --- | --- |
| `-u DN` | Bind name. |
| `-p password` | Bind password. |
| `-c name` | DSA name (`myname`). |
| `-T file` | OID table. |
| `-t file` | Tailor file (unless the token is `-test`). |
| `-test` | Use `./sd/duaconfig/` ; still needs a reachable DSA. |

## X.29 public access (`X29/`)

Files to run SD as a PAD service via Cambridge `x29d` (mail
`info-server@cl.cam.ac.uk` for that listener; not in this tree). Brunel
used a dedicated UID and `/usr/local/lib/public-widget`. `startup` logs
the call, asks for a terminal type (`vt100`, `vt52`, `tvi`, `cif2605`,
`xterm`), optionally `resize` for xterm, then runs `./xsd`. `READ-ME`
says telnet-on-a-special-port was not tested. `X29/CSHRC` and
`SITE_CSHRC` are ordinary csh profiles, not UI docs.

## Manuals

`sd.1c` (how to use it), `sd.5` (config syntax). Widget library comments
in `widget.c` / `widget.h`. Authors on the man pages:
`Damanjit.Mahl@brunel.ac.uk`, `Andrew.Findlay@brunel.ac.uk`.
