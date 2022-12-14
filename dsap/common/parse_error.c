/* parse_error.c - */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/parse_error.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/parse_error.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: parse_error.c,v $
 * Revision 9.0  1992/06/16  12:12:39  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


/* LINTLIBRARY */

#include "quipu/util.h"
#include "psap.h"

int print_parse_errors = TRUE;
int parse_status = 0;
#ifdef TURBO_DISK
char *parse_entry = NULL;
#endif
PS _opt = NULLPS;
int parse_line = 1;
extern LLog * log_dsap;

parse_error (a,b)
char *a, *b;
{
	char buffer [LINESIZE];

	parse_status++;

	if (print_parse_errors) {
		if (_opt == NULLPS) {
			_opt = ps_alloc (std_open);
			if (std_setup (_opt,stderr) != OK) {
				LLOG (log_dsap,LLOG_EXCEPTIONS,("cant open error (parse)..."));
				LLOG (log_dsap,LLOG_EXCEPTIONS,(a,b));
				return;
			}
		}
#ifdef TURBO_DISK
		if ( parse_entry != NULL )
			ps_printf(_opt, "key (%s): ", parse_entry);
#else
		if (parse_line != 0)
			ps_printf (_opt,"line %d: ",parse_line);
#endif
		ps_printf (_opt,a,b);
		ps_printf (_opt,"\n");
	} else {
#ifdef TURBO_DISK
		if ( parse_entry != NULL ) {
			sprintf (buffer,"key (%s): ", parse_entry);
			strcat (buffer,a);
			LLOG (log_dsap,LLOG_EXCEPTIONS,(buffer,b));
#else
		if (parse_line != 0) {
			sprintf (buffer,"line %d: ",parse_line);
			strcat (buffer,a);
			LLOG (log_dsap,LLOG_EXCEPTIONS,(buffer,b));
#endif
		} else
			LLOG (log_dsap,LLOG_EXCEPTIONS,(a,b));
	}
}
