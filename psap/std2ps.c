/* std2ps.c - stdio-backed abstraction for PStreams */

#include <stdio.h>
#include "psap.h"
static int std_read (PS ps, PElementData data, PElementLen n, int in_line);
static int std_write (PS ps, PElementData data, PElementLen n, int in_line);
int std_flush (PS ps);


static int std_read (PS ps, PElementData data, PElementLen n, int in_line) {
	int	    i;

	if ((i = fread ((char *) data, sizeof *data, (int) n,
					(FILE *) ps -> ps_addr)) == NOTOK)
		ps -> ps_errno = PS_ERR_IO;

	return i;
}

static int std_write (PS ps, PElementData data, PElementLen n, int in_line) {
	int	    i;

	if ((i = fwrite ((char *) data, sizeof *data, (int) n,
					 (FILE *) ps -> ps_addr)) == NOTOK)
		ps -> ps_errno = PS_ERR_IO;

	return i;
}

int std_flush (PS ps) {
	if (fflush ((FILE *) ps -> ps_addr) != EOF)
		return OK;

	return ps_seterr (ps, PS_ERR_IO, NOTOK);
}

int std_open (PS ps) {
	ps -> ps_readP = std_read;
	ps -> ps_writeP = std_write;
	ps -> ps_flushP = std_flush;

	return OK;
}
