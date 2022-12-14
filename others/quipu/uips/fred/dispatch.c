/* dispatch.c - fred dispatch */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/fred/RCS/dispatch.c,v 9.0 1992/06/16 12:44:30 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/others/quipu/uips/fred/RCS/dispatch.c,v 9.0 1992/06/16 12:44:30 isode Rel $
 *
 *
 * $Log: dispatch.c,v $
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
#include "fred.h"
#ifdef	BSD42
#include <sys/ioctl.h>
#endif
#include <sys/stat.h>

#define	UFN_ALL		0x03
#define	UFN_MASK	"\020\01approximate-matching\02full-wildcarding"

/*    DATA */

int	bflag = 1;
int	boundP = 0;
int	debug = 0;
int	fflag = 0;
int	kflag = 0;
int	mail = 0;
int	nametype = 2;
int	network = 0;
int	phone = 0;
int	query = 1;
int	readonly = 0;
int	soundex = 0;
int	timelimit = 300;
int	ufn_options = UFN_ALL;
int	verbose = 0;
int	watch = 0;

int	didbind = 0;
int	dontpage = 0;
int	rcmode = 00;
int	runcom = 0;
int	runsys = 0;

char   *manager = "internal";
char   *pager = "more";
char   *server = "internal";

char   *myarea = NULLCP;
char   *mydn = NULLCP;
char   *myhome = NULLCP;
char   *myuser = NULLCP;

extern char *isodeversion;

FILE   *stdfp = NULL;
FILE   *errfp = NULL;

#ifdef __GNUC__
__attribute__((constructor))
static void _init_fp ()
{
	stdfp = stdout;
}
#endif

/*  */

static int	f_set ();
int	f_help ();
int	f_alias (), f_area (), f_dish (), f_edit (), f_manual (), f_report (),
	f_thisis ();
int	f_bind (), f_quit ();
int	f_version (), f_whois ();


static struct dispatch dispatches[] = {
	"alias", f_alias, DS_SYOK,
	"report on active aliases",

	"area", f_area, DS_NULL,
	"set default area for searches",

	"dish", f_dish, DS_USER,
	"talk directly to dish (experts only!)",

	"edit", f_edit, DS_USER,
	"edit entry in the white pages",

	"help", f_help, DS_NULL,
	"print help information",

	"manual", f_manual, DS_NULL,
	"print detailed documentation",

	"quit", f_quit, DS_USER,
	"terminate fred",

	"report", f_report, DS_USER,
	"send report to white pages manager",

	"set", f_set, DS_SYOK,
	"display or change variables",

	"thisis", f_thisis, DS_USER,
	"identify self to the white pages",

	"whois", f_whois, DS_NULL,
	"find something in the white pages",

	"version", f_version, DS_NULL,
	"identify the version of the system",

	NULL
};

static struct dispatch *getds ();
static printvar ();
static	snarf ();

/*    DISPATCH */

int
fredloop (char **vec, int error) {
	struct dispatch *ds;

	if ((ds = getds (strcmp (*vec, "?") ? *vec : "help")) == NULL)
		return error;
	if (network) {
		if ((ds -> ds_flags & DS_USER)
				|| ((ds -> ds_flags & DS_SYOK) && !runsys)) {
			advise (NULLCP, "unavailable operation \"%s\"", *vec);
			return error;
		}
	} else
		vec[0] = ds -> ds_name;

	switch ((*ds -> ds_fnx) (vec)) {
	case NOTOK:
		return error;

	case OK:
	default:
		return OK;

	case DONE:
		return DONE;
	}
}

/*  */

static struct dispatch *
getds (char *name) {
	int    longest,
		   nmatches;
	char  *p,
		  *q;
	char    buffer[BUFSIZ];
	struct dispatch   *ds,
			   *fs;

	longest = nmatches = 0;
	for (ds = dispatches; p = ds -> ds_name; ds++) {
		for (q = name; *q == *p++; q++)
			if (*q == NULL)
				return ds;

		if (*q == NULL)
			if (q - name > longest) {
				longest = q - name;
				nmatches = 1;
				fs = ds;
			} else if (q - name == longest)
				nmatches++;
	}

	switch (nmatches) {
	case 0:
		if (*(p = name) == '!')
			p++;
		for (; *p; p++)
			if (!isdigit (*p))
				break;
		if (!*p || network)
			for (ds = dispatches; ds -> ds_name; ds++)
				if (strcmp (ds -> ds_name, "whois") == 0)
					return ds;

		advise (NULLCP, "unknown operation \"%s\"", name);
		return NULL;

	case 1:
		return fs;

	default:
		for (ds = dispatches, p = buffer; q = ds -> ds_name; ds++)
			if (strncmp (q, name, longest) == 0) {
				sprintf (p, "%s \"%s\"", p != buffer ? "," : "", q);
				p += strlen (p);
			}
		advise (NULLCP, "ambiguous operation, it could be one of:%s",
				buffer);
		return NULL;
	}
}

