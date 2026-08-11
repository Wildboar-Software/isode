/* explode.c - explode octets into ascii */

#include <stdio.h>
#include "general.h"
#include "manifest.h"

static char nib2hex[0x10] = {
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

int explode (char *a, u_char *b, int n) {
	int    i;
	u_char c;

	for (i = 0; i < n; i++) {
		c = *b++;
		*a++ = nib2hex[(u_char)(c & 0xf0) >> 4];
		*a++ = nib2hex[(c & 0x0f)];
	}
	*a = 0;

	return (n * 2);
}
