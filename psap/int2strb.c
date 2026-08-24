/* int2strb.c - integer to string of bits */

#include <stdio.h>
#include <strings.h>
#include "psap.h"

char *int2strb (int n, int len) {
	int    i;
	static char buffer[sizeof (int) + 1];

	bzero (buffer, sizeof (buffer));
	for (i = 0; i < len; i++)
		if (n & (1 << i)) {
			if (char_bis (&buffer[i / 8],
					  (unsigned) (1 << (7 - (i % 8)))) != 0)
				return NULLCP;
		}

	return buffer;
}
