# ftp-ftam — FTP daemon talking to FTAM

The other half of the FTAM/FTP gateway: an FTP server (`ftpd.c`, `ftpcmd.y`) that performs the requested file operations by acting as an FTAM initiator toward an OSI filestore.

Installs as `ftpd-ftam`. Pair with `ftam-ftp/` for the reverse mapping. Built by `all-ftam-ftp`.
