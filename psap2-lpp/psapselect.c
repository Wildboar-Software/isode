/* psapselect.c - PPM: map descriptors */
/* Contributed by The Wollongong Group, Inc. */
#include <signal.h>
#define	LPP
#include "ppkt.h"

/* map presentation descriptors for select() */
int	PSelectMask (int sd, fd_set *mask, int *nfds, struct PSAPindication *pi)
{
	int     smask;
	int	    reason,
			result;
	struct psapblk *pb;

	missingP (mask);
	missingP (nfds);
	missingP (pi);
	smask = sigioblock ();
	if ((pb = findpblk (sd)) == NULL) {
		sigiomask (smask);
		return psaplose (pi, PC_PARAMETER, NULLCP,
						 "invalid presentation descriptor");
	}
	result = pb -> pb_checkfnx ? (*pb -> pb_checkfnx) (pb, pi) : OK;
	if (result == NOTOK && (reason = pi -> pi_abort.pa_reason) != PC_TIMER) {
		if (PC_FATAL (reason))
			freepblk (pb);
	} else {
		FD_SET (pb -> pb_fd, mask);
		if (pb -> pb_fd > *nfds)
			*nfds = pb -> pb_fd + 1;
	}
	sigiomask (smask);
	return result;
}
