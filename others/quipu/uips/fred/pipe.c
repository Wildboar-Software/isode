/* pipe.c - fred talks to dish */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/fred/RCS/pipe.c,v 9.0 1992/06/16 12:44:30 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/others/quipu/uips/fred/RCS/pipe.c,v 9.0 1992/06/16 12:44:30 isode Rel $
 *
 *
 * $Log: pipe.c,v $
 * Revision 9.0  1992/06/16  12:44:30  isode
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


#include <ctype.h>
#include <signal.h>
#include <stdarg.h>
#include "fred.h"
#include "internet.h"

#include "sys.file.h"
#include <sys/stat.h>
#include "usr.dirent.h"
#ifdef	BSD42
#include <sys/wait.h>
#endif

/*  */

static int	dish_running = NOTOK;

static struct sockaddr_in sin;

static int	dafd = NOTOK;
static char	da_reply[BUFSIZ];

static	do_edit ();
static  foreground ();
static	mypager ();
static pagchar ();
#ifndef	lint
static int  da_command (char *, ...);
static int  _da_command ();
#endif
static int  da_response ();

/*    DISH */

int
dish (char *command, int silent) {
	int	    cc,
			isarea,
			isuser,
			n,
			sd,
			status;
	char    buffer[BUFSIZ],
			where[BUFSIZ];
	struct sockaddr_in *sock = &sin;
	FILE   *fp;

	if (watch) {
		fprintf (stderr, "%s\n", command);
		fflush (stderr);
	}

	isarea = strncmp (command, "moveto -pwd", sizeof "moveto -pwd" - 1)
			 ? 0 : 1;
	isuser = !isarea && strcmp (command, "squid -user") == 0;

	if (dafd != NOTOK) {
		if (da_command ("STAT") == NOTOK) {
			close_tcp_socket (dafd);
			dafd = NOTOK, boundP = 0;
		}
	} else if (dish_running != NOTOK
			   && kill (dish_running, 0) == NOTOK)
		dish_running = NOTOK, boundP = 0;

	if (dish_running == NOTOK && dafd == NOTOK) {
		int	vecp;
		char	dishname[BUFSIZ],
				*vec[4];
		static int very_first_time = 1;

		if (very_first_time) {
			unsetenv ("DISHPROC");
			unsetenv ("DISHPARENT");

			very_first_time = 0;
		}

		if (strcmp (server, "internal")) {
			int	    portno;
			char   *cp,
				   *dp;
			struct hostent *hp;
			struct servent *sp;

			if ((dafd = start_tcp_client ((struct sockaddr_in *) 0, 0))
					== NOTOK)
				adios ("control connection", "unable to start");

			if (cp = index (server, ':')) {
				if (sscanf (cp + 1, "%d", &portno) == 1)
					*cp = NULL;
				else
					cp = NULL;
			}
			if ((hp = gethostbystring (server)) == NULL)
				adios (NULLCP,
					   "%s: unknown host for directory assistance server",
					   server);
			if (cp)
				*cp = ':';
			bzero ((char *) sock, sizeof *sock);
			sock -> sin_family = hp -> h_addrtype;
			inaddr_copy (hp, sock);

			if (cp)
				sock -> sin_port = htons ((u_short) portno);
			else if ((sp = getservbyname ("da", "tcp")) == NULL)
				sock -> sin_port = htons ((u_short) 411);
			else
				sock -> sin_port = sp -> s_port;

			if (join_tcp_server (dafd, sock) == NOTOK)
				adios ("control connection", "unable to establish");

			if (da_response () == NOTOK)
				adios (NULLCP, "%s", da_reply);

			cp = da_reply + sizeof "+OK " - 1;
			if ((dp = index (cp, ' ')) == NULLCP
					|| sscanf (dp + 1, "%d", &portno) != 1)
				adios (NULLCP, "malformed response for data connection: %s",
					   da_reply);
			*dp = NULL;

			if ((hp = gethostbystring (cp)) == NULL)
				adios (NULLCP, "%s: unknown host for data connection", cp);
			bzero ((char *) sock, sizeof *sock);
			sock -> sin_family = hp -> h_addrtype;
			inaddr_copy (hp, sock);
			sock -> sin_port = htons ((u_short) portno);

			didbind = 0, boundP = 1;
			signal (SIGPIPE, SIG_IGN);

			if (cp = getenv ("DISPLAY")) {
				char cp_host [1024];
				char *cp_disp;
				struct sockaddr_in sinl;
				int sinl_size;

				strncpy (cp_host, cp, 1024);
				cp_host [1023] = '\0';
				if ((cp_disp = index (cp_host, ':')) == NULLCP)
					goto no_display;
				*cp_disp++ = '\0';
				if (strcmp (cp_host, "local") &&
						strcmp (cp_host, "localhost") &&
						strcmp (cp_host, "unix"))
					sprintf (buffer, "fred -display \"%s\"", cp);
				else {
					sinl_size = sizeof (struct sockaddr_in);
					if (getsockname (dafd, (struct sockaddr *)&sinl,
									 &sinl_size) == NOTOK)
						goto no_display;
					if (sinl.sin_addr.s_addr == sock->sin_addr.s_addr)
						sprintf (buffer, "fred -display \"%s\"", cp);
					else {
						cp = inet_ntoa (sinl.sin_addr);
						sprintf (buffer, "fred -display \"%s:%s\"",
								 cp, cp_disp);
					}
				}
				dish (buffer, 1);
			}
no_display:

			goto do_conn;
		}

		if (get_dish_sock (sock, getpid (), 1) == NOTOK)
			exit (1);

		strcpy (dishname, _isodefile (isodebinpath, "dish"));

fork_again:
		;
		switch (dish_running = vfork ()) {
		case NOTOK:
			adios ("fork", "unable to");
		/* NOT REACHED */

		case OK:
			vecp = 0;
			vec[vecp++] = "dish";
			vec[vecp++] = "-pipe";
			vec[vecp++] = "-fast";
			vec[vecp] = NULL;
			execv (dishname, vec);
			fprintf (stderr, "unable to exec ");
			perror (dishname);
			_exit (1);

		default:
			for (;;) {
				if ((sd = start_tcp_client ((struct sockaddr_in *) 0, 0))
						== NOTOK)
					adios ("client", "unable to start");
				if (join_tcp_server (sd, sock) != NOTOK)
					break;

				close_tcp_socket (sd);

				sleep (5);

				if (kill (dish_running, 0) == NOTOK)
					goto fork_again;
			}

			didbind = 0, boundP = 1;
			signal (SIGPIPE, SIG_IGN);
			break;
		}
	} else {
do_conn:
		;
		if ((sd = start_tcp_client ((struct sockaddr_in *) 0, 0)) == NOTOK)
			adios ("client", "unable to start");
		if (join_tcp_server (sd, sock) == NOTOK)
			adios ("server", "unable to join");
	}

	sprintf (buffer, "%s\n", command);
	n = send (sd, buffer, cc = strlen (buffer), 0);
	if (debug)
		fprintf (stderr, "wrote %d of %d octets to DUA\n", n, cc);

	if (n != cc)
		if (n == NOTOK) {
			advise ("please retry", "write to DUA failed,");
			f_quit (NULLVP);
			close_tcp_socket (sd);
			return NOTOK;
		} else
			adios (NULLCP, "write to DUA truncated, sent %d of %d octets",
				   n, cc);

	status = OK;
	for (;;) {
		if ((cc = recv (sd, buffer, sizeof buffer - 1, 0)) == NOTOK) {
err_recv:
			;
			if (!interrupted)
				adios ("failed", "read from DUA");

			if (dafd != NOTOK)
				da_command ("INTR");
			else
				kill (dish_running, SIGINT);
			interrupted = 0;
			continue;
		}

		buffer[cc] = NULL;
		if (debug)
			fprintf (stderr, "read %d octets from DUA: '%c'\n", cc,
					 cc > 0 ? buffer[0] : ' ');
		if (cc == OK) {
lost_dua:
			;
			if ((dafd != NOTOK ? da_command ("STAT") : kill (dish_running, 0))
					== NOTOK) {
				if (dafd != NOTOK) {
					close_tcp_socket (dafd);
					dafd = NOTOK;
				}
				boundP = 0;

				advise (NULLCP, "lost DUA");
			}

			break;
		}

		if (!isdigit (buffer[0])) {
			char  *cp,
				  *ep;

			cp = buffer + cc - 1;
			ep = buffer + sizeof buffer - 1;

			while (*cp != '\n') {
				++cp;
				switch (cc = recv (sd, cp, ep - cp, 0)) {
				case NOTOK:
					goto err_recv;

				case OK:
				default:
					if (debug)
						fprintf (stderr, "read %d more octets from DUA\n",
								 cc);
					if (cc == OK)
						goto lost_dua;
					cp += cc - 1;
					if (cp < ep)
						continue;
					if (debug)
						fprintf (stderr,
								 "'%c' directive exceeds %d octets",
								 buffer[0], sizeof buffer - 1);
					cp++;
					break;
				}
				break;
			}
			*cp = NULL;
		}

		switch (buffer[0]) {
		case '2':
			if ((fp = errfp) == NULL)
				fp = stdfp != stdout ? stdfp : stderr;
			status = NOTOK;

copy_out:
			;
			if (cc > 1 && !silent)
				paginate (fp, buffer + 1, cc - 1);

			while ((cc = recv (sd, buffer, sizeof buffer - 1, 0)) > OK)
				if (!silent)
					paginate (fp, buffer, cc);

			if (!silent)
				paginate (fp, NULLCP, 0);
			break;

		case '1':
		case '3':
			if (isarea || isuser) {
				char   *cp,
					   **vp;

				if (cp = index (buffer + 1, '\n'))
					*cp = NULL;
#ifdef	notdef
				if (buffer[1] == NULL)
					break;
#endif
				buffer[0] = '@';
				vp = isarea ? &myarea : &mydn;

				if (*vp)
					free (*vp);
				*vp = strdup (buffer);
			}
			fp = stdfp;
			goto copy_out;

		case 'e':
			if (watch) {
				fprintf (stderr, "%s\n", buffer + 1);
				fflush (stderr);
			}
			if (dafd != NOTOK)
				switch (do_edit (sd, buffer + 1)) {
				case NOTOK:
					return NOTOK;

				case OK:
					goto user_abort;

				default:
					continue;
				}
			if (system (buffer + 1)) {
user_abort:
				;
				strcpy (where, "e");
			} else
				getcwd (where, sizeof where);
			goto stuff_it;

		case 'm':
			strcpy (where, "m");
			if (!network) {
				fprintf (stderr, "\n%s\n", buffer + 1);
				fflush (stderr);
			}
			goto stuff_it;

		case 'y':
			if (network)
				strcpy (where, "n");
			else
				switch (ask ("%s", buffer + 1)) {
				case NOTOK:
				default:
					strcpy (where, "n");
					break;

				case OK:
					strcpy (where, "y");
					break;

				case DONE:
					putchar ('\n');
					strcpy (where, "N");
					break;
				}
			goto stuff_it;

		case 'p':
			sprintf (where, "Enter password for \"%s\": ",
					 buffer + 1);
			sprintf (where, "p%s", getpassword (where));

stuff_it:
			;
			strcat (where, "\n");
			if (watch) {
				fprintf (stderr, "%s", where);
				fflush (stderr);
			}
			n = send (sd, where, cc = strlen (where), 0);
			if (debug)
				fprintf (stderr, "wrote %d of %d octets to DUA\n", n, cc);

			if (n != cc)
				if (n == NOTOK) {
					advise ("please retry", "write to DUA failed,");
					f_quit (NULLVP);
					close_tcp_socket (sd);
					return NOTOK;
				} else
					adios (NULLCP,
						   "write to DUA truncated, sent %d of %d octets",
						   n, cc);
			continue;

		default:
			advise (NULLCP, "unexpected opcode 0x%x -- contact a camayoc",
					buffer[0]);
			break;
		}
		break;
	}

	close_tcp_socket (sd);

	return status;
}

