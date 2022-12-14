/* ts2sunlink.c - TPM: SunLink OSI TP4 interface */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/tsap/RCS/ts2sunlink.c,v 9.0 1992/06/16 12:40:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/tsap/RCS/ts2sunlink.c,v 9.0 1992/06/16 12:40:39 isode Rel $
 *
 * Contributed by John A. Scott, The MITRE Corporation
 *
 *
 * $Log: ts2sunlink.c,v $
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

#include <stdio.h>
#include <signal.h>
#include "tpkt.h"
#include "mpkt.h"
#include "af_osi.h"

#ifdef	TP4
#include "tp4.h"
#endif

#ifdef	SUN_TP4
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include "tailor.h"

#ifdef SUNLINK_7_0 /* 13-Jun-91: added by Dave D. for SunNet OSI 7.0 bug */
#include "tsap.h"
#include "isoservent.h"
#include "isoaddrs.h"
#endif

#define MAXTP4 (1 << SIZE_8K)

#ifndef	SUNLINK_6_0
#define	OSI_AF_GENERIC	AF_GENERIC
#endif
#ifndef	MSG_OSI_OOB
#define	MSG_OSI_OOB	MSG_OOB
#endif


/* A driver for SunLink OSI's TP4!
 *
 *	SunLink OSI TP4 user interface is very much like a ``datagram''
 *	interface.  Some of the hair in this port involves converting
 *	ISODE TP packets into SUN TP4 packets.  My gut feeling is that the
 *	port should be redone to make better use of existing ISODE
 *	structures (but this works and I hesitate to change something works).
 *			-- John
 *
 *	Actually, I think is fairly close to optimal now.
 *			-- /mtr
 *
 * TODO:
 *
 * 1. Figure out how to implement tsaplisten.c$tp4unique()
 *
 * 2. On failure of sendto, sendmsg, or recvfrom, try to figure out if a
 *    disconnect happened and return the right DR_ reason.
 *
 * 3. SunLink OSI should support TSELs of length greater than two.  In fact,
 *    a transport address really should be:
 *	nsap - 64 octets, tsel - 44 octets (would prefer 64, but mbufs limit)
 *
 * 4. Should do QOS mappings for error recovery (not class 0 or 2) and cost
 *    (class 0).
 *
 */

/*    DATA */

/*    UPPER HALF */

static int
TConnect (struct tsapblk *tb, int expedited, char *data, int cc, struct TSAPdisconnect *td) {
	int	    result;
	struct tp4pkt *t;
#ifdef SUNLINK_7_0   /* 13-Jun-91: added by Dave D. for SunNet OSI 7.0 bug */
	struct TSAPaddr *ta;
#endif

	if ((t = newtp4pkt (TP_CONNECT_REQ)) == NULL)
		return tsaplose (td, DR_CONGEST, NULLCP, NULLCP);

	if (gen2tp4X (&tb -> tb_responding, &t -> tp4_called,
				  tb -> tb_initiating.ta_present
				  ? &tb -> tb_initiating.ta_addr : NULLNA) == NOTOK) {
		result = tsaplose (td, DR_ADDRESS, NULLCP,
						   "unable to parse remote address");
		goto out;
	}

#ifdef SUNLINK_7_0   /* 13-Jun-91: added by Dave D. for SunNet OSI 7.0 bug. */
	if (tb -> tb_initiating.ta_present == 0)
		if ((ta = is2taddr( TLocalHostName(), "default",
							((struct isoservent *) 0))) == NULLTA) {
			result = tsaplose (td, DR_ADDRESS, NULLCP,
							   "unable to parse remote address");
			goto out;
		} else {
			tb -> tb_initiating.ta_addr      = ta -> ta_addrs[0];
			tb -> tb_initiating.ta_present   = ta -> ta_naddr;
			tb -> tb_initiating.ta_selectlen = ta -> ta_selectlen;
			tb -> tb_initiating.un_ta        = ta -> un_ta;
		}
#endif

	if (gen2tp4X (&tb -> tb_initiating, &t -> tp4_calling,
				  tb -> tb_responding.ta_present
				  ? &tb -> tb_responding.ta_addr : NULLNA) == NOTOK) {
		result = tsaplose (td, DR_ADDRESS, NULLCP,
						   "unable to parse local address");
		goto out;
	}

	if (expedited) {
		tb -> tb_flags |= TB_EXPD;
		t -> tp4_expedited = 1;
	}

	if (sendto (tb -> tb_fd, data, cc, 0, (struct sockaddr *) t,
				sizeof (TP_MSG_CONNECT)) == NOTOK)
		result = tsaplose (td, DR_CONGEST, "failed", "sendto");
	else
		result = CONNECTING_2;

out:
	;
	freetp4pkt (t);

	return result;
}

