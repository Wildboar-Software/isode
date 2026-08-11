/* char2bcd.c - convert to BCD (for X.25 and others) */

#include "general.h"

int bcd2char (u_char *s, char *d, int len) {
	int i, g;

	for (i = 0; i < len; i++) {
		g = s[i >> 1];
		if ((i & 1) == 0)
			g >>= 4;
		g &= 0xf;

		if (g < 0x0a)
			*d++ = g + '0';
		else
			*d++ = g + 'a' - 0x0a;
	}
	*d = 0;
	return len;
}
