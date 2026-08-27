/* tp0ts.c - TPM: TP0 engine */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <search.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "tpkt.h"
#include "mpkt.h"
#include "tailor.h"
#include "internet.h"
#include "compat.h"

#ifdef SUN_X25
#include <netx25/x25_ioctl.h>
static int TConnect (struct tsapblk *tb, int expedited, char *data, int cc, struct TSAPdisconnect *td);
static int TRetry (struct tsapblk *tb, int async, struct TSAPconnect *tc, struct TSAPdisconnect *td);
static int TStart (struct tsapblk *tb, char *cp, struct TSAPstart *ts, struct TSAPdisconnect *td);
static int TAccept (struct tsapblk *tb, int responding, char *data, int cc, struct QOStype *qos, struct TSAPdisconnect *td);
static int TWrite (struct tsapblk *tb, struct udvec *uv, int expedited, struct TSAPdisconnect *td);
static int TRead (struct tsapblk *tb, struct TSAPdata *tx, struct TSAPdisconnect *td, int async, int oob);
static int TDisconnect (struct tsapblk *tb, char *data, int cc, struct TSAPdisconnect *td);
static void TLose (struct tsapblk *tb, int reason, struct TSAPdisconnect *td);
static int TDrain (struct tsapblk *tb, struct TSAPdisconnect *td);

#endif

#if	defined(TCP) || defined(X25)

static int TConnect (struct tsapblk *tb, int expedited, char *data, int cc, struct TSAPdisconnect *td) {
	struct tsapkt *t;

	if (!(tb -> tb_flags & TB_TCP)) {
		expedited = 0;
		if (cc > 0)
			return tsaplose (td, DR_PARAMETER, NULLCP,
							 "initial user data not allowed with class 0");
	}

	tb -> tb_srcref = htons ((uint16_t) (getpid () & 0xffff));
	tb -> tb_dstref = htons ((uint16_t) 0);

	if ((t = newtpkt (TPDU_CR)) == NULL)
		return tsaplose (td, DR_CONGEST, NULLCP, "out of memory");

	t -> t_cr.cr_dstref = tb -> tb_dstref;
	t -> t_cr.cr_srcref = tb -> tb_srcref;
	t -> t_cr.cr_class = CR_CLASS_TP0;
	{
		int    i,
			   j;
		int     k;

		i = k = tb -> tb_tsdusize + tb -> tb_tpduslop;
		for (j = 0; i > 0; j++)
			i >>= 1;
		if (k == (1 << (j - 1)))
			j--;
		if (tb -> tb_flags & TB_TCP) {
			if (int2u8 (j, &t -> t_tpdusize) != 0) {
				freetpkt (t);
				return tsaplose (td, DR_CONGEST, NULLCP,
						 "TPDU size exponent out of range");
			}
		} else {
			if (j > SIZE_MAXTP0) {
				j = SIZE_MAXTP0;
				tb -> tb_tsdusize = (1 << j) - tb -> tb_tpduslop;
			}
			if (int2u8 (j, &t -> t_tpdusize) != 0) {
				freetpkt (t);
				return tsaplose (td, DR_CONGEST, NULLCP,
						 "TPDU size exponent out of range");
			}
		}
	}
	t -> t_callinglen = tb -> tb_initiating.ta_selectlen;
	if (bcopy_int (tb -> tb_initiating.ta_selector, t -> t_calling,
			   t -> t_callinglen) != 0) {
		freetpkt (t);
		return tsaplose (td, DR_PARAMETER, NULLCP,
				 "invalid calling selector length");
	}

	t -> t_calledlen = tb -> tb_responding.ta_selectlen;
	if (bcopy_int (tb -> tb_responding.ta_selector, t -> t_called,
			   t -> t_calledlen) != 0) {
		freetpkt (t);
		return tsaplose (td, DR_PARAMETER, NULLCP,
				 "invalid called selector length");
	}
	if (expedited) {
		tb -> tb_flags |= TB_EXPD;
		t -> t_options |= OPT_TEXPEDITE;
	}

	copyTPKTdata (t, data, cc);	/* XXX: user musn't touch! */

	tb -> tb_retry = t;

	return OK;
}

