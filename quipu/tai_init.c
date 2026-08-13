/* tai_init.c - */

#include "quipu/util.h"
#include "tailor.h"

extern  char    *dsatailfile;

#define MAXTAIARGS      100

extern  LLog    *log_dsap;

int dsa_tai_init (char *name) {
	FILE    *fp;
	char    buf[BUFSIZ];
	char   *cp;

	isodetailor (name,0);
	if( (fp = fopen(cp = isodefile(dsatailfile, 0), "r")) == (FILE *)NULL) {
		LLOG (log_dsap,LLOG_FATAL, ("Cannot open tailor file '%s'", cp));
		fatal (-46, "Cannot open quiputailor");
	}
	while(fgets(buf, sizeof(buf), fp) != NULLCP)
		if ( (*buf != '#') && (*buf != '\n') )
			/* not a comment or blank */
			if (dsa_tai_string (buf) == NOTOK)
				LLOG (log_dsap,LLOG_EXCEPTIONS,("tai_string failed %s",buf));
	fclose(fp);
	isodexport(NULLCP);
	return OK;
}

int dsa_tai_string (char *str) {
	char    *args[MAXTAIARGS];
	char    *p;
	int     ac;

	if( (p = index(str, '\n')) != NULLCP)
		*p = '\0';
	if((ac = sstr2arg(str, MAXTAIARGS, args, " \t,")) == NOTOK) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("too many tailor parameters"));
		return(NOTOK);
	}
	if(ac <= 1) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("no option set",str));
		return (NOTOK);
	}
	return (dsa_sys_tai(ac, args));
}
