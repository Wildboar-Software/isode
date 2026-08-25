/* fd2tpkt.c - read/write a TPDU thru a socket */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <signal.h>
#include <arpa/inet.h>
#include "tpkt.h"
#include "tailor.h"
#include "internet.h"

static int  fd2tpktaux (int fd, struct tsapkt *t, int (*initfnx)(int fd, struct tsapkt *t, char *buffer, int n), int (*readfnx)(int fd, char *buffer, int n));
static int  readx (int fd, char *buffer, int n, int (*readfnx)(int fd, char *buffer, int n));
static int  set_tpdu_li (struct tsapkt *t, size_t hdr);
static int  set_varlen (struct tsapkt *t, size_t minlen, int *vlen);

static int
set_varlen (struct tsapkt *t, size_t minlen, int *vlen)
{
	int n;

	if (u8_minus_sizet (t -> t_li, minlen, &n) != 0)
		return DR_LENGTH;
	t -> t_vlen = n;
	*vlen = n;
	return OK;
}

static int
set_tpdu_li (struct tsapkt *t, size_t hdr)
{
	size_t n;

	if (int2sizet (t -> t_vlen, &n) != 0)
		return NOTOK;
	if (hdr > 0xfe || n > 0xfe - hdr)
		return NOTOK;
	t -> t_li = (uint8_t) (hdr + n);
	return OK;
}

struct tsapkt *fd2tpkt (int fd, int (*initfnx)(int fd, struct tsapkt *t, char *buffer, int n), int (*readfnx)(int fd, char *buffer, int n)) {
	struct tsapkt *t;

	if ((t = newtpkt (0)) == NULL)
		return NULL;

	if ((t -> t_errno = fd2tpktaux (fd, t, initfnx, readfnx)) != OK) {
		if (t -> t_vdata != NULL)
			free (t -> t_vdata), t -> t_vdata = NULL, t -> t_vlen = 0;

		if (t -> t_qbuf)
			free ((char *) t -> t_qbuf), t -> t_qbuf = NULL;
	}

#ifdef	DEBUG
	if (tsap_log -> ll_events & LLOG_PDUS)
		tpkt2text (tsap_log, t, 1);
#endif

	return t;
}