static int TRetry (struct tsapblk *tb, int async, struct TSAPconnect *tc, struct TSAPdisconnect *td) {
	int	    len;
	struct tsapkt *t;

	if (t = tb -> tb_retry) {
		tb -> tb_retry = NULL;

		if (async && tb -> tb_retryfnx)
			switch ((*tb -> tb_retryfnx) (tb, td)) {
			case NOTOK:
				goto out;

			case OK:
				tb -> tb_retry = t;
				return CONNECTING_1;

			case DONE:
				break;
			}

		if (tpkt2fd (tb, t, tb -> tb_writefnx) == NOTOK) {
			tsaplose (td, t -> t_errno, NULLCP, NULLCP);
			goto out;
		}

		freetpkt (t), t = NULL;
	}

	if (async) {
		fd_set	mask;

		FD_ZERO (&mask);
		FD_SET (tb -> tb_fd, &mask);

		if (xselect (tb -> tb_fd + 1, &mask, NULLFD, NULLFD, 0) == OK)
			return CONNECTING_2;
	}

	if ((t = fd2tpkt (tb -> tb_fd, tb -> tb_initfnx, tb -> tb_readfnx)) == NULL
			|| t -> t_errno != OK) {
		tsaplose (td, t ? t -> t_errno : DR_CONGEST, NULLCP, NULLCP);
		goto out;
	}

	switch (TPDU_CODE (t)) {
	case TPDU_CC:
		tc -> tc_sd = tb -> tb_fd;
		if (CR_CLASS (t) != CR_CLASS_TP0) {
			tpktlose (tb, td, DR_PROTOCOL, NULLCP,
					  "proposed class 0, got back 0x%x", CR_CLASS (t));
			goto out;
		}
		if (tb -> tb_srcref != t -> t_cc.cc_dstref) {
			tpktlose (tb, td, DR_MISMATCH, NULLCP,
					  "sent srcref of 0x%x, got 0x%x",
					  ntohs (tb -> tb_srcref), ntohs (t -> t_cc.cc_dstref));
			goto out;
		}
		tb -> tb_dstref = t -> t_cc.cc_srcref;
		if (!(tb -> tb_flags & TB_TCP) || t -> t_tpdusize) {
			if (t -> t_tpdusize == 0)
				t -> t_tpdusize = SIZE_DFLT;
			else if (t -> t_tpdusize > SIZE_MAXTP0
					 && !(tb -> tb_flags & TB_TCP))
				t -> t_tpdusize = SIZE_MAXTP0;
			tb -> tb_tpdusize = 1 << t -> t_tpdusize;
			tb -> tb_tsdusize = tb -> tb_tpdusize - tb -> tb_tpduslop;
		}
		if ((len = t -> t_calledlen) > 0) {
			size_t nlen,
				cap = sizeof tb -> tb_responding.ta_selector;

			if (int2sizet (len, &nlen) != 0)
				goto out;
			if (nlen > cap)
				nlen = cap;
			memmove (tb -> tb_responding.ta_selector, t -> t_called,
					 nlen);
			if (sizet2int (nlen, &tb -> tb_responding.ta_selectlen) != 0)
				goto out;
		}
		copyTSAPaddrX (&tb -> tb_responding, &tc -> tc_responding);
		if (!(t -> t_options & OPT_TEXPEDITE)
				|| !(tb -> tb_flags & TB_TCP))
			tb -> tb_flags &= ~TB_EXPD;
		tc -> tc_expedited = (tb -> tb_flags & TB_EXPD) ? 1 : 0;
		tc -> tc_tsdusize = tb -> tb_tsdusize;
		tc -> tc_qos = tb -> tb_qos;	/* struct copy */
		if (t -> t_qbuf) {
			copyTSAPdata (t -> t_qbuf -> qb_data, t -> t_qbuf -> qb_len,
						  tc);
		} else
			tc -> tc_cc = 0;

		freetpkt (t);
		tb -> tb_flags |= TB_CONN;
#ifdef  MGMT
		if (tb -> tb_manfnx)
			(*tb -> tb_manfnx) (OPREQOUT, tb);
#endif
		DLOG (tsap_log, LLOG_TRACE,
			  ("Connection established fd=%d flags=0x%x tpdusize=%d, tsdusize=%d",
			   tb -> tb_fd, tb -> tb_flags, tb -> tb_tpdusize, tb -> tb_tsdusize));
#ifdef notanymore	/* Will get done in freetblk */

		if (tb -> tb_calling)
			free ((char *) tb -> tb_calling), tb -> tb_calling = NULL;
		if (tb -> tb_called)
			free ((char *) tb -> tb_called), tb -> tb_called = NULL;
#endif

		return DONE;

	case TPDU_DR:
		td -> td_reason = t -> t_dr.dr_reason;
		if (t -> t_qbuf) {
			copyTSAPdata (t -> t_qbuf -> qb_data, t -> t_qbuf -> qb_len,
						  td);
		} else
			td -> td_cc = 0;
		goto out;

	case TPDU_ER:
		switch (t -> t_er.er_reject) {
		case ER_REJ_NOTSPECIFIED:
		default:
			td -> td_reason = DR_CONNECT;
			break;

		case ER_REJ_CODE:
		case ER_REJ_TPDU:
		case ER_REJ_VALUE:
			td -> td_reason = DR_PROTOCOL;
			break;
		}
		td -> td_cc = 0;
		goto out;

	default:
		tpktlose (tb, td, DR_PROTOCOL, NULLCP,
				  "transport protocol mangled: expecting 0x%x, got 0x%x",
				  TPDU_CC, TPDU_CODE (t));
		goto out;
	}

out:
	;
	freetpkt (t);
	/*    freetblk (tb); */

	return NOTOK;
}

