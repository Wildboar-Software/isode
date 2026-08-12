# protoize converts K&R-style function definitions to ANSI C function definitions
# How to use this docker container to protoize the pepsy code:
#  podman build -f ./protoize.dockerfile -t protoize .
#  podman run -it --rm -v $(pwd):/work protoize /bin/bash
#  cd pepsy
#  protoize -N -c "-DLINUX -I../h" *.c
FROM docker.io/dockette/debian:wheezy
RUN apt-get update -y --force-yes && apt-get install --force-yes -y gcc protoize
RUN mkdir -p /work
WORKDIR /work