/*  */

static int
TRetry (struct tsapblk *tb, int async, struct TSAPconnect *tc, struct TSAPdisconnect *td) {
	int	    cc,
			header_len,
			onoff;
	char    data[TS_SIZE];
	struct tp4pkt *t;

	t = NULL;
	if (async && tb -> tb_retryfnx)
		switch ((*tb -> tb_retryfnx) (tb, td)) {
		case NOTOK:
			goto out;

		case OK:
			return CONNECTING_2;

		case DONE:
			break;
		}

	if ((t = newtp4pkt ((TP_EVENT) 0)) == NULL) {
		tsaplose (td, DR_CONGEST, NULLCP, NULLCP);
		goto out;
	}

	header_len = sizeof (TP_MSG_CONNECT);
	if ((cc = recvfrom (tb -> tb_fd, data, sizeof data, 0,
						(struct sockaddr *) t, &header_len)) == NOTOK) {
		if (errno == EWOULDBLOCK) {
			freetp4pkt (t);
			return CONNECTING_2;
		}

		tsaplose (td, DR_CONGEST, "failed", "recvfrom");
		goto out;
	}

	if (async)
		ioctl (tb -> tb_fd, FIONBIO, (onoff = 0, (char *) &onoff));

	switch (t -> tp4_event) {
	case TP_CONNECT_CONF:
		tc -> tc_sd = tb -> tb_fd;
		tc -> tc_tsdusize = tb -> tb_tsdusize = MAXTP4;
		tp42genX (&tb -> tb_responding, &t -> tp4_calling);
		copyTSAPaddrX (&tb -> tb_responding, &tc -> tc_responding);
		if ((tb -> tb_flags & TB_EXPD) && !t -> tp4_expedited)
			tb -> tb_flags &= ~TB_EXPD;
		tc -> tc_expedited = (tb -> tb_flags & TB_EXPD) ? 1 : 0;
		if ((tc -> tc_cc = cc) > 0)
			bcopy (data, tc -> tc_data, cc);

		freetp4pkt (t);
		tb -> tb_flags |= TB_CONN;
#ifdef  MGMT
		if (tb -> tb_manfnx)
			(*tb -> tb_manfnx) (OPREQOUT, tb);
#endif
#ifdef notanymore	/* Will get done in freetblk */
		if (tb -> tb_calling)
			free ((char *) tb -> tb_calling), tb -> tb_calling = NULL;
		if (tb -> tb_called)
			free ((char *) tb -> tb_called), tb -> tb_called = NULL;
#endif
		return DONE;

	case TP_DISCONNECT_IND:
		if ((td -> td_reason = (int) t -> tp4_reason) == DR_UNKNOWN)
			td -> td_reason = DR_NETWORK;
		if ((td -> td_cc = cc) > 0)
			bcopy (data, td -> td_data, cc);
		break;

	default:
		tsaplose (td, DR_NETWORK, NULLCP,
				  "expecting 0x%x, got 0x%x",
				  TP_CONNECT_CONF, t -> tp4_event);
		break;
	}

out:
	;
	if (t)
		freetp4pkt (t);
	freetblk (tb);

	return NOTOK;
}

/*  */

static int
TStart (struct tsapblk *tb, char *cp, struct TSAPstart *ts, struct TSAPdisconnect *td) {
	int	    cc,
			i,
			result;
	struct tp4pkt *tp;

	if ((i = strlen (cp)) < (cc = 2 * sizeof (TP_MSG_CONNECT)))
		return tsaplose (td, DR_PARAMETER, NULLCP,
						 "bad initialization vector");

	if ((tp = newtp4pkt ((TP_EVENT) 0)) == NULL) {
		result = tsaplose (td, DR_CONGEST, NULLCP, NULLCP);
		goto out;
	}

	cc = 2 * implode ((u_char *) tp, cp, cc);
	cp += cc, i -= cc;

	if (tp -> tp4_expedited)
		tb -> tb_flags |= TB_EXPD;

	tp42genX (&tb -> tb_initiating, &tp -> tp4_calling);
	tp42genX (&tb -> tb_responding, &tp -> tp4_called);

	ts -> ts_sd = tb -> tb_fd;
	copyTSAPaddrX (&tb -> tb_initiating, &ts -> ts_calling);
	copyTSAPaddrX (&tb -> tb_responding, &ts -> ts_called);
	ts -> ts_expedited = (tb -> tb_flags & TB_EXPD) ? 1 : 0;
	ts -> ts_tsdusize = tb -> tb_tsdusize;

	if (i > 0) {
		if (i > 2 * TS_SIZE) {
			result = tsaplose (td, DR_CONNECT, NULLCP,
							   "too much initial user data");
			goto out;
		}

		ts -> ts_cc = implode ((u_char *) ts -> ts_data, cp, i);
	} else
		ts -> ts_cc = 0;

	result = OK;

out:
	;
	if (tp)
		freetp4pkt (tp);

	return result;
}