static int TStart (struct tsapblk *tb, char *cp, struct TSAPstart *ts, struct TSAPdisconnect *td) {
	int	    len,
			result;
	struct tsapkt *t;

	if ((t = str2tpkt (cp)) == NULL || t -> t_errno != OK) {
		result = tsaplose (td, DR_PARAMETER, NULLCP,
						   "bad initialization vector");
		goto out;
	}

	if (CR_CLASS (t) != CR_CLASS_TP0) {
		if (t -> t_cr.cr_alternate & (ALT_TP0 | ALT_TP1))
			t -> t_cr.cr_class = CR_CLASS_TP0;
		else {
			result = tpktlose (tb, td, DR_CONNECT, NULLCP,
							   "only class 0 supported, not 0x%x", CR_CLASS (t));
			goto out;
		}
	}

	tb -> tb_srcref = htons ((uint16_t) (getpid () & 0xffff));
	tb -> tb_dstref = t -> t_cr.cr_srcref;
	if (!(tb -> tb_flags & TB_TCP) || t -> t_tpdusize) {
		if (t -> t_tpdusize == 0)
			t -> t_tpdusize = SIZE_DFLT;
		else if (t -> t_tpdusize > SIZE_MAXTP0
				 && !(tb -> tb_flags & TB_TCP))
			t -> t_tpdusize = SIZE_MAXTP0;
		tb -> tb_tpdusize = 1 << t -> t_tpdusize;
		tb -> tb_tsdusize = tb -> tb_tpdusize - tb -> tb_tpduslop;
	}
	if ((len = t -> t_callinglen) > 0) {
		size_t nlen,
			cap = sizeof tb -> tb_initiating.ta_selector;

		if (int2sizet (len, &nlen) != 0) {
			result = NOTOK;
			goto out;
		}
		if (nlen > cap)
			nlen = cap;
		memmove (tb -> tb_initiating.ta_selector, t -> t_calling, nlen);
		if (sizet2int (nlen, &tb -> tb_initiating.ta_selectlen) != 0) {
			result = NOTOK;
			goto out;
		}
	}
	if ((len = t -> t_calledlen) > 0) {
		size_t nlen,
			cap = sizeof tb -> tb_responding.ta_selector;

		if (int2sizet (len, &nlen) != 0) {
			result = NOTOK;
			goto out;
		}
		if (nlen > cap)
			nlen = cap;
		memmove (tb -> tb_responding.ta_selector, t -> t_called, nlen);
		if (sizet2int (nlen, &tb -> tb_responding.ta_selectlen) != 0) {
			result = NOTOK;
			goto out;
		}
	}
	if ((t -> t_options & OPT_TEXPEDITE) && (tb -> tb_flags & TB_TCP))
		tb -> tb_flags |= TB_EXPD;

	ts -> ts_sd = tb -> tb_fd;
	copyTSAPaddrX (&tb -> tb_initiating, &ts -> ts_calling);
	copyTSAPaddrX (&tb -> tb_responding, &ts -> ts_called);
	ts -> ts_expedited = (tb -> tb_flags & TB_EXPD) ? 1 : 0;
	ts -> ts_tsdusize = tb -> tb_tsdusize;
	ts -> ts_qos = tb -> tb_qos;	/* struct copy */

	if (t -> t_qbuf) {
		copyTSAPdata (t -> t_qbuf -> qb_data, t -> t_qbuf -> qb_len, ts);
	} else
		ts -> ts_cc = 0;

	result = OK;

out:
	;
	freetpkt (t);

	return result;
}

static int TAccept (struct tsapblk *tb, int responding, char *data, int cc, struct QOStype *qos, struct TSAPdisconnect *td) {
	int	    result;
	struct tsapkt *t;

	if (!(tb -> tb_flags & TB_TCP) && cc > 0)
		return tsaplose (td, DR_PARAMETER, NULLCP,
						 "initial user data not allowed with class 0");

	if ((t = newtpkt (TPDU_CC)) == NULL)
		return tsaplose (td, DR_CONGEST, NULLCP, "out of memory");

	t -> t_cc.cc_dstref = tb -> tb_dstref;
	t -> t_cc.cc_srcref = tb -> tb_srcref;
	t -> t_cc.cc_class = CR_CLASS_TP0;
	{
		int    i,
			   j;
		int     k;

		i = k = tb -> tb_tsdusize + tb -> tb_tpduslop;
		for (j = 0; i > 0; j++)
			i >>= 1;
		if (k == (1 << (j - 1)))
			j--;
		if (tb -> tb_flags & TB_TCP) {
			if (int2u8 (j, &t -> t_tpdusize) != 0) {
				freetpkt (t);
				return tsaplose (td, DR_CONGEST, NULLCP,
						 "TPDU size exponent out of range");
			}
		} else {
			if (j > SIZE_MAXTP0) {
				j = SIZE_MAXTP0;
				tb -> tb_tsdusize = (1 << j) - tb -> tb_tpduslop;
			}
			if (int2u8 (j, &t -> t_tpdusize) != 0) {
				freetpkt (t);
				return tsaplose (td, DR_CONGEST, NULLCP,
						 "TPDU size exponent out of range");
			}
		}
	}
	if (responding)
		t -> t_calledlen = tb -> tb_responding.ta_selectlen;
		if (bcopy_int (tb -> tb_responding.ta_selector, t -> t_called,
				   t -> t_calledlen) != 0) {
			freetpkt (t);
			return tsaplose (td, DR_PARAMETER, NULLCP,
					 "invalid called selector length");
		}
	if (tb -> tb_flags & TB_EXPD)
		t -> t_options |= OPT_TEXPEDITE;
	copyTPKTdata (t, data, cc);

	if ((result = tpkt2fd (tb, t, tb -> tb_writefnx)) == NOTOK)
		tsaplose (td, t -> t_errno, NULLCP, NULLCP);
	else {
		tb -> tb_flags |= TB_CONN;
#ifdef  MGMT
		if (tb -> tb_manfnx)
			(*tb -> tb_manfnx) (OPREQIN, tb);
#endif
	}

	freetpkt (t);

	return result;
}

