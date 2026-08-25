# ISODE

The
[ISO Development Environment](https://en.wikipedia.org/wiki/ISO_Development_Environment)
as preserved and maintained by
[Wildboar Software](https://wildboarsoftware.com/en).

This project was recently updated to build and run within an Ubuntu Docker
container and to support `UTF8String` attribute values. There may be additional
changes from the original ISODE code, which will be documented in
[the changelog](./CHANGELOG.md) from here on out.

Wildboar Software publishes Docker containers for running Quipu from this
repository, and might do so for some of the other apps in the future.

If you find any bugs, please report them to the
[issues page](https://github.com/Wildboar-Software/isode/issues).

## Versioning

The `version.*` files within each project folder in this repository will no
longer be maintained. Eventually this will be eliminated.

## Build and Install

If you plan to build or use GDBM-requiring features (or just develop them),
you will need to install the headers for this. On Ubuntu, you can run

```
sudo apt install libgdbm-dev
```

after which, included `<gdbm.h>` headers will resolve.

Either way, on a POSIX system, such as Linux, run:

```shell
cp config/linux.h h/config.h
cp config/linux.make config/CONFIG.make
cp config/*.local support/
./make everything
sudo ./make inst-everything
```

Currently, only the `everything` and `inst-everything` Make targets work. I hope
to change this eventually. `inst-everything` must be run as an administrative
user.

If you are building and installing on a different platform, read the
documentation in [`doc/isode-gen.8.md`](./doc/isode-gen.8.md).

## Source tree

ISODE is layered the way the manuals are. Volumes 1–2 of `doc/volume*.pdf` walk the stack top-down; Volume 3 is FTAM/VT/little services; Volume 4 is the ROSY/PEPY/PEPSY compilers; Volume 5 is Quipu.

| Directory | Role |
| --- | --- |
| [`config/`](config/README.md) | Per-OS Makefile and `config.h` skeletons used before any compile |
| [`compat/`](compat/README.md) | Portability helpers, logging, ISO addresses, and `isotailor` parsing |
| [`dirent/`](dirent/README.md) | Vendored POSIX-style directory reading for historic UNIX |
| [`tsap/`](tsap/README.md) | OSI transport (usually TP0 over TCP / RFC 1006) |
| [`ssap/`](ssap/README.md) | OSI session service |
| [`psap/`](psap/README.md) | Presentation elements and BER encoding, not the presentation protocol |
| [`psap2/`](psap2/README.md) | Connection-oriented presentation protocol |
| [`psap2-lpp/`](psap2-lpp/README.md) | RFC 1085 lightweight presentation over TCP/UDP |
| [`acsap/`](acsap/README.md) | Association Control (ACSE) — how applications bind |
| [`rosap/`](rosap/README.md) | Remote Operations (invoke / result / error) |
| [`rtsap/`](rtsap/README.md) | Reliable Transfer of large APDUs |
| [`ronot/`](ronot/README.md) | RO-BIND / RO-UNBIND mapped onto ACSE |
| [`rosy/`](rosy/README.md) | Stub generator and runtime for `.ry` remote-operations specs |
| [`pepy/`](pepy/README.md) | Historic ASN.1 encoder/decoder compiler (still used by VT and some demos) |
| [`pepsy/`](pepsy/README.md) | Table-driven ASN.1 compiler that replaced pepy/posy for Quipu and SNMP |
| [`support/`](support/README.md) | `tsapd`, `isotailor`, and the ISO aliases/entities/services databases |
| [`dsap/`](dsap/README.md) | X.500 DAP/DSP library, attribute syntaxes, and OID tables |
| [`quipu/`](quipu/README.md) | Quipu DSA (`ros.quipu`) and DISH DAP client |
| [`ftam/`](ftam/README.md) | FTAM protocol library |
| [`ftam2/`](ftam2/README.md) | FTAM client (`ftam`) and server (`ftamd`) |
| [`ftam-ftp/`](ftam-ftp/README.md) | Gateway: FTAM initiator that speaks FTP |
| [`ftp-ftam/`](ftp-ftam/README.md) | Gateway: FTP daemon that speaks FTAM |
| [`vt/`](vt/README.md) | OSI Virtual Terminal (TELNET profile) |
| [`imisc/`](imisc/README.md) | “Little services” ROS demo (time, finger, ping, …) |
| [`snmp/`](snmp/README.md) | 4BSD/ISODE SNMP agent, SMUX, and MIB compiler inputs |
| [`others/`](others/README.md) | Optional demos, MOSY, TS-bridge, and extra Quipu DUAs |
| [`paradise/`](paradise/README.md) | COSINE PARADISE overlay (DE/IDM DUAs and a few Quipu patches) |
| [`include/`](include/README.md) | Symlink so `#include <isode/…>` works against `h/` |
| [`patches/`](patches/README.md) | Historic ISODE 8.0 Linux/compiler patches, not the live build |

Headers live in `h/` (including a generated `config.h`). Docker, Kubernetes, tests, and `util/` are packaging and tooling around this tree rather than OSI protocol code.

## Documentation

The five-volume User's Manual and related notes are in [`doc/`](doc/). Start with [`doc/isode-gen.8.md`](doc/isode-gen.8.md) for configuration, [`doc/volume5.pdf`](doc/volume5.pdf) for Quipu, and the per-directory `README.md` files when browsing source.

## Using the Docker Image for Quipu DSA

The docker image for Quipu DSA is published to
`ghcr.io/wildboar-software/quipu`. You can run it like so below without any
further configuration:

```bash
docker run --rm -it \
     -p 17003:17003/tcp \
     -v $(pwd)/docker/isotailor:/usr/local/etc/isode/isotailor \
     -v $(pwd)/docker/quiputailor:/usr/local/etc/isode/quiputailor \
     -v $(pwd)/docker/db:/usr/local/etc/isode/quipu-db \
     ghcr.io/wildboar-software/quipu:v8.2.1
```

In the above example, we expose TCP port 17003, which is what listens for
ISO Transport Over TCP (ITOT) traffic.

The first volume is the configuration file for OSI networking services. As far
as I can see, the only thing it really does it configure logging. In this
configuration, all logging is turned on, since directories can involve some
debugging out of the box. You can turn the logging volume down in this file;
doing so is highly recommended when you're ready to deploy Quipu DSA.

The second volume is the configuration file for Quipu DSA specifically. In this
file, you can tell Quipu DSA what it's AE Title, where to find superiors,
control replication and caching, and more. Read the documentation in
[`doc/volume5.pdf`](./doc/volume5.pdf) for more information.

The third volume mounted is the database. Note that you CANNOT start with an
empty database. The EDB file MUST be present, and it MUST have an entry defined
for `cn=Toucan` (or whatever you configure your DSA's name to be in
`quiputailor`). Review the documentation on how the `mydsaname` option in
`quiputailor` affects your startup in [`doc/volume5.pdf`](./doc/volume5.pdf).

## To Do

- [ ] Remove `pepy` and `posy` entirely. `pepsy` is the replacement, per Volume 1.
  - `vt` is the only major project using it.
  - All subprojects in `others` seem to use it.
- [ ] Unit Tests (using Unity)
  - [ ] `prim2real`
  - [ ] BCD code
  - [ ] `str2pe`
- [ ] Fuzz Testing
- [ ] `AGENTS.md` file?
- [ ] `.gitattributes` file
- [ ] `.editorconfig` file
- [ ] AI Security Review
- [ ] Clean up IFP Usage
- [ ] Reproducible Builds
  - [ ] Quipu Packaging
    - [ ] Debian
    - [ ] RPM
    - [ ] Snap
    - [ ] Arch
    - [ ] Brew
    - [ ] OCI Container
  - [ ] Dish Packaging
    - [ ] Debian
    - [ ] RPM
    - [ ] Snap
    - [ ] Arch
    - [ ] Brew
    - [ ] OCI Container
    - [ ] Flatpak
- [ ] End-to-End Tests
- [ ] Doxygen Documentation
- [ ] Clang Formatting
- [ ] Sigstore
- [ ] SBOM
- [ ] Ensure that LPP works
- [ ] Ensure that IDM works
- [ ] New LDAP Syntaxes and TIME types