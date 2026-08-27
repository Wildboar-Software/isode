/* fdx2ps.c - full-duplex abstraction for PStreams */

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "psap.h"

int	fdx_reset (PS ps);

static int fdx_prime (PS ps, int waiting);
static int fdx_read (PS ps, PElementData data, PElementLen n, int in_line);
static int fdx_write (PS ps, PElementData data, PElementLen n, int in_line);
int fdx_reset (PS ps);
static int  fdx_flush (PS ps);

struct ps_fdx {
	int	    ps_fd;

	struct ps_inout {
		char   *pio_base;
		int	pio_bufsiz;

		char   *pio_ptr;
		int	pio_cnt;
	}	    ps_input,
	  ps_output;

	int	    ps_nflush;
};

extern int (*set_check_fd (const int fd, int (*fnx)(int fd, caddr_t data), caddr_t data))(int, void *);

static int fdx_prime (PS ps, int waiting) {
	struct ps_fdx *pt = (struct ps_fdx *) ps -> ps_addr;
	struct ps_inout *pi = &pt -> ps_input;
	return (waiting > 0 && pi -> pio_cnt > 0 ? DONE : OK);
}

static int fdx_read (PS ps, PElementData data, PElementLen n, int in_line) {
	int	    cc;
	struct ps_fdx *pt = (struct ps_fdx *) ps -> ps_addr;
	struct ps_inout *pi = &pt -> ps_input;

	if ((cc = pi -> pio_cnt) <= 0) {
		if (n > pi -> pio_bufsiz) {
			ssize_t nread;

			nread = read_int (pt -> ps_fd, (char *) data, n);
			if (ssize2int (nread, &cc) != 0)
				return ps_seterr (ps, PS_ERR_IO, NOTOK);
			if (cc == NOTOK)
				return ps_seterr (ps, PS_ERR_IO, NOTOK);
			return cc;
		}
		{
			ssize_t nread;

			nread = read_int (pt -> ps_fd, pi -> pio_base, pi -> pio_bufsiz);
			if (ssize2int (nread, &cc) != 0)
				return ps_seterr (ps, PS_ERR_IO, NOTOK);
			if (cc == NOTOK)
				return ps_seterr (ps, PS_ERR_IO, NOTOK);
		}
		pi -> pio_ptr = pi -> pio_base, pi -> pio_cnt = cc;
	}
	if (cc > n)
		cc = n;
	if (bcopy_int (pi -> pio_ptr, (char *) data, cc) != 0)
		return ps_seterr (ps, PS_ERR_IO, NOTOK);
	pi -> pio_ptr += cc, pi -> pio_cnt -= cc;
	return cc;
}

static int fdx_write (PS ps, PElementData data, PElementLen n, int in_line) {
	int	    cc;
	struct ps_fdx *pt = (struct ps_fdx *) ps -> ps_addr;
	struct ps_inout *po = &pt -> ps_output;
#ifdef	oldef
	if (n > po -> pio_bufsiz) {
		ssize_t nwritten;

		if (fdx_flush (ps) == NOTOK)
			return ps_seterr (ps, PS_ERR_IO, NOTOK);
		nwritten = write (pt -> ps_fd, (char *) data, n);
		if (ssize2int (nwritten, &cc) != 0 || cc != n)
			return ps_seterr (ps, PS_ERR_IO, NOTOK);

		return cc;
	}
#else
	if (n > po -> pio_bufsiz && po -> pio_ptr <= po -> pio_base) {
		ssize_t nwritten;

		nwritten = write_int (pt -> ps_fd, (char *) data, n);
		if (ssize2int (nwritten, &cc) != 0 || cc != n)
			return ps_seterr (ps, PS_ERR_IO, NOTOK);

		return cc;
	}
#endif
	if (n > po -> pio_cnt)
		n = po -> pio_cnt;
	if (bcopy_int ((char *) data, po -> pio_ptr, n) != 0)
		return ps_seterr (ps, PS_ERR_IO, NOTOK);
	po -> pio_ptr += n, po -> pio_cnt -= n;
	if (po -> pio_cnt <= 0 && fdx_flush (ps) == NOTOK)
		return ps_seterr (ps, PS_ERR_IO, NOTOK);
	return n;
}

