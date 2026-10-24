/* isore.c - help out ISODE TSAP programs */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "general.h"
#include "manifest.h"
#include "compat.h"

static void EMTser (int sig);

int main (int argc, char **argv, char **envp) {
	int	    fd,
			mask,
			nfds,
			ppid;
	fd_set  ifds,
			rfds;

	if (argc != 4)
		exit (1);
	if ((nfds = atoi (argv[1])) < 0
			|| sscanf (argv[2], "0x%x", &mask) != 1
			|| (ppid = atoi (argv[3])) < 0)
		exit (2);

	FD_ZERO (&rfds);
	for (fd = 0; fd < nfds; fd++)
		if (mask & (1 << fd))
			FD_SET (fd, &rfds);
#ifndef BSDSIGS
	signal (SIGEMT, EMTser);
#endif

	for (;;) {
		ifds = rfds;
		switch (xselect (nfds, &ifds, NULLFD, NULLFD, NOTOK)) {
		case NOTOK:
			fprintf (stderr, "NOTOK\n");
			break;

		case OK:
			fprintf (stderr, "OK\n");
			break;

		default:
#ifndef BSDSIGS
			kill (ppid, SIGEMT);
#endif
			pause();
			break;
		}
	}
}

static void EMTser (int sig) {
	(void) sig;
#ifndef	BSDSIGS
	signal (SIGEMT, EMTser);
#endif
}
