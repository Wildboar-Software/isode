# ISODE

The [ISO Development Environment](https://en.wikipedia.org/wiki/ISO_Development_Environment)
as preserved and maintained by [Wildboar Software](https://wildboarsoftware.com/en).

This project was recently updated to build and run within an Ubuntu Docker
container.

Wildboar Software publish Docker containers for running Quipu from this
repository, and possibly some of the other apps in the future.

If you find any bugs, please report them to the
[issues page](https://github.com/Wildboar-Software/isode/issues).

## Build and Install

On Linux, run:

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

## Documentation

Documentation can be found in the `doc` folder.

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
