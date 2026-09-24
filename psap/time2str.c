/* time2str.c - time string to string */

#include <stdio.h>
#include <string.h>
#include "psap.h"

char *time2str (UTC u, int generalized) {
	int    hours,
		   mins,
		   zone;
	char  *bp;
	static char buffer[BUFSIZ];

	if (u == NULLUTC)
		return NULLCP;

	bp = buffer;

	if (generalized)
		sprintf (bp, "%04d", u -> ut_year);
	else
		sprintf (bp, "%02d", u -> ut_year % 100);
	bp += strlen (bp);

	sprintf (bp, "%02d%02d%02d%02d", u -> ut_mon, u -> ut_mday,
			 u -> ut_hour, u -> ut_min);
	bp += strlen (bp);

	if (u -> ut_flags & UT_SEC
			|| (generalized && (u -> ut_flags & UT_USEC))) {
		sprintf (bp, "%02d", u -> ut_sec);
		bp += strlen (bp);
	}
	if (generalized && (u -> ut_flags & UT_USEC)) {
		sprintf (bp, ".%06d", u -> ut_usec);
		bp += strlen (bp);
	}

	if (u -> ut_flags & UT_ZONE)
		if ((zone = u -> ut_zone) == 0)
			*bp++ = 'Z';
		else {
			if (zone < 0)
				mins = (-zone) % 60, hours = (-zone) / 60;
			else
				mins = zone % 60, hours = zone / 60;
			sprintf (bp, "%c%02d%02d", zone < 0 ? '-' : '+',
					 hours, mins);
			bp += strlen (bp);
		}

	*bp = 0;

	return buffer;
}