static int TWrite (struct tsapblk *tb, struct udvec *uv, int expedited, struct TSAPdisconnect *td) {
	int	    cc,
			j,
			len,
			result;
#if	defined(X25) || defined(MGMT)
	int	    dlen;
#endif
	char *bp,
		 *ep;
	struct tsapkt *t;
	struct udvec *vv,
			   *wv;

#if	defined(X25) || defined(MGMT)
	dlen = 0;
#endif

	ep = (bp = uv -> uv_base) + (cc = uv -> uv_len);
	while (uv -> uv_base) {
		if ((t = newtpkt (expedited ? TPDU_ED : TPDU_DT)) == NULL)
			return tsaplose (td, DR_CONGEST, NULLCP, "out of memory");

		wv = (vv = t -> t_udvec) + NTPUV - 1;
		len = tb -> tb_tpdusize ? (tb -> tb_tpdusize - tb -> tb_tpduslop)
			  : tb -> tb_tsdusize;
		for (; len > 0 && vv < wv; len -= j) {
			j = min (cc, len);
#if	defined(X25) || defined(MGMT)
			dlen += j;
#endif
			vv -> uv_base = bp, vv -> uv_len = j, vv++;
			bp += j, cc -= j;

			if (bp >= ep) {
				if ((bp = (++uv) -> uv_base) == NULL)
					break;
				ep = bp + (cc = uv -> uv_len);
			}
		}

		if (uv -> uv_base == NULL)
			t -> t_dt.dt_nr |= DT_EOT;

		if ((result = tpkt2fd (tb, t, tb -> tb_writefnx)) == NOTOK) {
			tsaplose (td, t -> t_errno, NULLCP, NULLCP);
#ifdef	X25
			if (tb -> tb_flags & TB_X25)
				LLOG (x25_log, LLOG_NOTICE,
					  ("connection %d broken, %d/%d octets sent/recv",
					   tb -> tb_fd, tb -> tb_sent, tb -> tb_recv));
#endif
			freetblk (tb);
		}

		freetpkt (t);
		if (result == NOTOK)
			return NOTOK;
	}

#ifdef	X25
	tb -> tb_sent += dlen;
#endif
#ifdef  MGMT
	if (tb -> tb_manfnx)
		(*tb -> tb_manfnx) (USERDT, tb, dlen);
#endif

	return OK;
}

