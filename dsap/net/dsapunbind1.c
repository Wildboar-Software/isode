/* dsapunbind1.c - DSAP: Maps D-UNBIND onto RO-UNBIND.REQUEST */

#include "quipu/dsap.h"
#include "quipu/watchdog.h"

/* D-UNBIND.REQUEST */

int DUnBindRequest (int sd, const int secs, struct DSAPrelease *dr, struct DSAPindication *di) {
	int			  result;
	struct AcSAPrelease	  acr_s;
	struct AcSAPrelease	* acr = &(acr_s);
	struct RoNOTindication	  rni_s;
	struct RoNOTindication	* rni = &(rni_s);

	watch_dog ("RoUnBindRequest");
	result = RoUnBindRequest (sd, NULLPE, secs, acr, rni);
	watch_dog_reset();
	if (result == OK) {
		dr->dr_affirmative = acr->acr_affirmative;
		dr->dr_reason = acr->acr_reason;
		ACRFREE(acr);
		return (OK);
	}
	if (result == NOTOK) {
		return (ronot2dsaplose (di, "D-UNBIND.REQUEST", rni));
	}
	return (result);
}

/* D-UNBIND.RETRY */

int DUnBindRetry (int sd, const int secs, struct DSAPrelease *dr, struct DSAPindication *di) {
	int			  result;
	struct AcSAPrelease	  acr_s;
	struct AcSAPrelease	* acr = &(acr_s);
	struct RoNOTindication	  rni_s;
	struct RoNOTindication	* rni = &(rni_s);

	watch_dog ("RoUnBindRetry");
	result = RoUnBindRetry (sd, secs, acr, rni);
	watch_dog_reset();
	if (result == OK) {
		dr->dr_affirmative = acr->acr_affirmative;
		dr->dr_reason = acr->acr_reason;
		ACRFREE (acr);
		return (OK);
	}
	if (result == NOTOK) {
		return (ronot2dsaplose (di, "D-UNBIND.RETRY", rni));
	}
	return (result);
}
