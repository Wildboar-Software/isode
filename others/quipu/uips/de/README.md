# others/quipu/uips/de — Directory Enquiries (DE)

A prompted, read-only Directory User Agent for **public enquiry**: look up a
person, department, organisation, or country without knowing X.500, DNs, or
DAP. COSINE/PARADISE used it as the helpdesk / kiosk interface (the
`:-` prompt). Successor to **DSC** (Volume 5 chapter 6), not a wrapper
around DISH.

This is the copy the top-level `paradise-dua` target builds
(`PARADISEDUA = others/quipu/uips/de`). A PARADISE-packaged tree also
lives at `paradise/others/quipu/uips/de/`.

It talks DAP through `libdsap` (same library as DISH) and links
`-ltermcap`. It does **not** link `libdish` and does not offer DISH
commands.

## How it differs from DISH and the other DUAs

DISH (`quipu/dish/`) is the general-purpose DAP client: you bind as a
Directory user, `moveto` a DN, and type protocol operations (`search`,
`list`, `add`, `modify`, `delete`, `compare`, …). It is aimed at
administrators and people who already understand the DIT. Configuration
is per-user (`.quipurc`).

DE is the opposite extreme: a **scripted interview**. The caller never
sees a DN or a filter. Typical session:

```text
Person's name, q to quit, * to list people, ? for help
:- barker
Dept name, * to list depts, <CR> to search all depts, ? for help
:- cs
Organisation name, <CR> to search `ucl', * to list orgs, ? for help
:-
Country name, <CR> to search `gb', * to list countries, ? for help
:-
```

Empty fields take the administrator’s defaults (`default_org`,
`default_country`). Several matches become a numbered pick-list. Blank
person/dept searches return organisation or department entries instead.
`?` is online help (written for sites with no paper manual). Ctrl-C
abandons the query.

DE does not add, delete, or modify entries. It binds as a **single
configured identity** (`username` in `detailor`, often a “Public Access
Dua” entry you must create in the DIT), not as whoever is sitting at the
terminal. Tailoring is **system-wide** (`ETCDIR/de/detailor`), not
`.quipurc`.

| Program | Relationship to DE |
| --- | --- |
| DISH | Full DAP shell for experts/admins. DE hides that protocol. |
| `others/quipu/uips/dish` | Extra DISH bits (auto `.quipurc`), still DISH. |
| `dish_manage` | DISH plus alias add/delete. |
| FRED | Whois-style White Pages (`whois`, aliases, areas); also `in.whitepages` for TELNET/WHOIS. Command language, not four prompts. NYSERNet pilot (`doc/user.pdf`). |
| SD | Full-screen curses DIT browser (ISODE 5 “widget” line). You navigate entries; DE only interviews for a search. SD can be a public PAD service via `x29/`. |
| POD | X11 novice DUA: read/list and some modify; no add/rename. Needs X11R4. DE is terminal/kiosk. |
| Doog | Incomplete UFN client (async `libdsap`). Not a kiosk UI. |
| IDM | PARADISE **management** DUA (add/modify/delete people and OUs). Same prompt style as DE, opposite purpose. See `paradise/others/quipu/uips/idm/`. |
| DSC | Older enquiry UI in Volume 5; DE replaces it. |

## Configuration

Edit `detailor` **before** install. Help files and the tailor file go to
`ETCDIR/de/`. Highly recommended:

- `dsa_address` — ITOT address of the access DSA (second line is a
  backup; further lines ignored). If omitted, the first `dsaptailor`
  DSA is used.
- `username` — bind DN; create that entry in the Directory.
- `default_country` / `default_org` / `default_dept` — RETURN defaults
  at the prompts (`default_dept` is usually empty for public access).

Other `detailor` knobs (see Appendix A and `TAILORING`): which
attributes to show (`commonatt`, `prratt`, …), friendly names
(`mapattname`), local phone rewriting (`mapphone`), UK grey-book mail
order, inverse video, search logging (`delogfile`, `logLevel`), and
alarms for slow remote searches.

`./make termhelp` builds terminal-type help from `/etc/termcap` (may
fail on some System V hosts). Without terminfo, DE assumes 80×24 and
can fall back to “dumb”.

## Build and run

```text
./make all
./make termhelp          # optional
# edit detailor
./make inst-all
de
```

At `:-`, type `?` for help, `q` to quit. Historic contact:
`helpdesk@paradise.ulcc.ac.uk`.

User/admin write-up: `doc/isode-8-update.pdf` Appendix A;
`paradise/isode-paradise.pdf` Chapter 3; `de.1` in this directory.
