/* photo_stub.c */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/photo_stub.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/photo_stub.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: photo_stub.c,v $
 * Revision 9.0  1992/06/16  12:12:39  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */
#include "unistd.h"
#include "stdio.h"
#include "quipu/photo.h"

/* Stub null photo routines - see others/quipu/photo for *real* examples */

int photo_start (char * name) {
	fprintf (stderr,"PHOTO: NYI (1)");
	return (-1);
}

int photo_end (char * name) {
	printf ("PHOTO: NYI (2)");
	fflush (stdout);
	close (1);
	return (-1);
}

void photo_black (int length) {
	;
}

void photo_white (int length) {
	;
}

void photo_line_end (bit_string * line) {
	;
}
