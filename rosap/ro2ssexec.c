/* ro2ssexec.c - RTPM: exec */
/*
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 */

#include <stdio.h>
#include <stdint.h>
#include <strings.h>
#include <unistd.h>
#include "ROS-types.h"
#include "../acsap/OACS-types.h"
#include "ropkt.h"
#include "isoservent.h"
#include "tailor.h"
#include "pvpdu.h"
#include "pepsycodec.h"
#include "internet.h"

/*    SERVER only */

int RoExec (struct SSAPstart *ss, struct RoSAPindication *roi, char *arg1, char *arg2, IFP hook, IFP setperms) {
	int     result,
			result2;
	struct isoservent *is;
	PE	    pe;
	struct SSAPref ref;
	struct SSAPindication   sis;
	struct SSAPindication *si = &sis;
	struct type_OACS_PConnect	*pcon;

	missingP (ss);
	missingP (roi);
	missingP (arg1);
	missingP (arg2);

	if ((pe = ssdu2pe (ss -> ss_data, ss -> ss_cc, NULLCP, &result)) == NULLPE
			|| parse_OACS_PConnect (pe, 1, NULL, NULLVP, &pcon) == NOTOK) {
		if (pe)
			pe_free (pe);
		if (result == PS_ERR_NMEM) {
congest:
			;
			result = SC_CONGESTION, result2 = ROS_CONGEST;
		} else
			result = SC_REJECTED, result2 = ROS_PROTOCOL;
		goto out;
	}

	PLOGP (rosap_log,OACS_PConnect, pe, "PConnect", 1);

	pe_free (pe);

	if (pcon -> pUserData -> member_OACS_2 -> offset
			!= type_OACS_ConnectionData_open) {
		result = SC_REJECTED, result2 = ROS_ADDRESS;
		goto out;
	}
	if (is = getisoserventbyport ("rosap",
								  (uint16_t) htons ((uint16_t) pcon -> pUserData -> applicationProtocol))) {
		*is -> is_tail++ = arg1;
		*is -> is_tail++ = arg2;
		*is -> is_tail = NULL;
	} else {
		result = SC_REJECTED, result2 = ROS_ADDRESS;
		goto out;
	}

	switch (hook ? (*hook) (is, roi) : OK) {
	case NOTOK:
		return NOTOK;

	case DONE:
		return OK;

	case OK:
		if (setperms)
			(*setperms) (is);
		execv (*is -> is_vec, is -> is_vec);/* fall */
		SLOG (rosap_log, LLOG_FATAL, *is -> is_vec, ("unable to exec"));
	default:
		goto congest;
	}

out:
	;
	SSFREE (ss);

	bzero ((char *) &ref, sizeof ref);
	SConnResponse (ss -> ss_sd, &ref, NULLSA,
				   result, 0, 0, SERIAL_NONE, NULLCP, 0, si);
	return rosaplose (roi, result2, NULLCP, NULLCP);
}
