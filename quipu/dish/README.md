# quipu/dish — Directory Shell (DAP client)

DISH is the command-line Directory User Agent: bind, moveto, showentry, list,
search, add, delete, modify, compare, and a `fred` subcommand for white-pages
queries.

Volume 5 chapters 4–5 document the commands, `.quipurc`, sequences, and running
DISH from the shell. The built binary is `xdish` (installed as `dish`). Extra
DISH helpers (auto `.quipurc` creation) live in `others/quipu/uips/dish/`.

Suggested `.quipurc` from Volume 5 §5.6 (QUIPU Profile). `username` / `me` are
the bind DN; `us` is a nickname; `moveto` / `showentry` set default flags.
`dishinit` (`others/quipu/uips/dish/dishinit`) copies the same `notype` and
timing lines into a new user's file.

```text
username:c=GB@o=University College London@ou=Computer Science@cn=Steve Kille
me:c=GB@o=University College London@ou=Computer Science@cn=Steve Kille
password:steve

position: @c=GB@o=University College London@ou=Computer Science
notype: acl
notype: treestructure
notype: masterdsa
notype: slavedsa
notype: objectclass
notype: lastmodifiedby
notype: lastmodifiedtime
notype: userpassword
cache_time: 30
connect_time: 2

us: c=us
moveto: -pwd
showentry: -name
```

## Layout

| File | Role |
| --- | --- |
| `dish.c` | Main program |
| `bind.c`, `move.c`, `read.c`, `list.c`, `search.c`, … | One file per DISH command |
| `dishlib.c`, `dishhelp.c` | Shared command loop and help |
| `filteritem.c`, `get_filter.c`, `get_ava.c` | Filter / AVA parsing |
| `fred.c` | Built-in FRED-style whois |
| `pipe.c` | Talking to a DISH server over a pipe |
| `user.c`, `edit.c` | User identity and `editentry` |

`libdish.a` is also linked by `others/quipu/uips/manage` (`dish_manage`). GNU
Readline can be enabled by uncommenting `LGNUREADLINE` in the Makefile.