/*  */

/* ARGSUSED */

static int
TAccept (struct tsapblk *tb, int responding, char *data, int cc, struct QOStype *qos, struct TSAPdisconnect *td) {
	int	    result;
	struct tp4pkt *tp;
	SFP	    pstat;

	if ((tp = newtp4pkt (TP_CONNECT_RESP)) == NULL)
		return tsaplose (td, DR_CONGEST, NULLCP, NULLCP);

	if (responding)
		gen2tp4X (&tb -> tb_responding, &tp -> tp4_called,
				  tb -> tb_initiating.ta_present
				  ? &tb -> tb_initiating.ta_addr : NULLNA);

	if (tb -> tb_flags & TB_EXPD)
		tp -> tp4_expedited = 1;

	pstat = signal (SIGPIPE, SIG_IGN);
	if (sendto (tb -> tb_fd, data, cc, 0, (struct sockaddr *) tp,
				sizeof (TP_MSG_CONNECT)) == NOTOK)
		if (errno == EPIPE)
			result = ReadDisc(tb, td);
		else
			result = tsaplose (td, DR_CONGEST, "failed", "sendto");
	else {
		result = OK;

		tb -> tb_flags |= TB_CONN;
#ifdef  MGMT
		if (tb -> tb_manfnx)
			(*tb -> tb_manfnx) (OPREQIN, tb);
#endif
	}
	signal (SIGPIPE, pstat);

	freetp4pkt (tp);

	return result;
}

/*  */

/* life would be nice if we didn't have to worry about the maximum number of
   bytes that can be written in a single syscall() */

#ifndef	MSG_MAXIOVLEN
#define	MSG_MAXIOVLEN	NTPUV
#endif


