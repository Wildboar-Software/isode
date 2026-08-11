/* ns_ro_ureject.c - */

#include "rosap.h"
#include "quipu/util.h"
#include "quipu/connection.h"

extern	LLog	* log_dsap;

int send_ro_ureject (int ad, int *id_p, int urej) {
	struct RoSAPindication      roi_s;
	struct RoSAPindication      *roi = &roi_s;
	struct RoSAPpreject         *rop = &(roi->roi_preject);

	DLOG(log_dsap, LLOG_TRACE, ("send_ro_ureject()"));

	watch_dog("RoURejectRequest");
	if(RoURejectRequest(ad, id_p, urej, ROS_NOPRIO, roi) == NOTOK) {
		watch_dog_reset();
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("RO-U-REJECT.REQUEST: %s",
										 RoErrString(rop->rop_reason)));
		if(ROS_FATAL(rop->rop_reason) || (rop->rop_reason == ROS_PARAMETER)) {
			LLOG(log_dsap, LLOG_FATAL, ("RoUReject fatal PReject"));
		}
	} else
		watch_dog_reset();

}

