/* dgram.h - datagram (CL-mode TS) abstractions */

#ifndef	_DGRAM_
#define	_DGRAM_

#include <sys/types.h>
#include <sys/socket.h>
#include "manifest.h"

#define	MAXDGRAM	8192

int	join_dgram_aux (const int fd, struct sockaddr *sock, const int newfd);
int	read_dgram_socket (int fd, struct qbuf **q);
int	write_dgram_socket (int fd, struct qbuf *qb);
int	close_dgram_socket (int fd);
int	select_dgram_socket (int nfds, fd_set *rfds, fd_set *wfds, fd_set *efds, int secs);
int	check_dgram_socket (int fd);

#endif
