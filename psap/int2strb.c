/* int2strb.c - integer to string of bits */







#include <stdio.h>
#include "psap.h"

char *int2strb (int n, int len) {
	int    i;
	static char buffer[sizeof (int) + 1];

	bzero (buffer, sizeof (buffer));
	for (i = 0; i < len; i++)
		if (n & (1 << i))
			buffer[i / 8] |= (1 << (7 - (i % 8)));

	return buffer;
}
