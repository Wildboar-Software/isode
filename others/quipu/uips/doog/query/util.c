/* 

/*
 * 
 *
 *
 *
 */

#include <string.h>
#include "types.h"
#include "util.h"
#include "quipu/util.h"

char *copy_string (char *string) {
	char *new_string;

	if (string == NULLCP) return NULLCP;
	new_string = (char *) smalloc(strlen(string) + 1);
	strcpy(new_string, string);
	return new_string;
}

void friendlify (char *name, char *fname) {
	char *start, *end;
	char save;

	*fname = '\0';
	end = name;
	while (!isnull(*end)) end++;
	while (end > name && isspace(*(end - 1))) end--;
	while (1) {
		start = end;
		while (*start != '=') start--;
		start++;
		save = *end;
		*end = '\0';
		while (isspace(*start)) start++;
		if (!isnull(*fname))
			strcat(fname, ", ");
		strcat(fname, start);
		*end = save;
		end = start;
		while (end > name && *end != '@' && *end != '\n')
			end--;
		if (end == name)
			break;
		end--;
		while(isspace(*end)) end--;
		end++;
	}
}
