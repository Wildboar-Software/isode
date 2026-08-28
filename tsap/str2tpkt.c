/* str2tpkt.c - read/write a TPDU thru a string */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include "tpkt.h"
#include "tailor.h"
static int getfnx (int fd, struct tsapkt *t, char *buffer, int n);
static int readfnx (int fd, char *buffer, int n);
static int putfnx (struct tsapblk *tb, struct tsapkt *t, char *cp, int n);
static int writefnx (const struct tsapblk *tb, char *buffer, int n);


static int	readfnx (int fd, char *buffer, int n), getfnx (int fd, struct tsapkt *t, char *buffer, int n), writefnx (const struct tsapblk *tb, char *buffer, int n), putfnx (struct tsapblk *tb, struct tsapkt *t, char *cp, int n);

char *tpkt2str (struct tsapkt *t) {
	int	    cc;
	char    packet[BUFSIZ];
	static char buffer[2 * sizeof packet + 1];

	writefnx ((struct tsapblk *) NOTOK, packet, 0);
	if (tpkt2fd ((struct tsapblk *) 0, t, putfnx) == NOTOK)
		return NULLCP;

	cc = writefnx ((struct tsapblk *) NOTOK, NULLCP, 0);
	if (t -> t_qbuf) {
		if (bcopy_int (t -> t_qbuf -> qb_data, packet + cc,
				   t -> t_qbuf -> qb_len) != 0)
			return NULLCP;
		cc += t -> t_qbuf -> qb_len;
	}
	buffer[explode (buffer, (uint8_t *) packet, cc)] = 0;

	DLOG (tsap_log, LLOG_PDUS,
		  ("write %d bytes, \"%s\"", strlen (buffer), buffer));

	return buffer;
}

struct tsapkt *
str2tpkt (const char *buffer) {
	char    packet[BUFSIZ];
	struct tsapkt *t;

	DLOG (tsap_log, LLOG_PDUS,
		  ("read %d bytes, \"%s\"", strlen (buffer), buffer));

	{
		int blen;

		if (sizet2int (strlen (buffer), &blen) != 0)
			return NULLPKT;
		getfnx (NOTOK, NULLPKT, packet,
				implode ((uint8_t *) packet, buffer, blen));
	}
	t = fd2tpkt (0, getfnx, readfnx);

	return t;
}

static int getfnx (int fd, struct tsapkt *t, char *buffer, int n) {
	static int  cc;

	if (fd == NOTOK) {
		readfnx (NOTOK, buffer, cc = n);
		return OK;
	}

	{
		int hdr;

		if (sizet2int (sizeof t -> t_pkthdr, &hdr) != 0)
			return DR_LENGTH;
		if (cc < 0 || hdr < 0 || cc > INT_MAX - hdr)
			return DR_LENGTH;
		if (int2u16 (cc + hdr, &t -> t_length) != 0)
			return DR_LENGTH;
	}
	t -> t_vrsn = TPKT_VRSN;

	{
		int nli;

		if (sizet2int (sizeof t -> t_li, &nli) != 0)
			return DR_LENGTH;
		if (readfnx (fd, (char *) &t -> t_li, nli) != nli)
			return DR_LENGTH;
	}

	{
		int ncode;

		if (sizet2int (sizeof t -> t_code, &ncode) != 0)
			return DR_LENGTH;
		if (readfnx (fd, (char *) &t -> t_code, ncode) != ncode)
			return DR_LENGTH;
	}

	return OK;
}

static int readfnx (int fd, char *buffer, int n) {
	int    i;
	static int  cc;
	static char *bp;

	if (fd == NOTOK) {
		bp = buffer, cc = n;

		return OK;
	}

	if ((i = min (cc, n)) > 0) {
		if (bcopy_int (bp, buffer, i) != 0)
			return NOTOK;
		bp += i, cc -= i;
	}

	return i;
}

static int putfnx (struct tsapblk *tb, struct tsapkt *t, char *cp, int n) {
	int    cc;
	struct udvec  *uv;

	{
		int nli;

		if (sizet2int (sizeof t -> t_li, &nli) != 0)
			return NOTOK;
		cc = nli;
		if (writefnx (tb, (char *) &t -> t_li, cc) != cc)
			return NOTOK;
	}

	{
		int ncode;

		if (sizet2int (sizeof t -> t_code, &ncode) != 0)
			return NOTOK;
		if (writefnx (tb, (char *) &t -> t_code, ncode) != ncode)
			return NOTOK;
		cc += ncode;
	}

	if (writefnx (tb, cp, n) != n)
		return NOTOK;
	cc += n;

	if (t -> t_vdata
			&& writefnx (tb, t -> t_vdata, t -> t_vlen) != t -> t_vlen)
		return NOTOK;
	cc += t -> t_vlen;

	for (uv = t -> t_udvec; uv -> uv_base; uv++) {
		if (writefnx (tb, uv -> uv_base, uv -> uv_len) != uv -> uv_len)
			return NOTOK;
		cc += uv -> uv_len;
	}

	return cc;
}

static int writefnx (const struct tsapblk *tb, char *buffer, int n) {
	static int  cc;
	static char *bp;

	if (tb) {
		if (buffer == NULLCP)
			return cc;
		bp = buffer, cc = 0;

		return OK;
	}

	if (bcopy_int (buffer, bp, n) != 0)
		return NOTOK;
	bp += n, cc += n;

	return n;
}
