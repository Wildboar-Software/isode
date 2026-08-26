# others/rtf — Reliable-transfer file copy

Small file-transfer utility (`rtf` initiator, `rtfd` responder) used to
test RTS upcalls. The man pages call it “the TFTP of OSI” and tell you to
use FTAM instead. ASN.1 is `rtf.py` (PEPY/PEPSY input, not Python). See
the `rtsap/` README for the service this sits on.

`others/Makefile` lists `rtf` in `OTHERS` (cleaned with the rest) but not
in `ALL`, so even this directory’s `everything` target does not build it.
After the ISODE libraries exist, compile from here.

## Authentication and `-lcrypt`

The Request APDU carries `user`, `password`, and `file` (`rtf.py`).

- `rtf.c` (initiator) prompts for a password and sends it in the
  request. It does not call `crypt()`.
- `rtfd.c` (responder) looks the user up with `getpwnam()`. For a
  non-guest login it hashes the supplied password with `crypt(3)` and
  compares that to the hash from the passwd entry — password hashing for
  the demo, not a security product. Guest/anonymous (`ftp`, or a file
  named `ANON`) skips that check.
- `rtfsbr.c` is shared logging and RTS abort helpers. It does not call
  `crypt()`.

The Makefile still links both binaries with **`-lcrypt`**:

```make
LIBES	=	$(LIBISODE) -lcrypt
```

### What libcrypt is

Historic Unix **libcrypt** is the library behind `crypt(3)`: the
traditional password-hashing function (originally a DES-based hash from
Seventh Edition Unix). Some C libraries put `crypt()` in libc; others
split it into a separate **libcrypt** so the link line is `-lcrypt`.
POSIX still documents it that way: `<crypt.h>` (and/or `<unistd.h>`) plus
`-lcrypt`.

It still exists. On modern GNU/Linux it is no longer compiled into
glibc. glibc dropped its `crypt()` implementation; the replacement is
[libxcrypt](https://github.com/besser82/libxcrypt). Distros ship that as
`libcrypt.so.1`, so the historic linker flag **`-lcrypt` still works**.
The header is still `<crypt.h>`.

| Distro | Development package | Notes |
| --- | --- | --- |
| Debian / Ubuntu | `libcrypt-dev` | Binary name is `libcrypt-dev`; source package is `libxcrypt`. Provides `libcrypt1-dev`. Ships `<crypt.h>`, `libcrypt.so`, and `libxcrypt.pc`. There is no `libxcrypt-dev` binary package on Ubuntu 24.04. |
| Fedora | `libxcrypt-devel` | Same library; the RPM name matches upstream. |

Debian/Ubuntu:

```shell
sudo apt install libcrypt-dev
```

Fedora:

```shell
sudo dnf install libxcrypt-devel
```

The top-level `README.md` only calls out `libgdbm-dev` (Quipu TURBO /
GDBM). It does not mention libcrypt. On Ubuntu 24.04, `libcrypt-dev` is
a build-essential package (source: `libxcrypt`) and `libcrypt1` is in
main; this development image already has both (`<crypt.h>`,
`libcrypt.so`, `libcrypt.a`), so RTF needs no extra install step here.
A minimal system that lacks the C development files may still need
`libcrypt-dev` before `-lcrypt` will resolve.

`libcrypt` (`crypt(3)`) is unrelated to OpenSSL’s `libcrypto` and to
`libcryptsetup` (disk encryption).
