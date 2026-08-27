/* daprovider.c - DAP: Support for DAP Actions */

#include <stdio.h>
#include "quipu/dap2.h"
#include "tailor.h"

/*    BIND interface */

int ronot2daplose (struct DAPindication * di, const char * event, const struct RoNOTindication * rni) {
	char	* cp;
	char	  buffer[BUFSIZ];

	/*
		if (event)
			SLOG (addr_log, LLOG_EXCEPTIONS, NULLCP,
				((rni->rni_cc > 0) ? "%s: %s [%*.*s]" : "%s: %s",
				event, AcErrString (rni->rni_reason), rni->rni_cc,
				rni->rni_cc, rni->rni_data));
	*/
	sprintf (cp = buffer, " (Error in RO-BIND)");
	if (rni->rni_cc > 0)
		return (daplose (di, DA_RONOT, NULLCP, "%*.*s%s",
						 rni->rni_cc, rni->rni_cc, rni->rni_data, cp));
	else
		return (daplose (di, DA_RONOT, NULLCP, "%s", cp));
}

/*    ROS interface */

int ros2daplose (struct DAPindication *di, const char *event, struct RoSAPpreject *rop) {
	char	* cp;
	char	  buffer[BUFSIZ];

	/*
		if (event)
			SLOG (addr_log, LLOG_EXCEPTIONS, NULLCP,
				((rop->rop_cc > 0) ? "%s: %s [%*.*s]" : "%s: %s",
				event, AcErrString (rop->rop_reason), rop->rop_cc,
				rop->rop_cc, rop->rop_data));
	*/
	sprintf (cp = buffer, " (Error in ROS)");
	if (rop->rop_cc > 0)
		return (daplose (di, DA_ROS, NULLCP, "%*.*s%s",
						 rop->rop_cc, rop->rop_cc, rop->rop_data, cp));
	else
		return (daplose (di, DA_ROS, NULLCP, "%s", cp));
}

int ros2dapreject (struct DAPindication *di, const char *event, const struct RoSAPureject *rou) {
	char	* cp;
	char	  buffer[BUFSIZ];

	sprintf (cp = buffer, " (Reject at ROS)");
	if (rou->rou_noid)
		return (dapreject (di, DA_ROS, -1, NULLCP, " no op id, reason: %d%s", rou->rou_reason, cp));
	else
		return (dapreject (di, DA_ROS, rou->rou_id, NULLCP, " op id %d, reason: %d%s", rou->rou_id, rou->rou_reason, cp));
}
