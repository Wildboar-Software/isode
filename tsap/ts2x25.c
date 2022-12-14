/* ts2x25.c - TPM: X.25 interface */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/tsap/RCS/ts2x25.c,v 9.0 1992/06/16 12:40:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/tsap/RCS/ts2x25.c,v 9.0 1992/06/16 12:40:39 isode Rel $
 *
 *
 * $Log: ts2x25.c,v $
 * Revision 9.0  1992/06/16  12:40:39  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
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

#ifdef  X25
#include "x25.h"
#ifndef CCUR_X25
#include <sys/ioctl.h>
#endif
#ifdef	SYS5
#include <fcntl.h>
#endif

static  fd_set  inprogress;
static  struct NSAPaddr **peers = NULL;

#ifdef ULTRIX_X25_DEMSA
#include "/usr/include/x25.h"
#include <demsa.h>
extern t_list_of_conn_desc list_of_conn_desc[our_TABLE_SIZE];
#endif

#ifdef HEAVY_X25_DEBUG
#ifdef SUN_X25
#undef close_x25_socket
close_x25_socket (fd)
int fd;
{
	if (close (fd) != 0) {
		LLOG (x25_log, LLOG_EXCEPTIONS,
			  ("Close %d failed: %d", fd, errno));
	}
}
#endif
#endif

/*    N-CONNECT.REQUEST */

int
x25open (struct tsapblk *tb, struct NSAPaddr *local, struct NSAPaddr *remote, struct TSAPdisconnect *td, int async) {
	int    fd;
	int     onoff;

	/*
	 * start_x25_client does nothing with its arguments in the CAMTEC
	 * case but there's less #ifdef code this way so...
	 */
	if ((fd = start_x25_client (local)) == NOTOK)
		return tsaplose (td, DR_CONGEST, "socket", "unable to start");

	if (async) {
#ifdef	FIONBIO
		if (ioctl (fd, FIONBIO, (onoff = 1, (char *) &onoff)) < 0)
#else
#ifdef	O_NDELAY
		if (fcntl (fd, F_SETFL, O_NDELAY) < 0)
#endif
#endif
		{
			tsaplose (td, DR_CONGEST, "ioctl", "async on");
			close_x25_socket (fd);
			return NOTOK;
		}
	}
	tb -> tb_fd = fd;
	XTService (tb);

	if (join_x25_server (fd, remote) == NOTOK) {
		if (async)
			switch (errno) {
			case EINPROGRESS:
				if (peers == NULL) {
					peers = (struct NSAPaddr **)
							calloc ((unsigned) getdtablesize (),
									sizeof *peers);
					if (peers == NULL) {
						tsaplose (td, DR_CONGEST, NULLCP,
								  "out of memory");
						close_x25_socket (fd);
						return (tb -> tb_fd = NOTOK);
					}

					FD_ZERO (&inprogress);
				}
				if (peers[fd] == NULL
						&& (peers[fd] = (struct NSAPaddr *)
										malloc (sizeof **peers))
						== NULL) {
					tsaplose (td, DR_CONGEST, NULLCP,
							  "out of memory");
					close_x25_socket (fd);
					return (tb -> tb_fd = NOTOK);
				}
				*(peers[fd]) = *remote;	/* struct copy */
				FD_SET (fd, &inprogress);
				return OK;

			case EISCONN:
				goto done;

			default:
				break;
			}

		tsaplose (td, DR_REFUSED, "connection", "unable to establish");
		LLOG (x25_log, LLOG_NOTICE,
			  ("connection to %s failed", na2str (remote)));
		close_x25_socket (fd);
		return (tb -> tb_fd = NOTOK);
	}
done:
	;

	if (async)
#ifdef	FIONBIO
		if (ioctl (fd, FIONBIO, (onoff = 0, (char *) &onoff)) < 0)
#else
#ifdef	O_NDELAY
		if (fcntl (fd, F_SETFL, 0x00) < 0)
#endif
#endif
		{
			tsaplose (td, DR_CONGEST, "ioctl", "async off");
			close_x25_socket (fd);
			return NOTOK;
		}

	XTService (tb);      /* in case pktsize changed... */
	LLOG (x25_log, LLOG_NOTICE,
		  ("connection %d to %s", fd, na2str (remote)));

	tb -> tb_retryfnx = NULLIFP;	/* No need... */

	return DONE;
}

/*  */

