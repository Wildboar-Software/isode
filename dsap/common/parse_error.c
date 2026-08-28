/* parse_error.c - */

#include <string.h>
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

void parse_error (char *a, const char *b)
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
