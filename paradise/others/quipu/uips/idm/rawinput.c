#include <stdio.h>
#include <sys/termio.h>
#include <sys/ioctl.h>
#include "extmessages.h"
void setRawMode(void);
void unsetRawMode(void);


static short savemode;
static unsigned char savemin;
static struct termio t;

/* ought to be a bit friendlier than this - but at least this attempts
   to be safe */

void setRawMode(void) {
	if (ioctl(0, TCGETA, &t) == -1) {
		fprintf(stderr, "%s (1)\n", no_raw_mode);
		cleanup(-1);
	}
	savemode = t.c_lflag;
	savemin = t.c_cc[VMIN];
	t.c_lflag &= ~(ICANON|ECHO|ISIG);
	t.c_cc[VMIN] = 1;
	if (ioctl(0, TCSETA, &t) == -1) {
		fprintf(stderr, "%s (2)\n", no_raw_mode);
		cleanup(-1);
	}
}

void unsetRawMode(void) {
	t.c_lflag = savemode;
	t.c_cc[VMIN] = savemin;
	if (ioctl(0, TCSETA, &t) == -1) {
		fprintf(stderr, "%s\n", no_out_raw_mode);
		cleanup(-1);
	}
}
