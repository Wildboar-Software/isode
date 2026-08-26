# paradise/others — PARADISE DUAs

COSINE (Cooperation for Open Systems Interconnection Networking in
Europe) was the EC/EUREKA OSI networking programme; PARADISE was its
X.500 Directory pilot. This overlay’s “COSINE user interfaces” are the
pilot’s Directory DUAs under [quipu/uips](quipu/uips/README.md) —
[DE](quipu/uips/de/README.md) (`de`, Directory Enquiries: public-access
lookup) and [IDM](quipu/uips/idm/README.md) (`idm`, Interactive
Directory Manager: organisation-account add/modify/delete) — enquiry
versus management clients, not a GUI toolkit. The original tarball
replaced stock `others/quipu/uips/de/` and added `idm/`; it did not
overlay FRED, SD, POD, or Doog. This repo keeps both DE copies and
does not apply the overlay at build time — see
[quipu/uips/README.md](quipu/uips/README.md).