static int TRead (struct tsapblk *tb, struct TSAPdata *tx, struct TSAPdisconnect *td, int async, int oob) {
	int     eot;
	struct tsapkt *t = NULL;

	bzero ((char *) tx, sizeof *tx);
	tx -> tx_qbuf.qb_forw = tx -> tx_qbuf.qb_back = &tx -> tx_qbuf;

	for (;;) {
		if (oob) { /* out of band data should not be present! */
			tsaplose (td, DR_NETWORK, NULLCP,
					  "Out of band data received");
#ifdef  X25
			if (tb -> tb_flags & TB_X25)
				LLOG (x25_log, LLOG_NOTICE,
					  ("out of band data on %d, %d/%d octets sent/recv",
					   tb -> tb_fd, tb -> tb_sent, tb -> tb_recv));
#endif
			break;
		}

		if ((t = fd2tpkt (tb -> tb_fd, tb -> tb_initfnx, tb -> tb_readfnx))
				== NULL
				|| t -> t_errno != OK) {
			tsaplose (td, t ? t -> t_errno : DR_CONGEST, NULLCP,
					  NULLCP);
#ifdef	X25
			if (tb -> tb_flags & TB_X25)
				LLOG (x25_log, LLOG_NOTICE,
					  ("connection %d broken, %d/%d octets sent/recv",
					   tb -> tb_fd, tb -> tb_sent, tb -> tb_recv));
#endif
			break;
		}

		switch (TPDU_CODE (t)) {
		case TPDU_DT:
			eot = t -> t_dt.dt_nr & DT_EOT;
			if (t -> t_qbuf) {
				insque (t -> t_qbuf, tb -> tb_qbuf.qb_back);
				tb -> tb_len += t -> t_qbuf -> qb_len;
#ifdef	X25
				tb -> tb_recv += t -> t_qbuf -> qb_len;
#endif
				t -> t_qbuf = NULL;
			}
			freetpkt (t);
#ifdef  MGMT
			if (tb -> tb_manfnx)
				(*tb -> tb_manfnx) (USERDR, tb, tb -> tb_len);
#endif
			if (!eot) {
				if (async)
					return DONE;

				continue;
			}
			tx -> tx_expedited = 0;
			if (tb -> tb_qbuf.qb_forw != &tb -> tb_qbuf) {
				tx -> tx_qbuf = tb -> tb_qbuf;/* struct copy */
				tx -> tx_qbuf.qb_forw -> qb_back =
					tx -> tx_qbuf.qb_back -> qb_forw = &tx -> tx_qbuf;
				tx -> tx_cc = tb -> tb_len;
				tb -> tb_qbuf.qb_forw =
					tb -> tb_qbuf.qb_back = &tb -> tb_qbuf;
				tb -> tb_len = 0;
			}
			return OK;

		case TPDU_ED:
			if (t -> t_qbuf) {
				insque (t -> t_qbuf, tx -> tx_qbuf.qb_back);
				tx -> tx_cc = t -> t_qbuf -> qb_len;
				t -> t_qbuf = NULL;
			}
			freetpkt (t);
			tx -> tx_expedited = 1;
			return OK;

		case TPDU_DR:
			td -> td_reason = t -> t_dr.dr_reason;
			if (t -> t_qbuf) {
				copyTSAPdata (t -> t_qbuf -> qb_data,
							  t -> t_qbuf -> qb_len, td);
			} else
				td -> td_cc = 0;
			break;

		case TPDU_ER:
			switch (t -> t_er.er_reject) {
			case ER_REJ_NOTSPECIFIED:
			default:
				td -> td_reason = DR_UNKNOWN;
				break;

			case ER_REJ_CODE:
			case ER_REJ_TPDU:
			case ER_REJ_VALUE:
				td -> td_reason = DR_PROTOCOL;
				break;
			}
			td -> td_cc = 0;
			break;

		default:
			tpktlose (tb, td, DR_PROTOCOL, NULLCP,
					  "transport protocol mangled: not expecting 0x%x",
					  TPDU_CODE (t));
			break;
		}
		break;
	}

	if (t) freetpkt (t);
	freetblk (tb);

	return NOTOK;
}

static int TDisconnect (struct tsapblk *tb, char *data, int cc, struct TSAPdisconnect *td) {
	int     result;
#ifdef	TCP
	struct tsapkt *t;
#endif

	result = OK;
#ifdef	TCP
	if (tb -> tb_flags & TB_TCP) {
		if (t = newtpkt (TPDU_DR)) {
			t -> t_dr.dr_srcref = tb -> tb_srcref;
			t -> t_dr.dr_dstref = tb -> tb_dstref;
			t -> t_dr.dr_reason = DR_NORMAL;
			copyTPKTdata (t, data, cc);

			if ((result = tpkt2fd (tb, t, tb -> tb_writefnx)) == NOTOK)
				tsaplose (td, t -> t_errno, NULLCP, NULLCP);

			freetpkt (t);
		} else
			result = tsaplose (td, DR_CONGEST, NULLCP, "out of memory");
	}
#endif
#ifdef X25
	if (tb -> tb_flags & TB_X25)
		LLOG (x25_log, LLOG_NOTICE,
			  ("connection %d closed, %d/%d octets sent/recv",
			   tb -> tb_fd, tb -> tb_sent, tb -> tb_recv));
#endif
	freetblk (tb);

	return result;
}

static void TLose (struct tsapblk *tb, int reason, struct TSAPdisconnect *td) {
	struct tsapkt  *t;

	switch (reason) {
	case DR_UNKNOWN:
	case DR_CONGEST:
	case DR_SESSION:
	case DR_ADDRESS:
		if ((t = newtpkt (TPDU_DR)) == NULLPKT)
			break;

		t -> t_dr.dr_srcref = tb -> tb_srcref;
		t -> t_dr.dr_dstref = tb -> tb_dstref;
		if (int2u8 (reason, &t -> t_dr.dr_reason) != 0)
			break;
		copyTPKTdata (t, td -> td_data, td -> td_cc);
		break;
	default:
		if ((t = newtpkt (TPDU_ER)) == NULLPKT)
			break;
		t -> t_er.er_dstref = tb -> tb_dstref;
		switch (reason) {
		case DR_PROTOCOL:
			t -> t_er.er_reject = ER_REJ_TPDU;
			break;
		default:
			t -> t_er.er_reject = ER_REJ_NOTSPECIFIED;
			break;
		}
		break;
	}
	if (t) {
		tpkt2fd (tb, t, tb -> tb_writefnx);
		freetpkt (t);
	}
}

/* at present, used by TCP and X.25 back-ends... */

