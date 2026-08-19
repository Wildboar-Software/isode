/* strdup.c - create a duplicate copy of the given string */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "general.h"
#include "manifest.h"
#include "tailor.h"

#if !defined(STRDUP) && (!defined(SVR4) || defined(_AIX))
/* strdup in AIX does not seem to use QUIPUs malloc! */

char   *strdup (char *str)
{
	char *ptr;

	if ((ptr = malloc((unsigned) (strlen (str) + 1))) == NULL) {
		LLOG (compat_log,LLOG_FATAL, ("strdup malloc() failure"));
		abort ();
		/* NOTREACHED */
	}

	(void) strcpy (ptr, str);

	return ptr;
}

#else

int strdup_stub (void) {
	;
}

#endif
