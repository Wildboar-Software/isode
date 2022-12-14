/* edit.c - */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/dish/RCS/edit.c,v 9.0 1992/06/16 12:35:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/dish/RCS/edit.c,v 9.0 1992/06/16 12:35:39 isode Rel $
 *
 *
 * $Log: edit.c,v $
 * Revision 9.0  1992/06/16  12:35:39  isode
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


#include <errno.h>
#include "manifest.h"
#include "quipu/util.h"
#include "psap.h"
#include "tailor.h"
#include "sys.file.h"
#include <sys/stat.h>

extern char     fname[];

#define	OPT	(!frompipe || rps -> ps_byteno == 0 ? opt : rps)
#define	RPS	(!frompipe || opt -> ps_byteno == 0 ? rps : opt)
extern	char	frompipe;
extern	PS	opt, rps;

int
editentry (int argc, char **argv) {
	char            str[LINESIZE];
	char            prog[LINESIZE];
	int             res;
	extern char     inbuf[];
	extern int      fd;
	extern char	remote_prob;
	extern char	dad_flag;

	if (argc != 1) {
		Usage (argv[0]);
		return (NOTOK);
	}

	sprintf (str, "%s %s",
			 _isodefile (isodebinpath, "editentry"), fname);

	if (!frompipe)
		return (system (str) ? NOTOK : OK);

	if (!dad_flag) {
		sprintf (prog, "e%s\n", str);

		send_pipe_aux (prog);

		if ((res = read_pipe_aux (prog,sizeof prog)) < 1) {
			fprintf (stderr, "read failure\n");
			remote_prob = TRUE;
			return (NOTOK);
		} else {
			if ((res == 1) && (*prog == 'e')) {
				remote_prob = FALSE;
				return (NOTOK);	 /* remote error - abandon ! */
			}
			if (*fname != '/') {
				char            tempbuf[LINESIZE];

				/* relative path... prefix cwd */
				*(prog + res) = 0;
				sprintf (tempbuf, "%s/%s", prog, fname);
				strcpy (fname, tempbuf);
			}
		}
	} else {
#ifndef	SOCKETS
		ps_printf (OPT,
				   "operation not allowed when using directory assistance server!\n");
		return NOTOK;
#else
		int	cc, i, j;
		char *cp, *dp;
		FILE *fp;
		struct stat st;


		if ((fp = fopen (fname, "r+")) == NULL) {
			ps_printf (OPT, "unable to open %s for rw: %s\n",
					   fname, sys_errname (errno));
			return NOTOK;
		}
		if (fstat (fileno (fp), &st) == NOTOK
				|| (st.st_mode & S_IFMT) != S_IFREG
				|| (cc = st.st_size) == 0) {
			ps_printf (OPT, "%s: not a regular file\n", fname);
out:
			;
			fclose (fp);
			return NOTOK;
		}

		sprintf (prog, "e%d\n", cc);
		send_pipe_aux (prog);

		if ((res = read_pipe_aux (prog, sizeof prog)) < 1) {
			fprintf (stderr, "read failure\n");
			remote_prob = TRUE;
			goto out;
		} else if ((res == 1) && (*prog == 'e')) {
			remote_prob = FALSE;
			goto out;
		}

		if ((cp = malloc ((unsigned) (cc))) == NULL) {
			ps_printf (OPT, "out of memory\n");
			goto out;
		}
		for (dp = cp, j = cc; j > 0; dp += i, j -= i)
			switch (i = fread (dp, sizeof *dp, j, fp)) {
			case NOTOK:
				ps_printf (OPT, "error reading %s: %s\n",
						   fname, sys_errname (errno));
				goto out2;

			case OK:
				ps_printf (OPT, "premature eof reading %s\n",
						   fname);
out2:
				;
				free (cp);
				goto out;

			default:
				break;
			}

		send_pipe_aux2 (cp, cc);
		free (cp), cp = NULL;

		if ((res = read_pipe_aux2 (&cp, &cc)) < 1) {
			ps_printf (OPT, "read failure\n");
			remote_prob = TRUE;
			goto out;
		}
		if (res == 1) {
			if (*cp != 'e')
				ps_printf (OPT, "remote protocol error: %s\n",
						   cp);
			goto out;
		}

		fclose (fp);
		if ((fp = fopen (fname, "w")) == NULL) {
			ps_printf (OPT, "unable to re-open %s for writing: %s\n",
					   fname, sys_errname (errno));
			free (cp);
			return NOTOK;
		}

		if (fwrite (cp, sizeof *cp, cc, fp) == 0) {
			ps_printf (OPT, "error writing %s: %s\n",
					   fname, sys_errname (errno));
			goto out2;
		}

		free (cp);
		fclose (fp);
#endif
	}

	return (OK);
}


int
get_password (char *str, char *buffer) {

	char            prog[LINESIZE];
	int             res;
	extern char     inbuf[];
	extern int      fd;
	extern char	remote_prob;
	char * 		getpassword ();

	if (frompipe) {
		sprintf (prog, "p%s\n", str);

		send_pipe_aux (prog);

		if ((res = read_pipe_aux (prog,sizeof prog)) < 1) {
			fprintf (stderr, "read failure\n");
			remote_prob = TRUE;
			return;
		} else {
			*(prog+res) = 0;
			strcpy (buffer, prog + 1);
		}
	} else {
		sprintf (buffer,"Enter password for \"%s\": ",str);
		strcpy (buffer,getpassword (buffer));
	}
}

int
yesno (char *str) {
	char            prog[LINESIZE];
	extern char     inbuf[];
	extern int      fd;
	extern char	remote_prob;
	char * 		getpassword ();

	if (frompipe) {
		sprintf (prog, "y%s\n", str);

		send_pipe_aux (prog);

		if (read_pipe_aux (prog,sizeof prog) < 1) {
			fprintf (stderr, "read failure\n");
			remote_prob = TRUE;
			return FALSE;
		}
	} else {
		ps_printf (OPT,"%s",str);
		fgets (prog, sizeof prog, stdin);
	}

	switch (prog[0]) {
	case 'y':
		return OK;

	case 'n':
	default:
		return NOTOK;

	case 'N':
		return DONE;
	}
}
