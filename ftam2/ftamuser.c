/* ftamuser.c - FTAM initiator routines */

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include "ftamuser.h"
#include "tailor.h"
#if	defined(SYS5) && !defined(HPUX)
#include <sys/times.h>
#define	TMS
#endif
#ifdef	BSD42
#include <sys/ioctl.h>
#endif

int   ftamfd = NOTOK;

char *host = NULLCP;
char *user = NULLCP;
char *account = NULLCP;
#ifndef	BRIDGE
char *userdn = NULLCP;
char *storename = NULLCP;
#endif

int   bell = 0;
int   concurrency = 1;
#ifndef	BRIDGE
int   debug = 0;
#endif
#ifdef	BRIDGE
int   globbing = 1;
#else
int   globbing = 0;
#endif
int   hash = 0;
int   marks = 0;
int   omode = FOVER_WRITE;
int   query = 1;
int   runcom = 0;
#ifdef	BRIDGE
int   tmode = VFS_UTF;
#else
int   tmode = VFS_DEF;
#endif
int   trace = 0;
#ifndef	BRIDGE
int   verbose = 0;
int   watch = 0;
#endif

char *myuser = NULLCP;
char *myhome = NULLCP;

#ifdef	BRIDGE
int realstore = RFS_UNIX;
#else
int realstore = 0;
#endif

char *rs_unknown =
	"type of remote realstore is unknown; use \"set realstore\"";
char *rs_support = "operation not supported on remote realstore";

char *rcwd = NULL;

#ifdef	BRIDGE
int	ftp_default = VFS_UTF;
int	ftp_directory;
char	ftam_error[BUFSIZ];
#endif

struct QOStype myqos;

/* DISPATCH */

static int f_set (char **vec);
static char ** getval (char *name, char **choices);
static int f_help (char **vec);
static void tvsub (struct timeval *tdiff, struct timeval *t1, struct timeval *t0);

struct var {
	char   *v_name;
	int	   *v_value;

	char   *v_dname;
	char  **v_dvalue;
	char   *v_mask;

	void (*v_hook)(struct var *v);
};

static struct var * getvar (char *name);
static void printvar (struct var *v);

#ifndef	BRIDGE
static struct dispatch  dispatches[] = {
	"append", f_put, DS_OPEN | DS_MODES, FCLASS_TRANSFER, FUNIT_WRITE,
	"append to a file in the virtual filestore",

	"cd", f_cd, DS_OPEN, 0, 0,
	"change working directory on virtual filestore",

	"chgrp", f_chgrp, DS_OPEN | DS_MODES, FCLASS_MANAGE, FUNIT_ENHANCED,
	"change group of a file",

	"close", f_close, DS_OPEN, 0, 0,
	"terminate association with virtual filestore",

	"dir", f_ls, DS_OPEN | DS_MODES, FCLASS_MANAGE, FUNIT_LIMITED,
	"print long directory listing",

	"echo", f_echo, DS_OPEN | DS_MODES, FCLASS_MANAGE, FUNIT_LIMITED,
	"echo globbed filenames",

	"fdir", f_fls, DS_OPEN | DS_MODES, FCLASS_MANAGE, FUNIT_LIMITED,
	"print long directory listing to a file/program",

	"fls", f_fls, DS_OPEN | DS_MODES, FCLASS_MANAGE, FUNIT_LIMITED,
	"print directory listing to a file/program",

	"get", f_get, DS_OPEN | DS_MODES, FCLASS_TRANSFER, FUNIT_READ,
	"retrieve file",

	"help", f_help, DS_NULL, 0, 0,
	"print help information",

	"lcd", f_lcd, DS_NULL, 0, 0,
	"change working directory on local system",

	"ls", f_ls, DS_OPEN | DS_MODES, FCLASS_MANAGE, FUNIT_LIMITED,
	"print directory listing",

	"mkdir", f_mkdir, DS_OPEN | DS_MODES, FCLASS_MANAGE, FUNIT_LIMITED,
	"create directory",

