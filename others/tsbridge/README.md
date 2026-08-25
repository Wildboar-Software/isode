# others/tsbridge — Transport Service Bridge

Bridges OSI transport across different network technologies so an application that speaks TP0/TCP, TP0/X.25, and TP4/CLNP can interwork. Typical use: a dual-homed host on a TCP LAN and an X.25 WAN runs the TSB; RFC 1006 hosts on the LAN can then reach “pure” OSI hosts on X.25.

Volume 2 §8.3 describes client vs server hosts. See `READ-ME` in this directory for the original operational sketch.