/*  */

static
do_edit (int sd, char *octets) {
	int	    cc,
			i,
			j,
			k,
			n;
	char   *cp,
		   *dp,
		   buffer[BUFSIZ],
		   tmpfil[BUFSIZ];
	FILE   *fp;
	struct stat st;

	strcpy (tmpfil, "/tmp/fredXXXXXX");
	close (mkstemp (tmpfil));

	if (sscanf (octets, "%d", &j) != 1 || j < 0) {
		advise (NULLCP, "protocol botch");
losing:
		;
		f_quit (NULLVP);
		close_tcp_socket (sd);
		unlink (tmpfil);
		return NOTOK;
	}

	if (watch) {
		fprintf (stderr, "y\n");
		fflush (stderr);
	}
	n = send (sd, "y\n", cc = sizeof "y\n" - 1, 0);
	if (debug)
		fprintf (stderr, "wrote %d of %d octets to DUA\n", n, cc);

	if (n != cc)
		if (n == NOTOK) {
			advise ("please retry", "write to DUA failed,");
			goto losing;
		} else
			adios (NULLCP, "write to DUA truncated, sent %d of %d cotets",
				   n, cc);

	if ((fp = fopen (tmpfil, "w")) == NULL)
		adios (tmpfil, "unable to write");

	for (cc = j; j > 0; j -= i)
		switch (i = recv (sd, buffer, j < sizeof buffer ? j : sizeof buffer,
						  0)) {
		case NOTOK:
			if (!interrupted)
				adios ("failed", "read from DUA");
			da_command ("INTR");
			interrupted = 0;
			fclose (fp);
			unlink (tmpfil);
			return DONE;

		case OK:
			advise (NULLCP, "premature eof from peer");
			goto losing;

		default:
			if (debug)
				fprintf (stderr, "read %d %soctets from DUA\n",
						 i, j != cc ? "more " : "");
			if (fp && fwrite (buffer, sizeof *buffer, i, fp) == 0) {
				advise (tmpfil, "error writing to");
				fclose (fp);
				fp = NULL;
			}
			break;
		}

	if (fp == NULL) {
all_done:
		;
		unlink (tmpfil);
		return OK;
	}
	fclose (fp);

	sprintf (buffer, "%s %s",
			 _isodefile (isodebinpath, "editentry"), tmpfil);
	if (system (buffer))
		goto all_done;

	cp = NULL;
	if ((fp = fopen (tmpfil, "r")) == NULL) {
		advise ("reading", "unable to re-open %s for", tmpfil);
		goto all_done;
	}
	if (fstat (fileno (fp), &st) == NOTOK
			|| (st.st_mode & S_IFMT) != S_IFREG
			|| (cc = st.st_size) == 0) {
		advise (NULLCP, "%s: not a regular file", tmpfil);
nearly_done:
		;
		if (cp)
			free (cp);
		fclose (fp);
		goto all_done;
	}

	sprintf (buffer, "e%d\n", cc);
	j = strlen (buffer);
	if ((cp = malloc ((unsigned) (k = cc + j + 1))) == NULL)
		adios (NULLCP, "out of memory");
	strcpy (dp = cp, buffer);
	for (dp += j, j = cc; j > 0; dp += i, j -= i)
		switch (i = fread (dp, sizeof *dp, j, fp)) {
		case NOTOK:
			advise (tmpfil, "error reading");
			goto nearly_done;

		case OK:
			advise (NULLCP, "premature eof reading %s", tmpfil);
			goto nearly_done;

		default:
			break;
		}
	*dp = NULL;

	fclose (fp);
	unlink (tmpfil);

	if (watch) {
		fprintf (stderr, "///////\n%s///////\n", cp);
		fflush (stderr);
	}
	for (dp = cp, j = k; j > 0; dp += i, j -= i)
		switch (i = send (sd, dp, j, 0)) {
		case NOTOK:
			advise ("please retry", "write to DUA failed,");
			f_quit (NULLVP);
			close_tcp_socket (sd);
			free (cp);
			return NOTOK;

		case OK:
			adios (NULLCP, "zero-length write to peer");

		default:
			if (debug)
				fprintf (stderr, "wrote %d %soctets to DUA\n",
						 j, dp != cp ? "more " : "");
			break;
		}

	free (cp);
	return DONE;
}

