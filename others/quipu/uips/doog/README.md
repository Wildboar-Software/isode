# others/quipu/uips/doog — User-Friendly Naming DUA

Pre-beta Directory User Agent for Steve Kille's User-Friendly Naming (UFN)
as described in [IETF RFC 1484](https://www.rfc-editor.org/rfc/rfc1484.html):
type an approximate human name instead of an X.500 distinguished name. Uses
asynchronous `libdsap`. Included as example async Directory code, not a
finished product (no help pages, limited status reporting).

Build `xdoog`; `./xdoog -test` reads `doogrc` from the current directory. Sample
`config.*` files can replace `doogrc`. `query/` holds the query engine and a
longer UFN explanation. See `README`.
