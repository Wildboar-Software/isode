# others/quipu/uips/fred — FRED (FRont-End to Dish)

Whois-style Directory User Agent for the **NYSERNet White Pages**
pilot: look up people and organisations in the OSI Directory without
speaking DAP. Volume 5: “a DUA optimised for White Pages queries”,
implemented as a front-end to `dish` — hence the name. Marshall T.
Rose (NYSERNet, then PSI); `fred.1c` is dated 6 Jan 1990.

Unlike DISH (`search`, `list`, `moveto`, …) and unlike DE’s four
interview prompts, FRED’s language is the old Unix **whois** /
**finger** style: `whois name`, handle, mailbox, area.

Install also hard-links (or copies) the same binary as
`in.whitepages` for WHOIS-port access. `fredrc` / `ufnrc` are
configuration; `fredsh` (TELNET guest shell) and `dad` (Directory
Assistance Daemon) are the helpers described below.

## What “whois-style” means

The NIC WHOIS service (Unix `whois`; NICNAME/WHOIS,
[RFC 954](https://www.rfc-editor.org/rfc/rfc954.html), typically
TCP 43) is a one-line lookup: you type a name, a short handle, or a
mailbox, and get a listing. FRED copies that **command language**,
not the old centralised NIC database.

Typical session (`fred.1c`, `doc/quick.pdf`):

```text
fred> whois schoffstall -org psi
Trying @c=US@o=Performance Systems International ...
3 matches found.
  2. Marvin Schoffstall                         marv@psi.com
  3. Martin Schoffstall                         schoff@psi.com
  4. Steve Schoffstall                          steve@psi.com

fred> whois !3
Martin Schoffstall (3)                          schoff@psi.com
    ...
```

`whois` takes, in any order, an **input field**, optional **record
type**, **area designator**, and **output control**:

| Piece | What you type |
| --- | --- |
| Input | `name` / surname / fullname; `handle` or `!N` (DN or session alias); `mailbox` |
| Record | `person` (`-title`), `organization`, `unit`, `role`, `locality`, `dsa` |
| Area | `-org NAME`, `-unit`, `-locality`, or `-area HANDLE` (optionally `-geo`) |
| Output | `expand` (`*`), `subdisplay` (`%`), `full` (a `|`), `summary` (`$`) |

Bare tokens are treated as a name unless they contain `@` (then
handle or mailbox). Session **aliases** (`!3`) stand in for long
Distinguished Names — NIC WHOIS used short handles such as `MTR`;
X.500 uses DNs, so FRED numbers entries as you retrieve them.
**Areas** replace a whole-database scan: searches start at a DIT
subtree (default from `fredrc`). `NAME.` is prefix match (`NAME*`)
for WHOIS compatibility.

That is a **command language**, not DE’s four prompts (person / dept
/ org / country) and not DISH’s DAP verbs. `fred.1c` is explicit:
FRED is “purposefully limited”; people who need a more powerful
interface should use `dish` directly.

The same binary also **emulates the WHOIS network service**. If
`argv[0]` is `in.whitepages`, FRED sets network mode: one query line
from stdin (CRLF), no `.fredrc`, no `fred>` prompt, reply on stdout.
`doc/administrator.pdf` §2.8.2 has `inetd` run that binary as
`whois` (TCP 43, so ordinary `whois(1)` clients work) and/or as
`whitepages` on 17005/tcp. That is RFC 954-style NICNAME/WHOIS over
the wire, answering from Quipu rather than the NIC database. The
emulation is imperfect (`fred.1c` BUGS: extra quoting).

## The NYSERNet White Pages pilot

`doc/user.pdf` is *NYSERNet White Pages Pilot Project: User’s
Handbook* (Marshall T. Rose, NYSERNet, May 1990). The matching
administrator’s guide is `doc/administrator.pdf`. Volume 5 chapter 7
is the ISODE-manual cut of the same FRED material (by 1991 the
banners say **PSI** White Pages; `fredsh.sh` still greets “PSI White
Pages Pilot Project”, and `whitepages.sh` calls `wp.psi.net`).

The early Internet used a **centralised WHOIS** database (~70,000
entries in the handbook). That does not scale to hundreds of
thousands of users in thousands of organisations: one handle space,
one search of the whole file, one administration. NYSERNet sponsored
a 1990 pilot to put **internet-scale white pages on the OSI
Directory** (X.500 / Quipu) instead:

- Each participating organisation runs its own Directory Management
  Domain (usually one DSA).
- People are identified by Distinguished Names, not three-letter
  NIC handles.
- Searches are relative to an **area** (a DIT subtree), like
  telephone white pages per locality — not a scan of every entry.
- The DIT holds typed attributes (mailbox, telephone, postal
  address, photograph, …), not a free-text WHOIS blob.

This is **not** the NIC whois service with a new front-end. It is a
distributed X.500 directory whose novice UI happens to speak whois.

### Why it needs its own DUA

DISH is a general DAP shell: bind, `moveto` a DN, type protocol
operations. A White Pages user does not know DNs, filters, or which
DSA holds the entry. The **product** of the pilot is that novice
path:

- a DIT laid out as country / organisation / unit / person;
- **areas** so a query has a place to start;
- User-Friendly Naming (UFN;
  [IETF RFC 1484](https://www.rfc-editor.org/rfc/rfc1484.html))
  when `namesearch` is `friendly` (FRED’s default): comma-separated
  untyped names such as `rose, psi` rather than
  `@c=US@o=…@cn=…`;
- the whois command language, session aliases, and `area`;
- the same client over **login**, **TELNET**, **WHOIS**, and
  **mail**, so sites whose users have no ISODE binary can still
  query.

A generic DISH session does none of that. DE (PARADISE) is the other
novice extreme: a scripted interview, not a whois language, and not
this US/NYSERNet DIT or access story.

| Program | Relationship to FRED |
| --- | --- |
| DISH | Full DAP shell; FRED drives it over a pipe (`dish -pipe`). |
| DE | Four prompts, PARADISE kiosk; FRED is commands + areas. |
| SD / POD | Curses / X11 browsers; FRED is line-oriented whois. |
| Doog | Standalone UFN client; FRED can use UFN inside `whois`. |

Interactive FRED also has `edit` (own entry), `thisis`, `report`
(mail the local manager), and `manual`. Network/mail sessions are
read-only: only `whois`, `area`, `help`, and `manual`.

## `fredsh` and `dad`

Both are built and installed from this directory. They are not
alternate DUAs; they wrap FRED / DISH for the access methods the
pilot actually shipped.

### `fredsh` — TELNET login shell

`fredsh.sh` (installed as `SBINDIR/fredsh`, with `@(BINDIR)`
substituted) is the **guest-account shell** for White Pages via
TELNET (`doc/user.pdf` §2.1.2, `doc/administrator.pdf` §2.8.1).

Passwd line from the administrator’s guide:

```text
fred::30:30:Anonymous White Pages User:/:$(SBINDIR)fredsh
```

A caller `telnet`s to the White Pages host and logs in as `fred`.
The script prints the PSI/NYSERNet welcome, optional X11 `DISPLAY`
prompt (if `TERM` is `xterm` and `BINDIR/xwp` exists it may exec
`xwp -D` instead of `fred`), then:

```text
exec /bin/csh -ic "@(BINDIR)fred -a -l -r"
```

`-a` inactivity alarm, `-l` log the session, `-r` read-only (pager
`internal`). `DISHDRAFT` is set to a per-process temp file. That is
how “White Pages via TELNET” is just FRED with a banner and
restricted flags — not a second protocol.

### `dad` — Directory Assistance Daemon

`dad.c` / `dad.8c`: a **lightweight TCP helper** so other programs
can use the Directory. The man page is explicit: it “helps other
programs use the Directory, it doesn’t directly help people find
things.” Start it from `/etc/rc.local` (`dad -p portno` optional).

What the code does:

- Listens on TCP service `da` (port **411** if `/etc/services` has
  no `da/tcp`).
- Drops privileges to passwd user `fred` when that account exists.
- Forks `dish -pipe -dad -fast` as the real DUA (`-dad` in DISH
  shortens idle/cache timers).
- On connect, replies `+OK` plus the `DISHPROC` socket; then accepts
  control lines `INTR`, `QUIT`, `STAT` (`+OK` / `-ERR`).
- Reads system `fredrc` and pre-issues `moveto` for each `area`
  line so the dish is already sitting in the default White Pages
  subtree.

FRED itself (`pipe.c`) either forks dish locally (`set server` =
`internal`, the default) or opens a TCP control connection to
**dad** when `server` is a host/`host:port`. MH 6.5 patches in
`mh-patches` use the same daemon: `<< rose, psi, us >>` in a draft
is expanded via dad. So dad is the **shared dish-on-a-port** for
FRED and mail, not a people-finder UI.

## Configuration

On start-up FRED reads `ETCDIR/fredrc`, then `~/.fredrc` (unless
`-f` / network mode). `ufnrc` is the system User-Friendly Naming
tailor file (`init_ufnrc()`). Sample `fredrc` sets per-record-type
`area` roots (`@c=…@o=…`) and `set manager` (mailbox for `report`).

`set` variables (see `fred.1c`): `namesearch`
(`fullname`/`surname`/`friendly`), `soundex`, `timelimit`, `ufn`
(`none`/`approx`/`wild`), `server` (dad host), `watch` (dish
dialogue), `pager`, `phone`, `query`, `verbose`.

Optional `ETCDIR/fredmap` (administrator’s guide §2.8.4) maps
IP-address to a bind DN/password so local or `in.whitepages` callers
can bind without typing `thisis`.

## Build and run

Top-level `./make everything` (or `others/quipu` `pilot` /
`inst-pilot`, which includes this directory). Locally:

```text
./make all
# edit fredrc / ufnrc
./make inst-all
fred
fred whois "smith" -org psi
```

`inst-all` installs `fred`, `in.whitepages`, `fredrc`, `ufnrc`,
`fredsh`, `dad`, and the man pages. `whitepages` (`whitepages.sh`:
`whois -h wp.psi.net`) is built but **not** in `inst-all`
(`inst-whitepages` is commented out). Mail access is `fred -m` from
a sendmail alias (`whitepages: "|…/fred -m"`); the Subject or body
is the whois query.

Network WHOIS (`doc/administrator.pdf` §2.8.2):

```text
# /etc/services
whitepages 17005/tcp
# inetd / /etc/servers — TCP 43 for ordinary whois(1) clients
whois      tcp  $(SBINDIR)in.whitepages
whitepages tcp  $(SBINDIR)in.whitepages
```

## Documentation

| Source | What |
| --- | --- |
| `fred.1c`, `dad.8c` | Command and daemon man pages |
| `doc/user.pdf` | NYSERNet White Pages User’s Handbook |
| `doc/quick.pdf` | Whois quick-reference sheet |
| `doc/administrator.pdf` §2.8 | TELNET, WHOIS, mail, `fredmap` |
| Volume 5 chapter 7 | FRED in the ISODE manual |
