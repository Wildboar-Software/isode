/* isobject.c - lookup Object IDentifiers/DEscriptors */

#include <ctype.h>
#include <stdio.h>
#include <strings.h>
#include "psap.h"
#include "tailor.h"

static char *isobjects = "isobjects";

static FILE *servf = NULL;
static int  stayopen = 0;

static struct isobject    ios;

int setisobject (int f) {
	if (servf == NULL)
		servf = fopen (isodefile (isobjects, 0), "r");
	else
		rewind (servf);
	stayopen |= f;

	return (servf != NULL);
}

int endisobject (void) {
	if (servf && !stayopen) {
		fclose (servf);
		servf = NULL;
	}

	return 1;
}

struct isobject *
getisobject()  {
	int i;
	struct isobject *io = &ios;
	char  *cp;
	static char buffer[BUFSIZ + 1];
	static char *vec[NVEC + NSLACK + 1];
	static unsigned int elements[NELEM + 1];

	if (servf == NULL
			&& (servf = fopen (isodefile (isobjects, 0), "r")) == NULL)
		return NULL;

	while (fgets (buffer, sizeof buffer, servf) != NULL) {
		if (*buffer == '#')
			continue;
		if (cp = index (buffer, '\n'))
			*cp = 0;
		if (str2vec (buffer, vec) < 2)
			continue;

		if ((i = str2elem (vec[1], elements)) <= 1)
			continue;

		io -> io_descriptor = vec[0];
		io -> io_identity.oid_elements = elements;
		io -> io_identity.oid_nelem = i;

		return io;
	}
	return NULL;
}