static int fd2tpktaux (int fd, struct tsapkt *t, int (*initfnx)(int fd, struct tsapkt *t, char *buffer, int n), int (*readfnx)(int fd, char *buffer, int n)) {
	int    code, len, vlen;
	char  *vptr;

	if ((code = (*initfnx) (fd, t, NULL, 0)) != OK)
		return code;

	if (t -> t_li > TPDU_MAXLEN (t))
		return DR_LENGTH;

	switch (TPDU_CODE (t)) {
	case TPDU_CR:
	case TPDU_CC:
		if (t -> t_li < TPDU_MINLEN (t, CR))
			return DR_LENGTH;
		if (readx (fd, (char *) &t -> t_cr, CR_SIZE (t), readfnx)
				!= CR_SIZE (t))
			return DR_NETWORK;

		if (set_varlen (t, TPDU_MINLEN (t, CR), &vlen) != OK)
			return DR_LENGTH;
		if (vlen) {
			if ((vptr = t -> t_vdata = malloc ((unsigned) vlen)) == NULL)
				return DR_CONGEST;
			if (readx (fd, t -> t_vdata, t -> t_vlen, readfnx)
					!= t -> t_vlen)
				return DR_NETWORK;
			for (; vlen > 0; vptr += len, vlen -= len) {
				int	    ilen;

				if (vlen < 2)
					return DR_LENGTH;
				code = *vptr++ & 0xff;
				len = *vptr++ & 0xff;
				if ((vlen -= 2) < len)
					return DR_LENGTH;

				switch (code) {
				case VDAT_TSAP_SRV:
					if ((ilen = len) > sizeof t -> t_called)
						ilen = sizeof t -> t_called;
					if (bcopy_int (vptr, t -> t_called, ilen) != 0)
						return DR_LENGTH;
					t -> t_calledlen = ilen;
					break;

				case VDAT_TSAP_CLI:
					if ((ilen = len) > sizeof t -> t_calling)
						ilen = sizeof t -> t_calling;
					if (bcopy_int (vptr, t -> t_calling, ilen) != 0)
						return DR_LENGTH;
					t -> t_callinglen = ilen;
					break;

				case VDAT_SIZE:
					if (len != 1)
						return DR_LENGTH;
					t -> t_tpdusize = *(uint8_t *) vptr;
					break;

				case VDAT_OPTIONS:
					if (len != 1)
						return DR_LENGTH;
					t -> t_options = *vptr & 0xff;
					break;

				case VDAT_ALTERNATE: {
					int i;
					char *ap;

					for (ap = vptr, i = len; i > 0; ap++, i--)
						t -> t_cr.cr_alternate |=
							1 << ((*ap >> 4) & 0x0f);
				}
				break;

				case VDAT_VRSN:
				case VDAT_SECURITY:
				case VDAT_CHECKSUM:
				case VDAT_ACKTIME:
				case VDAT_THROUGHPUT:
				case VDAT_ERRORATE:
				case VDAT_PRIORITY:
				case VDAT_DELAY:
				case VDAT_TTR:
					break;

				default: 	/* IS 8073 says to ignore it on CRs */
					SLOG (tsap_log, LLOG_EXCEPTIONS, NULLCP,
						  ("unknown option 0x%x (length 0x%x) in %s TPDU",
						   code, len,
						   TPDU_CODE (t) == TPDU_CR ? "CR" : "CC"));
					if (TPDU_CODE (t) == TPDU_CR)
						break;
					return DR_PROTOCOL;
				}
			}
		}
		break;

	case TPDU_DR:
		if (t -> t_li < TPDU_MINLEN (t, DR))
			return DR_LENGTH;
		if (readx (fd, (char *) &t -> t_dr, DR_SIZE (t), readfnx)
				!= DR_SIZE (t))
			return DR_NETWORK;

		if (set_varlen (t, TPDU_MINLEN (t, DR), &vlen) != OK)
			return DR_LENGTH;
		if (vlen) {
			if ((vptr = t -> t_vdata = malloc ((unsigned) vlen)) == NULL)
				return DR_CONGEST;
			if (readx (fd, t -> t_vdata, t -> t_vlen, readfnx)
					!= t -> t_vlen)
				return DR_NETWORK;
			for (; vlen > 0; vptr += len, vlen -= len) {
				if (vlen < 2)
					return DR_LENGTH;
				code = *vptr++ & 0xff;
				len = *vptr++ & 0xff;
				if ((vlen -= 2) < len)
					return DR_LENGTH;

				switch (code) {
				case VDAT_ADDITIONAL:
				case VDAT_CHECKSUM:
					break;

				default:
					return DR_PROTOCOL;
				}
			}
		}
		break;

	case TPDU_DT:
		if (t -> t_li < TPDU_MINLEN (t, DT))
			return DR_LENGTH;
		if (readx (fd, (char *) &t -> t_dt, DT_SIZE (t), readfnx)
				!= DT_SIZE (t))
			return DR_NETWORK;

		if (set_varlen (t, TPDU_MINLEN (t, DT), &vlen) != OK)
			return DR_LENGTH;
		if (vlen) {
			if ((vptr = t -> t_vdata = malloc ((unsigned) vlen)) == NULL)
				return DR_CONGEST;
			if (readx (fd, t -> t_vdata, t -> t_vlen, readfnx)
					!= t -> t_vlen)
				return DR_NETWORK;
			for (; vlen > 0; vptr += len, vlen -= len) {
				if (vlen < 2)
					return DR_LENGTH;
				code = *vptr++ & 0xff;
				len = *vptr++ & 0xff;
				if ((vlen -= 2) < len)
					return DR_LENGTH;

				switch (code) {
				case VDAT_CHECKSUM:
					break;

				default:
					return DR_PROTOCOL;
				}
			}
		}
		break;

	case TPDU_ED:
		if (t -> t_li < TPDU_MINLEN (t, ED))
			return DR_LENGTH;
		if (readx (fd, (char *) &t -> t_ed, ED_SIZE (t), readfnx)
				!= ED_SIZE (t))
			return DR_NETWORK;

		if (set_varlen (t, TPDU_MINLEN (t, ED), &vlen) != OK)
			return DR_LENGTH;
		if (vlen) {
			if ((vptr = t -> t_vdata = malloc ((unsigned) vlen)) == NULL)
				return DR_CONGEST;
			if (readx (fd, t -> t_vdata, t -> t_vlen, readfnx)
					!= t -> t_vlen)
				return DR_NETWORK;

			for (; vlen > 0; vptr += len, vlen -= len) {
				if (vlen < 2)
					return DR_LENGTH;
				code = *vptr++ & 0xff;
				len = *vptr++ & 0xff;
				if ((vlen -= 2) < len)
					return DR_LENGTH;

				switch (code) {
				case VDAT_CHECKSUM:
				case VDAT_SUBSEQ:
				case VDAT_FLOWCTL:
					break;

				default:
					return DR_PROTOCOL;
				}
			}
		}
		break;

	case TPDU_ER:
		if (t -> t_li < TPDU_MINLEN (t, ER))
			return DR_LENGTH;
		if (readx (fd, (char *) &t -> t_er, ER_SIZE (t), readfnx)
				!= ER_SIZE (t))
			return DR_NETWORK;

		if (set_varlen (t, TPDU_MINLEN (t, ER), &vlen) != OK)
			return DR_LENGTH;
		if (vlen) {
			if ((vptr = t -> t_vdata = malloc ((unsigned) vlen)) == NULL)
				return DR_CONGEST;
			if (readx (fd, t -> t_vdata, t -> t_vlen, readfnx)
					!= t -> t_vlen)
				return DR_NETWORK;
			for (; vlen > 0; vptr += len, vlen -= len) {
				if (vlen < 2)
					return DR_LENGTH;
				code = *vptr++ & 0xff;
				len = *vptr++ & 0xff;
				if ((vlen -= 2) < len)
					return DR_LENGTH;

				switch (code) {
				case VDAT_INVALID:
				case VDAT_CHECKSUM:
					break;

				default:
					return DR_PROTOCOL;
				}
			}
		}
		break;

	default:
		return DR_PROTOCOL;
	}

	{
		size_t hdr,
			ulen_n;

		hdr = sizeof t -> t_pkthdr + sizeof t -> t_li + (size_t) t -> t_li;
		if ((size_t) t -> t_length <= hdr)
			len = 0;
		else if (sizet2int ((size_t) t -> t_length - hdr, &len) != 0)
			return DR_LENGTH;
	}
	if (len) {
		if ((t -> t_qbuf = (struct qbuf *)
						   malloc (sizeof *t -> t_qbuf + (unsigned) len))
				== NULL)
			return DR_CONGEST;
		t -> t_qbuf -> qb_forw = t -> t_qbuf -> qb_back = t -> t_qbuf;
		if (readx (fd, t -> t_qbuf -> qb_data = t -> t_qbuf -> qb_base,
				   t -> t_qbuf -> qb_len = len, readfnx) != len)
			return DR_NETWORK;
	}

	return OK;
}

