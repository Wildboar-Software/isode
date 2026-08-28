/* char2bcd.c - convert to BCD (for X.25 and others) */

#include <stdint.h>
#include "general.h"
#include "manifest.h"

// FIXME: This has no idea how big the output buffer d is. Make this more secure.
// len is the length of digits; there is no break. The loop always iterates len times.
// That kind of makes sense, because there is no way to tell how long s is. You can't use a null terminator.
int bcd2char (const uint8_t *s, char *d, int len) {
	int i, g;

	for (i = 0; i < len; i++) {
		g = s[i >> 1];
		if ((i & 1) == 0)
			g >>= 4;
		g &= 0xf;

		if (g < 0x0a) {
			if (int2char (g + '0', d) != 0)
				return NOTOK;
		} else {
			if (int2char (g + 'a' - 0x0a, d) != 0)
				return NOTOK;
		}
		d++;
	}
	*d = 0;
	return len;
}
