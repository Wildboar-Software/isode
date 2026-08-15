/* audio.c - play a sound */

#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "quipu/util.h"
#include "quipu/attr.h"
#include "psap.h"
#include <errno.h>
#ifndef	SVR4
#include <sys/wait.h>
#endif

#ifndef BINPATH
#define BINPATH "/usr/local/bin/"
#endif

/* Assumes the 'play' routine from Mike (bauer@cns.ucalgary.ca) is
   installed in BINPATH directory.
*/

/* Use the {FILE} mechanism to pull in the sound files */

extern void r_octprint(PS ps, struct qbuf *qb, int format);
extern PE r_octenc();
extern struct qbuf * r_octsdec();
extern struct qbuf * r_octparse();
extern struct qbuf * qb_cpy();
extern int qb_cmp(struct qbuf *qb1, struct qbuf *qb2);
extern int file_attr_length;

static void audio_print (
	PS ps,
	struct qbuf * qb,
	int format
) {
	int     pd[2];
	char    buffer [LINESIZE];
	char	execvector [LINESIZE];
	struct qbuf   *p;

	int	pid;
	int	childpid;
#ifndef UNIONWAIT
	int	status;
#else
	union wait status;
#endif
#ifdef LINUX
	__sighandler_t pstat;
#else
	SFP	    pstat;
#endif
	if (format != READOUT) {
		for (p = qb -> qb_forw; p != qb; p = p -> qb_forw)
			ps_write (ps,(PElementData)p->qb_data,p->qb_len);
		return;
	}
#if	!defined(sparc) || !defined(SUNOS41)
	ps_print (ps, "(No audio process defined)");
#else
	if (pipe(pd) == -1) {
		ps_print (ps,"ERROR: could not create pipe");
		return;
	}
	pstat = signal (SIGPIPE, (__sighandler_t)SIG_IGN);
	switch (childpid = fork()) {

	case -1:
		close (pd[1]);
		close (pd[0]);
		signal (SIGPIPE, pstat);
		ps_print (ps,"ERROR: could not fork");
		return;

	case 0:
		signal (SIGPIPE, pstat);
		if (dup2(pd[0], 0) == -1)
			_exit (-1);
		close (pd[0]);
		close (pd[1]);
		sprintf (execvector,"%splay",BINPATH);
		execl (execvector,execvector,NULLCP);
		while (read (0, buffer, sizeof buffer) > 0)
			continue;
		printf ("ERROR: can't execute '%s'",execvector);
		fflush (stdout);
		/* safety catch */
		_exit (-1);
	/* NOTREACHED */

	default:
		close (pd[0]);
		for (p = qb -> qb_forw; p != qb; p = p -> qb_forw) {
			if (write (pd[1],p->qb_data,p->qb_len) != p->qb_len) {
				close (pd[1]);
				signal (SIGPIPE, pstat);
				ps_print (ps,"ERROR: write error");
				return;
			}
		}
		close (pd[1]);
		ps_printf (ps,"%splay invoked",BINPATH);
#ifdef SVR4
		while ((pid = wait (&status)) != NOTOK
#else
		while ((pid = wait (&status.w_status)) != NOTOK
#endif
				&& childpid != pid)
			continue;
		signal (SIGPIPE, pstat);
		return;
	}
#endif
}

static void *audio_parse (char *str) {
	if (file_attr_length)
		return str2qb (str, file_attr_length, 1);
	else
		return str2qb (str, strlen (str), 1);
}

void audio_syntax (void) {
	add_attribute_syntax ("audio",
						  (AttributeValueEncoder)r_octenc,	(AttributeValueDecoder)r_octsdec,
						  audio_parse,	(AttributeValuePrinter)audio_print,
						  (AttributeValueCopier)qb_cpy,	(AttributeValueComparator)qb_cmp,
						  (AttributeValueFree)qb_free,		NULLCP,
						  NULL,			TRUE);
}
