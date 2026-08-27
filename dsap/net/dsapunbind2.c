/* dsapunbind2.c - DSAP: maps D-UNBIND mapping onto RO-UNBIND */

#include "quipu/dsap.h"
#include "quipu/watchdog.h"

/* D-UNBIND.ACCEPT */

int DUnBindAccept (const int sd, struct DSAPindication *di) {
	int			  result;
	struct RoNOTindication	  rni_s;
	struct RoNOTindication	* rni = &(rni_s);

	watch_dog ("RoUnBindResult");
	result = RoUnBindResult (sd, NULLPE, rni);
	watch_dog_reset();
	if (result == NOTOK) {
		ronot2dsaplose (di, "D-UNBIND.ACCEPT", rni);
		return (NOTOK);
	}
	return (result);
}

/* D-UNBIND.REJECT */

int DUnBindReject (const int sd, const int status, const int reason, struct DSAPindication *di) {
	int			  result;
	struct RoNOTindication	  rni_s;
	struct RoNOTindication	* rni = &(rni_s);

	watch_dog ("DUnBindReject");
	result = RoUnBindReject (sd, status, reason, rni);
	watch_dog_reset ();
	if (result == NOTOK) {
		ronot2dsaplose (di, "D-UNBIND.REJECT", rni);
		return (NOTOK);
	}
	return (result);
}