static int
TWrite (struct tsapblk *tb, struct udvec *uv, int expedited, struct TSAPdisconnect *td) {
	int	    cc,
			flags,
			j,
			len,
			size;
#ifdef	MGMT
	int	    dlen;
#endif
	char *bp,
		 *ep;
#ifndef	SUNLINK_6_0
	char *dp;
	char    data[MAXTP4];
#endif
	struct msghdr msgs;
	struct msghdr *msg = &msgs;
	struct tp4pkt *tp;
	struct iovec iovs[MSG_MAXIOVLEN];
	struct iovec *vv,
			   *wv;
	SFP	    pstat;

	if ((tp = newtp4pkt (expedited ? TP_X_DATA_REQ : TP_DATA_REQ)) == NULL) {
		tsaplose (td, DR_CONGEST, NULLCP, NULLCP);
		goto out;
	}

	if (expedited)
		size = sizeof (TP_MSG_X_DATA), flags = MSG_OOB;
	else
		size = sizeof (TP_MSG_DATA), flags = 0;

#ifdef	MGMT
	dlen = 0;
#endif

	if (!expedited && (tb -> tb_flags & TB_QWRITES)) {
		int	onoff,
			nc;
		struct qbuf *qb;
		struct udvec *xv;

		cc = 0;
		for (xv = uv; xv -> uv_base; xv++)
			cc += xv -> uv_len;
#ifdef	MGMT
		dlen = cc;
#endif

		if ((qb = (struct qbuf *) malloc (sizeof *qb + (unsigned) cc))
				== NULL) {
			tsaplose (td, DR_CONGEST, NULLCP,
					  "unable to malloc %d octets for pseudo-writev, failing...",
					  cc);
			freetp4pkt (tp);
			freetblk (tb);

			return NOTOK;
		}
		qb -> qb_forw = qb -> qb_back = qb;
		qb -> qb_data = qb -> qb_base, qb -> qb_len = cc;

		bp = qb -> qb_data;
		for (xv = uv; xv -> uv_base; xv++) {
			bcopy (xv -> uv_base, bp, xv -> uv_len);
			bp += xv -> uv_len;
		}

		if (tb -> tb_qwrites.qb_forw != &tb -> tb_qwrites) {
			nc = 0;
			goto insert;
		}

		tp -> tp4_eot = 1;

		vv = iovs;
		vv -> iov_base = qb -> qb_data, vv -> iov_len = qb -> qb_len;
		vv++;

		msg -> msg_name = (caddr_t) tp;
		msg -> msg_namelen = size;
		msg -> msg_iov = iovs;
		msg -> msg_iovlen = vv - iovs;
		msg -> msg_accrights = (caddr_t) NULL;
		msg -> msg_accrightslen = 0;

		pstat = signal (SIGPIPE, SIG_IGN);
		ioctl (tb -> tb_fd, FIONBIO, (onoff = 1, (char *) &onoff));

		nc = sendmsg (tb -> tb_fd, msg, flags);

		ioctl (tb -> tb_fd, FIONBIO, (onoff = 0, (char *) &onoff));
		signal (SIGPIPE, pstat);

		if (nc != cc) {
			if (nc == NOTOK) {
				if (errno == EPIPE) {
					ReadDisc(tb, td);
					goto losing;
				}
				if (errno != EWOULDBLOCK) {
					tsaplose (td, DR_CONGEST, "failed", "sendmsg");
					goto losing;
				}

				nc = 0;
			}
			if ((*tb -> tb_queuePfnx) (tb, 1, td) == NOTOK)
				goto losing;

			qb -> qb_data += nc, qb -> qb_len -= nc;
insert:
			;
			insque (qb, tb -> tb_qwrites.qb_back);
			DLOG (tsap_log, LLOG_TRACE,
				  ("queueing blocked write of %d of %d octets", nc, cc));
		} else
			free ((char *) qb);
		goto done;

losing:
		;
		free ((char *) qb);
		freetp4pkt (tp);
		freetblk (tb);

		return NOTOK;
	}

	pstat = signal (SIGPIPE, SIG_IGN);

	ep = (bp = uv -> uv_base) + (cc = uv -> uv_len);
	while (uv -> uv_base) {
		wv = (vv = iovs) + MSG_MAXIOVLEN;
		for (len = tb -> tb_tsdusize; len > 0 && vv < wv; len -= j) {
			j = min (cc, len);
#ifdef	MGMT
			dlen += j;
#endif
			vv -> iov_base = bp, vv -> iov_len = j, vv++;
			bp += j, cc -= j;

			if (bp >= ep) {
				if ((bp = (++uv) -> uv_base) == NULL)
					break;
				ep = bp + (cc = uv -> uv_len);
			}
		}

		if (!expedited)
			tp -> tp4_eot = uv -> uv_base == NULL;

#ifndef	SUNLINK_6_0
		dp = data, len = 0;
		for (wv = iovs; wv < vv; wv++) {
			bcopy (wv -> iov_base, dp, wv -> iov_len);
			dp += wv -> iov_len, len += wv -> iov_len;
		}
		vv = iovs;
		vv -> iov_base = data, vv -> iov_len = len, vv++;
#endif

		msg -> msg_name = (caddr_t) tp;
		msg -> msg_namelen = size;
		msg -> msg_iov = iovs;
		msg -> msg_iovlen = vv - iovs;
		msg -> msg_accrights = (caddr_t) NULL;
		msg -> msg_accrightslen = 0;

		if (sendmsg (tb -> tb_fd, msg, flags) == NOTOK) {
			signal (SIGPIPE, pstat);

			if (errno == EPIPE) {
				ReadDisc(tb, td);
				goto out;
			}
			tsaplose (td, DR_CONGEST, "failed", "sendmsg");
			goto out;
		}
	}

	signal (SIGPIPE, pstat);

done:
	;
	freetp4pkt (tp);

#ifdef  MGMT
	if (tb -> tb_manfnx)
		(*tb -> tb_manfnx) (USERDT, tb, dlen);
#endif

	return OK;

out:
	;
	if (tp)
		freetp4pkt (tp);
	freetblk (tb);

	return NOTOK;
}

/*  */

