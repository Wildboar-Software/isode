/* dsapabort.c - DSAP: Lose the binding identified by the descriptor */

#include "logger.h"
#include "quipu/dsap.h"
#include "quipu/watchdog.h"

extern LLog	* log_dsap;

/* RO-ABORT.REQUEST */

int DUAbortRequest (int sd, struct DSAPindication *di) {
	int			  result;
	struct RoNOTindication    rni_s;
	struct RoNOTindication  * rni = &(rni_s);

	LLOG (log_dsap, LLOG_NOTICE,
		  ("RO-ABORT-BIND.REQUEST called on %d", sd));
	watch_dog ("RoBindUAbort");
	result = RoBindUAbort (sd, rni);
	watch_dog_reset();
	if (result != OK)
		return (ronot2dsaplose (di, "D-ABORT-BIND.REQUEST", rni));
	return (OK);
}
