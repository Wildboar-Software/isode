# others/quipu — Quipu extras (DUAs, photos, sample DIT)

Everything Directory-related that is not the DSA (`quipu/`) or `libdsap` (`dsap/`). Sample databases, photo renderers, DSA configurator, and the family of user interfaces (FRED, POD, DE, SD, Doog, …).

The default `others` Makefile builds `photo` and `image` here; `make pilot` / `inst-pilot` installs sample DIT files via `tools/dsaconfig`.

## Subdirectories

| Directory | Role |
| --- | --- |
| [uips/](uips/README.md) | Directory User Agents (DISH extras, FRED, POD, DE, SD, Doog, manage) |
| [photo/](photo/README.md) | G3 fax / photo attribute display programs |
| [image/](image/README.md) | Face-from-Directory helpers (`xwho`, …) |
| [quipu-db/](quipu-db/README.md) | Example EDB trees (organisation, national, non-root) |
| [tools/](tools/README.md) | `dsaconfig`, `dsastats`, DM tools, AEI scripts |
