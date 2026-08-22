/* template.c - your comments here */

#include <stdio.h>
#ifdef __linux__
#include <linux/termios.h>
#else
#include <sys/termio.h>
#endif
#include <sys/ioctl.h>
int setRawMode (void);
int unsetRawMode (void);


static short savemode;
static unsigned char savemin;
static struct termio t;

/* ought to be a bit friendlier than this - but at least this attempts
   to be safe */

int setRawMode (void) {
	if (ioctl(0, TCGETA, &t) == -1) {
		fprintf(stderr, "Couldn't go into raw mode (1), aaaaaagggggghhhhh!!!!\n");
		cleanup(-1);
	}
	savemode = t.c_lflag;
	savemin = t.c_cc[VMIN];
	t.c_lflag &= ~(ICANON|ECHO|ISIG);
	t.c_cc[VMIN] = 1;
	if (ioctl(0, TCSETA, &t) == -1) {
		fprintf(stderr, "Couldn't go into raw mode (2), aaaaaagggggghhhhh!!!!\n");
		cleanup(-1);
	}
}

int unsetRawMode (void) {
	t.c_lflag = savemode;
	t.c_cc[VMIN] = savemin;
	if (ioctl(0, TCSETA, &t) == -1) {
		fprintf(stderr, "Couldn't get out of raw mode, aaaaaagggggghhhhh!!!!\n");
		cleanup(-1);
	}
}
