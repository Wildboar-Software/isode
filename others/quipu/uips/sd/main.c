/* main.c - widget */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/sd/RCS/main.c,v 9.0 1992/06/16 12:45:08 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/others/quipu/uips/sd/RCS/main.c,v 9.0 1992/06/16 12:45:08 isode Rel $
 */


#include <signal.h>
#include <stdio.h>
#define IP _IP
#include <curses.h>
#undef OK
#include <stdarg.h>

#include "widget.h"
#include "quipu/util.h"

extern  char    goto_path[], namestr[], passwd[];
extern  char   *myname;
extern  WINDOW *Text;
extern  WIDGET  mainwdgts[];
extern  WIDGET  cnnctwdgts[];
extern  int     print_parse_errors;
extern  char   *oidtable, *tailfile, *myname;
extern  char    testing;

void quit(), die(), setsignals(), int_quit(), sd_quit(), read_args();
void user_tailor(), main_bind(), cnnct_bind(), interact(), help_init();

void exit();

int
main (unsigned int argc, char *argv[]) {
	print_parse_errors = FALSE;
	quipu_syntaxes();

#ifdef USE_PP
	pp_quipu_init (argv[0]);
#endif

	namestr[0] = '\0';
	passwd[0] = '\0';

	read_args(argc, &argv);
	dsap_init((int *) 0, &argv);

#ifdef USE_PP
	pp_quipu_run ();
#endif

	initwidgets();
	setsignals();

	user_tailor();
	main_bind();
	cnnct_bind();
	help_init();
	interact();
	return(0);
}

void
read_args (unsigned int argc, char ***avptr) {
	char **av;
	char *cp;

	if (argc <= 1) return;

	av = *avptr;
	av++;

	while ((cp = *av) && (*cp == '-')) {
		switch (*++cp) {
		case 'u':
			if (*++av != NULLCP)  strcpy(namestr, *av);
			break;
		case 'p':
			if (*++av != NULLCP)  strcpy(passwd, *av);
			break;
		case 'T':
			if (*++av != NULLCP) oidtable = *av;
			break;
		case 'c':
			if (*++av != NULLCP) myname = *av;
			break;
		case 't':
			if (lexequ(*av, "-test") != 0) {
				if (*++av != NULLCP) tailfile = *av;
			} else {
				testing = TRUE;
			}
			break;
		}
		av++;
	}
}

void
setsignals () {
	int     i;

	for (i=0; i<18; i++)
		signal(i, SIG_DFL);
}

void
eprint (char *str) {
	tprint(str);
}

void
sd_quit () {
	quit("\n", 0);
}

void
quit (char *error, int sig) {
	endwidgets();
	ds_unbind();
	hide_picture();
	printf(error);
	exit(sig);
}

void
int_quit (int sig) {
	quit("\n", sig);
}


advise (int code, char *what, char *fmt, ...) {
	va_list ap;
	extern LLog * log_dsap;

	va_start (ap, fmt);

	_ll_log (log_dsap, code, what, fmt, ap);

	va_end (ap);
}

