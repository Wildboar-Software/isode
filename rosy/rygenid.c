/* rygenid.c - ROSY: generate unique invoke ID */







#include <stdio.h>
#include "rosy.h"

/*    generate unique invoke ID */

int	RyGenID (sd)
int	sd;
{
	static int	id = 0;

	return (++id);
}