	"mv", f_mv, DS_OPEN | DS_MODES, FCLASS_MANAGE, FUNIT_ENHANCED,
	"rename file",

	"open", f_open, DS_CLOSE, 0, 0,
	"associate with virtual filestore",

	"put", f_put, DS_OPEN | DS_MODES, FCLASS_TRANSFER, FUNIT_WRITE,
	"store file",

	"pwd", f_pwd, DS_NULL, 0, 0,
	"print working directories",

	"quit", f_quit, DS_NULL, 0, 0,
	"terminate association with virtual filestore and exit",

	"rm", f_rm, DS_OPEN | DS_MODES, FCLASS_MANAGE, FUNIT_LIMITED,
	"delete file",

	"set", f_set, DS_NULL, 0, 0,
	"display or change variables",

	"status", f_status, DS_OPEN, 0, 0,
	"show current status",

	NULL
};

struct dispatch *getds (char *name) {
	int    longest,
		   nmatches;
	int	    d;
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
		if (*q == NULL) {
			if (ptrdiff2int (q - name, &d) != 0)
				continue;
			if (d > longest) {
				longest = d;
				nmatches = 1;
				fs = ds;
			} else if (d == longest)
				nmatches++;
		}
	}

	switch (nmatches) {
	case 0:
		advise (NULLCP, "unknown operation \"%s\"", name);
		return NULL;

	case 1:
		return fs;

	default:
		for (ds = dispatches, p = buffer; q = ds -> ds_name; ds++)
			if (strncmp_int (q, name, longest) == 0) {
				sprintf (p, "%s \"%s\"", p != buffer ? "," : "", q);
				p += strlen (p);
			}
		advise (NULLCP, "ambiguous operation, it could be one of:%s",
				buffer);
		return NULL;
	}
}
#endif

/* VARIABLES */

#ifndef	BRIDGE
static char *bool[] = {
	"off", "on", NULL
};

static char *hmodes[] = {
	"off", "on", "total", NULL
};

static char *omodes[] = {
	"fail", "select", "write", "delete", NULL
};

static char *tmodes[] = {
	"default", "binary", "text", NULL
};

static char *realstores[] = {
	"unknown", "unix", NULL
};

static char *xsaplevels[] = {
	"none", "fatal", "exceptions", "notice", "pdus", "trace", "debug", NULL
};

static char *sversions[] = {
	"default", "v1", "v2", NULL
};

static void set_prompt (struct var *v);
static void set_realstore (struct var *v);
static void set_trace (struct var *v);
static void set_type (struct var *v);

/* prompt stuff */
# define DEFAULT_PROMPT 	"%s> "
# define DEFAULT_PROMPT_STR	"default"
char 	*command_prompt = DEFAULT_PROMPT;
char	*var_prompt = DEFAULT_PROMPT_STR;

static struct var vars[] = {
	"acsaplevel", &_acsap_log.ll_events, "ACSAP logging", xsaplevels,
	LLOG_MASK, NULL,
	"acsapfile", NULL, "ACSAP trace file", &_acsap_log.ll_file, NULLCP,
	NULL,

	"addrlevel", &_addr_log.ll_events, "address logging", xsaplevels,
	LLOG_MASK, NULL,
	"addrfile", NULL, "address trace file", &_addr_log.ll_file, NULLCP,
	NULL,

	"bell", &bell, "ring the bell when a command finishes", bool, NULLCP,
	NULL,

	"compatlevel", &_compat_log.ll_events, "COMPAT logging", xsaplevels,
	LLOG_MASK, NULL,
	"compatfile", NULL, "COMPAT trace file", &_compat_log.ll_file, NULLCP,
	NULL,

	"concurrency", &concurrency,	/* Olivier Dubois */
	"request concurrency control for transfers", bool, NULLCP, NULL,

	"debug", &debug, "debug FTAM", bool, NULLCP, NULL,

	"glob", &globbing, "expand metacharacters like the shell", bool, NULLCP,
	NULL,

	"hash", &hash, "hash mark printing", hmodes, NULLCP, NULL,

