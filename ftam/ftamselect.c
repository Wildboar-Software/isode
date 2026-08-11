/* ftamselect.c - FPM: map descriptors */







#include <stdio.h>
#include <signal.h>
#include "fpkt.h"

/*    map ftam descriptors for select() */

int FSelectMask (int sd, fd_set *mask, int *nfds, struct FTAMindication *fti) {
	SBV     smask;
	struct ftamblk *fsb;
	struct PSAPindication   pis;
	struct PSAPabort  *pa = &pis.pi_abort;

	missingP (mask);
	missingP (nfds);
	missingP (fti);

	smask = sigioblock ();

	ftamPsig (fsb, sd);

	if (fsb -> fsb_data.px_ninfo > 0)
		goto waiting;
	if (PSelectMask (fsb -> fsb_fd, mask, nfds, &pis) == NOTOK)
		switch (pa -> pa_reason) {
		case PC_WAITING:
waiting:
			;
			sigiomask (smask);
			return ftamlose (fti, FS_GEN_WAITING, 0, NULLCP, NULLCP);

		default:
			ps2ftamlose (fsb, fti, "PSelectMask", pa);
			freefsblk (fsb);
			sigiomask (smask);
			return NOTOK;
		}

	sigiomask (smask);

	return OK;
}
