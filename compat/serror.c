/* serror.c - get system error */

#include <stdio.h>
#include <string.h>
#include "general.h"
#include "manifest.h"

char *sys_errname (int i)
{
	static char buffer[128];
	char* se;

#ifdef	LINUX
	if (strerror_r(i, buffer, 128) != 0)
		(void) sprintf (buffer, "Error %d", i);
#else
	se = strerror(i);
	strncpy(buffer, se, 128);
	buffer[127] = '\0';		/* to be sure... */
#endif

	return buffer;
}
