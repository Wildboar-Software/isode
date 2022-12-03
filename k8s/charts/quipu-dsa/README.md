# Quipu DSA Helm Chart

Before you can use this Helm Chart, you'll probably want to configure your DSA.

1. Create a `quipu-dsa` container, but with your volume mounted somewhere other
   than `/usr/local/etc/isode`, and with `command` `/bin/bash`.
2. Edit the configuration and data in `/usr/local/etc/isode`.
3. `cp /usr/local/etc/isode/* <your mounted volume>`.
4. Kill the container. You now have seeded your volume as you'd like.
