/* ps_prime.c - prime a presentation stream */

#include <stdio.h>
#include "psap.h"

int ps_prime (PS ps, const int waiting) {
	if (ps -> ps_primeP)
		return (*ps -> ps_primeP) (ps, waiting);

	return OK;
}
