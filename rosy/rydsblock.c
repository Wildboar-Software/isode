/* rydsblock.c - manage dispatch blocks */
#include <stdlib.h>
#include <search.h>
#include "rosy.h"

static int  once_only = 0;
static struct dspblk dspque;
static struct dspblk *DSHead = &dspque;

/*    DISPATCH BLOCKS */

struct dspblk  *newdsblk (const int sd, const struct RyOperation *ryo)
{
	struct dspblk *dsb;

	dsb = (struct dspblk   *) calloc (1, sizeof *dsb);
	if (dsb == NULL)
		return NULL;
	dsb -> dsb_fd = sd;
	dsb -> dsb_ryo = ryo;
	if (once_only == 0) {
		DSHead -> dsb_forw = DSHead -> dsb_back = DSHead;
		once_only++;
	}
	insque (dsb, DSHead -> dsb_back);
	return dsb;
}

void freedsblk (struct dspblk *dsb) {
	if (dsb == NULL)
		return;
	remque (dsb);
	free ((char *) dsb);
}

struct dspblk *finddsblk (const int sd, const int op) {
	struct dspblk *dsb;
	if (once_only == 0)
		return NULL;
	for (dsb = DSHead -> dsb_forw; dsb != DSHead; dsb = dsb -> dsb_forw)
		if (dsb -> dsb_fd == sd && dsb -> dsb_ryo -> ryo_op == op)
			return dsb;
	return NULL;
}

void losedsblk (const int sd) {
	struct dspblk *dsb, *ds2;
	if (once_only == 0)
		return;
	for (dsb = DSHead -> dsb_forw; dsb != DSHead; dsb = ds2) {
		ds2 = dsb -> dsb_forw;
		if (dsb -> dsb_fd == sd)
			freedsblk (dsb);
	}
}
