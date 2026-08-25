/* implode.c - explode ascii into octets */

#include <stdint.h>
#include "general.h"
#include "manifest.h"

char	hex2nib[0x80] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0, 0, 0, 0, 0, 0,
	0, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

int implode (uint8_t *a, char *b, int n) {
	int    i;

	for (i = 0; i < n; i += 2) {
		int octet;

		octet = ((hex2nib[b[0] & 0x7f] << 4)
			 | (hex2nib[b[1] & 0x7f] & 0x0f));
		if (int2u8 (octet, a) != 0)
			return NOTOK;
		a++;
		b += 2;
	}

	return (n / 2);
}
