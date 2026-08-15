#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <gdbm.h>
#include "sys.file.h"
#include <ctype.h>
#include <string.h>
#include <unistd.h>

extern int gdbm_errno;

int main (int argc, char **argv) {
	GDBM_FILE	db;
	datum		prv, key, content;
	char		*p, gfname[1024];

	if ( argc != 2 ) {
		printf("usage: %s edbdbmfile\n", argv[0]);
		return(1);
	}

	strcpy(gfname, argv[1]);
	if ( (p = rindex(argv[1], '.')) == NULL
			|| strcmp(p, ".gdbm") != 0 )
		strcat(gfname, ".gdbm");

	if ( (db = gdbm_open(gfname, 0, GDBM_READER, 0, 0)) == NULL ) {
		fprintf( stderr, "Can't open (%s)\ndbm_error is (%d)\n",
				 gfname, gdbm_errno );
		return(1);
	}

	key.dptr = "HEADER";
	key.dsize = sizeof("HEADER");
	content = gdbm_fetch(db, key);
	if ( content.dptr == NULL )
		printf("No header!  Continuing...\n");
	else
		printf("%s\n", content.dptr);

	prv.dptr = NULL;
	for ( key = gdbm_firstkey(db); key.dptr; key = gdbm_nextkey(db, prv) ) {
		if ( prv.dptr != NULL )
			free( prv.dptr );
		if ( strcmp(key.dptr, "HEADER") == 0 ) {
			prv = key;
			continue;
		}

		content = gdbm_fetch(db, key);
		printf("%s", content.dptr);
		free(content.dptr);
		content.dptr = NULL;
		prv = key;
	}

	gdbm_close(db);
	return(0);
}
