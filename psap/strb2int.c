/* converts a bit string - output of bitstr2strb() - to an integer */

#include <stdio.h>
#include "psap.h"

int strb2int (char *cp, int len) {
	int    i,
		   j,
		   bit,
		   mask,
		   n;

	n = 0;
	for (bit = (*cp & 0xff), i = 0, mask = 1 << (j = 7); i < len; i++) {
		if (bit & mask)
			n |= 1 << i;
		if (j-- == 0)
			bit = *++cp & 0xff, mask = 1 << (j = 7);
		else
			mask >>= 1;
	}

	return n;
}
