# ftam-ftp — FTAM initiator talking to FTP

One half of the FTAM/FTP gateway (Volume 3). This side is an FTAM initiator that
drives a local FTP conversation (`ftp.c`, `ftp_lib.c`) so an OSI FTAM user can
reach an Internet FTP server.

The opposite direction is `ftp-ftam/`. Built by the top-level `all-ftam-ftp`
target. Berkeley UNIX only, historically.
