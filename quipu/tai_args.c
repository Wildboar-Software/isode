/* tai_args.c - Argument processing routines */

#include "quipu/util.h"
#include "quipu/attr.h"
#include "tailor.h"
#ifdef	TCP
#include "internet.h"

extern	int	_listen_opts;
#endif

extern char *dsaoidtable,
	   *dsatailfile,
	   *mydsaname,
	   *treedir;

static char        *usage = "[-t <tailor>] [-c <dsa name>] [-T <oidtable>] [-D <directory>]";

extern LLog * log_dsap;

int dsa_tai_args (const int *acptr, char ***avptr) {
	char ** av;
	char *cp;
	int cnt;
	extern char quipu_faststart;

	if (acptr == (int *)NULL)
		return 0;
	av = *avptr;
	av++, cnt = 1;
	while ((cp = *av) && *cp == '-') {
		switch (*++cp) {
		case 'T':
			dsaoidtable = *++av;
			cnt++;
			break;
		case 'D':
			treedir = *++av;
			cnt++;
			break;
		case 'c':
			mydsaname = *++av;
			cnt++;
			break;
		case 't':
			dsatailfile = *++av;
			cnt++;
			break;

		case 'f':
			quipu_faststart = 1;
			break;

		case 's':
			quipu_faststart = 0;
			break;

		case 'r':
#ifdef	SO_REUSEADDR
			_listen_opts = SO_REUSEADDR;
#endif
			break;

		default:
			LLOG (log_dsap,LLOG_FATAL,("Unknown option\nUsage: %s %s\n",*avptr[0],usage));
			fatal(-46,"Usage...");
		}
		av++;
		cnt++;
	}
	*acptr -= cnt;
	*avptr = av;
	return 0;
}
