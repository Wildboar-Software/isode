/* baduser.c - check file of bad users */

#include <stdio.h>
#include <string.h>
#include "general.h"
#include "manifest.h"
#include "tailor.h"

int baduser (char *file, char *user) {
	int     hit,
			tries;
	char  *bp;
	char    buffer[BUFSIZ];
	FILE   *fp;

	hit = 0;
	for (tries = 0; tries < 2 && !hit; tries++) {
		switch (tries) {
		case 0:
			if (file) {
				bp = isodefile (file, 0);
				break;
			}
			tries++;
		/* and fall */
		default:
			bp = "/etc/ftpusers";
			break;
		}
		if ((fp = fopen (bp, "r")) == NULL)
			continue;

		while (fgets (buffer, sizeof buffer, fp)) {
			if (bp = index (buffer, '\n'))
				*bp = 0;
			if (strcmp (buffer, user) == 0) {
				hit++;
				break;
			}
		}

		fclose (fp);
	}

	return hit;
}