static int  fdx_flush (PS ps)
{
	int	    cc;
	struct ps_fdx *pt = (struct ps_fdx *) ps -> ps_addr;
	struct ps_inout *po = &pt -> ps_output;
	if (ptrdiff2int (po -> pio_ptr - po -> pio_base, &cc) != 0)
		return ps_seterr (ps, PS_ERR_IO, NOTOK);
	if (cc <= 0)
		return OK;
	pt -> ps_nflush++;
	{
		ssize_t nwritten;
		int nw;

		nwritten = write_int (pt -> ps_fd, po -> pio_base, cc);
		if (ssize2int (nwritten, &nw) != 0 || nw != cc)
			return ps_seterr (ps, PS_ERR_IO, NOTOK);
	}
	po -> pio_ptr = po -> pio_base, po -> pio_cnt = po -> pio_bufsiz;
	return OK;
}

static int  fdx_close (PS ps)
{
	struct ps_fdx *pt = (struct ps_fdx *) ps -> ps_addr;
	if (pt == NULL)
		return OK;
	if (pt -> ps_input.pio_base)
		free (pt -> ps_input.pio_base);
	if (pt -> ps_output.pio_base)
		free (pt -> ps_output.pio_base);
	set_check_fd (pt -> ps_fd, NULL, NULLCP);
	free ((char *) pt);
	return OK;
}

static int  fdx_check (int fd, caddr_t data) {
	return (ps_prime ((PS) data, 1) > 0 ? DONE : OK);
}

int	fdx_open (PS ps)
{
	ps -> ps_primeP = fdx_prime;
	ps -> ps_readP = fdx_read;
	ps -> ps_writeP = fdx_write;
	ps -> ps_flushP = fdx_flush;
	ps -> ps_closeP = fdx_close;
	return OK;
}

int	fdx_setup (PS ps, const int fd)
{
	int	    pz;
	size_t	    n;
	struct ps_fdx *pt;

	if ((pt = (struct ps_fdx *) calloc (1, sizeof *pt)) == NULL)
		return ps_seterr (ps, PS_ERR_NMEM, NOTOK);
	ps -> ps_addr = (caddr_t) pt;
	pt -> ps_fd = fd;
#ifdef	BSD42
	if ((pz = getpagesize ()) <= 0)
#endif
		pz = BUFSIZ;
	if (int2sizet (pz, &n) != 0)
		return ps_seterr (ps, PS_ERR_NMEM, NOTOK);
	if ((pt -> ps_input.pio_base = malloc (n)) == NULL
			|| (pt -> ps_output.pio_base = malloc (n)) == NULL)
		return ps_seterr (ps, PS_ERR_NMEM, NOTOK);
	pt -> ps_input.pio_bufsiz = pz, pt -> ps_output.pio_cnt = 0;
	pt -> ps_input.pio_ptr = pt -> ps_input.pio_base;
	pt -> ps_output.pio_bufsiz = pt -> ps_output.pio_cnt = pz;
	pt -> ps_output.pio_ptr = pt -> ps_output.pio_base;
	set_check_fd (fd, fdx_check, (caddr_t) ps);
	return OK;
}

int	fdx_reset (PS ps)
{
	struct ps_fdx *pt = (struct ps_fdx *) ps -> ps_addr;
	struct ps_inout *po = &pt -> ps_output;

	if (pt -> ps_nflush == 0)
		po -> pio_ptr = po -> pio_base, po -> pio_cnt = po -> pio_bufsiz;
	else
		return ps_seterr (ps, PS_ERR_NONE, NOTOK);
	return OK;
}
