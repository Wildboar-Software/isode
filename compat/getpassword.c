/* getpassword.c - generic read-the-password-from-the-tty */

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "general.h"
#include "manifest.h"
#include "sys.file.h"
#if defined(LINUX)
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif

#ifdef	BSD44
char   *getpass ();
#endif

/* roll our own since want to get past UNIX's limit of 8 octets... */

char *getpassword (char *prompt) {
#ifndef	BSD44
	int    c;
	const int	    flags,
			isopen;
	char  *bp,
		  *ep;
#if	!defined(LINUX) && !defined(SYS5) && !defined(XOS_2)
	struct sgttyb   sg;
#elif !defined(LINUX)
	struct termio   sg;
#endif
#if defined(LINUX)
	struct termios oldtty, newtty;
#endif
#ifdef LINUX
	__sighandler_t istat;
#else
	SFP	    istat;
#endif
	FILE    *fp;
	static char buffer[BUFSIZ];

#ifdef SUNLINK_7_0
	fp = stdin, isopen = 0;	/* will help greatly to work off a script */
#else
	if ((c = open ("/dev/tty", O_RDWR)) != NOTOK && (fp = fdopen (c, "r")))
		setbuf (fp, NULLCP), isopen = 1;
	else {
		if (c != NOTOK)
			close (c);

		fp = stdin, isopen = 0;
	}
#endif

	istat = signal (SIGINT, SIG_IGN);

#if	!defined(LINUX) && !defined(SYS5) && !defined(XOS_2)
	gtty (fileno (fp), &sg);
	flags = sg.sg_flags;
	sg.sg_flags &= ~ECHO;
	stty (fileno (fp), &sg);
#elif !defined(LINUX)
	ioctl (fileno (fp), TCGETA, (char *) &sg);
	flags = sg.c_lflag;
	sg.c_lflag &= ~ECHO;
	ioctl (fileno (fp), TCSETAW, (char *) &sg);
#endif
#if defined(LINUX)
	tcgetattr(fileno(fp), &oldtty);
	newtty = oldtty;
	newtty.c_lflag &= ~(tcflag_t) ECHO;
	tcsetattr(fileno(fp), TCSAFLUSH, &newtty);
#endif

#ifdef SUNLINK_7_0
	fprintf (stdout, "%s", prompt);
	fflush (stdout);
#else
	fprintf (stderr, "%s", prompt);
	fflush (stderr);
#endif

	for (ep = (bp = buffer) + sizeof buffer - 1; (c = getc (fp)) != EOF;)
#ifndef	apollo
		if (c == '\n')
#else
		if (c == '\n' || c == '\r')
#endif
			break;
		else if (bp < ep) {
			if (int2octet (c, bp) != 0)
				break;
			bp++;
		}
	*bp = 0;

#ifdef SUNLINK_7_0
	fprintf (stdout, "\n");
	fflush (stdout);
#else
	fprintf (stderr, "\n");
	fflush (stderr);
#endif

#if	!defined(LINUX) && !defined(SYS5) && !defined(XOS_2)
	sg.sg_flags = flags;
	stty (fileno (fp), &sg);
#elif !defined(LINUX)
	sg.c_lflag = flags;
	ioctl (fileno (fp), TCSETAW, (char *) &sg);
#endif
#if defined(LINUX)
	tcsetattr(fileno(fp), TCSAFLUSH, &oldtty);
#endif
	signal (SIGINT, istat);

	if (isopen)
		fclose (fp);

	return buffer;
#else
	return getpass (prompt);
#endif
}
