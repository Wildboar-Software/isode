/* ronotabort.c - RONOT: bail-out routine which logs abort to rosap log */

#include "tailor.h"
#include "logger.h"
#include "rosap.h"
#include "ronot.h"

/* RO-ABORT.REQUEST */

int RoBindUAbort (int sd, struct RoNOTindication *rni) {
	int			  result;
	struct AcSAPindication    aci_s;
	struct AcSAPindication  * aci = &(aci_s);
	struct AcSAPabort       * aca = &(aci->aci_abort);

	LLOG (rosap_log, LLOG_EXCEPTIONS, ("RO-ABORT-BIND.REQUEST called on %d", sd));

	result = AcUAbortRequest (sd, NULLPEP, 0, aci);

	if (result != OK) {
		LLOG (rosap_log, LLOG_EXCEPTIONS, ("RO-ABORT-BIND.REQUEST failed on %d", sd));
		return (acs2ronotlose (rni, "RO-ABORT-BIND.REQUEST", aca));
	}

	return (OK);
}

