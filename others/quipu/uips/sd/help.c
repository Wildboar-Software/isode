/* help.c - Display of assorted help texts */

/*
 * 
 */

#include "sequence.h"
#include <stdio.h>
#include <string.h>
#include "quipu/util.h"
#include "tailor.h"
#include "wdgtdefs.h"

void help_cncs (void);
void help_init (void);
static void get_help (char *filename, char *line);


#define BUFLEN 1024
#define TEXT 1

#ifdef lint
#define ETCDIR "/etc"
#endif

#define  HELPDIR "sd/helpdir/"

extern str_seq textseq;
extern str_seq curr_dnseq;
extern int text_state;
extern int current_entry, entry_number, display_entry;

void tprint(char *fmt, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i, char *j), scrollbar(int command);

void get_help(char *filename, char *line);

void help_cncs (void) {
	get_help("help", "   Press <KEY> to get detailed help.\n");
}

void help_init (void) {
	get_help("help", "   SD X.500 Directory Agent - Concise Help\n");
}

void help_up (void) {
	get_help("widen", "   The \"Widen Area\" Function.\n");
}

void help_back (void) {
	get_help("look", "   The \"Look Back\" Function.\n");
}

void help_number (void) {
	get_help("number", "   The \"Go To Number\" Function.\n");
}

void help_srch (void) {
	get_help("search", "   The \"Search\" Function");
}

void help_list (void) {
	get_help("list", "   The \"List\" Function");
}

void get_help (char *filename, char *line) {
	FILE * helpfp;
	char helpbuf[BUFLEN];
	char filebuf[BUFLEN];
	char *str;

	text_state = TEXT;
	entry_number = 0;
	display_entry = current_entry = 1;
	free_seq(curr_dnseq);
	free_seq(textseq);
	curr_dnseq = 0;
	textseq = 0;
	strcpy(filebuf, HELPDIR);
	strcat(filebuf, filename);
	strcpy(helpbuf, isodefile(filebuf, 0));
	if ((helpfp = fopen(helpbuf, "r")) == (FILE *)NULL ) {
		tprint("Can't open help file '%s'.\n",helpbuf);
		return;
	}
	if (line) {
		add_seq(&textseq, line);
		entry_number++;
	}
	while(fgets(filebuf, BUFLEN, helpfp) != (char *) NULL) {
		str = filebuf;
		while(*str != '\n' && *str != '\0') str++;
		*str = '\0';
		add_seq(&textseq, filebuf);
		entry_number++;
	}
	fclose(helpfp);
	scrollbar('\0');
}
