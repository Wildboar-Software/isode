/* explode.c - explode octets into ascii */

#include <stdint.h>
#include "general.h"
#include "manifest.h"

static char nib2hex[0x10] = {
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

int explode (char *a, uint8_t *b, int n) {
	int    i;
	uint8_t c;

	for (i = 0; i < n; i++) {
		c = *b++;
		*a++ = nib2hex[(uint8_t)(c & 0xf0) >> 4];
		*a++ = nib2hex[(c & 0x0f)];
	}
	*a = 0;

	return (n * 2);
}
