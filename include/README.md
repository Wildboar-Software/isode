# include — Compile-time include path

Not a source library. `include/isode` is a symlink to `../h`, so compilers can use `-Iinclude` and write `#include <isode/acsap.h>` the same way installed headers work under `/usr/include/isode`.

Real headers live in `h/` (gitignored copies of `config/*.h` land there as `h/config.h`). There is nothing to edit here.
