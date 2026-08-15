/* asprintf.c - sprintf with errno */

#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include "general.h"
#include "manifest.h"

#ifndef ASPRINTF

void	asprintf (char *bp, va_list ap) {	/* what, fmt, args, ... */
    char   *what, *fmt;

    what = va_arg (ap, char *);
    fmt  = va_arg (ap, char *);

    _asprintf (bp, what, fmt, ap);
}
#endif

#ifdef X25
unsigned char isode_x25_err[2];
char isode_x25_errflag = 0;
#endif

void	_asprintf (char *bp, char *what, char *fmt, va_list ap) {	/* fmt, args, ... */
	int    eindex;
	eindex = errno;

	*bp = 0;

	if (fmt) {
#ifndef	VSPRINTF
		struct _iobuf iob;
#endif

#ifndef	VSPRINTF
#ifdef	pyr
		bzero ((char *) &iob, sizeof iob);
		iob._file = _NFILE;
#endif
		iob._flag = _IOWRT | _IOSTRG;
#if	!defined(vax) && !defined(pyr)
		iob._ptr = (unsigned char *) bp;
#else
		iob._ptr = bp;
#endif
		iob._cnt = BUFSIZ;
		_doprnt (fmt, ap, &iob);
		putc (NULL, &iob);
#else
		vsprintf (bp, fmt, ap);
#endif
		bp += strlen (bp);

	}

	if (what) {
		if (*what) {
			sprintf (bp, " %s: ", what);
			bp += strlen (bp);
		}
		strcpy (bp, sys_errname (eindex));
		bp += strlen (bp);

#ifdef X25
		if (isode_x25_errflag) {
			sprintf (bp, " (%02x %02x)",isode_x25_err[0],isode_x25_err[1]);
			bp += strlen (bp);
		}
#endif

	}

	errno = eindex;
}
