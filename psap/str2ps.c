/* str2ps.c - string-backed abstraction for PStreams */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "psap.h"
static int str_read (PS ps, PElementData data, PElementLen n, int in_line);
static int str_write (PS ps, PElementData data, PElementLen n, int in_line);
static int str_close (PS ps);


static int str_read (PS ps, PElementData data, PElementLen n, int in_line) {
	int    cc;

	if (ps -> ps_base == NULLCP || (cc = ps -> ps_cnt) <= 0)
		return 0;
	if (cc > n)
		cc = n;

	if (bcopy_int (ps -> ps_ptr, data, cc) != 0)
		return 0;
	ps -> ps_ptr += cc, ps -> ps_cnt -= cc;

	return cc;
}

static int str_write (PS ps, PElementData data, PElementLen n, int in_line) {
	int    cc;
	char  *cp;

	if (ps -> ps_base == NULLCP) {
		cc = n;
		if (add_int_to_int (&cc, BUFSIZ) != 0
				|| (cp = malloc_int (cc)) == NULLCP)
			return ps_seterr (ps, PS_ERR_NMEM, NOTOK);
		ps -> ps_base = ps -> ps_ptr = cp;
		ps -> ps_bufsiz = ps -> ps_cnt = cc;
	} else if (ps -> ps_cnt < n) {
		int    curlen;

		if (ptrdiff2int (ps -> ps_ptr - ps -> ps_base, &curlen) != 0)
			return ps_seterr (ps, PS_ERR_NMEM, NOTOK);

		if (ps -> ps_inline) {
			n = ps -> ps_cnt;
			goto partial;
		}

		cc = n;
		if (add_int_to_int (&cc, BUFSIZ) != 0)
			return ps_seterr (ps, PS_ERR_NMEM, NOTOK);
		{
			int newsize = ps -> ps_bufsiz;
			int newcnt = ps -> ps_cnt;

			if (add_int_to_int (&newsize, cc) != 0
					|| add_int_to_int (&newcnt, cc) != 0)
				return ps_seterr (ps, PS_ERR_NMEM, NOTOK);
			if ((cp = realloc_int (ps -> ps_base, newsize)) == NULLCP)
				return ps_seterr (ps, PS_ERR_NMEM, NOTOK);
			ps -> ps_ptr = (ps -> ps_base = cp) + curlen;
			ps -> ps_bufsiz = newsize;
			ps -> ps_cnt = newcnt;
		}
	}
partial:
	;

	if (bcopy_int (data, ps -> ps_ptr, n) != 0)
		return ps_seterr (ps, PS_ERR_NMEM, NOTOK);
	ps -> ps_ptr += n, ps -> ps_cnt -= n;

	return n;
}

static int str_close (PS ps) {
	if (ps -> ps_base && !ps -> ps_inline)
		free (ps -> ps_base);

	return OK;
}

int str_open (PS ps) {
	ps -> ps_readP = str_read;
	ps -> ps_writeP = str_write;
	ps -> ps_closeP = str_close;

	return OK;
}

int str_setup (PS ps, char *cp, const int cc, const int in_line) {
	char  *dp;

	if (in_line) {
		ps -> ps_inline = 1;
		ps -> ps_base = ps -> ps_ptr = cp;
		ps -> ps_bufsiz = ps -> ps_cnt = cc;
	} else if (cc > 0) {
		if ((dp = malloc_int (cc)) == NULLCP)
			return ps_seterr (ps, PS_ERR_NMEM, NOTOK);
		ps -> ps_base = ps -> ps_ptr = dp;
		if (cp != NULLCP && bcopy_int (cp, dp, cc) != 0) {
			free (dp);
			ps -> ps_base = ps -> ps_ptr = NULLCP;
			return ps_seterr (ps, PS_ERR_NMEM, NOTOK);
		}
		ps -> ps_bufsiz = ps -> ps_cnt = cc;
	}

	return OK;
}
