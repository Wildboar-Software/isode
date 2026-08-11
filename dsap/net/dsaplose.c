/* dsaplose.c - DSAP: Support for directory protocol mappings */







#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "tailor.h"
#include "quipu/dsap.h"

#ifndef	lint
static int  _dsaplose ();
static int  _dsapreject ();
#endif

#ifndef	lint
int	dsaplose (struct DSAPindication *di, ...) {
	int	    reason,
			result;

	va_list ap;

	va_start (ap, di);

	reason = va_arg (ap, int);

	result = _dsaplose (di, reason, ap);

	va_end (ap);

	return result;
}
#else
/* VARARGS4 */

int dsaplose (struct DSAPindication *di, int reason, char *what, char *fmt) {
	return dsaplose (di, reason, what, fmt);
}
#endif

#ifndef	lint
static int _dsaplose (  struct DSAPindication *di, int reason, va_list ap) { /* what, fmt, args ... */
	char  *bp, *what, *fmt;
	char    buffer[BUFSIZ];
	struct DSAPabort	* da;

	if (di) {
		bzero ((char *) di, sizeof *di);
		di->di_type = DI_ABORT;
		da = &(di->di_abort);
		da->da_reason = reason;
		what = va_arg (ap, char *);
		fmt = va_arg (ap, char *);
		_asprintf (bp = buffer, what, fmt, ap);
		bp += strlen (bp);
		copyDSAPdata (buffer, bp - buffer, da);
	}

	return NOTOK;
}
#endif

#ifndef	lint
int	dsapreject (struct DSAPindication *di, ...) {
	int	    reason,
			id,
			result;

	va_list ap;

	va_start (ap, di);

	reason = va_arg (ap, int);
	id = va_arg (ap, int);

	result = _dsapreject (di, reason, id, ap);

	va_end (ap);

	return result;
}
#else
/* VARARGS4 */

int dsapreject (struct DSAPindication *di, int reason, int id, char *what, char *fmt) {
	return dsapreject (di, reason, id, what, fmt);
}
#endif

#ifndef	lint
static int
_dsapreject (  /* what, fmt, args ... */
	struct DSAPindication *di,
	int reason,
	int id,
	va_list ap
) {
	char  *bp, *what, *fmt;
	char    buffer[BUFSIZ];
	struct DSAPpreject	* dp;

	if (di) {
		bzero ((char *) di, sizeof *di);
		di->di_type = DI_PREJECT;
		dp = &(di->di_preject);
		dp->dp_id = id;
		dp->dp_reason = reason;
		what = va_arg (ap, char *);
		fmt = va_arg (ap, char *);
		_asprintf (bp = buffer, what, fmt, ap);
		bp += strlen (bp);
		copyDSAPdata (buffer, bp - buffer, dp);
	}

	return (NOTOK);
}
#endif
