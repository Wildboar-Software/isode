/* tusaplose.c - TPM: you lose */

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
#include <starg.h>
#include "tpkt.h"

/*  */

#ifndef	lint
static int _tusaplose ();

int	tusaplose (struct TSAPdisconnect *td, ...) {
	int	    reason,
		    result;
	va_list ap;

	va_start (ap, td);

	reason = va_arg (ap, int);

	result = _tusaplose (td, reason, ap);

	va_end (ap);

	return result;

}
#else
/* VARARGS */

int
tusaplose (struct TSAPdisconnect *td, int reason, char *what, char *fmt) {
	return tusaplose (td, reason, what, fmt);
}
#endif

/*  */

#ifndef	lint
static int
_tusaplose (	/* what, fmt, args ... */
	struct TSAPdisconnect *td,
	int reason,
	va_list ap
) {
	char  *bp;
	char    buffer[BUFSIZ];
	char    *what, *fmt;

	if (td) {
		bzero ((char *) td, sizeof *td);

		what = va_arg (ap, char *);
		fmt = va_arg (ap, char *);
		_asprintf (bp = buffer, what, fmt, ap);
		bp += strlen (bp);

		td -> td_reason = reason;
		copyTSAPdata (buffer, bp - buffer, td);
	}

	return NOTOK;
}
#endif
