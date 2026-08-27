/* ps_io.c - presentation stream I/O dispatch */

#include <stdio.h>
#include "psap.h"

int ps_io (PS ps, int (*io)(PS ps, PElementData data, PElementLen n, int in_line), PElementData data, PElementLen n, const int in_line) {
	int    cc;

	if (io == NULL)
		return ps_seterr (ps, PS_ERR_EOF, NOTOK);

	while (n > 0)
		switch (cc = (*io) (ps, data, n, in_line)) {
		case NOTOK:
			return NOTOK;

		case OK:
			return ps_seterr (ps, PS_ERR_EOF, NOTOK);

		default:
			data += cc, n -= cc;
			ps -> ps_byteno += cc;
			break;
		}

	return OK;
}