static int readx (int fd, char *buffer, int n, int (*readfnx)(int fd, char *buffer, int n)) {
	int    i,
		   cc;
	char   *bp;

	for (bp = buffer, i = n; i > 0; bp += cc, i -= cc) {
		switch (cc = (*readfnx) (fd, bp, i)) {
		case NOTOK: {
			int got;

			if (ptrdiff2int (bp - buffer, &got) != 0)
				return NOTOK;
			return got ? got : NOTOK;
		}

		case OK:
			break;

		default:
			continue;
		}
		break;
	}

	{
		int got;

		if (ptrdiff2int (bp - buffer, &got) != 0)
			return NOTOK;
		return got;
	}
}

int tpkt2fd (struct tsapblk *tb, struct tsapkt *t, int (*writefnx)(struct tsapblk *tb, struct tsapkt *t, char *cp, int n)) {
	int     i,
			ilen,
			ulen;
	char   *cp,
		   *vptr,
		   *outptr;
	struct udvec  *uv;
#ifdef LINUX
	__sighandler_t pstat;
#else
	SFP	    pstat;
#endif
	SBV	    smask;

	if (t -> t_errno != OK)
		return t -> t_errno;

	if (t -> t_vrsn != TPKT_VRSN)
		if (t -> t_vrsn) {
			return DR_PROTOCOL;
		} else {
			t -> t_vrsn = TPKT_VRSN;
		}

	if (t -> t_vdata != NULL) {
		free (t -> t_vdata);
		t -> t_vdata = NULL;
	}
	t -> t_vlen = 0;

	for (ulen = 0, uv = t -> t_udvec; uv -> uv_base; uv++)
		ulen += uv -> uv_len;

	switch (TPDU_CODE (t)) {
	case TPDU_CR:
	case TPDU_CC:
		if ((vptr = t -> t_vdata =
						malloc ((unsigned) (3 + 7 + (2 + t -> t_callinglen)
											+ (2 + t -> t_calledlen) + 3))) == NULL)
			return DR_CONGEST;
		if (t -> t_options) {
			if (put_octet (&vptr, VDAT_OPTIONS) != 0
					|| put_octet (&vptr, 1) != 0
					|| u16tooctet (t -> t_options, vptr) != 0)
				return DR_LENGTH;
			vptr++;
			t -> t_vlen += 3;
		}
		if (CR_CLASS (t) != CR_CLASS_TP0 && t -> t_cr.cr_alternate) {
			/* XXX: this doesn't preserve the order of alternates */
			if (put_octet (&vptr, VDAT_ALTERNATE) != 0)
				return DR_LENGTH;
			cp = vptr++;
			if (t -> t_cr.cr_alternate & ALT_TP0) {
				if (put_octet (&vptr, CR_CLASS_TP0) != 0)
					return DR_LENGTH;
			}
			if (t -> t_cr.cr_alternate & ALT_TP1) {
				if (put_octet (&vptr, CR_CLASS_TP1) != 0)
					return DR_LENGTH;
			}
			if (t -> t_cr.cr_alternate & ALT_TP2) {
				if (put_octet (&vptr, CR_CLASS_TP2) != 0)
					return DR_LENGTH;
			}
			if (t -> t_cr.cr_alternate & ALT_TP3) {
				if (put_octet (&vptr, CR_CLASS_TP3) != 0)
					return DR_LENGTH;
			}
			if (t -> t_cr.cr_alternate & ALT_TP4) {
				if (put_octet (&vptr, CR_CLASS_TP4) != 0)
					return DR_LENGTH;
			}
			if (ptrdiff2int ((vptr - cp) - 1, &i) != 0
					|| int2octet (i & 0xff, cp) != 0)
				return DR_LENGTH;
			t -> t_vlen += (2 + i) & 0xff;
		}
		if (t -> t_callinglen > 0) {
			uint8_t nlen;

			if (int2u8 (t -> t_callinglen, &nlen) != 0)
				return DR_LENGTH;
			if (put_octet (&vptr, VDAT_TSAP_CLI) != 0)
				return DR_LENGTH;
			*(uint8_t *) vptr++ = nlen;
			if (bcopy_int (t -> t_calling, vptr, t -> t_callinglen) != 0)
				return DR_LENGTH;
			vptr += t -> t_callinglen;
			t -> t_vlen += 2 + t -> t_callinglen;
		}
		if (t -> t_calledlen > 0) {
			uint8_t nlen;

			if (int2u8 (t -> t_calledlen, &nlen) != 0)
				return DR_LENGTH;
			if (put_octet (&vptr, VDAT_TSAP_SRV) != 0)
				return DR_LENGTH;
			*(uint8_t *) vptr++ = nlen;
			if (bcopy_int (t -> t_called, vptr, t -> t_calledlen) != 0)
				return DR_LENGTH;
			vptr += t -> t_calledlen;
			t -> t_vlen += 2 + t -> t_calledlen;
		}
		if (t -> t_tpdusize) {
			if (put_octet (&vptr, VDAT_SIZE) != 0
					|| put_octet (&vptr, 1) != 0)
				return DR_LENGTH;
			*(uint8_t *) vptr++ = t -> t_tpdusize;
			t -> t_vlen += 3;
		}
		if (t -> t_vlen == 0) {
			free (t -> t_vdata);
			t -> t_vdata = NULL;
		}
		if (set_tpdu_li (t, TPDU_MINLEN (t, CR)) != OK)
			return DR_LENGTH;
		outptr = (char *) &t -> t_cr;
		ilen = CR_SIZE (t);
		break;

	case TPDU_DR:
		if (set_tpdu_li (t, TPDU_MINLEN (t, DR)) != OK)
			return DR_LENGTH;
		outptr = (char *) &t -> t_dr;
		ilen = DR_SIZE (t);
		break;

	case TPDU_DT:
		if (set_tpdu_li (t, TPDU_MINLEN (t, DT)) != OK)
			return DR_LENGTH;
		outptr = (char *) &t -> t_dt;
		ilen = DT_SIZE (t);
		break;

	case TPDU_ED:
		if (set_tpdu_li (t, TPDU_MINLEN (t, ED)) != OK)
			return DR_LENGTH;
		outptr = (char *) &t -> t_ed;
		ilen = ED_SIZE (t);
		break;

	case TPDU_ER:
		if (set_tpdu_li (t, TPDU_MINLEN (t, ER)) != OK)
			return DR_LENGTH;
		outptr = (char *) &t -> t_er;
		ilen = ER_SIZE (t);
		if (ulen > 0)
			return DR_PROTOCOL;
		break;

	default:
		return DR_PROTOCOL;
	}

	{
		int pktlen;
		uint16_t nlen;

		if (t -> t_li > INT_MAX - 5
				|| ulen > INT_MAX - 5 - t -> t_li)
			return DR_LENGTH;
		pktlen = t -> t_li + 5 + ulen;
		if (int2u16 (pktlen, &nlen) != 0)
			return DR_LENGTH;
		t -> t_length = htons (nlen);
	}

#ifdef	DEBUG
	if (tsap_log -> ll_events & LLOG_PDUS)
		tpkt2text (tsap_log, t, 0);
#endif

	pstat = signal (SIGPIPE, (__sighandler_t)SIG_IGN);
	smask = sigioblock ();

	i = (*writefnx) (tb, t, outptr, ilen);

	sigiomask (smask);
	signal (SIGPIPE, pstat);

	if (i != NOTOK)
		i = OK;
	else if (t -> t_errno == DR_UNKNOWN)
		t -> t_errno = DR_NETWORK;

	return i;
}

struct tsapkt *newtpkt (int code) {
	struct tsapkt *t;

	t = (struct tsapkt *) calloc (1, sizeof *t);
	if (t == NULL)
		return NULL;

	t -> t_vrsn = TPKT_VRSN;
	if (int2u8 (code, &t -> t_code) != 0) {
		free (t);
		return NULL;
	}

	return t;
}

void freetpkt (struct tsapkt *t) {
	if (t == NULL)
		return;

	if (t -> t_vdata)
		free (t -> t_vdata);

	if (t -> t_qbuf)
		free ((char *) t -> t_qbuf);

	free ((char *) t);
}
