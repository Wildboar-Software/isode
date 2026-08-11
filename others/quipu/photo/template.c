/* template.c - template for display processes */







#include "stdio.h"
#include "quipu/photo.h"

/* Any errors should be written to *stdout* */
/* if the process exits, with out giving an error message, quipu may hang */

	int
	photo_start (char *name) {
		/* Initialise a window to recieve a photo of 'name' */

		/* return 0 if sucessful -1 if not */
		fprintf (stderr,"PHOTO: not implemented yet");
		return (-1);
	}

	int
	photo_end (char *name) {
		/* Decoding has finished - display the photo */
		printf ("done it");
		fflush (stdout);
		close (1);	/* this is needed for QUIPU if the process does not exit */

		/* return 0 if sucessful -1 if not */
		return (-1);
	}

	int
	photo_black (int length) {
		/* draw a black line of 'length' pixels */
	}

	int
	photo_white (int length) {
		/* draw a white line of 'length' pixels */
	}

	photo_line_end (line)
	bit_string * line;
	{
		/* the end of a line has been reached */
		/* A bit string is stored in line->dbuf_top */
	}
