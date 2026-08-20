#ifndef COMPAT_H
#define COMPAT_H
#include <sys/select.h>
#include <sys/socket.h>

int	xselect (int nfds, fd_set* rfds, fd_set* wfds, fd_set* efds, int secs);
int chkpassword (char *usrname, char *pwpass, char *usrpass);
int hack_dgram_socket (int fd, struct sockaddr *sock);

#endif /* COMPAT_H */