#ifndef	TCP
#undef	WRITEV
#endif

#include <errno.h>
#ifdef	SYS5
#include <fcntl.h>
#else
#include <sys/ioctl.h>
#endif
#ifdef	WRITEV
#include <sys/uio.h>
#endif
#ifdef	TCP
#include "internet.h"
#else
#define	write_tcp_socket	NULLIFP
#endif
#ifdef	X25
#include "x25.h"
#else
#define	write_x25_socket	NULL
#endif

#if	defined(FIONBIO) || defined(O_NDELAY)
#define	NODELAY
#endif

#ifdef LINUX
#include <errno.h>
#else
extern	int	errno;
#endif

#if	defined(WRITEV) || defined(SUN_X25) || defined(CAMTEC_CCL)
static int
iov_push (struct iovec **iovp, const int *cc, const void *base, const size_t len)
{
	if (add_sizet_to_int (cc, len) != 0)
		return NOTOK;
	(*iovp) -> iov_base = base;
	(*iovp) -> iov_len = len;
	(*iovp)++;
	return OK;
}

static int
iov_push_int (struct iovec **iovp, const int *cc, const void *base, const int len)
{
	size_t n;

	if (int2sizet (len, &n) != 0)
		return NOTOK;
	return iov_push (iovp, cc, base, n);
}
#endif

