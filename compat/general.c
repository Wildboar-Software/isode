/* general.c - general utilities for emulation of 4.2BSD */

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "general.h"
#include "manifest.h"

/*    Berkeley UNIX: 4.2 */

#ifdef	BSD42

/* Simply including "general.h" is sufficient. */

static int	_general_stub (void);
static int	_general_stub (void) {}

#endif

/*    non-Berkeley UNIX */

#if	!defined(BSDLIBC) || defined(BSD44)

#ifndef	lint

#include <search.h>

void insque (struct qelem *elem, struct qelem *pred);
void remque (struct qelem *elem);

struct qelem {
	struct qelem   *q_forw;
	struct qelem   *q_back;
	char	    q_data[1];	/* extensible */
};

void insque (struct qelem *elem, struct qelem *pred)
{
	if (pred -> q_forw)
		pred -> q_forw -> q_back = elem;
	elem -> q_forw = pred -> q_forw;
	elem -> q_back = pred;
	pred -> q_forw = elem;
}

void remque (struct qelem *elem)
{
	if (elem -> q_forw)
		elem -> q_forw -> q_back = elem -> q_back;
	elem -> q_back -> q_forw = elem -> q_forw;
}

#endif
#endif

/* DUP2 */

#ifndef	BSD42
#ifdef	SYS5
#include <fcntl.h>
#endif

int     dup2 (int d1, int d2);
int     dup2 (int d1, int d2)
{
	int     d;

	if (d1 == d2)
		return OK;

	close (d2);
#ifdef	F_DUPFD
	if ((d = fcntl (d1, F_DUPFD, d2)) == NOTOK)
		return NOTOK;
#else
	if ((d = dup2_aux (d1, d2)) == NOTOK)
		return NOTOK;
#endif
	if (d == d2)
		return OK;

	errno = 0;
	return NOTOK;
}

#ifndef	F_DUPFD
static int dup2_aux (int d1, int d2);
static int dup2_aux (int d1, int d2)
{
	int fd, result;
	if ((fd = dup (d1)) == NOTOK || fd == d2)
		return fd;
	result = dup2_aux (d1, d2);
	close (fd);
	return result;
}
#endif
#endif

/* BYTEORDER */

#ifndef	SWABLIB

#include <stdint.h>

/* ROS and HP-UX don't seem to have these in libc.a */

#undef	ntohs
uint16_t	ntohs (uint16_t netshort)
{
	return netshort;
}

#undef	htons
uint16_t	htons (uint16_t hostshort)
{
	return hostshort;
}

#undef	ntohl
uint32_t	ntohl (uint32_t netlong)
{
	return netlong;
}

#undef	htonl
uint32_t	htonl (uint32_t hostlong)
{
	return hostlong;
}

#endif
