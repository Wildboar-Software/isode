/* time.c - */

/*
	SYNTAX:
		time ::= 'yymmddhhmmssz'

		where yy   = year
		      mm   = month
		      dd   = day
		      hh   = hours
		      mm   = minutes
		      ss   = seconds
	              z    = timezone

	EXAMPLE:
		890602093221Z -> 09:32:21 GMT, on June 2nd 1989.
*/

#include <string.h>
#include "quipu/util.h"
#include "quipu/attr.h"
#include "psap.h"

extern int strprint (PS ps, char *str, int format);
extern int sfree (char *x);
extern int pstrcmp(char *a, char *b);
extern char *strdup(const char *);

static UTC	qstr2utct (char *s, int len)
{
	UTC	    ut;
	if (len > 14
			&& strncmp (s, "1989", 4) == 0
			&& (ut = str2utct (s + 2, len - 2)))
		return ut;
	return str2utct (s, len);
}

#define	str2utct	qstr2utct

static PE timeenc (void *value)
{
	char *x = (char *) value;
	PE ret_pe = NULLPE;
	/* Should switch to pepsy -> need to use qbufs! */
	build_UNIV_UTCTime (&ret_pe,0,0,x,NULL);
	return (ret_pe);
}

static void * timedec (PE pe)
{
	char * x;
	if (parse_UNIV_UTCTime (pe,0,0,&x,NULL) == NOTOK)
		return (NULLCP);
	return (x);
}

void utcprint (PS ps, char *xtime, int format)
{
	UTC	    ut;

	if (format == READOUT && (ut = str2utct (xtime, strlen (xtime)))) {
		long    mtime;
		mtime = gtime (ut2tm (ut));
		ps_printf (ps, "%-24.24s", ctime (&mtime));
	} else
		ps_printf (ps, "%s", xtime);
}

static int utccmp (void *value1, void *value2) {
	char *a = (char *) value1;
	char *b = (char *) value2;
	long a_time, mdiff;
	UTC ut;

	if ((ut = str2utct (a, strlen (a))) == NULL)
		return pstrcmp (a, b);
	a_time = gtime (ut2tm (ut));
	if ((ut = str2utct (b, strlen (b))) == NULL)
		return pstrcmp (a, b);
	return ((mdiff = a_time - gtime (ut2tm (ut))) == 0L ? 0
			: mdiff > 0L ? 1 : -1);
}

void time_syntax (void) {
	add_attribute_syntax ("UTCTime",
						  timeenc,		timedec,
						  (AttributeValueParser)strdup,		(AttributeValuePrinter)utcprint,
						  (AttributeValueCopier)strdup,		utccmp,
						  free,		NULLCP,
						  NULL,		FALSE);
}
