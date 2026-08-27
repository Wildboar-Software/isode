/* rt2ssexec.c - RTPM: exec */

#include <stdio.h>
#include <stdint.h>
#include <strings.h>
#include <unistd.h>
#include "RTS-types.h"
#include "OACS-types.h"
#include "rtpkt.h"
#include "isoservent.h"
#include "tailor.h"
#include "pvpdu.h"
#include "pepsycodec.h"
#include "internet.h"

/*    SERVER only */

int RtExec (
	struct SSAPstart *ss,
	struct RtSAPindication *rti,
	const char *arg1,
	const char *arg2,
	int (*hook)(struct isoservent *is, struct RtSAPindication *rti),
#ifndef	IAE
	int (*setperms) (struct isoservent *)
#else
	int (*setperms) (struct IAEntry *)
#endif
) {
	int     result,
			result2;
	struct isoservent *is;
	PE	    pe;
	struct SSAPref ref;
	struct SSAPindication   sis;
	struct SSAPindication *si = &sis;
	struct type_OACS_PConnect	*pcon = NULL;

	missingP (ss);
	missingP (rti);
	missingP (arg1);
	missingP (arg2);

	/* acsap_conntype = CN_OPEN, acsap_data = NULLPE; */
	if ((pe = ssdu2pe (ss -> ss_data, ss -> ss_cc, NULLCP, &result)) == NULLPE
			|| parse_OACS_PConnect (pe, 1, NULL, NULLVP, &pcon) == NOTOK) {
		if (pe)
			pe_free (pe);
		if (result == PS_ERR_NMEM) {
congest:
			;
			result = SC_CONGESTION, result2 = RTS_CONGEST;
		} else
			result = SC_REJECTED, result2 = RTS_PROTOCOL;
		goto out;
	}

	PLOGP (rtsap_log,OACS_PConnect, pe, "PConnect", 1);

	pe_free (pe);

	if (pcon -> pUserData -> member_OACS_2 -> offset
			!= type_OACS_ConnectionData_open) {
		result = SC_REJECTED, result2 = RTS_ADDRESS;
		goto out;
	}
	if (is = getisoserventbyport ("rtsap",
								  (uint16_t) htons ((uint16_t) pcon -> pUserData -> applicationProtocol))) {
		*is -> is_tail++ = arg1;
		*is -> is_tail++ = arg2;
		*is -> is_tail = NULL;
	} else {
		result = SC_REJECTED, result2 = RTS_ADDRESS;
		goto out;
	}

	switch (hook ? (*hook) (is, rti) : OK) {
	case NOTOK:
		if (pcon)
			free_OACS_PConnect(pcon);
		return NOTOK;

	case DONE:
		if (pcon)
			free_OACS_PConnect(pcon);
		return OK;

	case OK:
		if (setperms)
			(*setperms) (is);
		execv (*is -> is_vec, is -> is_vec);/* fall */
		SLOG (rtsap_log, LLOG_FATAL, *is -> is_vec, ("unable to exec"));
	default:
		goto congest;
	}

out:
	;
	if (pcon)
		free_OACS_PConnect(pcon);
	SSFREE (ss);

	bzero ((char *) &ref, sizeof ref);
	SConnResponse (ss -> ss_sd, &ref, NULLSA, result, 0, 0,
				   SERIAL_NONE, NULLCP, 0, si);
	return rtsaplose (rti, result2, NULLCP, NULLCP);
}
