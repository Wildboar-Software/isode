/* photo_stub.c */

#include "unistd.h"
#include "stdio.h"
#include "quipu/photo.h"

/* Stub null photo routines - see others/quipu/photo for *real* examples */

int photo_start (const char * name) {
	fprintf (stderr,"PHOTO: NYI (1)");
	return (-1);
}

int photo_end (const char * name) {
	printf ("PHOTO: NYI (2)");
	fflush (stdout);
	close (1);
	return (-1);
}

void photo_black (const int length) {
	;
}

void photo_white (const int length) {
	;
}

void photo_line_end (const bit_string * line) {
	;
}
