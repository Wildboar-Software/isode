/* ftamd-manage.c - FTAM responder -- management */







#include <stdio.h>
#include "ftamsystem.h"

/* MANAGEMENT */

int ftam_managementindication (struct FTAMgroup *ftg) {
	struct FTAMgroup    ftms;
	struct FTAMgroup   *ftm = &ftms;
	struct FTAMindication   ftis;
	struct FTAMindication *fti = &ftis;

	ftam_selection (ftg, ftm);

	if (myfd != NOTOK) {
#ifndef	BRIDGE
		unlock ();
		close (myfd);
#else
		close (myfd);
		ftp_reply ();
#endif
		myfd = NOTOK;
	}

	if (FManageResponse (ftamfd, ftm, fti) == NOTOK)
		ftam_adios (&fti -> fti_abort, "F-MANAGE.RESPONSE");

	FTGFREE (ftg);
}
