/* rtsapwait.c - RTPM: wait for an indication */
#include <signal.h>
#include "ropkt.h"
#include "rtpkt.h"

int RtWaitRequestAux (
	struct assocblk *acb,
	const int secs,
	const int trans,
	struct RtSAPindication *rti
);

/*    RT-WAIT.REQUEST (pseudo) */

int RtWaitRequest (const int sd, const int secs, struct RtSAPindication *rti) {
	SBV	    smask;
	int     result;
	struct assocblk   *acb;

	missingP (rti);
	smask = sigioblock ();
	rtsapPsig (acb, sd);
	result = RtWaitRequestAux (acb, secs, 0, rti);
	sigiomask (smask);
	return result;
}

int RtWaitRequestAux (
	struct assocblk *acb,
	const int secs,
	const int trans,
	struct RtSAPindication *rti
) {
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
