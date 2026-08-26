# dirent — Portable directory-reading routines

A public-domain, nearly POSIX implementation of `opendir` / `readdir` /
`closedir` and friends, originally by Douglas A. Gwyn. Historic UNIX C libraries
did not agree on directory layout; this package hides those differences behind
`struct dirent`.

ISODE vendors this copy so the rest of the tree can walk directories portably on
BSD, System V, and NFS. On modern Linux the system libc already provides these
calls, but the library is still built as `libdirent`.

See `NOTES` in this directory for the original portability discussion (name
change from `struct direct` to `struct dirent`, `d_namlen` vs `strlen`, and so
on).