int tp0write (struct tsapblk *tb, struct tsapkt *t, char *cp, int n) {
	int    cc;
	char   *p,
		   *q;
	struct qbuf *qb;
	struct udvec  *uv;
#if	defined(WRITEV) || defined(SUN_X25) || defined(CAMTEC_CCL)
#ifdef	UBC_X25_WRITEV
	struct iovec iovs[NTPUV + 5];
	char no_mbit = 0;
#else
	struct iovec iovs[NTPUV + 4];
#endif
	struct iovec *iov;
#endif

#if	defined(WRITEV) || defined(SUN_X25) || defined(CAMTEC_CCL)
#ifdef	NODELAY
	if (tb -> tb_flags & TB_QWRITES)
		goto single;
#endif
	iov = iovs;
	cc = 0;

	if (tb -> tb_flags & TB_X25) {
#ifdef CCUR_X25
		goto single;
#else
#ifdef UBC_X25_WRITEV
		if (iov_push (&iov, &cc, &no_mbit, sizeof no_mbit) != OK)
			return NOTOK;
#endif
		if (iov_push (&iov, &cc, (char *) &t -> t_li, sizeof t -> t_li) != OK)
			return NOTOK;

		if (iov_push (&iov, &cc, (char *) &t -> t_code, sizeof t -> t_code) != OK)
			return NOTOK;
#endif /* CCUR_X25 */
	} else {
		if (iov_push (&iov, &cc, (char *) &t -> t_pkthdr, TPKT_HDRLEN (t)) != OK)
			return NOTOK;
	}

	if (iov_push_int (&iov, &cc, cp, n) != OK)
		return NOTOK;

	if (t -> t_vdata) {
		if (iov_push_int (&iov, &cc, t -> t_vdata, t -> t_vlen) != OK)
			return NOTOK;
	}

	for (uv = t -> t_udvec; uv -> uv_base; uv++) {
		if (iov_push_int (&iov, &cc, uv -> uv_base, uv -> uv_len) != OK)
			return NOTOK;
	}

	{
		int iovcnt,
			nout;
		ssize_t nsent;

		if (ptrdiff2int (iov - iovs, &iovcnt) != 0)
			return NOTOK;
		nsent = writev (tb -> tb_fd, iovs, iovcnt);
		if (ssize2int (nsent, &nout) != 0 || nout != cc) {
			cc = NOTOK;
#ifdef	SUN_X25
			if (tb -> tb_flags & TB_X25
					&& compat_log -> ll_events & LLOG_EXCEPTIONS)
				log_cause_and_diag (tb -> tb_fd);
#endif
		} else if (tb -> tb_flags & TB_X25) {
			DLOG (compat_log, LLOG_DEBUG, ("X.25 write %d bytes", cc));
		}
	}
	goto out;

single:
	;
#endif

	{
		size_t hdr;
		int hdr_i;

		if (tb -> tb_flags & TB_X25)
			hdr = sizeof t -> t_li + sizeof t -> t_code;
		else
			hdr = TPKT_HDRLEN (t);
		if (sizet2int (hdr, &hdr_i) != 0)
			return NOTOK;
		cc = hdr_i;
		if (add_int_to_int (&cc, n) != 0)
			return NOTOK;
		if (t -> t_vdata && add_int_to_int (&cc, t -> t_vlen) != 0)
			return NOTOK;
		for (uv = t -> t_udvec; uv -> uv_base; uv++)
			if (add_int_to_int (&cc, uv -> uv_len) != 0)
				return NOTOK;
	}

	{
		size_t nbytes,
			extra;

		if (int2sizet (cc, &extra) != 0)
			return NOTOK;
		nbytes = sizeof *qb;
		if (nbytes > SIZE_MAX - extra)
			return NOTOK;
		nbytes += extra;
		if (p = malloc (nbytes)) {
		int	nc,
			onoff;
#ifdef LINUX
		ssize_t (*wfnx) (int, const void *, size_t) = (tb -> tb_flags & TB_X25)
			? write_x25_socket
			: write_tcp_socket;
#else
		int (*wfnx)(int, const void *, size_t) = (tb -> tb_flags & TB_X25)
			? write_x25_socket
			: write_tcp_socket;
#endif

#ifdef	NODELAY
		if (tb -> tb_flags & TB_QWRITES) {
			qb = (struct qbuf *) p;
			qb -> qb_forw = qb -> qb_back = qb;
			qb -> qb_data = qb -> qb_base, qb -> qb_len = cc;
			p = qb -> qb_data;
		}
#endif

		if (tb -> tb_flags & TB_X25) {
			bcopy ((char *) &t -> t_li, q = p, sizeof t -> t_li);
			q += sizeof t -> t_li;

			bcopy ((char *) &t -> t_code, q, sizeof t -> t_code);
			q += sizeof t -> t_code;
		} else {
			bcopy ((char *) &t -> t_pkthdr, q = p, TPKT_HDRLEN (t));
			q += TPKT_HDRLEN (t);
		}

		if (bcopy_int (cp, q, n) != 0)
			goto losing;
		q += n;

		if (t -> t_vdata) {
			if (bcopy_int (t -> t_vdata, q, t -> t_vlen) != 0)
				goto losing;
			q += t -> t_vlen;
		}

		for (uv = t -> t_udvec; uv -> uv_base; uv++) {
			if (bcopy_int (uv -> uv_base, q, uv -> uv_len) != 0)
				goto losing;
			q += uv -> uv_len;
		}

#ifdef	NODELAY
		if (tb -> tb_qwrites.qb_forw != &tb -> tb_qwrites) {
			nc = 0;
			goto insert;
		}

		if (tb -> tb_flags & TB_QWRITES) {
#ifdef	FIONBIO
			ioctl (tb -> tb_fd, FIONBIO, (onoff = 1, (char *) &onoff));
#else
#ifdef	O_NDELAY
			fcntl (tb -> tb_fd, F_SETFL, O_NDELAY);
#endif
#endif
		}
#endif

		{
			size_t wlen;
			ssize_t nw;

			if (int2sizet (cc, &wlen) != 0)
				goto losing;
			nw = (*wfnx) (tb -> tb_fd, p, wlen);
			if (ssize2int (nw, &nc) != 0)
				goto losing;
		}

#ifdef	NODELAY
		if (tb -> tb_flags & TB_QWRITES) {
#ifdef	FIONBIO
			ioctl (tb -> tb_fd, FIONBIO, (onoff = 0, (char *) &onoff));
#else
#ifdef	O_NDELAY
			fcntl (tb -> tb_fd, F_SETFL, 0x00);
#endif
#endif
		}
#endif

		if (nc != cc) {
#ifdef	NODELAY
			if (tb -> tb_flags & TB_QWRITES) {
				if (nc == NOTOK) {
					if (errno != EWOULDBLOCK)
						goto losing;

#if defined(SUN_X25) && defined(X25_WRITE_BUFFER_FULL)
					/* Need SunNet 7.0 patch 100328-09 + full buffer fix */
					if (tb -> tb_flags & TB_X25)
						ioctl (tb -> tb_fd, X25_WRITE_BUFFER_FULL, 0);

#endif
					nc = 0;
				}
#ifndef CCUR_X25	/* Since the MORE bit will have been set on what
			   we have sent, and will continue to be set on
			   any more fragments that we might send, and is
			   not needed for the last fragment, the fact that
			   so far we have only managed part of the NSDU
			   is not signigicant.  This is probably true for
			   other X.25 providers too.
			*/
				else if (nc > 0 && (tb -> tb_flags & TB_X25)) {
					SLOG (tsap_log, LLOG_EXCEPTIONS, NULLCP,
						  ("partial write (%d of %d octets) to X.25",
						   nc, cc));
					goto losing;
				}
#endif /* CCUR_X25 */

				if ((*tb -> tb_queuePfnx) (tb, 1, (struct TSAPdisconnect *) 0)
						== NOTOK)
					goto losing;

				qb -> qb_data += nc, qb -> qb_len -= nc;
insert:
				;
				insque (qb, tb -> tb_qwrites.qb_back);
				DLOG (tsap_log, LLOG_TRACE,
					  ("queueing blocked write of %d of %d octets", nc, cc));
				qb = NULL;
			} else
#endif
			{
losing:
				;
				cc = NOTOK;
			}
		}

#ifdef	NODELAY
		if (tb -> tb_flags & TB_QWRITES) {
			if (qb)
				free ((char *) qb);
		} else
#endif
			free (p);
		goto out;
		}
	}
	if ((tb -> tb_flags & TB_X25) || tb -> tb_flags & TB_QWRITES) {
		SLOG (tsap_log, LLOG_EXCEPTIONS, NULLCP,
			  ("unable to malloc %d octets for pseudo-writev, failing...",
			   cc));

		cc = NOTOK;
		goto out;
	}

#ifdef	TCP
	SLOG (tsap_log, LLOG_EXCEPTIONS, NULLCP,
		  ("unable to malloc %d octets for pseudo-writev, continuing...",
		   cc));

	if (sizet2int (TPKT_HDRLEN (t), &cc) != 0)
		goto err;
	if (write_int (tb -> tb_fd, (char *) &t -> t_pkthdr, cc) != cc) {
err:
		;
		cc = NOTOK;
		goto out;
	}

	if (write_int (tb -> tb_fd, cp, n) != n)
		goto err;
	cc += n;

	if (t -> t_vdata
			&& write_int (tb -> tb_fd, t -> t_vdata, t -> t_vlen)
			!= t -> t_vlen)
		goto err;
	cc += t -> t_vlen;

	for (uv = t -> t_udvec; uv -> uv_base; uv++) {
		if (write_int (tb -> tb_fd, uv -> uv_base, uv -> uv_len)
				!= uv -> uv_len)
			goto err;
		cc += uv -> uv_len;
	}
#endif

out:
	;

	return cc;
}

