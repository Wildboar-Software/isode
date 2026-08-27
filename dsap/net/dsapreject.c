/* dsapreject.c - DSAP: Reject a remote operations event */

#include "logger.h"
#include "quipu/util.h"
#include "quipu/dsap.h"
#include "quipu/watchdog.h"

extern LLog	* log_dsap;

int DRejectRequest (int sd, int reason, const int id) {
	int	  result;
	struct RoSAPindication	  roi_s;
	struct RoSAPindication	* roi = &(roi_s);

	DLOG (log_dsap,LLOG_TRACE,( "DRejectRequest()"));
	watch_dog("RoURejectRequest");
	result = RoURejectRequest(sd, &id, reason, ROS_NOPRIO, roi);
	watch_dog_reset();
	if (result == NOTOK) {
		LLOG (log_dsap, LLOG_EXCEPTIONS,( "DBindReject: RoURejectRequest failed"));
	}
}
