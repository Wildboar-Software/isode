/* char2bcd.c - convert to BCD (for X.25 and others) */

#include <stdint.h>
#include "general.h"
#include "manifest.h"

int char2bcd (char *s, int n, uint8_t *d) {
	int c, i;

	for (i = 0; *s && n-- > 0; i++) {
		if ((c = *s++) >= 'a' && c <= 'f')
			c -= 'a' - 0x0a;
		else if (c >= 'A' && c <= 'F')
			c -= 'A' - 0x0a;
		else if (c >= '0' && c <= '9')
			c -= '0';
		else
			c = 0;

		if (i & 1) {
			uint8_t lo;

			if (int2u8 (c & 0xf, &lo) != 0)
				return NOTOK;
			*d++ |= lo;
		} else {
			if (int2u8 ((c & 0xf) << 4, d) != 0)
				return NOTOK;
		}
	}
	return i;
}
