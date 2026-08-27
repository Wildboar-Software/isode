/* rygenid.c - ROSY: generate unique invoke ID */
#include "rosy.h"

/* generate unique invoke ID */

int	RyGenID (const int sd) {
	static int id = 0;
	return (++id);
}