static int
TDrain (struct tsapblk *tb, struct TSAPdisconnect *td) {
	int	    nc,
			onoff,
			result;
	struct qbuf *qb;
	struct msghdr msgs;
	struct msghdr *msg = &msgs;
	struct tp4pkt *tp;
	struct iovec vvs;
	struct iovec *vv = &vvs;
	SFP	    pstat;
	SBV	    smask;

	if ((tp = newtp4pkt (TP_DATA_REQ)) == NULL)
		return tsaplose (td, DR_CONGEST, NULLCP, NULLCP);
	tp -> tp4_eot = 1;

	msg -> msg_name = (caddr_t) tp;
	msg -> msg_namelen = sizeof (TP_MSG_DATA);
	msg -> msg_iov = vv, msg -> msg_iovlen = 1;
	msg -> msg_accrights = (caddr_t) NULL;
	msg -> msg_accrightslen = 0;

	pstat = signal (SIGPIPE, SIG_IGN);
	smask = sigioblock ();

	ioctl (tb -> tb_fd, FIONBIO, (onoff = 1, (char *) &onoff));

	while ((qb = tb -> tb_qwrites.qb_forw) != &tb -> tb_qwrites) {
		vv -> iov_base = qb -> qb_data, vv -> iov_len = qb -> qb_len;

		if (nc = sendmsg (tb -> tb_fd, msg, 0) != qb -> qb_len) {
			if (nc == NOTOK) {
				if (errno == EPIPE) {
					result = ReadDisc(tb, td);
					goto out;
				}
				if (errno != EWOULDBLOCK) {
					result = tsaplose (td, DR_NETWORK, "failed",
									   "write to network");
					goto out;
				}

				nc = 0;
			}

			DLOG (tsap_log, LLOG_TRACE,
				  ("wrote %d of %d octets from blocked write", nc,
				   qb -> qb_len));
			qb -> qb_data += nc, qb -> qb_len -= nc;

			result = OK;
			goto out;
		}

		DLOG (tsap_log, LLOG_TRACE,
			  ("finished blocked write of %d octets", qb -> qb_len));
		remque (qb);
		free ((char *) qb);
	}
	result = DONE;

out:
	;
	ioctl (tb -> tb_fd, FIONBIO, (onoff = 0, (char *) &onoff));

	sigiomask (smask);
	signal (SIGPIPE, pstat);

	freetp4pkt (tp);
	return result;
}

/*  */

static int
TRead (struct tsapblk *tb, struct TSAPdata *tx, struct TSAPdisconnect *td, int async, int oob) {
	int	    cc,
			header_len;
	struct qbuf *qb;
	struct tp4pkt *tp;

	bzero ((char *) tx, sizeof *tx);
	tx -> tx_qbuf.qb_forw = tx -> tx_qbuf.qb_back = &tx -> tx_qbuf;

	for (;;) {
		if ((qb = (struct qbuf *) malloc ((unsigned) (sizeof *qb + sizeof *tp
										  + tb -> tb_tsdusize)))
				== NULL) {
			tsaplose (td, DR_CONGEST, NULLCP, NULLCP);
			break;
		}
		tp = (struct tp4pkt *) qb -> qb_base;
		qb -> qb_data = qb -> qb_base + sizeof *tp;

		header_len = sizeof (struct tp4pkt);
		if ((cc = recvfrom (tb -> tb_fd, qb -> qb_data, tb -> tb_tsdusize,
							oob ? MSG_OSI_OOB : 0,
							(struct sockaddr *) tp, &header_len)) == NOTOK) {
			tsaplose (td, DR_CONGEST, "failed", "recvfrom");
			break;
		}

		switch (tp -> tp4_event) {
		case TP_DATA_IND:
			if (cc > 0) {
				insque (qb, tb -> tb_qbuf.qb_back);
				tb -> tb_len += (qb -> qb_len = cc);
			} else
				free ((char *) qb);
#ifdef	MGMT
			if (tb -> tb_manfnx)
				(*tb -> tb_manfnx) (USERDR, tb, tb -> tb_len);
#endif
			if (!tp -> tp4_eot) {
				if (async)
					return DONE;

				continue;
			}
			tx -> tx_expedited = 0;
			if (tb -> tb_qbuf.qb_forw != &tb -> tb_qbuf) {
				tx -> tx_qbuf = tb -> tb_qbuf;	/* struct copy */
				tx -> tx_qbuf.qb_forw -> qb_back =
					tx -> tx_qbuf.qb_back -> qb_forw = &tx -> tx_qbuf;
				tx -> tx_cc = tb -> tb_len;
				tb -> tb_qbuf.qb_forw =
					tb -> tb_qbuf.qb_back = &tb -> tb_qbuf;
				tb -> tb_len = 0;
			}

			return OK;

		case TP_X_DATA_IND:
			if (cc > 0) {
				insque (qb, tx -> tx_qbuf.qb_back);
				tx -> tx_cc = (qb -> qb_len = cc);
			} else
				free ((char *) qb);
			tx -> tx_expedited = 1;

			return OK;

		case TP_DISCONNECT_IND:
			td -> td_reason = (int) tp -> tp4_reason;
			if ((td -> td_cc = cc) > 0)
				bcopy (qb -> qb_data, td -> td_data, cc);
			break;

		default:
			tsaplose (td, DR_NETWORK, NULLCP,
					  "unexpected response 0x%x",
					  (int) (tp -> tp4_event));
			break;
		}

		break;
	}
	if (qb)
		free ((char *) qb);

	freetblk (tb);

	return NOTOK;
}

