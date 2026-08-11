/* tai_args.c - Argument processing routines */

#include "quipu/util.h"
#include "quipu/attr.h"

extern char *oidtable,
	   *tailfile,
	   *myname;

extern LLog *log_dsap;

char        *dsap_usage = "[flags]";
char        *options = "T:t:c:";

void tai_args (int *acptr, char ***avptr) {
	char ** av;

	int cnt;
	char *cp;

	if (acptr == (int *)NULL)
		return;

	if (*acptr <= 1)
		return;

	av = *avptr;
	av++, cnt = 1;

	while ((cp = *av) && *cp == '-') {
		switch (*++cp) {
		case 'T':
			oidtable = *++av;
			cnt++;
			break;
		case 'c':
			myname = *++av;
			cnt++;
			break;
		case 't':
			tailfile = *++av;
			cnt++;
			break;

		default:
			LLOG (log_dsap,LLOG_FATAL,("Usage: %s %s\n",*avptr[0],dsap_usage));
			fatal(-1,"Usage...");
		}
		av++;
		cnt++;
	}

	*acptr -= cnt;
	*avptr = av;
}
