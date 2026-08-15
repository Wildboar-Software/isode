/* tusaplose.c - TPM: you lose */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "tpkt.h"

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

int tusaplose (struct TSAPdisconnect *td, int reason, char *what, char *fmt) {
	return tusaplose (td, reason, what, fmt);
}
#endif

#ifndef	lint
static int _tusaplose (	/* what, fmt, args ... */
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