/*  */

static int
ReadDisc (struct tsapblk *tb, struct TSAPdisconnect *td) {
	TP_MSG_DISCONNECT tps, *tp = &tps;
	int header_len = sizeof (*tp);
	int cc;

	if ((cc = recvfrom(tb->tb_fd, td->td_data, sizeof(td->td_data),
					   MSG_OSI_OOB,
					   (struct sockaddr *) tp, &header_len)) == NOTOK) {
		tsaplose (td, DR_CONGEST, "failed", "recvfrom");
	} else if (tp -> tp_event != TP_DISCONNECT_IND) {
		tsaplose (td, DR_NETWORK, NULLCP,
				  "unexpected non-DR 0x%x",
				  (int) (tp -> tp_event));
	} else {
		td -> td_reason = (int) tp -> reason;
		td -> td_cc = cc;
	}
	return (NOTOK);
}

/*  */

static int
TDisconnect (struct tsapblk *tb, char *data, int cc, struct TSAPdisconnect *td) {
	int	    result;
	struct tp4pkt *tp;
	SFP	    pstat;

	if (tp = newtp4pkt (TP_DISCONNECT_REQ)) {
		tp -> tp4_reason = (TP_DR_REASON) DR_NORMAL;

		pstat = signal (SIGPIPE, SIG_IGN);
		if (sendto (tb -> tb_fd, data, cc, 0, (struct sockaddr *) tp,
					sizeof (TP_MSG_DISCONNECT)) == NOTOK)
			if (errno == EPIPE) {
				/* Read DR */
				result = ReadDisc(tb, td);
			} else
				result = tsaplose (td, DR_CONGEST, "failed", "sendto");
		else
			result = OK;
		signal (SIGPIPE, pstat);

		freetp4pkt (tp);
	} else
		result = tsaplose (td, DR_CONGEST, NULLCP, NULLCP);

	freetblk (tb);

	return result;
}

/*  */

/* ARGSUSED */

static int
TLose (struct tsapblk *tb, int reason, struct TSAPdisconnect *td) {
	struct tp4pkt *tp;

	SLOG (tsap_log, LLOG_EXCEPTIONS, NULLCP, ("TPM error %d", reason));

	if (tp = newtp4pkt (TP_DISCONNECT_REQ)) {
		tp -> tp4_reason = (TP_DR_REASON) reason;

		sendto (tb -> tb_fd, NULLCP, 0, 0, (struct sockaddr *) tp,
				sizeof (TP_MSG_DISCONNECT));

		freetp4pkt (tp);
	}
}

/*    LOWER HALF */

/* ARGSUSED */

int
tp4open (struct tsapblk *tb, struct TSAPaddr *local_ta, struct NSAPaddr *local_na, struct TSAPaddr *remote_ta, struct NSAPaddr *remote_na, struct TSAPdisconnect *td, int async) {
	int	    fd,
			onoff;
	struct TSAPaddr tzs;
	struct TSAPaddr *tz = &tzs;
	struct NSAPaddr *nz = tz -> ta_addrs;
	OSI_ADDR	ifaddr;

	bzero ((char *) tz, sizeof *tz);
	if (local_ta)
		*tz = *local_ta;	/* struct copy */
	if (local_na) {
		*nz = *local_na;	/* struct copy */
		tz -> ta_naddr = 1;
	}

	gen2tp4 (tz, &ifaddr, remote_na);

	if ((fd = socket (AF_OSI, SOCK_EVENT, OSIPROTO_TP_EVENT)) == NOTOK)
		return tsaplose (td, DR_CONGEST, "socket", "unable to start");

	if (bind (fd, (struct sockaddr *) &ifaddr, sizeof ifaddr) == NOTOK) {
		tsaplose (td, DR_ADDRESS, "socket", "unable to bind");
		close (fd);
		return NOTOK;
	}

	tb -> tb_fd = fd;
	tp4init (tb);

	if (async)
		ioctl (fd, FIONBIO, (onoff = 1, (char *) &onoff));

	return (async ? OK : DONE);
}

