# others/quipu/uips — Directory User Agents

User interfaces on top of `libdsap` / DISH. Historic `READ-ME` expected DISH to
have the broadest appeal; FRED was the White Pages UI; DE was PARADISE public
access.

| Directory | Interface |
| --- | --- |
| [dish/](dish/README.md) | Extra DISH features (auto `.quipurc`) |
| [fred/](fred/README.md) | FRED / whois White Pages DUA |
| [pod/](pod/README.md) | X11 Athena-widget DUA |
| [sd/](sd/README.md) | Curses “full screen” DUA |
| [de/](de/README.md) | Directory Enquiries (PARADISE) |
| [doog/](doog/README.md) | User-Friendly Naming (UFN; [IETF RFC 1484](https://www.rfc-editor.org/rfc/rfc1484.html)) DUA (pre-beta) |
| [manage/](manage/README.md) | `dish_manage` alias-management DISH |

`./make` in this directory builds all of them; many need extra libraries
(curses, X11).
