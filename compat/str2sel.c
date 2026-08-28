/* str2sel.c - string to selector */

#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <strings.h>
#include "general.h"
#include "manifest.h"
#include "tailor.h"

#define	QUOTE	'\\'

/* STR2SEL */

int str2sel (char *s, const int quoted, char *sel, int n) {
	int     i,
			r;
	char  *cp;

	if (*s == 0)
		return 0;

	if (quoted <= 0) {
		for (cp = s; *cp; cp++)
			if (!isxdigit ((uint8_t) *cp))
				break;

		if (ptrdiff2int (cp - s, &i) != 0)
			return NOTOK;
		if (*cp == 0 && i >= 2 && (i & 0x01) == 0) {
			if (i > (r = n * 2))
				i = r;
			i = implode ((uint8_t *) sel, s, i);
			if (i == NOTOK)
				return NOTOK;
			if ((r = (n - i)) > 0 && bzero_int (sel + i, r) != 0)
				return NOTOK;
			return i;
		}
		if (*s == '#') {	/* gosip style, network byte-order */
			i = atoi (s + 1);
			if (int2octet ((i >> 8) & 0xff, &sel[0]) != 0
					|| int2octet (i & 0xff, &sel[1]) != 0)
				return NOTOK;

			return 2;
		}

		DLOG (compat_log, LLOG_EXCEPTIONS, ("invalid selector \"%s\"", s));
	}

	for (cp = sel; *s && n > 0; cp++, s++, n--)
		if (*s != QUOTE)
			*cp = *s;
		else
			switch (*++s) {
			case 'b':
				*cp = '\b';
				break;
			case 'f':
				*cp = '\f';
				break;
			case 'n':
				*cp = '\n';
				break;
			case 'r':
				*cp = '\r';
				break;
			case 't':
				*cp = '\t';
				break;

			case 0:
				s--;
			case QUOTE:
				*cp = QUOTE;
				break;

			default:
				if (!isdigit ((uint8_t) *s)) {
					*cp++ = QUOTE;
					*cp = *s;
					break;
				}
				r = *s != '0' ? 10 : 8;
				for (i = 0; isdigit ((uint8_t) *s); s++)
					i = i * r + *s - '0';
				s--;
				*cp = toascii (i);
				break;
			}
	if (n > 0)
		*cp = 0;

	if (ptrdiff2int (cp - sel, &i) != 0)
		return NOTOK;
	return i;
}