/*  */

/* ARGSUSED */

static int
retry_tp4_socket (struct tsapblk *tb, struct TSAPdisconnect *td) {
	fd_set  mask;

	FD_ZERO (&mask);
	FD_SET (tb -> tb_fd, &mask);
	if (xselect (tb -> tb_fd + 1, &mask, NULLFD, NULLFD, 0) < 1)
		return OK;

	return DONE;
}

/*  */

/* ARGSUSED */

char *
tp4save (int fd, struct TSAPdisconnect *td) {
	static char buffer[BUFSIZ];

	sprintf (buffer, "%c%d", NT_SUN, fd);
	return buffer;
}

/*  */

int
tp4restore (struct tsapblk *tb, char *buffer, struct TSAPdisconnect *td) {
	int	    fd;

	if (sscanf (buffer, "%d", &fd) != 1 || fd < 0)
		return tsaplose (td, DR_PARAMETER, NULLCP,
						 "bad initialization vector \"%s\"", buffer);

	tb -> tb_fd = fd;
	tp4init (tb);

	return OK;
}

/*  */

int
tp4init (struct tsapblk *tb) {
	tb -> tb_connPfnx = TConnect;
	tb -> tb_retryPfnx = TRetry;

	tb -> tb_startPfnx = TStart;
	tb -> tb_acceptPfnx = TAccept;

	tb -> tb_writePfnx = TWrite;
	tb -> tb_readPfnx = TRead;
	tb -> tb_discPfnx = TDisconnect;
	tb -> tb_losePfnx = TLose;

	tb -> tb_drainPfnx = TDrain;

#ifdef  MGMT
	tb -> tb_manfnx = TManGen;
#endif

	tb -> tb_flags &= ~TB_STACKS;
	tb -> tb_flags |= TB_TP4;

	tb -> tb_tsdusize = MAXTP4 - (tb -> tb_tpduslop = 0);

	tb -> tb_retryfnx = retry_tp4_socket;

	tb -> tb_closefnx = close_tp4_socket;
	tb -> tb_selectfnx = select_tp4_socket;
}

/*  */

/* ARGSUSED */

int
start_tp4_server (struct TSAPaddr *sock, int backlog, int opt1, int opt2, struct TSAPdisconnect *td) {
	int	    sd;
	OSI_ADDR	ifaddr;

	gen2tp4 (sock, &ifaddr, NULLNA);

	if ((sd = socket (AF_OSI, SOCK_EVENT, OSIPROTO_TP_EVENT)) == NOTOK)
		return tsaplose (td, DR_CONGEST, "socket", "unable to start");

	if (bind (sd, (struct sockaddr *) &ifaddr, sizeof ifaddr) == NOTOK) {
		tsaplose (td, DR_ADDRESS, "socket", "unable to bind");
		close (sd);
		return NOTOK;
	}

	if (listen (sd, backlog) == NOTOK) {
		tsaplose (td, DR_ADDRESS, "listen", "");
		close (sd);
		return NOTOK;
	}

	return sd;
}

/*  */

#ifndef	notdef
/* ARGSUSED */
#endif

int
join_tp4_client (int fd, struct TSAPaddr *sock, struct TSAPdisconnect *td) {
	int	    len,
			sd;
	OSI_ADDR	ifaddr;

	len = sizeof (OSI_ADDR);
	if ((sd = accept (fd, (struct sockaddr *) &ifaddr, &len)) == NOTOK)
		return tsaplose (td, DR_NETWORK, "socket", "unable to accept");

#ifdef	notdef
	/* Ouch!!  Trying to get the remote address off the socket
	 * only works for local connections.  Non-local connections
	 * causes a core dump when I try to convert.
	 */
	tp42gen (sock, &ifaddr);
#endif

	return sd;
}

/*  */

/* SunLink OSI address encoding/decoding */

#ifdef	SUNLINK_5_2
/* ARGSUSED */
#endif

static int  gen2tp4 (generic, specific, template)
struct TSAPaddr *generic;
OSI_ADDR	*specific;
struct NSAPaddr	*template;
{
#ifndef	SUNLINK_6_0
	int	    len,
	paddr_type;
#endif
#ifndef	SUNLINK_5_2
	char    buffer[BUFSIZ];
#endif
	struct NSAPaddr *na;