	"override", &omode, "creation override mode", omodes, NULLCP, NULL,

	"prompt", NULL, "command prompt string", &var_prompt, NULLCP,
	set_prompt,

	"psaplevel", &_psap_log.ll_events, "PSAP logging", xsaplevels,
	LLOG_MASK, NULL,
	"psapfile", NULL, "PSAP trace file", &_psap_log.ll_file, NULLCP,
	NULL,

	"psap2level", &_psap2_log.ll_events, "PSAP2 logging", xsaplevels,
	LLOG_MASK, NULL,
	"psap2file", NULL, "PSAP2 trace file", &_psap2_log.ll_file, NULLCP,
	NULL,

	"qualifier", NULL, "service qualifier", &storename, NULLCP, NULL,

	"query", &query, "confirm operations on globbing", bool, NULLCP, NULL,

	"realstore", &realstore, "type of remote realstore", realstores, NULLCP,
	set_realstore,

	"ssaplevel", &_ssap_log.ll_events, "SSAP logging", xsaplevels,
	LLOG_MASK, NULL,
	"ssapfile", NULL, "SSAP trace file", &_ssap_log.ll_file, NULLCP,
	NULL,

	"sversion", &myqos.qos_sversion, "session version number", sversions,
	NULLCP, NULL,

	"trace", &trace, "trace FPDUs", bool, NULLCP, set_trace,
	"tracefile", NULL, "FTAM trace file", &_ftam_log.ll_file, NULLCP,
	NULL,

	"tsaplevel", &_tsap_log.ll_events, "TSAP logging", xsaplevels,
	LLOG_MASK, NULL,
	"tsapfile", NULL, "TSAP trace file", &_tsap_log.ll_file, NULLCP,
	NULL,

	"type", &tmode, "file transfer mode", tmodes, NULLCP, set_type,

	"verbose", &verbose, "verbose interaction", bool, NULLCP, NULL,

	"userdn", NULL, "DN to use when binding for AE-lookup", &userdn, NULLCP,
	NULL,

	"watch", &watch, "watch transfers", bool, NULLCP, NULL,

	NULL
};

static int varwidth1;
static int varwidth2;