static int
x25retry (struct tsapblk *tb, struct TSAPdisconnect *td) {
	int     onoff;
	int     fd = tb -> tb_fd;
	fd_set  mask;
	struct NSAPaddr *remote = peers[fd];

	FD_ZERO (&mask);
	FD_SET (fd, &mask);
#ifndef CCUR_X25
	if (xselect (fd + 1, NULLFD, &mask, NULLFD, 0) < 1)
#else
	if (xselect (fd + 1, &mask, NULLFD, NULLFD, 0) < 1)
#endif
		return OK;

	if (!FD_ISSET (fd, &inprogress))
		return DONE;

	if (join_x25_server (fd, remote) == NOTOK) {
		switch (errno) {
		case EINPROGRESS:
			return OK;

		case EISCONN:
			goto done;

		case EINVAL:        /* UNIX bug: could be any socket errno, e.g.,
                                   ETIMEDOUT */
			errno = ECONNREFUSED;
		/* and fall */
		default:
			break;
		}

		tsaplose (td, DR_REFUSED, "connection", "unable to establish");
		FD_CLR (fd, &inprogress);
		close_x25_socket (fd);
		LLOG (x25_log, LLOG_NOTICE,
			  ("connection to %s failed", na2str (remote)));
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

	XTService (tb);      /* in case pktsize changed... */
	LLOG (x25_log, LLOG_NOTICE,
		  ("connection %d to %s", fd, na2str (remote)));

	return DONE;
}

/*    init for read from network */

static char nsdu[MAXNSDU];
static char *np;
static int  bl;


static int
x25init (int fd, struct tsapkt *t) {
	int    cc;

	/* XXX: cc should be set to the maximum acceptable NSDU length.
	   Longer NSDUs will be truncated without notification.
	   Should be configurable (or set during N-CONNECT and remembered) */

	cc = sizeof nsdu;

	switch (cc = read_x25_socket (fd, nsdu, cc)) {
	case OK:                /* no data ? */
	case NOTOK:
#ifdef  SUN_X25
		if (compat_log -> ll_events & LLOG_EXCEPTIONS)
			log_cause_and_diag(fd);
#endif
		return DR_NETWORK;

	default:
		t -> t_length = cc + sizeof t -> t_pkthdr;
		break;
	}

	if (t -> t_length < TPKT_HDRLEN (t))
		return DR_LENGTH;

	t -> t_li = nsdu[0];
	t -> t_code = nsdu[1];

	np = nsdu + 2;
	bl = t -> t_length - TPKT_HDRLEN (t);

	t -> t_vrsn = TPKT_VRSN;    /* Not really needed! */

	return OK;
}


/* ARGSUSED */

static int
read_nsdu_buffer (int fd, char *buffer, int cc) {
	if (cc > bl)
		cc = bl;

	if (cc > 0) {
		bcopy (np, buffer, cc);
		np += cc, bl -= cc;
	}

	return cc;
}

/*  */

/* ARGSUSED */

char *
x25save (int fd, struct NSAPaddr *rem, struct NSAPaddr *loc, struct TSAPdisconnect *td) {
	static char buffer[BUFSIZ];
	char tbuf1[NASIZE*2+1], tbuf2[NASIZE*2+1];
#ifdef ULTRIX_X25_DEMSA
	int     vci;
	int     error;

	vci = (int) our_get_vci(fd,"called by x25save");

#endif

	switch (loc -> na_stack) {
	case NA_NSAP:
		explode (tbuf1, (u_char *)loc -> na_address, loc -> na_addrlen);
		explode (tbuf2, (u_char *)rem -> na_address, rem -> na_addrlen);
#ifdef ULTRIX_X25_DEMSA
		sprintf (buffer, "%c%d %d %s %s", NT_X2584,
				 fd, vci, tbuf1, tbuf2);
#else
		sprintf (buffer, "%c%d %s %s", NT_X2584,
				 fd, tbuf1, tbuf2);
#endif
		break;
	case NA_X25:
#ifdef ULTRIX_X25_DEMSA
		sprintf (buffer, "%c%d %d %*s %*s",
				 NT_X25, fd, vci, loc->na_dtelen, loc->na_dte,
				 rem -> na_dtelen, rem -> na_dte);
#else
		sprintf (buffer, "%c%d %*s %*s",
				 NT_X25, fd, loc->na_dtelen, loc->na_dte,
				 rem -> na_dtelen, rem -> na_dte);
#endif
		break;
	default:
		return NULLCP;
	}
	return buffer;
}


int
x25restore (struct tsapblk *tb, char *buffer, struct TSAPdisconnect *td) {
	int     fd;
	char    dte1[NASIZE + 1],
			dte2[NASIZE + 1];
	struct NSAPaddr   *na;
	struct tsapADDR   *ta;

#ifdef ULTRIX_X25_DEMSA

	int     vci;

	if(sscanf(buffer,"%d %d %s %s", &fd, &vci, dte1, dte2) != 4 || fd<0 || vci<0)
		return tsaplose (td, DR_PARAMETER, NULLCP,
						 "bad initialization vector \"%s\"", buffer);
	if ( X25RegisterVC((X25vc)vci,rhandler,whandler,fd) < NULL ) {
		SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("X25RegisterVC"));
		return NOTOK;
	}
	if( fd >= our_TABLE_SIZE) {
		SLOG (compat_log, LLOG_EXCEPTIONS, "invalid",
			  ("x25restore:increase our_TABLE_SIZE in demsb.h"));
		return NOTOK;
	}
	list_of_conn_desc[fd].TCP_IP_OR_X25_SOCKET = our_X25_SOCKET;
	list_of_conn_desc[fd].descriptor           = vci;

#else
	if (sscanf (buffer, "%d %s %s", &fd, dte1, dte2) != 3 || fd < 0)
		return tsaplose (td, DR_PARAMETER, NULLCP,
						 "bad initialization vector \"%s\"", buffer);
#endif

	ta = &tb -> tb_responding;
	ta -> ta_present = 1;
	na = &ta -> ta_addr;
	na -> na_stack = NA_X25;
	na -> na_community = ts_comm_x25_default;
	bcopy(dte1, na -> na_dte, strlen(dte1));
	na -> na_dtelen = strlen (na -> na_dte);

	tb -> tb_fd = fd;
	XTService (tb);

	ta = &tb -> tb_initiating;
	ta -> ta_present = 1;
	na = &ta -> ta_addr;
	na -> na_stack = NA_X25;
	na -> na_community = ts_comm_x25_default;
	bcopy(dte2, na -> na_dte, strlen(dte2));
	na -> na_dtelen = strlen (na -> na_dte);

#ifdef  SUN_X25
	set_x25_facilities (tb -> tb_fd, -1, "Negotiated");
#endif

	return OK;
}

