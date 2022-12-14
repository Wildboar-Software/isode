/* ts2tcp.c - TPM: TCP interface */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/tsap/RCS/ts2tcp.c,v 9.0 1992/06/16 12:40:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/tsap/RCS/ts2tcp.c,v 9.0 1992/06/16 12:40:39 isode Rel $
 *
 *
 * $Log: ts2tcp.c,v $
 * Revision 9.0  1992/06/16  12:40:39  isode
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


/* LINTLIBRARY */

#include <unistd.h>
#define getdtablesize() (sysconf (_SC_OPEN_MAX))
#include <stdio.h>
#include "tpkt.h"
#include "tailor.h"

#ifdef	TCP
#include "internet.h"
#include <errno.h>
#ifdef	BSD42
#include <sys/ioctl.h>
#endif
#ifdef	SYS5
#include <fcntl.h>
#endif


#define	MAX1006		2048		/* could be as high as TPKT_MAXLEN */

/*    DATA */

#if	defined(FIONBIO) || defined(O_NDELAY)
#define	NODELAY
#endif

#ifdef	NODELAY
static  fd_set  inprogress;
static struct sockaddr_in *peers = NULL;
#endif

#ifdef ULTRIX_X25_DEMSA
#include <demsa.h>
extern t_list_of_conn_desc list_of_conn_desc[our_TABLE_SIZE];
#endif

/*    N-CONNECT.REQUEST */

int
tcpopen (struct tsapblk *tb, struct NSAPaddr *local, struct NSAPaddr *remote, struct TSAPdisconnect *td, int async) {
	int     fd;
#ifdef	FIONBIO
	int	    onoff;
#endif
	struct sockaddr_in  lo_socket,
			   in_socket;
	struct sockaddr_in *lsock = &lo_socket,
							*isock = &in_socket;
	struct hostent *hp;
	struct servent *sp;

#ifndef	NODELAY
	if (async)
		return tsaplose (td, DR_PARAMETER, NULLCP,
						 "asynchronous not supported");
#endif

	bzero ((char *) isock, sizeof *isock);

	if (remote -> na_port == 0) {
		if ((sp = getservbyname ("tsap", "tcp")) == NULL)
			sp = getservbyname ("iso-tsap", "tcp");
		isock -> sin_port = sp ? sp -> s_port : htons ((u_short) 102);
	} else
		isock -> sin_port = remote -> na_port;

	if ((hp = gethostbystring (remote -> na_domain)) == NULL)
		return tsaplose (td, DR_ADDRESS, NULLCP, "%s: unknown host",
						 remote -> na_domain);
#ifdef	notanymore
	strncpy (remote -> na_domain, hp -> h_name,
			 sizeof remote -> na_domain);
#endif

	isock -> sin_family = hp -> h_addrtype;
	inaddr_copy (hp, isock);

#ifndef	notanymore
	strcpy (remote -> na_domain, inet_ntoa (isock -> sin_addr));
#endif

	if (local && local -> na_domain[0]) {
		bzero ((char *) lsock, sizeof *lsock);

		if ((hp = gethostbystring (local -> na_domain)) == NULL)
			return tsaplose (td, DR_ADDRESS, NULLCP, "%s: unknown host",
							 local -> na_domain);

		if ((lsock -> sin_family = hp -> h_addrtype) != isock -> sin_family)
			return tsaplose (td, DR_ADDRESS, NULLCP,
							 "address family mismatch");

		inaddr_copy (hp, lsock);
	} else
		lsock = NULL;

	if ((fd = start_tcp_client (lsock, 0)) == NOTOK)
		return tsaplose (td, DR_CONGEST, "socket", "unable to start");

#ifdef	FIONBIO
	if (async)
		ioctl (fd, FIONBIO, (onoff = 1, (char *) &onoff));
#else
#ifdef	O_NDELAY
	if (async)
		fcntl (fd, F_SETFL, O_NDELAY);
#endif
#endif
	tb -> tb_fd = fd;
	TTService (tb);

	if (join_tcp_server (fd, isock) == NOTOK) {
#ifdef	NODELAY
		if (async)
			switch (errno) {
			case EINPROGRESS:
				if (peers == NULL) {
					peers = (struct sockaddr_in *)
							calloc ((unsigned) getdtablesize (),
									sizeof *peers);
					if (peers == NULL) {
						tsaplose (td, DR_CONGEST, NULLCP,
								  "out of memory");
						close_tcp_socket (fd);
						return (tb -> tb_fd = NOTOK);
					}

					FD_ZERO (&inprogress);
				}
				FD_SET (fd, &inprogress);
				peers[fd] = *isock;/* struct copy */
				return OK;

			case EISCONN:
				goto done;

			default:
				break;
			}
#endif

		tsaplose (td, DR_REFUSED, "connection", "unable to establish");
		close_tcp_socket (fd);
		return (tb -> tb_fd = NOTOK);
	}
#ifdef	NODELAY
done:
	;
#endif

#ifdef	FIONBIO
	if (async)
		ioctl (fd, FIONBIO, (onoff = 0, (char *) &onoff));
#else
#ifdef	O_NDELAY
	if (async)
		fcntl (fd, F_SETFL, 0x00);
#endif
#endif

	tb -> tb_retryfnx = NULLIFP;	/* No need... */

	return DONE;
}

/*  */

#ifndef	NODELAY
/* ARGSUSED */
#endif

