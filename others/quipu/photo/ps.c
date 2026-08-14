/* ps.c - fax to postscript */

#include <stdio.h>
#include <quipu/photo.h>

#define HEIGHT 2200

static int x, y;

extern int two_passes;

int photo_start (char *name) {
	x = 0;
	y = HEIGHT;
	two_passes = 0;
	puts ("%!\n0 setlinewidth 72 200 div 72 200 div scale");
	return 0;
}

int photo_end (char *name) {
	/* Decoding has finished - display the image */

	if (y < HEIGHT) puts ("showpage");
	return 0;
}

int photo_black (int length) {
	if (length > 0)
		printf ("%d %d moveto %d %d lineto stroke\n", x, y, x + length - 1, y);
	x += length;
}

int photo_white (int length) {
	x += length;
}

void photo_line_end (bit_string *line) {
	x = 0;
	--y;
	if (y < 0) {
		puts ("showpage");
		y = HEIGHT;
	}
}