/*    VARIABLES */

static char *bool[] = {
	"off", "on", NULL
};

static char *names[] = {
	"fullname", "surname", "friendly", NULL
};

static char *ufnoptions[] = {
	"none", "approx", "wild", NULL
};


struct var {
	char   *v_name;
	IP	    v_value;

	char   *v_dname;
	char  **v_dvalue;
	char   *v_mask;

	IFP	    v_hook;

	int	    v_flags;
#define	V_NULL		0x00
#define	V_RDONLY	0x01
#define	V_SERVER	0x02
};

static struct var *getvar ();


static struct var vars[] = {
	"bell", &bflag, "ring bell at end of screen", bool,
	NULLCP, NULLIFP, V_NULL,

	"debug", &debug, "debug FRED", bool,
	NULLCP, NULLIFP, V_NULL,

	"manager", NULLIP, "mail-address of local white pages manager", &manager,
	NULLCP, NULLIFP, V_RDONLY,

	"namesearch", &nametype, "type of name used for matching", names,
	NULLCP, NULLIFP, V_NULL,

	"pager", NULLIP, "program to use for output pagination", &pager,
	NULLCP, NULLIFP, V_RDONLY,

	"phone", &phone, "display phone numbers in one-liner", bool,
	NULLCP, NULLIFP, V_NULL,

	"query", &query, "confirm two-step operations", bool,
	NULLCP, NULLIFP, V_NULL,

	"server", NULLIP, "IP-address of directory assistance server", &server,
	NULLCP, NULLIFP, V_RDONLY | V_SERVER,

	"soundex", &soundex, "use soundex for matching", bool,
	NULLCP, NULLIFP, V_NULL,

	"timelimit", &timelimit, "maximum time (in seconds) for matching", NULLVP,
	NULLCP, NULLIFP, V_NULL,

	"ufn", &ufn_options, "UFN customization options", ufnoptions,
	UFN_MASK, NULLIFP, V_NULL,

	"verbose", &verbose, "verbose interaction", bool,
	NULLCP, NULLIFP, V_NULL,

	"watch", &watch, "watch dialogue with dish", bool,
	NULLCP, NULLIFP, V_NULL,

	NULL
};


static int varwidth1;
static int varwidth2;

static char    **getval ();

/*  */