#ifdef AEF_NSAP
int
x25nsaprestore (struct tsapblk *tb, char *buffer, struct TSAPdisconnect *td) {
	int     fd;
	char    dte1[NASIZE*2 + 1],
			dte2[NASIZE*2 + 1];
	struct NSAPaddr   *na;
	struct tsapADDR   *ta;

	if (sscanf (buffer, "%d %s %s", &fd, dte1, dte2) != 3 || fd < 0)
		return tsaplose (td, DR_PARAMETER, NULLCP,
						 "bad initialization vector \"%s\"", buffer);

	ta = &tb -> tb_responding;
	ta -> ta_present = 1;
	na = &ta -> ta_addr;
	na -> na_stack = NA_NSAP;
	na -> na_community = ts_comm_nsap_default;
	na->na_addrlen = implode ((u_char *)na->na_address, dte1, strlen(dte1));

	tb -> tb_fd = fd;
	XTService (tb);

	ta = &tb -> tb_initiating;
	ta -> ta_present = 1;
	na = &ta -> ta_addr;
	na -> na_stack = NA_NSAP;
	na -> na_community = ts_comm_nsap_default;
	na -> na_addrlen = implode ((u_char *)na -> na_address, dte2, strlen(dte2));

#ifdef  SUN_X25
	set_x25_facilities (tb -> tb_fd, -1, "Negotiated");
#endif

	return OK;
}
#endif

/*  */

int
XTService (struct tsapblk *tb) {
#ifndef	UBC_X25
	int     maxnsdu = MAXNSDU;
#else
	int     maxnsdu = X25_PACKETSIZE;
#endif

	tb -> tb_flags &= ~TB_STACKS;
	tb -> tb_flags |= TB_X25;

#ifdef  notyet
	if (recvpktsize > DT_MAGIC && recvpktsize < maxnsdu)
		maxnsdu = recvpktsize;
	if (sendpktsize > DT_MAGIC && sendpktsize < maxnsdu)
		maxnsdu = sendpktsize;
#endif
	tb -> tb_tsdusize = maxnsdu - (tb -> tb_tpduslop = DT_MAGIC);

	tb -> tb_retryfnx = x25retry;

	tb -> tb_initfnx = x25init;
	tb -> tb_readfnx = read_nsdu_buffer;
	tb -> tb_writefnx = tp0write;
	tb -> tb_closefnx = close_x25_socket;
	tb -> tb_selectfnx = select_x25_socket;

	tp0init (tb);
}
#else
int
_ts2x25_stub() {
	;
}
#endif
