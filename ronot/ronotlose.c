/* ronotlose.c - RONOT: Support for ABSTRACT-BIND mappings */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/ronot/RCS/ronotlose.c,v 9.0 1992/06/16 12:36:36 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/ronot/RCS/ronotlose.c,v 9.0 1992/06/16 12:36:36 isode Rel $
 *
 *
 * $Log: ronotlose.c,v $
 * Revision 9.0  1992/06/16  12:36:36  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


/* LINTLIBRARY */

#include <stdio.h>
#include <stdarg.h>
#include "tailor.h"
#include "logger.h"
#include "ronot.h"

#ifndef	lint
static int  _ronotlose ();
#endif

/*  */

#ifndef	lint
int	ronotlose (struct RoNOTindication* rni, ...) {
	int	    reason, result;
	va_list ap;

	va_start (ap, rni);
	reason = va_arg (ap, int);
	SLOG (rosap_log, LLOG_EXCEPTIONS, NULLCP, ("ROSE operation failed with reason %d", reason));
	result = _ronotlose (rni, reason, ap);
	va_end (ap);
	return result;
}
#else
/* VARARGS4 */

int
ronotlose (struct RoNOTindication *rni, int reason, char *what, char *fmt) {
	return ronotlose (rni, reason, what, fmt);
}
#endif

/*  */

#ifndef	lint
static int
_ronotlose (  /* what, fmt, args ... */
	struct RoNOTindication *rni,
	int reason,
	va_list ap
) {
	char  *bp;
	char    buffer[BUFSIZ];

	if (rni) {
		bzero ((char *) rni, sizeof *rni);
		rni -> rni_reason = reason;
		bp = buffer;
		bp += strlen (bp);
		copyRoNOTdata (buffer, bp - buffer, rni);
	}

	return NOTOK;
}
#endif

/*    ACSAP interface */

int
acs2ronotlose (struct RoNOTindication *rni, char *event, struct AcSAPabort *aca) {

	char	* cp;

	/*
	* ADT: Not sure about having events logged from the BIND abstraction,
	* since it is a MACRO defined abstraction and not a level or ASE.
	* Currently not performed but would be something like the following:
	*/
	/*
		if (event)
			SLOG (addr_log, LLOG_EXCEPTIONS, NULLCP,
				((aca->aca_cc > 0) ? "%s: %s [%*.*s]" : "%s: %s",
				event, AcErrString (aca->aca_reason), aca->aca_cc,
				aca->aca_cc, aca->aca_data));
	*/

	/*
		if (event)
			 sprintf (cp = buffer, " {%s} (%s at ACSE)",
				event, AcErrString (aca->aca_reason));
		else
			 sprintf (cp = buffer, " (%s at ACSE)",
				AcErrString (aca->aca_reason));
	*/

	cp = "";

	if (aca->aca_cc > 0)
		return (ronotlose (rni, RBI_ACSE, NULLCP, "%*.*s%s",
						   aca->aca_cc, aca->aca_cc, aca->aca_data, cp));
	else
		return (ronotlose (rni, RBI_ACSE, NULLCP, "%s", cp));
}

