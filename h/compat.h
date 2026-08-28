#ifndef COMPAT_H
#define COMPAT_H
#include <sys/select.h>
#include <sys/socket.h>

int	xselect (int nfds, fd_set* rfds, fd_set* wfds, fd_set* efds, const int secs);
int chkpassword (const char *usrname, const char *pwpass, const char *usrpass);
int hack_dgram_socket (const int fd, struct sockaddr *sock);

#endif /* COMPAT_H */