/*  */

paginate (fp, buffer, cc)
FILE   *fp;
char   *buffer;
int	cc;
{
	static int first_time = 1;
	static int doing_pager = 0;
	static int pid = NOTOK;
	static int sd = NOTOK;
	static SFP Istat, Qstat;

	if (cc == 0) {
#ifndef UNIONWAIT
		int	status;
#else
		union wait status;
#endif
		int	child;

		first_time = 1;
		fflush (fp);
		if (!doing_pager)
			return;

		doing_pager = 0;

		if (dup2 (sd, fileno (fp)) == NOTOK)
			adios ("standard output", "unable to dup2");

		clearerr (fp);
		close (sd);

#ifndef UNIONWAIT
		while ((child = wait (&status)) != NOTOK
#else
		while ((child = wait (&status.w_status)) != NOTOK
#endif
				&& child != pid)
			continue;

		signal (SIGINT, Istat);
		signal (SIGQUIT, Qstat);

		return;
	}

	if (first_time) {
		int	pd[2];

		first_time = 0;

		if (dontpage || network || *pager == NULL || !isatty (fileno (fp)))
			goto no_pager;

		fflush (fp);

		foreground ();

		if ((sd = dup (fileno (fp))) == NOTOK) {
			advise ("dup", "unable to");
			goto no_pager;
		}

		if (pipe (pd) == NOTOK) {
			advise ("pipe", "unable to");
			goto no_pager;
		}
		switch (pid = fork ()) {
		case NOTOK:
			advise ("fork", "unable to");
			close (pd[0]);
			close (pd[1]);
			goto no_pager;

		case OK:
			signal (SIGINT, SIG_DFL);
			signal (SIGQUIT, SIG_DFL);

			close (pd[1]);
			if (pd[0] != fileno (stdin)) {
				dup2 (pd[0], fileno (stdin));
				close (pd[0]);
			}
			if (readonly)
				mypager (stdin);
			else {
				execlp (pager, pager, NULLCP);
				fprintf (stderr, "unable to exec ");
				perror (pager);
			}
			_exit (-1);

		default:
			close (pd[0]);
			if (pd[1] != fileno (fp)) {
				dup2 (pd[1], fileno (fp));
				close (pd[1]);
			}
			break;
		}

		Istat = signal (SIGINT, SIG_IGN);
		Qstat = signal (SIGQUIT, SIG_IGN);

		doing_pager = 1;
	}

no_pager:
	;
	if (network && !mail) {
		char *cp,
			 *dp;

		for (dp = (cp = buffer) + cc; cp < dp; cp++) {
			if (*cp == '\n')
				fputc ('\r', fp);
			fputc (*cp, fp);
		}
	} else
		fwrite (buffer, sizeof buffer[0], cc, fp);
}

/*  */

/* if you start a command and then background fred, if your pager is less,
   then for some reason, less gets the terminal modes/process groups messed up.

   this code pretty much ensures that fred is running in the foreground when
   it forks less.  there is still a critical window, but it is very small...
 */

static
foreground () {
#ifdef	TIOCGPGRP
	int     pgrp,
	tpgrp;
	SFP	    tstat;

	if ((pgrp = getpgrp (0)) == NOTOK)
		return;

	tstat = signal (SIGTTIN, SIG_DFL);
	for (;;) {
		if (ioctl (fileno (stdin), TIOCGPGRP, (char *) &tpgrp) == NOTOK)
			break;
		if (pgrp == tpgrp)
			break;

		kill (0, SIGTTIN);
	}
	signal (SIGTTIN, tstat);
#endif
}

/*  */

static	int	cols;
static	int	rows;
static	int	length = 0;
static	int	width = 0;


static	mypager (fp)
FILE   *fp;
{
	char *bp;
	char    buffer[BUFSIZ];
#ifdef	TIOCGWINSZ
	struct winsize    ws;
#endif

#ifdef	TIOCGWINSZ
	if (ioctl (fileno (stdout), TIOCGWINSZ, (char *) &ws) != NOTOK)
		length = ws.ws_row, width = ws.ws_col;
#endif
	if (--length <= 0)
		length = 23;
	if (--width <= 0)
		width = 79;

	rows = cols = 0;

	while (fgets (buffer, sizeof buffer, fp))
		for (bp = buffer; *bp; bp++)
			pagchar (*bp);

	fflush (stdout);
}


static
pagchar (int ch) {
	char    buffer[BUFSIZ];

	switch (ch) {
	case '\n':
		cols = 0;
		if (++rows < length)
			break;
		if (bflag)
			putc (0x07, stdout);
		fflush (stdout);
		buffer[0] = NULL;
		read (fileno (stdout), buffer, sizeof buffer);
		if (buffer[0] == '\n')
			rows = 0;
		else {
			putc ('\n', stdout);
			rows = length / 3;
		}
		return;

	case '\t':
		cols |= 07;
		cols++;
		break;

	case '\b':
		cols--;
		break;

	case '\r':
		cols = 0;
		break;

	default:
		if (ch >= ' ')
			cols++;
		break;
	}

	if (cols >= width) {
		pagchar ('\n');
		pagchar (ch);
	} else
		putc (ch, stdout);
}

/*    BIND */

/* ARGSUSED */

int
f_bind (char **vec) {
	if (didbind) {
		didbind = 0;
		return OK;
	}

	return dish ("bind", 0);
}

/*    QUIT */

int
f_quit (char **vec) {
	if (vec && *++vec != NULL && strcmp (*vec, "-help") == 0) {
		fprintf (stdfp, "quit\n");
		fprintf (stdfp, "    terminate fred\n");

		return OK;
	}

	if (dafd != NOTOK) {
		da_command ("QUIT");

		close_tcp_socket (dafd);
	} else if (dish_running != NOTOK)
		kill (dish_running, SIGHUP);

	dafd = dish_running = NOTOK, boundP = 0;

	return DONE;
}

/*    DA */

#ifndef	lint
static int  da_command (char *fmt, ...) {
	int	    val;
	va_list ap;

	va_start (ap, fmt);

	val = _da_command (fmt, ap);

	va_end (ap);

	return val;
}

static int  _da_command (char *fmt, va_list ap)
{
	int	    cc,
			len;
	char    buffer[BUFSIZ];

	if (dafd == NOTOK)
		return NOTOK;

	_asprintf (buffer, NULLCP, fmt, ap);
	if (watch) {
		fprintf (stderr, "<--- %s\n", buffer);
		fflush (stderr);
	}

	strcat (buffer, "\r\n");
	len = strlen (buffer);

	if (write_tcp_socket (dafd, buffer, len) != len)
		adios ("failed", "write_tcp_socket to control connection");

	return (da_response ());
}
#else
/* VARARGS1 */

static int
da_command (char *fmt) {
	return da_command (fmt);
}
#endif

/*  */

static int
da_response () {
	char *cp,
	*ep;

	for (ep = (cp = da_reply) + sizeof da_reply - 1; cp < ep;) {
		if (read_tcp_socket (dafd, cp, sizeof *cp) != 1)
			adios ("control connection", "eof or error reading");
		if (*cp++ == '\n')
			break;
	}
	*cp = NULL;

	if (cp > da_reply)
		cp--;
	if (*cp == '\n') {
		*cp = NULL;
		if (cp > da_reply)
			cp--;
	}
	if (*cp == '\r')
		*cp = NULL;

	if (watch) {
		fprintf (stderr, "---> %s\n", da_reply);
		fflush (stderr);
	}

	switch (da_reply[0]) {
	case '+':
		return OK;

	case '-':
		return NOTOK;

	default:
		adios (NULLCP, "unrecognized response");
		/* NOTREACHED */
	}
}

/*  */

int
sync_ufnrc () {
	char *bp;
	char    buffer[BUFSIZ];
	struct area_guide *ag;

	sprintf (bp = buffer, "fred -ufnrc");
	bp += strlen (bp);

	for (ag = areas; ag -> ag_record; ag++)
		if (ag -> ag_record == W_ORGANIZATION)
			break;

	sprintf (bp, " 1 1 \"%s", myarea + 1);
	bp += strlen (bp);
	if ((ag -> ag_record) && (ag -> ag_area)) {
		sprintf (bp, "$%s", ag -> ag_area + 1);
		bp += strlen (bp);
	}
	sprintf (bp, "$-\"");
	bp += strlen (bp);

	sprintf (bp, " 2 2 \"");
	bp += strlen (bp);
	if ((ag -> ag_record) && (ag -> ag_area)) {
		sprintf (bp, "%s$", ag -> ag_area + 1);
		bp += strlen (bp);
	}
	sprintf (bp, "%s$-\"", myarea + 1);
	bp += strlen (bp);

	sprintf (bp, " 3 32767 \"-");
	bp += strlen (bp);
	if ((ag -> ag_record) && (ag->ag_area)) {
		sprintf (bp, "$%s", ag -> ag_area + 1);
		bp += strlen (bp);
	}
	sprintf (bp, "$%s\"", myarea + 1);
	bp += strlen (bp);

	return dish (buffer, 1);
}

/*  */

int
init_ufnrc () {
	int   i;
	int	    inprogress;
	char *bp,
	*cp,
	*ep;
	char    buffer[BUFSIZ],
	sync[BUFSIZ],
	ufnrc[BUFSIZ];
	FILE   *fp;

	if (bp = getenv ("UFNRC"))
		strcpy (ufnrc, bp);
	else {
		if ((bp = getenv ("HOME")) == NULL)
			bp = ".";

		sprintf (ufnrc, "%s/.ufnrc", bp);
	}

	if ((fp = fopen (ufnrc, "r")) == NULL) {
		strcpy (ufnrc, isodefile ("ufnrc", 0));

		if ((fp = fopen (ufnrc, "r")) == NULL)
			return NOTOK;
	}

	sprintf (ep = sync, "fred -ufnrc");
	ep += strlen (ep);

	inprogress = 0;
	for (i = 0; fgets (bp = buffer, sizeof buffer, fp); i++) {
		if (*buffer == '#')
			continue;
		if (bp = index (buffer, '\n'))
			*bp = NULL;

		bp = buffer;
		if (*bp == NULL) {
			if (inprogress) {
				strcpy (ep, "\"");
				ep += strlen (ep);
				inprogress = 0;
			}
			continue;
		}

		if (!isspace (*bp)) {
			int	    lower,
					upper;
			char *dp;

			if ((cp = index (bp, ':')) == NULL) {
				advise (NULLCP, "%s: missing ':' at line %d", ufnrc, i);
				return NOTOK;
			}
			*cp++ = NULL;

			if (dp = index (bp, ',')) {
				*dp++ = NULL;

				while (isspace (*dp))
					dp++;
				upper = *dp == '+' ? 32767 : atoi (dp);
			} else
				upper = 0;

			lower = atoi (bp);
			sprintf (ep, " %d %d \"", lower, upper ? upper : lower);
			ep += strlen (ep);

			bp = cp;
			inprogress = 1;
		} else if (!inprogress) {
			advise (NULLCP, "%s: unexpected blank at start of line %d",
					ufnrc, i);
			return NOTOK;
		} else
			*ep++ = '$';

		while (isspace (*bp))
			bp++;
		sprintf (ep, "%s", bp);
		ep += strlen (ep);
	}

	if (inprogress) {
		strcpy (ep, "\"");
		ep += strlen (ep);
		inprogress = 0;
	}

	fclose (fp);

	return dish (sync, 1);
}
