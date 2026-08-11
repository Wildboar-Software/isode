/* str2elem.c - string to list of integers */

#include <ctype.h>
#include <stdlib.h>
#include "general.h"
#include "manifest.h"

int str2elem (char *s, unsigned int elements[]) {
	int    i;
	unsigned int  *ip;
	char  *cp,
		  *dp;

	if (s == NULLCP || *s == 0)
		return NOTOK;

	ip = elements, i = 0;
	for (cp = s; *cp && i <= NELEM; cp = ++dp) {
		for (dp = cp; isdigit ((u_char) *dp); dp++)
			continue;
		if ((cp == dp) || (*dp && *dp != '.'))
			break;
		*ip++ = (unsigned int) atoi (cp), i++;
		if (*dp == 0)
			break;
	}
	if (*dp || i >= NELEM)
		return NOTOK;
	*ip = 0;

	return i;
}
