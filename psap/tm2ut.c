/* tm2ut.c - tm to time string */

#include <stdio.h>
#include <strings.h>
#include <time.h>
#include "psap.h"
#ifdef	OSX
#include <sys/time.h>
#endif

void tm2ut (struct tm *tm, UTC ut) {
	bzero ((char *) ut, sizeof *ut);

	ut -> ut_year = tm -> tm_year + 1900;	/* ut_year is the full year */
	ut -> ut_mon = tm -> tm_mon + 1;
	ut -> ut_mday = tm -> tm_mday;
	ut -> ut_hour = tm -> tm_hour;
	ut -> ut_min = tm -> tm_min;
	ut -> ut_sec = tm -> tm_sec;
	ut -> ut_zone = 0;

	ut -> ut_flags = UT_ZONE | UT_SEC;
}
