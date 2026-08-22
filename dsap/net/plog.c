/* plog.c - lower-layer logging routines */

#include "quipu/util.h"
#include "quipu/dsap.h"
#include "tsap.h"
#include <signal.h>

extern	LLog	* log_dsap;

#ifdef LINUX
__sighandler_t	abort_vector = NULL;
#else
SFP	abort_vector = NULL;
#endif

void ros_log (struct RoSAPpreject *rop, char *event) {
	int level = LLOG_EXCEPTIONS;

	if ((rop->rop_reason == ROS_TIMER) || (rop->rop_reason == ROS_ACS))
		level = LLOG_DEBUG;
	if(rop->rop_cc > 0)
		LLOG(log_dsap, level, ("%s: [%s] %*.*s", event,
							   RoErrString(rop->rop_reason), rop->rop_cc, rop->rop_cc,
							   rop->rop_data));
	else
		LLOG(log_dsap, level, ("%s: [%s]", event, RoErrString(rop->rop_reason)));
	if (rop -> rop_reason == ROS_CONGEST && abort_vector)
		(*abort_vector) (-2);
}

void acs_log (struct AcSAPabort *aca, char *event) {
	if(aca->aca_cc > 0)
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("%s: [%s] %*.*s", event,
										 AcErrString(aca->aca_reason),
										 aca->aca_cc, aca->aca_cc, aca->aca_data));
	else
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("%s: [%s]", event, AcErrString(aca->aca_reason)));
	if (aca -> aca_reason == ACS_CONGEST && abort_vector)
		(*abort_vector) (-2);
}

int td_log (struct TSAPdisconnect *td, char *event) {
	if(td->td_cc > 0) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("%s: [%s] %*.*s", event,
										 TErrString(td->td_reason),
										 td->td_cc, td->td_cc, td->td_data));
	} else {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("%s: [%s]", event, TErrString(td->td_reason)));
	}
}

int SetROPS (int ad) {
	struct RoSAPindication      roi_s;
	struct RoSAPindication      *roi = &(roi_s);
	struct RoSAPpreject         *rop = &(roi->roi_preject);

	if(RoSetService(ad, RoPService, roi) != OK) {
		ros_log(rop, "RoSetService");
		return NOTOK;
	}
	return(OK);
}