static int
tcpretry (struct tsapblk *tb, struct TSAPdisconnect *td) {
#ifdef	NODELAY
#ifdef	FIONBIO
	int	    onoff;
#endif
	int	    fd = tb -> tb_fd;
	fd_set  mask;
	struct sockaddr_in *isock = &peers[fd];

	FD_ZERO (&mask);
	FD_SET (fd, &mask);
	if (xselect (fd + 1, NULLFD, &mask, NULLFD, 0) < 1)
		return OK;

	if (!FD_ISSET (fd, &inprogress))
		return DONE;

	isock = &peers[fd];
	if (join_tcp_server (fd, isock) == NOTOK) {
		switch (errno) {
		case EINPROGRESS:
			return OK;

		case EISCONN:
			goto done;

		case EINVAL:	/* UNIX bug: could be any socket errno, e.g.,
				   ETIMEDOUT */
			errno = ECONNREFUSED;
		/* and fall */
		default:
			break;
		}

		tsaplose (td, DR_REFUSED, "connection", "unable to establish");
		FD_CLR (fd, &inprogress);
		close_tcp_socket (fd);
		return (tb -> tb_fd = NOTOK);
	}
done:
	;

#ifdef	FIONBIO
	ioctl (fd, FIONBIO, (onoff = 0, (char *) &onoff));
#else
#ifdef	O_NDELAY
	fcntl (fd, F_SETFL, 0x00);
#endif
#endif

	FD_CLR (fd, &inprogress);

	return DONE;
#else
	return tsaplose (td, DR_OPERATION, NULLCP, "connection not in progress");
#endif
}

/*    init for read from network */

static int
tcpinit (int fd, struct tsapkt *t) {
	int    cc,
		   i;
	char  *bp;

	for (bp = (char *) &t -> t_pkthdr, i = TPKT_HDRLEN (t);
			i > 0;
			bp += cc, i -= cc)
		switch (cc = read_tcp_socket (fd, bp, i)) {
		case NOTOK:
		case OK:
			return DR_NETWORK;

		default:
			break;
		}

	if (t -> t_vrsn != TPKT_VRSN)
		return DR_PROTOCOL;

	if ((t -> t_length = ntohs (t -> t_length)) < TPKT_HDRLEN (t))
		return DR_LENGTH;

	return OK;
}

/*  */

/* ARGSUSED */

char *
tcpsave (int fd, char *cp1, char *cp2, struct TSAPdisconnect *td) {
	static char buffer[BUFSIZ];

	sprintf (buffer, "%c%d %s %s", NT_TCP, fd, cp1, cp2);

	return buffer;
}

/*  */

int
tcprestore (struct tsapblk *tb, char *buffer, struct TSAPdisconnect *td) {
	int     fd;
	char *cp;
	char    domain1[NSAP_DOMAINLEN + 1 + 5 + 1],
			domain2[NSAP_DOMAINLEN + 1 + 5 + 1];
	struct NSAPaddr *na;
	struct tsapADDR *ta;

	ta = &tb -> tb_initiating;
	ta -> ta_present = 1;
	na = &ta -> ta_addr;
	na -> na_stack = NA_TCP;
	na -> na_community = ts_comm_tcp_default;

	if (sscanf (buffer, "%d %s %s", &fd, domain1, domain2) != 3 || fd < 0)
		return tsaplose (td, DR_PARAMETER, NULLCP,
						 "bad initialization vector \"%s\"", buffer);

#ifdef ULTRIX_X25_DEMSA
	if ( X25RegisterFD(fd,rhandler,whandler,xhandler,fd) < NULL ) {
		SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("X25RegisterFD"));
		return NOTOK;
	}
	if( fd >= our_TABLE_SIZE) {
		SLOG (compat_log, LLOG_EXCEPTIONS, "invalid",
			  ("tcprestore:increase our_TABLE_SIZE in demsb.h"));
		return NOTOK;
	}

	list_of_conn_desc[fd].TCP_IP_OR_X25_SOCKET = our_TCP_IP_SOCKET;
	list_of_conn_desc[fd].descriptor           = fd;
#endif

	if (cp = index (domain1, '+')) {
		*cp++ = NULL;
		na -> na_port = htons ((u_short) atoi (cp));
	}
	strncpy (na -> na_domain, domain1, sizeof na -> na_domain);

	tb -> tb_fd = fd;
	TTService (tb);

	ta = &tb -> tb_responding;
	ta -> ta_present = 1;
	na = &ta -> ta_addr;
	na -> na_stack = NA_TCP;
	na -> na_community = ts_comm_tcp_default;

	if (cp = index (domain2, '+')) {
		*cp++ = NULL;
		na -> na_port = htons ((u_short) atoi (cp));
	}
	strncpy (na -> na_domain, domain2, sizeof na -> na_domain);

	return OK;
}

/*  */

int
TTService (struct tsapblk *tb) {
	struct tsapkt *t;

	tb -> tb_flags &= ~TB_STACKS;
	tb -> tb_flags |= TB_TCP;

	tb -> tb_tsdusize = MAX1006
						- (tb -> tb_tpduslop = sizeof t -> t_pkthdr + DT_MAGIC);

	tb -> tb_retryfnx = tcpretry;

	tb -> tb_initfnx = tcpinit;
	tb -> tb_readfnx = read_tcp_socket;
	tb -> tb_writefnx = tp0write;
	tb -> tb_closefnx = close_tcp_socket;
	tb -> tb_selectfnx = select_tcp_socket;

	tp0init (tb);
}
#endif
