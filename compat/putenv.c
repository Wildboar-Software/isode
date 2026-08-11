/* putenv.c - generic putenv() */







#include <stdio.h>
#include <string.h>
#include "general.h"
#include "manifest.h"

extern char **environ;
static int nvmatch (const char *s1, const char *s2);

int setenv (const char *name, const char *value, int overwrite) {
	int    i;
	char **ep, **nep, *cp;

	if ((cp = malloc ((unsigned) (strlen (name) + strlen (value) + 2)))
			== NULL)
		return 1;
	sprintf (cp, "%s=%s", name, value);
	for (ep = environ, i = 0; *ep; ep++, i++)
		if (nvmatch (name, *ep)) {
			*ep = cp;
			return 0;
		}
	if ((nep = (char **) malloc ((unsigned) ((i + 2) * sizeof *nep)))
			== NULL) {
		free (cp);
		return 1;
	}
	for (ep = environ, i = 0; *ep; nep[i++] = *ep++)
		continue;
	nep[i++] = cp;
	nep[i] = NULLCP;
	environ = nep;
	return 0;
}

int unsetenv (const char *name) {
	char  **ep,
		  **nep;

	for (ep = environ; *ep; ep++)
		if (nvmatch (name, *ep))
			break;
	if (*ep == NULLCP)
		return 1;

	for (nep = ep + 1; *nep; nep++)
		continue;
	*ep = *--nep;
	*nep = NULL;
	return 0;
}

static int nvmatch (const char *s1, const char *s2) {
	while (*s1 == *s2++)
		if (*s1++ == '=')
			return 1;
	return (*s1 == '\0' && *--s2 == '=');
}
