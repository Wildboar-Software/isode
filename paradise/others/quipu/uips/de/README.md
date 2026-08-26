# paradise/others/quipu/uips/de — Directory Enquiries (DE)

Full-screen public-access DUA used as the PARADISE helpdesk/enquiry
interface. ISODE 8.0 also ships DE under `others/quipu/uips/de/`; this
is a later PARADISE-packaged snapshot (User-Friendly Naming / UFN,
[IETF RFC 1484](https://www.rfc-editor.org/rfc/rfc1484.html); locality
fallback; bind password), not a copy-over at build time. Live
`PARADISEDUA` still points at the stock tree. Tree diff versus stock:
[../README.md](../README.md).

Configure `detailor` (DSA address, bind username, default country/org)
before install. `./make termhelp` builds terminal-type help. Run as
`de`; `?` prints online help.

Documented in `doc/isode-8-update.pdf` Appendix A and
`isode-paradise.pdf` Chapter 3. Comments historically went to
`helpdesk@paradise.ulcc.ac.uk`.
