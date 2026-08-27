/* sstr2arg: convert string into argument list */

#include <stdio.h>
#include <strings.h>
#include "manifest.h"
#include "general.h"
#include <errno.h>

/*
   stash a pointer to each field into the passed array. any common seperators
   split the words.  extra white-space between fields is ignored.

   specially-interpreted characters:
	double-quote, backslash (preceding a special char with a backslash
	removes its interpretation.  A backslash not followed by a special is
	used to preface an octal specification for one character a string begun
	with double-quote has only double-quote and backslash as special
	characters.

*/

int sstr2arg (
	const char *srcptr,  /* source data */
	const int maxpf,              /* maximum number of permitted fields */
	char *argv[],           /* where to put the pointers */
	const char *dlmstr           /* Delimiting character */
) {
	char gotquote;		/* currently parsing quoted string */
	int ind;
	char *destptr;
	char idex[256];

	if (srcptr == 0)
		return (NOTOK);

	bzero (idex, sizeof idex);
	for (destptr = dlmstr; *destptr; destptr++)
		idex[*destptr] = 1;

	for (ind = 0, maxpf -= 2;; ind++) {
		if (ind >= maxpf)
			return (NOTOK);

		/* Skip leading white space */
		for (; *srcptr == ' ' || *srcptr == '\t'; srcptr++);

		argv [ind] = srcptr;
		destptr = srcptr;

		for (gotquote = 0; ; ) {

			if (idex[*srcptr]) {
				if (gotquote) {	/* don't interpret the char */
					*destptr++ = *srcptr++;
					continue;
				}

				srcptr++;
				*destptr = '\0';
				goto nextarg;
			} else {
				switch (*srcptr) {
				default:	/* just copy it                     */
					*destptr++ = *srcptr++;
					break;

				case '\"':	/* beginning or end of string       */
					gotquote = (gotquote) ? 0 : 1 ;
					srcptr++;	/* just toggle */
					break;

				case '\\':	/* quote next character     */
					srcptr++;	/* skip the back-slash      */
					switch (*srcptr) {
					/* Octal character	    */
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
						*destptr = '\0';
						do
							*destptr = (*destptr << 3) | (*srcptr++ - '0');
						while (*srcptr >= '0' && *srcptr <= '7');
						destptr++;
						break;
					/* C escape char	    */
					case 'b':
						*destptr++ = '\b';
						srcptr++;
						break;
					case 'n':
						*destptr++ = '\n';
						srcptr++;
						break;
					case 'r':
						*destptr++ = '\r';
						srcptr++;
						break;
					case 't':
						*destptr++ = '\t';
						srcptr++;
						break;

					/* Boring -- just copy ASIS */
					default:
						*destptr++ = *srcptr++;
					}
					break;

				case '\0':
					*destptr = '\0';
					ind++;
					argv[ind] = (char *) 0;
					return (ind);
				}
			}
		}
nextarg:
		continue;
	}
}
