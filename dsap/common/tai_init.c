/* tai_init.c - */

#include "quipu/util.h"
#include "tailor.h"

extern LLog * log_dsap;
extern  char    *tailfile;

#define MAXTAIARGS      100

extern  LLog    *log_dsap;

int dsap_tai_init (void) {
	FILE    *fp;
	char    *cp;
	char    buf[BUFSIZ];

	if(!isstr(tailfile))    /* it's compiled in */
		return(OK);
	if( (fp = fopen(cp = isodefile(tailfile, 0), "r")) == (FILE *)NULL) {
		LLOG (log_dsap,LLOG_FATAL,("can't open tailor file '%s'", cp));
		fatal (-1, "Cannot open tailor file");
	}
	while(fgets(buf, sizeof(buf), fp) != NULLCP)
		if ( (*buf != '#') && (*buf != '\n') )
			/* not a comment or blank */
			if (tai_string (buf) == NOTOK)
				LLOG (log_dsap,LLOG_EXCEPTIONS,("tai_string failed %s",buf));
	fclose(fp);
	return OK;
}

int tai_string (const char *str) {
	char    *args[MAXTAIARGS];
	char    *p;
	int     ac;

	if( (p = index(str, '\n')) != NULLCP)
		*p = '\0';
	if((ac = sstr2arg(str, MAXTAIARGS, args, " \t,")) == NOTOK) {
		LLOG(log_dsap,LLOG_EXCEPTIONS,("too many tailor parameters"));
		return(NOTOK);
	}
	if(ac <= 1) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("no option set '%s'",str));
		return (NOTOK);
	}
	return (dsap_tai(ac, args));
}
