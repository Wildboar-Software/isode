/* sprintb.c - sprintf on bits */

#include <stdio.h>
#include <string.h>
#include "general.h"

char *sprintb (const int v, const char *bits) {
	int    i,
		   j;
	char   c,
		   *bp;
	static char buffer[BUFSIZ];

	sprintf (buffer, bits && *bits == 010 ? "0%o" : "0x%x", v);
	bp = buffer + strlen (buffer);

	if (bits && *++bits) {
		j = 0;
		*bp++ = '<';
		while (i = *bits++)
			if (v & (1 << (i - 1))) {
				if (j++)
					*bp++ = ',';
				for (; (c = *bits) > 32; bits++)
					*bp++ = c;
			} else
				for (; *bits > 32; bits++)
					continue;
		*bp++ = '>';
		*bp = 0;
	}

	return buffer;
}
