/* isofiles.c - ISODE files */

#include <stdio.h>
#include <string.h>
#include "general.h"
#include "manifest.h"
#include "tailor.h"

char *_isodefile (char *path, char *file) {
	static char buffer[BUFSIZ];

	isodetailor (NULLCP, 0);	/* not really recursive */

	if (*file == '/'
			|| (*file == '.'
				&& (file[1] == '/'
					|| (file[1] == '.' && file[2] == '/'))))
		strcpy (buffer, file);
	else
		sprintf (buffer, "%s%s", path, file);

	return buffer;
}
