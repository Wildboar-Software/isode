/* rtsapwait.c - RTPM: wait for an indication */

#include <stdio.h>
#include <signal.h>
#include "rtpkt.h"

/*    RT-WAIT.REQUEST (pseudo) */

int RtWaitRequest (int sd, int secs, struct RtSAPindication *rti) {
	int	    smask;
	int     result;
	struct assocblk   *acb;

	missingP (rti);

	smask = sigioblock ();

	rtsapPsig (acb, sd);

	result = RtWaitRequestAux (acb, secs, 0, rti);

	sigiomask (smask);

	return result;
}

int RtWaitRequestAux (struct assocblk *acb, int secs, int trans, struct RtSAPindication *rti) {
	if (!trans && (acb -> acb_flags & ACB_PLEASE)) {
		acb -> acb_flags &= ~ACB_PLEASE;

		rti -> rti_type = RTI_TURN;
		{
			struct RtSAPturn  *rtu = &rti -> rti_turn;

			rtu -> rtu_please = 1;
			rtu -> rtu_priority = acb -> acb_priority;
		}

		return DONE;
	}

	return (*acb -> acb_rtwaitrequest) (acb, secs, trans, rti);
}