#ifdef	NODELAY
static int TDrain (struct tsapblk *tb, struct TSAPdisconnect *td) {
	int	    nc,
			onoff,
			result;
	struct qbuf *qb;

#ifdef LINUX
	__sighandler_t pstat;
	ssize_t (*wfnx) (int, const void *, size_t) = (tb -> tb_flags & TB_X25)
		? write_x25_socket
		: write_tcp_socket;
#else
	SFP	    pstat;
	int (*wfnx)(int, const void *, size_t) = (tb -> tb_flags & TB_X25)
		? write_x25_socket
		: write_tcp_socket;
#endif
	SBV	    smask;

	pstat = signal (SIGPIPE, (__sighandler_t)SIG_IGN);
	smask = sigioblock ();

#ifdef	FIONBIO
	ioctl (tb -> tb_fd, FIONBIO, (onoff = 1, (char *) &onoff));
#else
#ifdef	O_NDELAY
	fcntl (tb -> tb_fd, F_SETFL, O_NDELAY);
#endif
#endif

	while ((qb = tb -> tb_qwrites.qb_forw) != &tb -> tb_qwrites) {
		{
			size_t wlen;
			ssize_t nw;

			if (int2sizet (qb -> qb_len, &wlen) != 0) {
				result = tsaplose (td, DR_NETWORK, NULLCP,
								   "blocked write length overflow");
				goto out;
			}
			nw = (*wfnx) (tb -> tb_fd, qb -> qb_data, wlen);
			if (ssize2int (nw, &nc) != 0) {
				result = tsaplose (td, DR_NETWORK, NULLCP,
								   "blocked write result overflow");
				goto out;
			}
		}
		if (nc != qb -> qb_len) {
			if (nc == NOTOK) {
				if (errno != EWOULDBLOCK) {
					result = tsaplose (td, DR_NETWORK, "failed",
									   "write to network");
					goto out;
				}

#if defined(SUN_X25) && defined(X25_WRITE_BUFFER_FULL)
				/* Need SunNet 7.0 patch 100328-09 + full buffer fix */
				if (tb -> tb_flags & TB_X25)
					ioctl (tb -> tb_fd, X25_WRITE_BUFFER_FULL, 0);

#endif
				nc = 0;
			} else if (nc > 0 && (tb -> tb_flags & TB_X25)) {
				SLOG (tsap_log, LLOG_EXCEPTIONS, NULLCP,
					  ("partial write (%d of %d octets) to X.25",
					   nc, qb -> qb_len));
				result = tsaplose (td, DR_NETWORK, NULLCP,
								   "partial write (%d of %d octets) to X.25",
								   nc, qb -> qb_len);
				goto out;
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
#ifdef	FIONBIO
	ioctl (tb -> tb_fd, FIONBIO, (onoff = 0, (char *) &onoff));
#else
#ifdef	O_NDELAY
	fcntl (tb -> tb_fd, F_SETFL, 0x00);
#endif
#endif

	sigiomask (smask);
	signal (SIGPIPE, pstat);

	return result;
}
#endif

int tp0init (struct tsapblk *tb) {
	tb -> tb_connPfnx = TConnect;
	tb -> tb_retryPfnx = TRetry;

	tb -> tb_startPfnx = TStart;
	tb -> tb_acceptPfnx = TAccept;

	tb -> tb_writePfnx = TWrite;
	tb -> tb_readPfnx = TRead;
	tb -> tb_discPfnx = TDisconnect;
	tb -> tb_losePfnx = TLose;

#ifdef	NODELAY
	tb -> tb_drainPfnx = TDrain;
#endif

#ifdef  MGMT
	tb -> tb_manfnx = TManGen;
#endif
}
#endif
