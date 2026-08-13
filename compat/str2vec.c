/* str2vec.c - string to vector */

#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include "general.h"
#include "manifest.h"

#define	QUOTE	'\\'

int str2vecX (char *s, char **vec, int nmask, int *mask, char brk, int docomma) {
	int    i;
	char    comma = docomma ? ',' : ' ';

	if (mask)
		*mask = 0;

	for (i = 0; i <= NVEC;) {
		vec[i] = NULLCP;
		if (brk > 0) {
			if (i > 0 && *s == brk)
				*s++ = 0;
		} else
			while (isspace ((uint8_t) *s) || *s == comma)
				*s++ = 0;
		if (*s == 0)
			break;

		if (*s == '"') {
			if (i < nmask)
				*mask |= 1 << i;
			for (vec[i++] = ++s; *s != 0 && *s != '"'; s++)
				if (*s == QUOTE) {
					if (*++s == '"')
						strcpy (s - 1, s);
					s--;
				}
			if (*s == '"')
				*s++ = 0;
			continue;
		}
		if (*s == QUOTE && *++s != '"')
			s--;
		vec[i++] = s;

		if (brk > 0) {
			if (*s != brk)
				for (s++; *s != 0 && *s != brk; s++)
					continue;
		} else
			for (s++; *s != 0 && !isspace ((uint8_t) *s) && *s != comma; s++)
				continue;
	}
	vec[i] = NULLCP;

	return i;
}