static int f_set (char **vec) {
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
		{
			int nvars;

			if (ptrdiff2int (u - vars, &nvars) != 0)
				return NOTOK;
			lines = (nvars + columns - 1) / columns;
		}

		printf ("Variables:\n");
		for (i = 0; i < lines; i++)
			for (j = 0; j < columns; j++) {
				v = vars + j * lines + i;
				printf ("%s", v -> v_name);
				if (v + lines >= u) {
					printf ("\n");
					break;
				}
				if (strlen2int (v -> v_name, &w) != 0) {
					advise (NULLCP, "strlen2int failed");
					return NOTOK;
				}
				for (; w < width; w = (w + 8) & ~7)
					putchar ('\t');
			}

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

	if (v -> v_value == NULL) {
		int    w;

		if (*v -> v_dvalue)
			free (*v -> v_dvalue);
		*v -> v_dvalue = strdup (*vec);
		if (strlen2int (*v -> v_dvalue, &w) == 0
				&& add_int_to_int (&w, 2) == 0
				&& w > varwidth2)
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
			if (dp[0] == '0' && dp[1] == 'x')
				dp += 2;
			for (j = sscanf (dp, "%x", &value); *dp; dp++)
				if (!isxdigit ((uint8_t) *dp)) {
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
			if (ptrdiff2int (cp - v -> v_dvalue, &j) != 0 || j <= 0)
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
		if (ptrdiff2int (cp - v -> v_dvalue, v -> v_value) != 0) {
			advise (NULLCP, "value index out of range");
			return OK;
		}
		if (v -> v_hook)
			(*v -> v_hook) (v);
		if (vflag)
			printvar (v);
	} else if (!v -> v_dvalue)
		advise (NULLCP, "bad value \"%s\"", *vec);

	return OK;
}

static void printvar (struct var *v) {
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
					{
						int blen;

						if (strlen2int (buffer, &blen) != 0
								|| blen > varwidth2)
							printf ("%s\n%*s", buffer, varwidth1 + varwidth2 + 3,
									"");
						else
							printf ("%-*s", varwidth2, buffer);
					}
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

char *default_prompt (void) {
	return(DEFAULT_PROMPT);
}

static void set_prompt (struct var *v) {
	char	*new = *(v->v_dvalue);
	if(!new || !lexequ(new, DEFAULT_PROMPT_STR)) {
		command_prompt = default_prompt();
	} else {
		command_prompt = new;
	}
}

static void set_realstore (struct var *v) {
	char   *vec[2];
	if (ftamfd != NOTOK) {
		vec[0] = "sd";
		vec[1] = NULLCP;

		f_cd (vec);
	}
}

static void set_trace (struct var *v) {
	struct FTAMindication   ftis;
	struct FTAMindication *fti = &ftis;

	if (ftamfd == NOTOK)
		return;

	if (FHookRequest (ftamfd, trace ? FTraceHook : NULL, fti) == NOTOK)
		ftam_advise (&fti -> fti_abort, "F-HOOK.REQUEST");
}

static void set_type (struct var *v) {
	struct vfsmap *vf;

	if (ftamfd == NOTOK)
		return;

	if ((vf = &vfs[tmode]) != &vfs[VFS_DEF]
			&& (vf -> vf_oid == NULLOID || !(vf -> vf_flags & VF_OK))) {
		advise (NULLCP, "negotiation prevents transfer of %ss",
				vf -> vf_text);

		tmode = VFS_DEF;
	}
}

static char ** getval (char *name, char **choices) {
	int    longest,
		   nmatches;
	int	    d;
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
		if (*q == NULL) {
			if (ptrdiff2int (q - name, &d) != 0)
				continue;
			if (d > longest) {
				longest = d;
				nmatches = 1;
				fp = cp;
			} else if (d == longest)
				nmatches++;
		}
	}

	switch (nmatches) {
	case 0:
		advise (NULLCP, "unknown value \"%s\"", name);
		return NULL;

	case 1:
		return fp;

	default:
		for (cp = choices, p = buffer; q = *cp; cp++)
			if (strncmp_int (q, name, longest) == 0) {
				sprintf (p, "%s \"%s\"", p != buffer ? "," : "", q);
				p += strlen (p);
			}
		advise (NULLCP, "ambiguous value, it could be one of:%s",
				buffer);
		return NULL;
	}
}

static struct var * getvar (char *name) {
	int    longest,
		   nmatches;
	int	    d;
	char  *p,
		  *q;
	char    buffer[BUFSIZ];
	struct var *v,
			   *f;

	longest = nmatches = 0;
	for (v = vars; p = v -> v_name; v++) {
		for (q = name; *q == *p++; q++)
			if (*q == NULL)
				return v;
		if (*q == NULL) {
			if (ptrdiff2int (q - name, &d) != 0)
				continue;
			if (d > longest) {
				longest = d;
				nmatches = 1;
				f = v;
			} else if (d == longest)
				nmatches++;
		}
	}

	switch (nmatches) {
	case 0:
		advise (NULLCP, "unknown variable \"%s\"", name);
		return NULL;

	case 1:
		return f;

	default:
		for (v = vars, p = buffer; q = v -> v_name; v++)
			if (strncmp_int (q, name, longest) == 0) {
				sprintf (p, "%s \"%s\"", p != buffer ? "," : "", q);
				p += strlen (p);
			}
		advise (NULLCP, "ambiguous variable, it could be one of:%s",
				buffer);
		return NULL;
	}
}

/* HELP */

static int helpwidth;

static int f_help (char **vec) {
	int    i,
		   j,
		   w;
	int     columns,
			width,
			lines;
	struct dispatch   *ds,
			   *es;

	for (es = dispatches; es -> ds_name; es++)
		continue;
	width = helpwidth;

	if (*++vec == NULL) {
		if ((columns = ncols (stdout) / (width = (width + 8) & ~7)) == 0)
			columns = 1;
		{
			int nops;

			if (ptrdiff2int (es - dispatches, &nops) != 0)
				return NOTOK;
			lines = (nops + columns - 1) / columns;
		}

		printf ("Operations:\n");
		for (i = 0; i < lines; i++)
			for (j = 0; j < columns; j++) {
				ds = dispatches + j * lines + i;
				printf ("%s", ds -> ds_name);
				if (ds + lines >= es) {
					printf ("\n");
					break;
				}
				if (strlen2int (ds -> ds_name, &w) == 0)
					for (; w < width; w = (w + 8) & ~7)
						putchar ('\t');
			}

		printf ("\nversion info:\t%s\n\t\t%s\n", ftamversion, isodeversion);

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
#endif

/* FTAM */

/* When going from an FADU to an SSDU via FTAM, we are talking about:

	octets = 2 + <number of FADUs>*12 + <size of each FADU>

   in the best case, and probably

	octets = 3 + <number of FADUs>*16 + <size of each FADU>

   on the average.

   On a Berkeley UNIX system we typically see a blksize of 8192 octets.

   When deciding how to read from the filesystem when writing to the network,
   for the file's FADU size, we prefer to use the integral FADU size,
   unless the blksize is larger.  This works well on LANs.
 */

OID	context;
int	fqos;
int	ftam_class = FCLASS_TRANSFER | FCLASS_MANAGE | FCLASS_TM;
int	units;
int	attrs;
int	fadusize;

struct vfsmap vfs[] = {
	/* VFS_DEF */
	"default", NULLOID, NULLCP, VF_NULL, 0, 0, NULL, ' ', VFS_XXX,
	0,
	0, NULL,
	-1,
	NULLCP,

	/* VFS_UBF */
	"FTAM-3", NULLOID, NULLCP, VF_WARN, 0, S_IFREG, binarypeek, 'b', VFS_XXX,
	FA_ACC_UA,
	1, binarycheck,
	_ZFTAM_3_ParametersDOCS,
	"unstructured binary file",

	/* VFS_UTF */
	"FTAM-1", NULLOID, NULLCP, VF_WARN, 0, S_IFREG, textpeek, 't', VFS_UBF,
	FA_ACC_UA,
	1, textcheck,
	_ZFTAM_1_ParametersDOCS,
	"unstructured text file",

	/* VFS_FDF */
#ifdef COMPAT_OLD_NBS9OID
	"NBS-9",  NULLOID, NULLCP, VF_ALIASED, 0, S_IFDIR, fdfpeek, 'd', VFS_XXX,
#else
	"NBS-9",  NULLOID, NULLCP, VF_NULL, 0, S_IFDIR, fdfpeek, 'd', VFS_XXX,
#endif /* COMPAT_OLD_NBS9OID */
	FA_ACC_UA,
	0, NULL,
	_ZNBS_9_ParametersDOCS,
	"file directory file",

	NULL
};
#ifdef	BRIDGE
int	vfs_fdf = VFS_FDF;
#endif

struct vfsmap *myvf;

void ftam_advise (struct FTAMabort *fta, char *event) {
	if (hash && marks >= BUFSIZ) {
		marks = 0;
		printf ("\n");
	}

	fflush (stdout);

	if (fta -> fta_peer) {
#ifdef	BRIDGE
		sprintf (ftam_error, "%s: peer aborted association, due to ",
				 event);
#else
		fprintf (stderr, "%s: peer aborted association, due to ", event);
#endif
		switch (fta -> fta_action) {
		case FACTION_TRANS:
#ifdef	BRIDGE
			sprintf (ftam_error + strlen (ftam_error),
					 "transient-error");
#else
			fprintf (stderr, "transient-error");
#endif
			break;

		case FACTION_PERM:
#ifdef	BRIDGE
			sprintf (ftam_error + strlen (ftam_error),
					 "permanent-error");
#else
			fprintf (stderr, "permanent-error");
#endif
			break;

		default:
#ifdef	BRIDGE
			sprintf (ftam_error + strlen (ftam_error),
					 "action result %d", fta -> fta_action);
#else
			fprintf (stderr, "action result %d", fta -> fta_action);
#endif
			break;
		}
#ifndef	BRIDGE
		fprintf (stderr, "\n");
#endif
	} else
#ifdef	BRIDGE
		sprintf (ftam_error + strlen (ftam_error), "%s: failed\n",
				 event);
	if (verbose)
		advise (NULLCP, "%s", ftam_error);
#else
		fprintf (stderr, "%s: failed\n", event);
#endif
	ftam_diag (fta -> fta_diags, fta -> fta_ndiag, fta -> fta_peer,
			   FACTION_PERM);

	if (fta -> fta_action == FACTION_PERM) {
		ftamfd = NOTOK;
		if (rcwd) {
			free (rcwd);
			rcwd = NULL;
		}
	}
}

void ftam_chrg (struct FTAMcharging *charges) {
	int    i;
	char   *cp;
	struct fc_charge  *fc;

	cp = "charging information:\n    %s: %d %s\n";
	for (fc = charges -> fc_charges, i = charges -> fc_ncharge - 1;
			i >= 0;
			fc++, i--, cp = "    %s: %d %s\n")
#ifdef	BRIDGE
		sprintf (ftam_error, cp, fc -> fc_resource, fc -> fc_value,
				 fc -> fc_unit);
	advise (NULLCP, "%s", ftam_error);
#else
		printf (cp, fc -> fc_resource, fc -> fc_value, fc -> fc_unit);
#endif
}

static char *entity[] = {
	"unknown",
	"initiator",
	"initiator's FPM",
	"virtual filestore",
	"responder's FPM",
	"responder"
};

void ftam_diag (struct FTAMdiagnostic diag[], int ndiag, int peer, int action) {
	int    i;
	int     didit;
	struct FTAMdiagnostic *dp;

#ifdef	BRIDGE
	ftam_error[0] = 0;
#endif
	for (dp = diag, i = ndiag - 1; i >= 0; dp++, i--) {
		if (dp -> ftd_identifier != FS_GEN_NOREASON) {
#ifdef	BRIDGE
			sprintf (ftam_error + strlen (ftam_error),
					 "%s", FErrString (dp -> ftd_identifier));
#else
			printf ("%s", FErrString (dp -> ftd_identifier));
#endif
			if (dp -> ftd_cc > 0)
#ifdef	BRIDGE
				sprintf (ftam_error + strlen (ftam_error),
						 ": %*.*s", dp -> ftd_cc, dp -> ftd_cc,
						 dp -> ftd_data);
#else
				printf (": %*.*s", dp -> ftd_cc, dp -> ftd_cc, dp -> ftd_data);
#endif
		} else if (dp -> ftd_cc > 0)
#ifdef	BRIDGE
			sprintf (ftam_error + strlen (ftam_error),
					 "%*.*s", dp -> ftd_cc, dp -> ftd_cc,
					 dp -> ftd_data);
#else
			printf ("%*.*s", dp -> ftd_cc, dp -> ftd_cc, dp -> ftd_data);
#endif

#ifdef	BRIDGE
		advise (NULLCP, "%s", ftam_error);
#else
		printf ("\n");
#endif

		didit = 0;
		switch (dp -> ftd_type) {
		case DIAG_INFORM:
			if (action == FACTION_SUCCESS)
				break;
			didit++;
#ifdef	BRIDGE
			sprintf (ftam_error + strlen (ftam_error),
					 "    type informative");
#else
			printf ("    type informative");
#endif
			break;

		case DIAG_TRANS:
			didit++;
#ifdef	BRIDGE
			sprintf (ftam_error + strlen (ftam_error),
					 "    type transient");
#else
			printf ("    type transient");
#endif
			break;

		case DIAG_PERM:
			if (dp -> ftd_observer == EREF_IFSU)
				ftamfd = NOTOK;
			if (action != FACTION_SUCCESS)
				break;
			didit++;
#ifdef	BRIDGE
			sprintf (ftam_error + strlen (ftam_error),
					 "    type permanent");
#else
			printf ("    type permanent");
#endif
			break;

		default:
			didit++;
#ifdef	BRIDGE
			sprintf (ftam_error + strlen (ftam_error),
					 "    type %d", dp -> ftd_type);
#else
			printf ("    type %d", dp -> ftd_type);
#endif
			break;
		}

		switch (dp -> ftd_observer) {
		case EREF_IFSU:
			goto print_it;

		case EREF_IFPM:
			if (peer)
				goto print_it;
			break;

		case EREF_RFSU:
			if (peer)
				break;	/* else fall */
		case EREF_RFPM:
print_it:
			;
#ifdef	BRIDGE
			sprintf (ftam_error + strlen (ftam_error),
					 "%sobserver %s", didit++ ? ", " : "    ",
					 entity[dp -> ftd_observer]);
#else
			printf ("%sobserver %s", didit++ ? ", " : "    ",
					entity[dp -> ftd_observer]);
#endif
			break;

		default:
#ifdef	BRIDGE
			sprintf (ftam_error + strlen (ftam_error),
					 "%sobserver %d", didit++ ? ", " : "    ",
					 dp -> ftd_observer);
#else
			printf ("%sobserver %d", didit++ ? ", " : "    ",
					dp -> ftd_observer);
#endif
			break;
		}

		switch (dp -> ftd_source) {
		case EREF_NONE:
		case EREF_IFSU:
			break;

		case EREF_SERV:
		case EREF_RFSU:
			if (peer)
				break;	/* else fall */
		case EREF_IFPM:
		case EREF_RFPM:
#ifdef	BRIDGE
			sprintf (ftam_error + strlen (ftam_error),
					 "%ssource %s", didit++ ? ", " : "    ",
					 entity[dp -> ftd_source]);
#else
			printf ("%ssource %s", didit++ ? ", " : "    ",
					entity[dp -> ftd_source]);
#endif
			break;

		default:
#ifdef	BRIDGE
			sprintf (ftam_error + strlen (ftam_error),
					 "%ssource %d", didit++ ? ", " : "    ",
					 dp -> ftd_source);
#else
			printf ("%ssource %d", didit++ ? ", " : "    ",
					dp -> ftd_source);
#endif
			break;
		}

		if (dp -> ftd_delay != DIAG_NODELAY)
#ifdef	BRIDGE
			sprintf (ftam_error + strlen (ftam_error),
					 "%ssuggested-delay %d", didit++ ? ", " : "    ",
					 dp -> ftd_delay);
#else
			printf ("%ssuggested-delay %d", didit++ ? ", " : "    ",
					dp -> ftd_delay);
#endif

#ifndef	BRIDGE
		if (didit)
			printf ("\n");
#endif
	}
#ifdef	BRIDGE
	if (ftam_error[0])
		advise (NULLCP, "%s", ftam_error);
#endif
}

/* MISCELLANY */

void rcinit (void) {
#ifndef	BRIDGE
	int    w;
	char **cp;
	struct dispatch   *ds;
	struct var *v;
#endif
	struct isodocument *id;
	struct vfsmap *vf;

#ifndef	BRIDGE
	if ((myhome = getenv ("HOME")) == NULL)
		myhome = ".";		/* could do passwd search... */

	if ((myuser = getenv ("USER")) == NULLCP)
		myuser = getenv ("LOGNAME");
#endif

	for (vf = vfs + 1; vf -> vf_entry; vf++)  { /* skip "default" entry */
		if (id = getisodocumentbyentry (vf -> vf_entry)) {
			if ((vf -> vf_oid = oid_cpy (id -> id_type)) == NULLOID)
				adios (NULLCP, "out of memory");
#ifdef COMPAT_OLD_NBS9OID
#ifdef DEBUG
			if ((vf -> vf_flags & VF_ALIASED) && watch)
				advise (NULLCP,
						"warning: local realstore will support aliasing of %ss (%s)",
						vf -> vf_text, vf -> vf_entry);
#endif	/* DEBUG */
#endif	/* COMPAT_OLD_NBS9OID */
		} else if (vf -> vf_flags & VF_WARN)
			advise (NULLCP,
					"warning: local realstore has no support for %ss (%s)",
					vf -> vf_text, vf -> vf_entry);
	}
	bzero ((char *) &myqos, sizeof myqos);
	myqos.qos_sversion = 2;

#ifndef	BRIDGE
	for (ds = dispatches, helpwidth = 0; ds -> ds_name; ds++)
		if (strlen2int (ds -> ds_name, &w) == 0 && w > helpwidth)
			helpwidth = w;

	userdn = strdup ("");
	for (v = vars, varwidth1 = 0; v -> v_name; v++) {
		if (strlen2int (v -> v_name, &w) == 0 && w > varwidth1)
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
						if (strlen2int (*cp, &w) == 0 && w > varwidth2)
							varwidth2 = w;
			}
		} else if (*v -> v_dvalue) {
			*v -> v_dvalue = strdup (*v -> v_dvalue);
			if (strlen2int (*v -> v_dvalue, &w) == 0
					&& add_int_to_int (&w, 2) == 0
					&& w > varwidth2)
				varwidth2 = w;
		}
	}
#endif
}

#ifndef	TIOCGWINSZ
#endif

int	ncols (FILE *fp) {
#ifdef	TIOCGWINSZ
	int	    i;
	struct winsize ws;

	if (ioctl (fileno (fp), TIOCGWINSZ, (char *) &ws) != NOTOK
			&& (i = ws.ws_col) > 0)
		return i;
#endif

	return 80;
}

#ifndef	NBBY
#define	NBBY	8
#endif

#ifndef	TMS
void timer (int cc, char *action) {
	long    ms;
	float   bs;
	struct timeval stop, td;
	static struct timeval   start;
	if (cc == 0) {
		gettimeofday (&start, (struct timezone *) 0);
		return;
	} else
		gettimeofday (&stop, (struct timezone  *) 0);
	tvsub (&td, &stop, &start);
	ms = (td.tv_sec * 1000) + (td.tv_usec / 1000);
	bs = (((float) cc * NBBY * 1000) / (float) (ms ? ms : 1)) / NBBY;
	advise (NULLCP, "%d bytes %s in %d.%02d seconds (%.2f Kbytes/s)",
			cc, action, td.tv_sec, td.tv_usec / 10000, bs / 1024);
}

static void tvsub (struct timeval *tdiff, struct timeval *t1, struct timeval *t0) {
	tdiff -> tv_sec = t1 -> tv_sec - t0 -> tv_sec;
	tdiff -> tv_usec = t1 -> tv_usec - t0 -> tv_usec;
	if (tdiff -> tv_usec < 0)
		tdiff -> tv_sec--, tdiff -> tv_usec += 1000000;
}

#else
#ifndef	HZ
#define	HZ	60
#endif

int timer (int cc, char *action) {
	long    ms;
	float   bs;
	long    stop,
			td,
			secs,
			msecs;
	struct tms tm;
	static long start;

	if (cc == 0) {
		start = times (&tm);
		return;
	} else
		stop = times (&tm);

	td = stop - start;
	secs = td / HZ, msecs = (td % HZ) * 1000 / HZ;
	ms = (secs * 1000) +  msecs;
	bs = (((float) cc * NBBY * 1000) / (float) (ms ? ms : 1)) / NBBY;

	advise (NULLCP, "%d bytes %s in %d.%02d seconds (%.2f Kbytes/s)",
			cc, action, secs, msecs / 10, bs / 1024);
}
#endif

#ifdef	BRIDGE
/* FTP TYPE Function */

#include <arpa/ftp.h>

int f_type (int mode) {
	switch(mode) {
	case TYPE_A:
		tmode = VFS_UTF;
		return OK;

	case TYPE_I:
	case TYPE_L:
		tmode = VFS_UBF;
		return OK;

	case TYPE_E:
	default:
		return NOTOK;
	}
}
#endif
