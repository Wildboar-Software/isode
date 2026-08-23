/* char2bcd.c - convert to BCD (for X.25 and others) */

#include <stdint.h>
#include "general.h"

// FIXME: This has no idea how big the output buffer d is. Make this more secure.
int bcd2char (uint8_t *s, char *d, int len) {
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