static int
f_set (char **vec) {
	int    i,
		   j;
	int     value,
			vflag;
	char **cp,
		 *dp;
	struct var *v;

	if (*++vec == NULL) {
		int    w;
		int     columns,
				width,
				lines;
		struct var *u;

		for (u = vars; u -> v_name; u++)
			continue;
		width = varwidth1;

		if ((columns = ncols (stdout) / (width = (width + 8) & ~7)) == 0)
			columns = 1;
		lines = ((u - vars) + columns - 1) / columns;

		printf ("Variables:\n");
		for (i = 0; i < lines; i++)
			for (j = 0; j < columns; j++) {
				v = vars + j * lines + i;
				printf ("%s", v -> v_name);
				if (v + lines >= u) {
					printf ("\n");
					break;
				}
				for (w = strlen (v -> v_name); w < width; w = (w + 8) & ~7)
					putchar ('\t');
			}
		/*	 printf ("\nversion: %s\n", isodeversion); */
		return OK;
	}

	if (strcmp (*vec, "-help") == 0) {
		fprintf (stdfp, "set [variable [value]]\n");
		fprintf (stdfp,
				 "    with no arguments, lists variables which may be set\n");
		fprintf (stdfp,
				 "    with one argument, lists the value of the named variable\n");
		fprintf (stdfp,
				 "    with two arguments, sets the given variable accordingly\n");

		return OK;
	}

	if (strcmp (*vec, "?") == 0) {
		for (v = vars; v -> v_name; v++)
			printvar (v);

		return OK;
	}

	if ((v = getvar (*vec)) == NULL)
		return OK;

	if (*++vec == NULL) {
		printvar (v);

		return OK;
	}

	if (strcmp (*vec, "?") == 0) {
		if (v -> v_value && (cp = v -> v_dvalue)) {
			printf ("use %s of:", v -> v_mask ? "any" : "one");
			for (i = 0; *cp; cp++)
				printf ("%s \"%s\"", i++ ? "," : "", *cp);
			if (v -> v_mask)
				printf (";\n\tor  \"all\";\n\tor a hexadecimal number from 0 to 0x%x\n",
						(1 << (i - 1)) - 1);
			else
				printf (";\n\tor a number from 0 to %d\n",
						cp - v -> v_dvalue - 1);
		} else
			printf ("use any %s value\n",
					v -> v_value ? "integer" : "string");

		return OK;
	}

	if (!runsys && readonly && (v -> v_flags & V_RDONLY)) {
		advise (NULLCP, "variable is read-only");
		return OK;
	}

	if (boundP && (v -> v_flags & V_SERVER)) {
		advise (NULLCP, "too late!");
		return OK;
	}

	if (v -> v_value == NULLIP) {
		int    w;

		if (*v -> v_dvalue)
			free (*v -> v_dvalue);
		*v -> v_dvalue = strdup (*vec);
		if ((w = strlen (*v -> v_dvalue) + 2) > varwidth2)
			varwidth2 = w;
		if (v -> v_hook)
			(*v -> v_hook) (v);
		if (verbose)
			printvar (v);
		return OK;
	}

	if (v -> v_mask) {
		if (strcmp (dp = *vec, "all") == 0 && (cp = v -> v_dvalue)) {
			i = 1;
			while (*++cp)
				i <<= 1;
			value = i - 1;
			j = 1;
		} else {
			if (strncmp (dp, "0x", 2) == 0)
				dp += 2;
			for (j = sscanf (dp, "%x", &value); *dp; dp++)
				if (!isxdigit (*dp)) {
					j = 0;
					break;
				}
		}
	} else
		j = sscanf (*vec, "%d", &value);

	if (j == 1) {
		if (cp = v -> v_dvalue) {
			if (v -> v_mask) {
				i = 1;
				while (*++cp)
					i <<= 1;
				if (value >= i)
					goto out_of_range;
			} else {
				for (; *cp; cp++)
					continue;
				if (value >= cp - v -> v_dvalue) {
out_of_range:
					;
					advise (NULLCP, "value out of range \"%s\"", *vec);

					return OK;
				}
			}
		}

		vflag = verbose;
		*v -> v_value = value;
		if (v -> v_hook)
			(*v -> v_hook) (v);
		if (vflag)
			printvar (v);

		return OK;
	}

	if (v -> v_mask) {
		i = 0;
		for (; *vec; vec++) {
			if (!(cp = getval (*vec, v -> v_dvalue))) {
				advise (NULLCP, "bad value \"%s\"", *vec);

				return OK;
			}
			if ((j = cp - v -> v_dvalue) <= 0)
				continue;

			i |= 1 << (j - 1);
		}

		vflag = verbose;
		*v -> v_value = i;
		if (v -> v_hook)
			(*v -> v_hook) (v);
		if (vflag)
			printvar (v);

		return OK;
	}

	if (v -> v_dvalue && (cp = getval (*vec, v -> v_dvalue))) {
		vflag = verbose;
		*v -> v_value = cp - v -> v_dvalue;
		if (v -> v_hook)
			(*v -> v_hook) (v);
		if (vflag)
			printvar (v);
	} else if (!v -> v_dvalue)
		advise (NULLCP, "bad value \"%s\"", *vec);

	return OK;
}

/*  */

static
printvar (struct var *v) {
	int	    i;
	char    buffer[BUFSIZ];

	if (runcom)
		return;

	printf ("%-*s = ", varwidth1, v -> v_name);
	if (v -> v_value) {
		i = *v -> v_value;

		if (v -> v_mask) {
			if (v -> v_dvalue) {
				if (i == 0)
					printf ("%-*s", varwidth2, v -> v_dvalue[i]);
				else {
					strcpy (buffer, sprintb (i, v -> v_mask));
					if ((int)strlen (buffer) <= varwidth2)
						printf ("%-*s", varwidth2, buffer);
					else
						printf ("%s\n%*s", buffer, varwidth1 + varwidth2 + 3,
								"");
				}
			} else
				printf ("0x%-*x", varwidth2 - 2, i);
		} else {
			if (v -> v_dvalue)
				printf ("%-*s", varwidth2, v -> v_dvalue[i]);
			else
				printf ("%-*d", varwidth2, i);
		}
	} else if (*v -> v_dvalue) {
		sprintf (buffer, "\"%s\"", *v -> v_dvalue);
		printf ("%-*s", varwidth2, buffer);
	}
	printf ("    - %s\n", v -> v_dname);
}

