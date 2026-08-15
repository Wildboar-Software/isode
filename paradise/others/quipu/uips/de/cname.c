/* template.c - your comments here */

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "util.h"
#include "cnamelist.h"
#include "quipu/util.h"

extern struct cnamelist * cnamelp;
static struct cnamelist * taillp;

void addToCoList(char *str) {
	char * cp;

	cp = index(str, ' ');
	if (cp == NULLCP) {
		fprintf(stderr, "log an error message about parsing of country name mappings...\n");
		return;
	}
	*cp = '\0';
	if (strlen(str) != 2) {
		fprintf(stderr, "log an error message about short country name not 2 chars\n");
		return;
	}
	cp++;
	addCoNode(str, cp);
}

void addCoNode(char *shortname, char *longname) {
	struct cnamelist * colp;

	colp = cname_alloc();
	if (cnamelp != NULLCNLIST) {
		taillp->next = colp;
		taillp = colp;
	} else
		cnamelp = taillp = colp;
	colp->shortname = copy_string(shortname);
	colp->longname = copy_string(longname);
	colp->next = NULLCNLIST;
}

char *mapCoName(char *shortname) {
	struct cnamelist * cnlp;

	for (cnlp = cnamelp; cnlp != NULLCNLIST; cnlp = cnlp->next)
		if (strcmp(shortname, cnlp->shortname) == 0)
			return cnlp->longname;
	return shortname;
}