	OSI_ADDR_INIT (specific);

	if (generic -> ta_naddr > 0) {
		na = generic -> ta_addrs;
#ifndef	SUNLINK_6_0
		paddr_type = AF_OSI, len = 0;
		if (na -> na_addrlen > 0)
			switch (na -> na_address[0]) {
			case 0x49:
				paddr_type = AF_NBS;
				len = 1;
				break;

			case 0x47:
				if (na -> na_addrlen < 3
						|| na -> na_address[1] != 0x00
						|| na -> na_address[2] != 0x04)
					break;
				paddr_type = AF_OSINET;
				len = 3;
				break;
			}

		osi_set_sap (na -> na_address + len, na -> na_addrlen - len, specific,
					 OSI_NSAP, paddr_type);
#else
		osi_set_sap (na -> na_address, na -> na_addrlen, specific,
					 OSI_NSAP, OSI_AF_USER_DEFINED);
#endif
	} else {
#ifndef	SUNLINK_5_2
		/* The SunLink OSI I'm using seems to require something
		 * although the ``manual'' says I don't.  Hmmmm, I wonder
		     * if I still need the OSI_ADDR_INIT? BTW, osi_set_sap is one
		 * of the two functions I link from the -losi library.  When I
		 * get the source for SunLink (maybe before I retire) I'll
		 * write my own set/get sap function and punt libosi.a
		 */

		paddr_type = AF_OSI;
		if (template != NULLNA && template -> na_addrlen > 0)
			switch (template -> na_address[0]) {
			case 0x49:
				paddr_type = AF_NBS;
				break;

			case 0x47:
				if (template -> na_addrlen < 3
											 || template -> na_address[1] != 0x00
											 || template -> na_address[2] != 0x04)
							break;
				paddr_type = AF_OSINET;
				break;
			}

		buffer[0] = 0x00, len = 1;
		osi_set_sap (buffer, len, specific, OSI_NSAP, paddr_type);
#else
#ifndef        SUNLINK_7_0
		mds_lookup ("localhost", "CLIENT", specific);;
#endif
#endif
	}

	osi_set_sap (generic -> ta_selector, generic -> ta_selectlen, specific,
				 OSI_TSAP, OSI_AF_GENERIC);

	return OK;
}


static	int  gen2tp4X (generic, specific, template)
struct tsapADDR *generic;
OSI_ADDR	*specific;
struct NSAPaddr	*template;
{
	struct TSAPaddr tas;

	copyTSAPaddrX (generic, &tas);
	return gen2tp4 (&tas, specific, template);
}

/*  */

int	tp42gen (generic, specific)
struct TSAPaddr *generic;
OSI_ADDR	*specific;
{
	int	    len,
	paddr_type;
	char    buffer[NASIZE];
	struct NSAPaddr *na;

	paddr_type = 0;
	if ((len = osi_get_sap (specific, buffer, sizeof buffer, OSI_NSAP,
							&paddr_type)) <= 0)
				return NOTOK;

	na = generic -> ta_addrs;
	na -> na_stack = NA_NSAP;
	na -> na_community = ts_comm_nsap_default;
	switch (paddr_type) {
	case AF_NBS:
		na -> na_address[0] = 0x49;
		na -> na_addrlen = 1;
		break;

	case AF_OSINET:
		na -> na_address[0] = 0x47;
		na -> na_address[1] = 0x00;
		na -> na_address[2] = 0x04;
		na -> na_addrlen = 3;
		break;

	default:
		na -> na_addrlen = 0;
		break;
	}
	bcopy (buffer, na -> na_address + na -> na_addrlen, len);
	na -> na_addrlen += len;

	generic -> ta_naddr = 1;
	generic -> ta_selectlen = osi_get_sap (specific, generic -> ta_selector,
										   TSSIZE, OSI_TSAP, &paddr_type);

	return OK;
}


int	tp42genX (generic, specific)
struct tsapADDR *generic;
OSI_ADDR	*specific;
{
	int	    result;
	struct TSAPaddr tas;

	if ((result = tp42gen (&tas, specific)) == OK)
		copyTSAPaddrY (&tas, generic);

	return result;
}

/*  */

struct tp4pkt *newtp4pkt (code)
TP_EVENT code;
{
	struct tp4pkt *tp;

	tp = (struct tp4pkt *) calloc (1, sizeof *tp);
	if (tp != NULL)
		tp -> tp4_event = code;

	return tp;
}
#else
int
_ts2sunlink_stub() {}
#endif