/*  */

static char **
getval (char *name, char **choices) {
	int    longest,
		   nmatches;
	char  *p,
		  *q,
		  **cp,
		  **fp;
	char    buffer[BUFSIZ];

	longest = nmatches = 0;
	for (cp = choices; p = *cp; cp++) {
		for (q = name; *q == *p++; q++)
			if (*q == NULL)
				return cp;
		if (*q == NULL)
			if (q - name > longest) {
				longest = q - name;
				nmatches = 1;
				fp = cp;
			} else if (q - name == longest)
				nmatches++;
	}

	switch (nmatches) {
	case 0:
		advise (NULLCP, "unknown value \"%s\"", name);
		return NULL;

	case 1:
		return fp;

	default:
		for (cp = choices, p = buffer; q = *cp; cp++)
			if (strncmp (q, name, longest) == 0) {
				sprintf (p, "%s \"%s\"", p != buffer ? "," : "", q);
				p += strlen (p);
			}
		advise (NULLCP, "ambiguous value, it could be one of:%s",
				buffer);
		return NULL;
	}
}

/*  */

static char *ignore[] = {
	"level", "listings", "verify", NULL
};


static struct var *
getvar (char *name) {
	int    longest,
		   nmatches;
	char  *p,
		  *q,
		  **ip;
	char    buffer[BUFSIZ];
	struct var *v,
			   *f;

	if (runcom)
		for (ip = ignore; *ip; ip++)
			if (lexequ (*ip, name) == 0)
				return NULL;

	longest = nmatches = 0;
	for (v = vars; p = v -> v_name; v++) {
		for (q = name; *q == *p++; q++)
			if (*q == NULL)
				return v;
		if (*q == NULL)
			if (q - name > longest) {
				longest = q - name;
				nmatches = 1;
				f = v;
			} else if (q - name == longest)
				nmatches++;
	}

	switch (nmatches) {
	case 0:
		advise (NULLCP, "unknown variable \"%s\"", name);
		return NULL;

	case 1:
		return f;

	default:
		for (v = vars, p = buffer; q = v -> v_name; v++)
			if (strncmp (q, name, longest) == 0) {
				sprintf (p, "%s \"%s\"", p != buffer ? "," : "", q);
				p += strlen (p);
			}
		advise (NULLCP, "ambiguous variable, it could be one of:%s",
				buffer);
		return NULL;
	}
}

/*    HELP */

static int helpwidth;

/*  */

int
f_help (char **vec) {
	int    i,
		   j,
		   w;
	int     columns,
			width,
			lines;
	struct dispatch   *ds,
			   *es;

	if (network || vec == NULL) {
		char **ap;

		for (ap = whois_help; *ap; ap++)
			fprintf (stdfp, "%s%s", *ap, EOLN);

		return OK;
	}

	for (es = dispatches; es -> ds_name; es++)
		continue;
	width = helpwidth;

	if (*++vec == NULL) {
		if ((columns = ncols (stdout) / (width = (width + 8) & ~7)) == 0)
			columns = 1;
		lines = ((es - dispatches) + columns - 1) / columns;

		printf ("Operations:\n");
		for (i = 0; i < lines; i++)
			for (j = 0; j < columns; j++) {
				ds = dispatches + j * lines + i;
				printf ("%s", ds -> ds_name);
				if (ds + lines >= es) {
					printf ("\n");
					break;
				}
				for (w = strlen (ds -> ds_name); w < width; w = (w + 8) & ~7)
					putchar ('\t');
			}
		/*	 printf ("\nversion: %s\n", isodeversion); */

		return OK;
	}

	if (strcmp (*vec, "-help") == 0) {
		fprintf (stdfp, "help [commands ...]\n");
		fprintf (stdfp,
				 "    with no arguments, lists operations which may be invoked\n");
		fprintf (stdfp,
				 "    otherwise prints help for each operation given\n");

		return OK;
	}

	for (; *vec; vec++)
		if (strcmp (*vec, "?") == 0) {
			for (ds = dispatches; ds -> ds_name; ds++)
				printf ("%-*s\t- %s\n", width, ds -> ds_name, ds -> ds_help);

			break;
		} else if (ds = getds (*vec))
			printf ("%-*s\t- %s\n", width, ds -> ds_name, ds -> ds_help);

	return OK;
}

/*    MISCELLANY */

