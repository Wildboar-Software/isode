/* dgram.h - datagram (CL-mode TS) abstractions */

/*
 * $Header: /xtel/isode/isode/h/RCS/dgram.h,v 9.0 1992/06/16 12:17:57 isode Rel $
 *
 *
 * $Log: dgram.h,v $
 * Revision 9.0  1992/06/16  12:17:57  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#ifndef	_DGRAM_
#define	_DGRAM_

#include <sys/types.h>
#include <sys/socket.h>
#include "manifest.h"

#define	MAXDGRAM	8192

int	join_dgram_aux (int fd, struct sockaddr *sock, int newfd);
int	read_dgram_socket (int fd, struct qbuf **q);
int	write_dgram_socket (int fd, struct qbuf *qb);
int	close_dgram_socket (int fd);
int	select_dgram_socket (int nfds, fd_set *rfds, fd_set *wfds, fd_set *efds, int secs);
int	check_dgram_socket (int fd);

#endif