int
rcinit () {
	int    w;
	char **cp,
	*dp;
	char    buffer[BUFSIZ];
	struct dispatch   *ds;
	struct var *v;

	if (fflag)
		return;

	if ((myhome = getenv ("HOME")) == NULL)
		myhome = ".";		/* could do passwd search... */

	if ((myuser = getenv ("USER")) == NULLCP)
		myuser = getenv ("LOGNAME");

	if (dp = getenv ("QUIPURC"))
		strcpy (buffer, dp);
	else
		sprintf (buffer, "%s/.quipurc", myhome);
	snarf (buffer, "username:", &mydn);

	for (ds = dispatches, helpwidth = 0; ds -> ds_name; ds++)
		if ((w = strlen (ds -> ds_name)) > helpwidth)
			helpwidth = w;

	for (v = vars, varwidth1 = 0; v -> v_name; v++) {
		if ((w = strlen (v -> v_name)) > varwidth1)
			varwidth1 = w;

		if (v -> v_value) {
			if (cp = v -> v_dvalue) {
				if (v -> v_mask) {
#ifdef	notdef
					w = 1;
					while (*++cp)
						w <<= 1;
					w--;
					if ((w = strlen (sprintb (w, v -> v_mask))) > varwidth2)
						varwidth2 = w;
#endif
				} else
					for (; *cp; cp++)
						if ((w = strlen (*cp)) > varwidth2)
							varwidth2 = w;
			}
		} else if (*v -> v_dvalue) {
			*v -> v_dvalue = strdup (*v -> v_dvalue);
			if ((w = strlen (*v -> v_dvalue) + 2) > varwidth2)
				varwidth2 = w;
		}
	}
}

/*  */

static
snarf (char *file, char *name, char **variable) {
	int	    i;
	char   *bp,
		   *dp,
		   *ep;
	char    buffer[BUFSIZ];
	FILE   *fp;

	if (fp = fopen (file, "r")) {
		while (fgets (bp = buffer, sizeof buffer, fp)) {
			if (*bp == '#' || *bp == '\n')
				continue;
			if (bp = index (buffer, '\n'))
				*bp = NULL;
			if (lexnequ (buffer, name, strlen (name)))
				continue;

			bp = buffer + strlen (name);
			while (isspace (*bp))
				bp++;

			if (*bp == '"') {
				if (*(dp = bp + strlen (bp) - 1) == '"')
					bp++, *dp = NULL;
				goto set_variable;
			}

			i = 0;
			for (dp = ep = bp; *dp; ) {
				if (isspace (*dp)) {
					*ep = ' ';
					while (isspace (*++dp))
						i = 1;
					if (*dp)
						ep++;
					else
						break;
				}
				if (i == 1)
					*ep = *dp;

				dp++, ep++;
			}
			*ep = NULL;

set_variable:
			;
			if (*variable)
				free (*variable);
			*variable = strdup (bp);
			break;
		}

		fclose (fp);
	}
}

/*  */

int
rcfile (char *file, int op, int isystem) {
	char *cp;
	char    buffer[BUFSIZ + 1],
			*vec[NVEC + 1];
	FILE *fp;
	struct stat st;

	if ((fp = fopen (file, "r")) == NULL)
		return;

	runcom = 1, runsys = isystem;
	if (fstat (fileno (fp), &st) == NOTOK)
		adios (file, "unable to fstat");
	rcmode = st.st_mode & 0777;

	while (fgets (buffer, sizeof buffer, fp)) {
		if (*buffer == '#')
			continue;
		if (cp = index (buffer, '\n'))
			*cp = NULL;

		bzero ((char *) vec, sizeof vec);
		if (str2vecY (buffer, vec) < 1)
			continue;

		if (fredloop (vec, NOTOK) != OK && op) {
			f_quit (NULLVP);
			exit (1);
		}
	}

	runcom = runsys = 0;

	fclose (fp);
}

/*  */

#ifndef	TIOCGWINSZ
/* ARGSUSED */
#endif

int	ncols (fp)
FILE *fp;
{
#ifdef	TIOCGWINSZ
	int	    i;
	struct winsize ws;

	if (ioctl (fileno (fp), TIOCGWINSZ, (char *) &ws) != NOTOK
			&& (i = ws.ws_col) > 0)
		return i;
#endif

	return 80;
}


int
f_version (char **vec) {
	if (*++vec != NULL)
		if (strcmp (*vec, "-fred") == 0) {
			printf ("version: %s\n", isodeversion);
			return OK;
		}

	printf ("version: %s\n\nUsing server version:\n", isodeversion);
	return dish ("squid -version", 0);
}
