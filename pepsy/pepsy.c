/* pepsy.c - table driven posy/pepy replacement system */

/* OPEN QUESTIONS:

	How to do smarter DEFAULT determination for the other types and NULLs

	Perhaps pull-up primitive IDentifiers

	Abort a CHOICE encoding if the structure is empty


				  HEURISTICS

   1. LANGUAGE SIMPLIFICATIONS:


	Pull-up uni-member SEQUENCEs/SETs/CHOICEs


   2. LANGUAGE ASSUMPTIONS:

	Unique tags to avoid conflicts for internal structures (-h1 option)


   3. STYLE ISSUES:

	SEQUENCE/SET OF Type should have Type be an ID for nicer naming
 */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/pepsy/RCS/pepsy.c,v 9.0 1992/06/16 12:24:03 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/pepsy/RCS/pepsy.c,v 9.0 1992/06/16 12:24:03 isode Rel $
 *
 *
 * $Log: pepsy.c,v $
 * Revision 9.0  1992/06/16  12:24:03  isode
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
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include "pepsydefs.h"
#include "pass2.h"


#define	SVAL(s)		((s) ? (s) : "")
#define PARVAL(s)	((s) ? (s) : "parm")

/*    DATA */

static int aflag = 0;
int	Aflag = 0;	/* Don't so all modes by default */
int	Cflag = 1;      /* combine H files */
int	dflag = 0;
int	Pflag = 0;		/* pepy compat ... */
char    *bflag = NULL;		/*  .. */
char	*iflag = NULL;		/* -i file => #include users file */
char    *module_actions = NULL;
int	pepydebug = 0;
int	doexternals = 1;
int fflag = 0;
static int linepos = 0;
int mflag = 0;
int sflag = 0;

int p_debug = 3;
int options[NOPTIONS];

char *eval = NULLCP;

char   *mymodule = "";
OID	mymoduleid = NULLOID;
static char modulename[BUFSIZ];

int yysection = YP_DECODER;
char *yyencpref = "encode";
char *yydecpref = "decode";
char *yyprfpref = "print";
char *yyencdflt = "encode";
char *yydecdflt = "decode";
char *yyprfdflt = "print";

static char *classes[] = {
	"UNIVERSAL ",
	"APPLICATION ",
	"",
	"PRIVATE "
};

static char *tags[] = {
	"", "BOOLEAN", "INTEGER", "INTEGER", "BIT STRING", "BIT STRING",
	"OCTET STRING", "NULL", "SEQUENCE", "SEQUENCE OF", "SEQUENCE",  "SET",
	"SET OF", "SET", "CHOICE", "ANY", "OBJECT IDENTIFIER", "", "ENUMERATED",
	"REAL", "",

	NULL
};

struct tuple tuples[] = {
	YP_BOOL, "PE_CLASS_UNIV", "PE_FORM_PRIM", "PE_PRIM_BOOL",
	PE_CLASS_UNIV, PE_PRIM_BOOL,
	YP_INT, "PE_CLASS_UNIV", "PE_FORM_PRIM", "PE_PRIM_INT",
	PE_CLASS_UNIV, PE_PRIM_INT,
	YP_INTLIST, "PE_CLASS_UNIV", "PE_FORM_PRIM", "PE_PRIM_INT",
	PE_CLASS_UNIV, PE_PRIM_INT,
	YP_BIT, "PE_CLASS_UNIV", NULLCP, "PE_PRIM_BITS",
	PE_CLASS_UNIV, PE_PRIM_BITS,
	YP_BITLIST, "PE_CLASS_UNIV", NULLCP, "PE_PRIM_BITS",
	PE_CLASS_UNIV, PE_PRIM_BITS,
	YP_OCT, "PE_CLASS_UNIV", NULLCP, "PE_PRIM_OCTS",
	PE_CLASS_UNIV, PE_PRIM_OCTS,
	YP_NULL, "PE_CLASS_UNIV", NULLCP, "PE_PRIM_NULL",
	PE_CLASS_UNIV, PE_PRIM_NULL,
	YP_OID, "PE_CLASS_UNIV", "PE_FORM_PRIM", "PE_PRIM_OID",
	PE_CLASS_UNIV, PE_PRIM_OID,
	YP_SEQ, "PE_CLASS_UNIV", "PE_FORM_CONS", "PE_CONS_SEQ",
	PE_CLASS_UNIV, PE_CONS_SEQ,
	YP_SEQTYPE, "PE_CLASS_UNIV", "PE_FORM_CONS", "PE_CONS_SEQ",
	PE_CLASS_UNIV, PE_CONS_SEQ,
	YP_SEQLIST, "PE_CLASS_UNIV", "PE_FORM_CONS", "PE_CONS_SEQ",
	PE_CLASS_UNIV, PE_CONS_SEQ,
	YP_SET, "PE_CLASS_UNIV", "PE_FORM_CONS", "PE_CONS_SET",
	PE_CLASS_UNIV, PE_CONS_SET,
	YP_SETTYPE, "PE_CLASS_UNIV", "PE_FORM_CONS", "PE_CONS_SET",
	PE_CLASS_UNIV, PE_CONS_SET,
	YP_SETLIST, "PE_CLASS_UNIV", "PE_FORM_CONS", "PE_CONS_SET",
	PE_CLASS_UNIV, PE_CONS_SET,
	YP_ENUMLIST, "PE_CLASS_UNIV", "PE_FORM_PRIM", "PE_PRIM_ENUM",
	PE_CLASS_UNIV, PE_PRIM_ENUM,
	YP_REAL,     "PE_CLASS_UNIV", "PE_FORM_PRIM", "PE_PRIM_REAL",
	PE_CLASS_UNIV, PE_PRIM_REAL,

	YP_UNDF
};


static char autogen[BUFSIZ];

char   *sysin = NULLCP;
static char sysout[BUFSIZ];
static char sysdef[BUFSIZ];
static char sysact[BUFSIZ];

static FILE *fdef;


static  MD	mymodules = NULLMD;

typedef struct symlist {
	char   *sy_encpref;
	char   *sy_decpref;
	char   *sy_prfpref;
	char   *sy_module;
	char   *sy_name;

	YP	    sy_type;

	struct symlist *sy_next;
}		symlist, *SY;
#define	NULLSY	((SY) 0)

SY	mysymbols = NULLSY;

char *modsym ();
static char   *gensym (), *array ();
extern	char	*my_strcat(), *gfree(), *concat(), *my_new_str();
extern	char	*getid();
MD	lookup_module ();
static SY	new_symbol (), add_symbol ();
static double val2real ();
static void prime_default ();
static void merge_files ();

static	yyerror_aux ();
static	do_struct0 ();
static	do_struct1 ();
static	do_struct2 ();
static	printag ();
static	xalloc ();
static	balloc ();
static	choice_pullup ();
static	components_pullup ();
static int  val2int ();
static	val2prf ();
static dump_real ();
static int  dfl2int ();
static	print_value ();
static	modsym_aux ();
static	read_ph_file ();
static	write_ph_file ();

extern void exit();	/* to keep lint happy */

static FILE   *open_ph_file ();
extern FILE *yyin, *yyout;

YP	lookup_type ();

YT  lookup_tag ();

/*    MAIN */

/* ARGSUSED */

int
main (int argc, char **argv, char **envp) {
	int	    i;
	char  *cp,
		  *dp;

    yyin  = stdin;
    yyout = stdout;

    dp = pepsyversion + strlen ("pepsy ");
    fprintf (stderr, "pepsy %s\n", dp);

	sysout[0] = sysdef[0] = sysact[0] = NULL;
	for (argc--, argv++; argc > 0; argc--, argv++) {
		cp = *argv;

		if (strcmp (cp, "-A") == 0) {
			Aflag++;
			continue;
		}
		if (strcmp (cp, "-a") == 0) {
			aflag++;
			continue;
		}
		if (strcmp (cp, "-C") == 0) {
			Cflag = 0;
			continue;
		}
		if (strcmp (cp, "-d") == 0) {
			dflag++;
			continue;
		}
		if (strcmp (cp, "-f") == 0) {
			dflag++, fflag++;
			continue;
		}
		if (strncmp (cp, "-h", 2) == 0) {
			if (cp[2] == NULL) {
				hflag++;
				continue;
			}
			if (sscanf (cp + 2, "%d", &i) != 1 || i >= NOPTIONS)
				goto usage;
			hflag++, options[i]++;
			continue;
		}
		if (strcmp (cp, "-m") == 0) {
			mflag++;
			continue;
		}
		if (strcmp (cp, "-s") == 0) {
			sflag++;
			continue;
		}
		if (strcmp (cp, "-i") == 0) {
			argv++;
			if (argc-- > 0)
				iflag = *argv;
			continue;
		}

		if (sysin) {
usage:
			;
			fprintf (stderr,
					 "usage: pepsy [-A] [-a] [-C] [-d] [-f] [-h] [-s] module.py\n");
			exit (1);
		}

		if (*cp == '-') {
			if (*++cp != NULL)
				goto usage;
			sysin = "";
		}
		sysin = cp;
	}

	switch (pepydebug = (cp = getenv ("PEPSYTEST")) && *cp ? atoi (cp) : 0) {
	case 2:
		yydebug++;		/* fall */
	case 1:
		sflag++;		/*   .. */
	case 0:
		break;
	}

	if (sysin == NULLCP)
		sysin = "";

	if (*sysin && freopen (sysin, "r", stdin) == NULL) {
		fprintf (stderr, "unable to read "), perror (sysin);
		exit (1);
	}

	if (strcmp (sysout, "-") == 0)
		sysout[0] = NULL;
	if (*sysout && freopen (sysout, "w", stdout) == NULL) {
		fprintf (stderr, "unable to write "), perror (sysout);
		exit (1);
	}

	if (cp = index (dp, ')')) {
		for (cp++; *cp != ' '; cp++)
			if (*cp == NULL)
				break;
		if (*cp == NULL)
			cp = NULL;
	}
	if (cp == NULL)
		cp = dp + strlen (dp);
	sprintf (autogen, "pepsy %*.*s", cp - dp, cp - dp, dp);

	initoidtbl ();

	exit (yyparse ());		/* NOTREACHED */
}

/*    ERRORS */

int
yyerror (char *s) {
	yyerror_aux (s);

	if (*sysout)
		unlink (sysout);
	if (*sysdef)
		unlink (sysdef);
	if (*sysact)
		unlink (sysact);

	exit (1);
}

#ifndef lint
warning (char* fmt, ...) {
	char	buffer[BUFSIZ];
	char	buffer2[BUFSIZ];
	va_list	ap;

	va_start (ap, fmt);

	_asprintf (buffer, NULLCP, fmt, ap);

	va_end (ap);

	sprintf (buffer2, "Warning: %s", buffer);
	yyerror_aux (buffer2);
}

#else

/* VARARGS1 */
int
warning (char *fmt) {
	warning (fmt);
}
#endif

static
yyerror_aux (char *s) {
	if (linepos)
		fprintf (stderr, "\n"), linepos = 0;

	if (eval)
		fprintf (stderr, "type %s: ", eval);
	else
		fprintf (stderr, "line %d: ", yylineno);
	fprintf (stderr, "%s\n", s);
	if (!eval)
		fprintf (stderr, "last token read was \"%s\"\n", yytext);
}

/*  */

#ifndef	lint
myyerror (char* fmt, ...) {
	char    buffer[BUFSIZ];
	va_list ap;

	va_start (ap, fmt);

	_asprintf (buffer, NULLCP, fmt, ap);

	va_end (ap);

	yyerror (buffer);
}
#endif


#ifndef	lint
static	pyyerror (YP yp, char* fmt, ...) {
	char    buffer[BUFSIZ];
	va_list	ap;

	va_start (ap, fmt);

	_asprintf (buffer, NULLCP, fmt, ap);

	va_end (ap);

	yyerror_aux (buffer);
	print_type (yp, 0);


	if (*sysout)
		unlink (sysout);
	if (*sysdef)
		unlink (sysdef);
	if (*sysact)
		unlink (sysact);

	exit (1);
}
#else
/* VARARGS */

static	pyyerror (yp, fmt)
YP	yp;
char   *fmt;
{
	pyyerror (yp, fmt);
}
#endif

/*  */

int
yywrap()  {
	if (linepos)
		fprintf (stderr, "\n"), linepos = 0;

	return 1;
}

/*  */

/* ARGSUSED */

int
yyprint (char *s, int f, int top) {
	int     len;
	static int  nameoutput = 0;
	static int  outputlinelen = 79;
	static int didf = 0;

	if (sflag || !s)
		return;

	if (f && didf == 0) {
		if (linepos)
			fprintf (stderr, "\n\n");
		else  fprintf (stderr, "\n");

		fprintf (stderr, "%s includes:", mymodule);
		linepos = (nameoutput = strlen (mymodule) + 10) + 1;

		didf = 1;
	}

	if (!nameoutput || top) {
		if (linepos)
			fprintf (stderr, "\n\n");

		fprintf (stderr, "%s", mymodule);
		nameoutput = (linepos = strlen (mymodule)) + 1;

#define section(flag,prefix) \
        if (yysection & (flag)) { \
            fprintf (stderr, " %s", (prefix)); \
            linepos += strlen (prefix) + 1; \
        } \
        else \
            fprintf (stderr, " none"), linepos += 5
		section (YP_ENCODER, yyencpref);
		section (YP_DECODER, yydecpref);
		section (YP_PRINTER, yyprfpref);


		fprintf (stderr, ":");
		linepos += 2;

		if (top)
			return;
	}

	len = strlen (s) + (f ? 2 : 0);
	if (linepos != nameoutput)
		if (len + linepos + 1 > outputlinelen)
			fprintf (stderr, "\n%*s", linepos = nameoutput, "");
		else
			fprintf (stderr, " "), linepos++;
	fprintf (stderr, f ? "(%s)" : "%s", s);
	linepos += len;
}

/*    PASS1 */

int
pass1()  {
}

/*  */

pass1_type (encpref, decpref, prfpref, mod, id, yp)
char  *encpref,
	  *decpref,
	  *prfpref,
	  *mod,
	  *id;
YP	yp;
{
	SY	    sy;

	if (lookup_type (mod, id))	/* no duplicate entries, please... */
		return;

	if (pepydebug) {
		if (linepos)
			fprintf (stderr, "\n"), linepos = 0;

		fprintf (stderr, "%s.%s\n", mod ? mod : mymodule, id);
		print_type (yp, 0);
		fprintf (stderr, "--------\n");
	} else if (!(yp -> yp_flags & YP_IMPORTED))
		yyprint (id, 0, 0);

	sy = new_symbol (encpref, decpref, prfpref, mod, id, yp);
	mysymbols = add_symbol (mysymbols, sy);
}

static void hprologue (fp)
FILE *fp;
{
#define NFILES 	5
	char *files[NFILES];
	/* { "psap.h", "pepsy.h", "UNIV-types.h", (char *)0 }; */

	int last = 0;		/* last slot available in files */
	fprintf (fp, "/* automatically generated by %s, do not edit! */\n\n",
			 autogen);
	fprintf (fp, "#ifndef\t_module_%s_defined_\n", modulename);
	fprintf (fp, "#define\t_module_%s_defined_\n\n", modulename);

	fprintf (fp, "#ifndef\tPEPSY_VERSION\n");
	fprintf (fp, "#define\tPEPSY_VERSION\t\t%d\n", PEPSY_VERSION_NUMBER);
	fprintf (fp, "#endif\n\n");

	files[last++] = "psap.h";
	files[last++] = "pepsy.h";

	if (strcmp (mymodule, "UNIV")) {
		files[last++] = "pepsy/UNIV-types.h";
	}

	files[last++] = (char *)0;

	doincl(fp, files);

#undef NFILES
}

/*    PASS2 */

int
pass2()  {
	SY	    sy;
	YP	    yp;


	modsym_aux (mymodule, modulename);

	sprintf (sysdef, "%s%s", mymodule, Cflag ? "-t.tmp" : "-types.h");
	if ((fdef = fopen (sysdef, "w")) == NULL)
		myyerror ("unable to write %s", sysdef);
	if (!Cflag)
		hprologue (fdef);


	if (!Cflag) {
		if (mflag) {
			fprintf (fdef, "#ifndef\tPEPYPATH\n");
			fprintf (fdef, "#include <isode/pepsy/%s%s>\n", mymodule, HFILE1);
			fprintf (fdef, "#else\n");
			fprintf (fdef, "#include \"%s%s\"\n", mymodule, HFILE1);
			fprintf (fdef, "#endif\n");
		} else
			fprintf (fdef, "#include \"%s%s\"\n", mymodule, HFILE1);
	}
	/* User's #include file */
	if (iflag) {
		fprintf (fdef, "#include \"%s\"\n", iflag);
	}

	/*
	 * pull up some symbols
	 */
	for (sy = mysymbols; sy; sy = sy -> sy_next) {
		eval = sy -> sy_name;
		yp = sy -> sy_type;
		if (sy -> sy_module == NULLCP)
			yyerror ("no module name associated with symbol");
		if (yp -> yp_flags & YP_IMPORTED)
			continue;

		do_struct0 (yp, eval);
		if (ferror (stdout) || ferror (fdef))
			myyerror ("write error - %s", sys_errname (errno));
	}
	/* dump out the references to the external types */
	proc_extmod(fdef);

	/*
	 * generate the #define's for simple type definitions
	 */
	for (sy = mysymbols; sy; sy = sy -> sy_next) {
		eval = sy -> sy_name;
		yp = sy -> sy_type;
		if (yp -> yp_flags & YP_IMPORTED)
			continue;

		do_struct1 (yp, eval, NULLCP);
		if (ferror (stdout) || ferror (fdef))
			myyerror ("write error - %s", sys_errname (errno));
	}

	/*
	 * generate the C structure definitions
	 */
	for (sy = mysymbols; sy; sy = sy -> sy_next) {
		eval = sy -> sy_name;
		yp = sy -> sy_type;
		if (yp -> yp_flags & YP_IMPORTED)
			continue;

		do_struct2 (yp, eval, NULLCP);
		if (ferror (stdout) || ferror (fdef))
			myyerror ("write error - %s", sys_errname (errno));
	}

	peri_pass2();

	fprintf (fdef, "#endif\n");
	fflush (fdef);
	fflush (stdout);
	if (ferror (stdout) || ferror (fdef))
		myyerror ("write error - %s", sys_errname (errno));
	fclose (fdef);

	if (Cflag)
		merge_files (mymodule);

	write_ph_file();
	if (!sflag) {
		if (linepos)  fprintf (stderr, "\n"), linepos = 0;
		fflush (stderr);
	}
}

static void copy_file (fp1, fp2)
FILE *fp1, *fp2;
{
	char buf[BUFSIZ];
	int n;

	while ((n = fread (buf, 1, BUFSIZ, fp1)) > 0) {
		if (fwrite (buf, 1, n, fp2) != n) {
			fprintf (stderr, "Write error\n");
			exit (1);
		}
	}
}

static void
merge_files (char *stem) {
	FILE *fpin, *fpout;
	char fname[BUFSIZ];
	char buf[BUFSIZ];

	sprintf (fname, "%s-types.h", stem);
	if ((fpout = fopen (fname, "w")) == NULL)
		myyerror ("unable to write %s", fname);

	hprologue (fpout);
	sprintf (buf, "%s%s", stem, HFILE2);
	if ((fpin = fopen (buf, "r")) == NULL)
		myyerror ("unable to read file %s", buf);
	copy_file (fpin, fpout);
	fclose (fpin);
	unlink (buf);

	sprintf (buf, "%s%s", stem, HFILE1);
	if ((fpin = fopen (buf, "r")) == NULL)
		myyerror ("unable to read file %s", buf);
	copy_file (fpin, fpout);
	fclose (fpin);
	unlink (buf);

	sprintf (buf, "%s-t.tmp", stem);
	if ((fpin = fopen (buf, "r")) == NULL)
		myyerror ("unable to read file %s", buf);
	copy_file (fpin, fpout);
	fclose (fpin);
	unlink (buf);

	if (fclose (fpout) == EOF) {
		fprintf (stderr, "Write error to file");
		perror ("");
		exit (1);
	}
}

/*  */

/* ARGSUSED */

static	do_struct0 (yp, id)
YP	yp;
char   *id;
{
	YP	    y;
	MD	md;

	switch (yp -> yp_code) {
	case YP_SEQLIST:
	case YP_SETLIST:
		components_pullup (yp);
		break;

	default:
		break;
	}

	switch (yp -> yp_code) {
	case YP_SEQTYPE:
	case YP_SETTYPE:
		do_struct0 (yp -> yp_type, id);
		break;

	case YP_CHOICE:
	case YP_SETLIST:
		choice_pullup (yp, yp -> yp_code == YP_CHOICE ? CH_FULLY
					   : CH_PARTIAL);
	/* and fall */
	case YP_SEQLIST:
		for (y = yp -> yp_type; y; y = y -> yp_next)
			do_struct0 (y, id);
		break;

	case YP_IDEFINED:
		if (yp -> yp_module && strcmp (yp -> yp_module, mymodule)) {
			if ((md = lookup_module (yp -> yp_module, yp -> yp_modid))
					== NULLMD || (md->md_flags & MDF_SEEN) == 0) {
				addextmod(yp -> yp_module);
				if (md)
					md->md_flags |= MDF_SEEN;
			}
		}
		/* correct for IMPLICIT Tag of a CHOICE or ANY if possible */
		if ((yp->yp_flags & (YP_IMPLICIT|YP_TAG)) == (YP_IMPLICIT|YP_TAG)) {
			yp->yp_flags &= ~YP_IMPLICIT; /* clear it */
			yp->yp_flags |= comptag(YP_IMPLICIT, yp);
		}
		break;

	default:
		break;
	}
}

/*  */

static	do_struct1 (yp, id, pullup)
YP	yp;
char   *id,
	   *pullup;
{
	int    i,
		   j;
	char    buf1[BUFSIZ];
	YP	    y;
	YV	    yv;

	switch (yp -> yp_code) {
	case YP_BIT:
	case YP_BITLIST:
	case YP_SEQ:
	case YP_SET:
	case YP_ANY:
		fprintf (fdef, "\n");
		if (aflag)
			printag (yp, 4, pullup);
		fprintf (fdef, "#define\t%s\tPElement\n",
				 modsym (mymodule, id, "type"));
		if (yp -> yp_code == YP_BITLIST) {
			i = -1;
			for (yv = yp -> yp_value; yv; yv = yv -> yv_next)
				if ((j = val2int (yv)) < 0)
					pyyerror (yp, "invalid bit number in BIT STRING");
				else if (j > i)
					i = j;
			if (i < sizeof (int) * 8) {	/* NBBY */
				fprintf (fdef, "#define\t%s\t\"\\020",
						 modsym (mymodule, eval, "bits"));
				for (yv = yp -> yp_value; yv; yv = yv -> yv_next)
					if (yv -> yv_flags & YV_NAMED)
						fprintf (fdef, "\\0%o%s",
								 val2int (yv) + 1, yv -> yv_named);
					else
						fprintf (fdef, "\\0%oBIT%d",
								 val2int (yv) + 1, val2int (yv));
				fprintf (fdef, "\"\n");
			}
			for (yv = yp -> yp_value; yv; yv = yv -> yv_next) {
				modsym_aux (yv -> yv_named, buf1);
				fprintf (fdef, "#define\t%s_%s\t%d\n",
						 modsym (mymodule, eval, "bit"),
						 buf1, val2int (yv));
			}
		}
		if (fflag)
			fprintf (fdef, "#define\t%s\tpe_free\n",
					 modsym (mymodule, id, "free"));
		break;

	case YP_OCT:
		fprintf (fdef, "\n");
		if (aflag)
			printag (yp, 4, pullup);
		fprintf (fdef, "#define\t%s\tqbuf\n",
				 modsym (mymodule, id, "type"));
		if (fflag)
			fprintf (fdef, "#define\t%s\tqb_free\n",
					 modsym (mymodule, id, "free"));
		break;

	case YP_OID:
		fprintf (fdef, "\n");
		if (aflag)
			printag (yp, 4, pullup);
		fprintf (fdef, "#define\t%s\tOIDentifier\n",
				 modsym (mymodule, id, "type"));
		if (fflag)
			fprintf (fdef, "#define\t%s\toid_free\n",
					 modsym (mymodule, id, "free"));
		break;

	case YP_IDEFINED:
		fprintf (fdef, "\n");
		if (aflag)
			printag (yp, 4, pullup);
		fprintf (fdef, "#define\t%s\t",
				 modsym (mymodule, id, "type"));
		fprintf (fdef, "%s\n",
				 modsym (yp -> yp_module, yp -> yp_identifier, "type"));
		if (fflag) {
			fprintf (fdef, "#define\t%s\t",
					 modsym (mymodule, id, "free"));
			fprintf (fdef, "%s\n",
					 modsym (yp -> yp_module, yp -> yp_identifier, "free"));
		}
		break;

	case YP_SEQLIST:
	case YP_SETLIST:
	case YP_CHOICE:
		if (hflag && (y = yp -> yp_type) && !y -> yp_next) {
			do_struct1 (y, id, tags[yp -> yp_code]);
			break;
		}
	/* else fall */

	default:
		break;
	}
}

/*  */

static	do_struct2 (yp, id, pullup)
YP	yp;
char   *id,
	   *pullup;
{
	YP	    y;
	int	flg = (yp -> yp_code == YP_SEQTYPE || yp -> yp_code == YP_SETTYPE);

	switch (yp -> yp_code) {
	case YP_BIT:
	case YP_BITLIST:
	case YP_SEQ:
	case YP_SET:
	case YP_ANY:
	case YP_OCT:
	case YP_OID:
	case YP_IDEFINED:
		break;

	case YP_SEQLIST:
	case YP_SETLIST:
	case YP_CHOICE:
		if (hflag && (y = yp -> yp_type) && !y -> yp_next) {
			do_struct2 (y, id, tags[yp -> yp_code]);
			break;
		}
	/* else fall */

	default:
		fprintf (fdef, "\n");
		if (aflag)
			printag (yp, 4, pullup);
		fprintf (fdef, "struct %s {\n", modsym (mymodule, id, "type"));
		pepsy (yp, 1, 1, "parm", id, "parm", flg && h2flag);
		fprintf (fdef, "};\n");
		fprintf (fdef, "#define\t%s(parm)\\\n\t%s\n", modsym (mymodule, id, "free"), gfree(mymodule, id, "parm"));

		break;
	}
}

/*  */

/* ARGSUSED */

static	do_type1 (yp, top, level, id, var, action2, direction)
YP yp;
int	top,
	level;
char   *id,
	   *var,
	   *action2;
int	direction;
{
	int	    i;
	char   *cp,
		   *ep,
		   buffer[BUFSIZ],
		   varbuf[BUFSIZ];
	YP	    y;
	YV	    yv;
	YT	    yt;

	printf ("%*s", level * 4, "");

	if (yp -> yp_flags & YP_ID) {
		printf ("%s", yp -> yp_id);
		if (!(yp -> yp_flags & YP_TAG))
			printf ("\n%*s", ++level * 4, "");
	}

	if (yp -> yp_flags & YP_TAG) {
		yt = yp -> yp_tag;
		printf ("[%s%d]\n", classes[yt -> yt_class], val2int (yt -> yt_value));
		level++;
		printf ("%*s", level * 4, "");
	}
	if (yp -> yp_flags & YP_OPTIONAL && yp -> yp_varexp) {
		if ((ep = index (yp -> yp_varexp, ' ')) == NULL)
			yyerror ("Bug in varexp!");

		sprintf (varbuf, "%*.*s", ep - yp -> yp_varexp,
				 ep - yp -> yp_varexp, yp -> yp_varexp);
	}

	switch (yp -> yp_code) {
	case YP_BOOL:
		if ((yp -> yp_flags & (YP_OPTIONAL|YP_DEFAULT)) &&
				direction == YP_DECODER) {
			if (!top && !(yp -> yp_flags & (YP_ID | YP_TAG)))
				printf ("dummy-for-default\n%*s", ++level * 4, "");
			if (yp -> yp_flags & YP_OPTIONAL)
				printf ("%%{ %s -> optionals |= %s; %%}\n%*s",
						varbuf, yp -> yp_optcontrol, level * 4, "");
			else
				printf ("%%{ %s%s = %d; %%}\n%*s",
						var, SVAL (yp -> yp_varexp),
						val2int (yp -> yp_default) ? 1 : 0, level * 4, "");
		}
		break;

	case YP_INT:
		if ((yp -> yp_flags & (YP_OPTIONAL|YP_DEFAULT)) &&
				direction == YP_DECODER) {
			if (!top && !(yp -> yp_flags & (YP_ID | YP_TAG)))
				printf ("dummy-for-default\n%*s", ++level * 4, "");
			if (yp -> yp_flags & YP_OPTIONAL)
				printf ("%%{ %s -> optionals |= %s; %%}\n%*s",
						varbuf, yp -> yp_optcontrol, level * 4, "");
			else
				printf ("%%{ %s%s = %d; %%}\n%*s",
						var, SVAL (yp -> yp_varexp),
						val2int (yp -> yp_default), level * 4, "");
		}
		break;

	case YP_INTLIST:
	case YP_ENUMLIST:
		if ((yp -> yp_flags & (YP_OPTIONAL|YP_DEFAULT)) &&
				direction == YP_DECODER) {
			if (!top && !(yp -> yp_flags & (YP_ID | YP_TAG)))
				printf ("dummy-for-default\n%*s", ++level * 4, "");
			if (yp -> yp_flags & YP_OPTIONAL)
				printf ("%%{ %s -> optionals |= %s; %%}\n%*s",
						varbuf, yp -> yp_optcontrol, level * 4, "");
			else
				printf ("%%{ %s%s = %d; %%}\n%*s",
						var, SVAL (yp -> yp_varexp), dfl2int (yp),
						level * 4, "");
		}
		break;

	case YP_REAL:
		if ((yp -> yp_flags & (YP_OPTIONAL|YP_DEFAULT)) &&
				direction == YP_DECODER) {
			if (!top && !(yp -> yp_flags & (YP_ID | YP_TAG)))
				printf ("dummy-for-default\n%*s", ++level * 4, "");
			if (yp -> yp_flags & YP_OPTIONAL)
				printf ("%%{ %s -> optionals |= %s; %%}\n%*s",
						varbuf, yp -> yp_optcontrol, level * 4, "");
			else
				printf ("%%{ %s%s = %g; %%}\n%*s",
						var, SVAL (yp -> yp_varexp),
						val2real (yp -> yp_default), level * 4, "");
		}
		break;

	case YP_NULL:
		if ((yp -> yp_flags & YP_OPTIONAL) && direction == YP_DECODER) {
			if (!top && !(yp -> yp_flags & (YP_ID | YP_TAG)))
				printf ("dummy-for-default\n%*s", ++level * 4, "");
			printf ("%%{ %s -> optionals |= %s; %%}\n%*s",
					varbuf, yp -> yp_optcontrol, level * 4, "");
		}
		break;
	}

	if ((yp -> yp_flags & (YP_TAG | YP_IMPLICIT)) == (YP_TAG | YP_IMPLICIT))
		printf ("IMPLICIT ");
	if (yp -> yp_flags & YP_BOUND)
		printf ("%s < ", yp -> yp_bound);
	if (yp -> yp_flags & YP_COMPONENTS)
		printf ("COMPONENTS OF ");
	if (yp -> yp_flags & YP_ENCRYPTED)
		printf ("ENCRYPTED ");

	switch (yp -> yp_code) {
	case YP_BOOL:
		printf ("BOOLEAN");
		switch (direction) {
		case YP_ENCODER:
		case YP_DECODER:
			printf (top ? "\n%*s[[b %s -> %s ]]" : "\n%*s[[b %s%s ]]",
					level * 4, "", var, SVAL (yp -> yp_varexp));
			break;
		}
		break;

	case YP_INT:
		printf ("INTEGER");
		switch (direction) {
		case YP_ENCODER:
		case YP_DECODER:
			printf (top ? "\n%*s[[i %s -> %s ]]" : "\n%*s[[i %s%s ]]",
					level * 4, "", var, SVAL (yp -> yp_varexp));
			break;
		}
		break;

	case YP_INTLIST:
	case YP_ENUMLIST:
		if (yp -> yp_code == YP_ENUMLIST)
			printf ("ENUMERATED");
		else
			printf ("INTEGER");
		switch (direction) {
		case YP_ENCODER:
		case YP_DECODER:
			printf (top ? "\n%*s[[i %s -> %s ]]\n%*s{\n"
					: "\n%*s[[i %s%s ]]\n%*s{\n",
					level * 4, "", var, SVAL (yp -> yp_varexp),
					level * 4, "");
			break;

		default:
			printf (" {\n");
			break;
		}
		level++;
		for (yv = yp -> yp_value; yv; yv = yv -> yv_next)
			printf ("%*s%s(%d)%s\n", level * 4, "", yv -> yv_named,
					val2int (yv), yv -> yv_next ? "," : "");
		level--;
		printf ("%*s}", level * 4, "");
		break;

	case YP_BIT:
		printf ("BIT STRING");
		switch (direction) {
		case YP_ENCODER:
			printf ("\n%*s[[x bit_parm = bitstr2strb (%s%s, &len) $ len]]",
					level * 4, "", var, SVAL (yp -> yp_varexp));
			printf ("\n%*s%%{\n%*sfree (bit_parm);\n", level * 4, "",
					(level + 1) * 4, "");
			if (action2)
				printf ("%*s%s\n", (level + 1) * 4, "", action2);
			printf ("%*s%%}\n", level * 4, "");
			break;

		case YP_DECODER:
			balloc (yp, var, action2, level);
			break;
		}
		break;

	case YP_BITLIST:
		printf ("BIT STRING");
		switch (direction) {
		case YP_ENCODER:
			printf ("\n%*s[[x bit_parm = bitstr2strb (%s%s, &len) $ len]]\n%*s{\n",
					level * 4, "", var, SVAL (yp -> yp_varexp),
					level * 4, "");
			break;

		case YP_DECODER:
		default:
			printf (" {\n");
			break;
		}
		level++;
		for (yv = yp -> yp_value; yv; yv = yv -> yv_next)
			printf ("%*s%s(%d)%s\n", level * 4, "", yv -> yv_named,
					val2int (yv), yv -> yv_next ? "," : "");
		level--;
		printf ("%*s}", level * 4, "");
		switch (direction) {
		case YP_DECODER:
			balloc (yp, var, action2, level);
			break;

		case YP_ENCODER:
			printf ("\n%*s%%{\n%*sfree (bit_parm);\n", level * 4, "",
					(level + 1) * 4, "");
			if (action2)
				printf ("%*s%s\n", (level + 1) * 4, "", action2);
			printf ("%*s%%}\n", level * 4, "");
			break;
		}
		break;

	case YP_OCT:
		printf ("OCTET STRING");
		switch (direction) {
		case YP_ENCODER:
			printf ("\n%*s[[q %s%s ]]", level * 4, "",
					var, SVAL (yp -> yp_varexp));
			break;

		case YP_DECODER:
			printf ("\n%*s[[q %s%s ]]", level * 4, "",
					var, SVAL (yp -> yp_varexp));
			break;
		}
		break;

	case YP_REAL:
		printf ("REAL");
		printf (top ? "\n%*s[[r %s -> %s ]]" : "\n%*s[[r %s%s ]]",
				level * 4, "", var, SVAL (yp -> yp_varexp));
		break;

	case YP_NULL:
		printf ("NULL");
		break;

	case YP_SEQ:
	case YP_SET:
	case YP_ANY:
		printf ("%s", tags[yp -> yp_code]);
		switch (direction) {
		case YP_ENCODER:
		case YP_DECODER:
			printf ("\n%*s[[a %s%s ]]",
					level * 4, "", var, SVAL (yp -> yp_varexp));
			break;
		}
		break;

	case YP_SEQTYPE:
	case YP_SETTYPE:
		ep = yp -> yp_code != YP_SETTYPE ? "element" : "member";
		printf ("%s\n", tags [yp -> yp_code]);
		switch (direction) {
		case YP_ENCODER:
			if ((y = yp -> yp_type) -> yp_declexp) {
				printf ("%*s%%{ %s = %s; %%}\n",
						(level + 1) * 4, "", y -> yp_declexp,
						SVAL (y -> yp_varexp));
			}
			if (h2flag) {
				if (top) {
					printf ("%*s<<n_parm = 0; ", (level + 1) * 4, "");
					printf ("n_parm < parm -> nelem; n_parm++>>\n");
				} else {
					printf ("%*s<<n_%s = 0;\n%*sn_%s < %s -> nelem;\n",
							(level + 1) * 4, "", yp -> yp_declexp,
							(level + 3) * 4, "", yp -> yp_declexp,
							yp -> yp_declexp);
					printf ("%*sn_%s++>>\n",
							(level + 3) * 4, "", yp -> yp_declexp);
				}
			} else {
				if (top)
					printf ("%*s<<; parm; parm = parm -> next>>\n",
							(level + 1) * 4, "");
				else
					printf ("%*s<<%s = %s%s;\n%*s%s;\n%*s%s = %s -> next>>\n",
							(level + 1) * 4, "", yp -> yp_declexp,
							var, SVAL (yp -> yp_varexp),
							(level + 3) * 4, "", yp -> yp_declexp,
							(level + 3) * 4, "", yp -> yp_declexp,
							yp -> yp_declexp);
			}
			break;

		case YP_DECODER:
			if (h2flag) {
				y = yp -> yp_type;
				xalloc (y, 0, level + 2, y -> yp_declexp,
						y -> yp_declexp, 1);
			} else
				xalloc (yp, 0, level + 1,
						top ? "parm" : yp -> yp_declexp,
						top ? modsym (mymodule, eval, "type")
						: yp -> yp_declexp, 1);
			break;
		}
		do_type1 (yp -> yp_type, 0, level + 1, ep, "", NULLCP, direction);
		switch (direction) {
		case YP_DECODER:
			printf ("\n%*s%%{ ", (level + 1) * 4, "");
			if (h2flag)
				printf ("n_%s++;", top ? "parm" : yp -> yp_declexp);
			else if (top)
				printf ("parm = &((*parm) -> next);");
			else
				printf ("%s = &((*%s) -> next);",
						yp -> yp_declexp, yp -> yp_declexp);
			if (action2)
				printf (" %s", action2);
			printf (" %%}");
			break;
		}
		break;

	case YP_SEQLIST:
	case YP_SETLIST:
		ep = yp -> yp_code != YP_SETLIST ? "element" : "member";
		printf ("%s", tags [yp -> yp_code]);
		printf ("\n%*s%%{\n", (level + 1) * 4, "");
		if (direction == YP_DECODER)
			xalloc (yp, 1, level + 2, yp -> yp_declexp,
					yp -> yp_declexp, 0);
		for (y = yp -> yp_type; y; y = y -> yp_next) {
			if (y -> yp_declexp)
				switch (direction) {
				case YP_ENCODER:
					printf ("%*s%s = %s;\n",
							(level + 2) * 4, "",
							y -> yp_declexp, y -> yp_varexp);
					break;

				case YP_DECODER:
					printf ("%*s%s = &(%s);\n",
							(level + 2) * 4, "",
							y -> yp_declexp, y -> yp_varexp);
					break;
				}
			if (direction == YP_DECODER &&
					y -> yp_flags & YP_DEFAULT) {
				prime_default (y, level + 2);
			}
		}
		printf ("%*s%%}\n%*s{\n", (level + 1) * 4, "",
				level * 4, "");

		if (!hflag || !(y = yp -> yp_type) || y -> yp_next) {
			var = "";
			top = 0;
		}
		for (y = yp -> yp_type; y; y = y -> yp_next) {
			do_type1 (y, top,
					  level + ((y -> yp_flags & (YP_ID | YP_TAG)) ? 1 : 2),
					  ep, var, NULLCP, direction);
			printf ("%s\n", y -> yp_next ? ",\n" : "");
		}
		printf ("%*s}", level * 4, "");
		break;

	case YP_CHOICE:
		printf ("CHOICE");
		if (!hflag || !(y = yp -> yp_type) || y -> yp_next)
			var = "";
		i = 0;
		for (y = yp -> yp_type; y; y = y -> yp_next)
			if (y -> yp_declexp)
				i++;
		switch (direction) {
		case YP_ENCODER:
			if (i) {
				printf ("\n%*s%%{\n", (level + 1) * 4, "");
				for (y = yp -> yp_type; y; y = y -> yp_next)
					if (y -> yp_declexp)
						printf ("%*s%s = %s;\n", (level + 2) * 4, "",
								y -> yp_declexp, y -> yp_varexp);
				printf ("%*s%%}\n%*s", (level + 1) * 4, "",
						(level + 1) * 4 - 1, "" );
			}
			if (*var)
				printf (" <<1>>");
			else if (top)
				printf (" <<parm -> offset>>");
			else
				printf (" <<%s -> offset>>",
						yp -> yp_declexp);
			printf (i ? "\n%*s{\n" : " {\n", level * 4, "");
			break;

		case YP_DECODER:
			printf ("\n");
			xalloc (yp, 0, level + 1, yp -> yp_declexp,
					yp -> yp_declexp, 1);
			printf ("%*s{\n", level * 4, "");
			break;

		default:
			printf (" {\n");
			break;
		}
		if (direction == YP_DECODER) {
			sprintf (cp = buffer, "(*(%s)) -> offset = ",
					 top ? "parm" : yp -> yp_declexp);
			cp += strlen (cp);
		} else
			cp = NULL;
		if (!hflag || !(y = yp -> yp_type) || y -> yp_next)
			top = 0;
		else if (top)
			cp = NULL;
		for (y = yp -> yp_type; y; y = y -> yp_next) {
			if (cp)
				sprintf (cp, "%s;", y -> yp_offset);
			do_type1 (y, top, level + 1, "choice", var,
					  cp ? buffer : NULLCP, direction);
			printf ("%s\n", y -> yp_next ? ",\n" : "");
		}
		printf ("%*s}", level * 4, "");
		break;

	case YP_OID:
		printf ("OBJECT IDENTIFIER");
		switch (direction) {
		case YP_ENCODER:
		case YP_DECODER:
			printf ("\n%*s[[O %s%s ]]",
					level * 4, "", var, SVAL (yp -> yp_varexp));
			break;
		}
		break;

	case YP_IDEFINED:
		if (yp -> yp_module && strcmp (yp -> yp_module, mymodule))
			printf ("%s.", yp -> yp_module);
		printf ("%s", yp -> yp_identifier);
		switch (direction) {
		case YP_ENCODER:
			printf ("\n%*s[[p %s%s ]]",
					level * 4, "", var, SVAL (yp -> yp_varexp));
			break;

		case YP_DECODER:
			printf ("\n%*s[[p &(%s%s)]]",
					level * 4, "", var, SVAL (yp -> yp_varexp));
			break;
		}
		break;

	default:
		myyerror ("unknown type: %d", yp -> yp_code);
	}

	if (action2)
		switch (yp -> yp_code) {
		case YP_BIT:
		case YP_BITLIST:
			if (direction == YP_ENCODER)
				break;
		case YP_SEQTYPE:
		case YP_SETTYPE:
			if (direction == YP_DECODER)
				break;
		/* else fall */

		default:
			printf ("\n%*s%%{ %s %%}", level * 4, "", action2);
			break;
		}

	if (yp -> yp_flags & YP_OPTIONAL) {
		printf ("\n%*sOPTIONAL", level * 4, "");

		if (direction == YP_ENCODER)
			switch (yp -> yp_code) {
			case YP_BOOL:
			case YP_INT:
			case YP_INTLIST:
			case YP_ENUMLIST:
			case YP_NULL:
			case YP_REAL:
				printf (" <<%s -> optionals & %s >>",
						varbuf, yp -> yp_optcontrol);
			default:
				break;

			case YP_BIT:
			case YP_BITLIST:
			case YP_OCT:
			case YP_SEQ:
			case YP_SEQTYPE:
			case YP_SEQLIST:
			case YP_SET:
			case YP_SETTYPE:
			case YP_SETLIST:
			case YP_CHOICE:
			case YP_ANY:
			case YP_OID:
			case YP_IDEFINED:
				printf (" <<%s%s>>", var, SVAL (yp -> yp_varexp));
				break;
			}
	} else if (yp -> yp_flags & YP_DEFAULT) {
		printf ("\n%*sDEFAULT ", level * 4, "");
		val2prf (yp -> yp_default, level + 2);

		if (direction == YP_ENCODER)
			switch (yp -> yp_code) {
			case YP_BOOL:
				printf (" <<%s%s%s>>",
						val2int (yp -> yp_default) ? "!" : "",
						var, SVAL (yp -> yp_varexp));
				break;

			case YP_INT:
			case YP_INTLIST:
			case YP_ENUMLIST:
				printf (" <<%s%s != %d>>", var, SVAL (yp -> yp_varexp),
						dfl2int (yp));
				break;

			case YP_REAL:
				printf (" << %s%s != %g >>",
						var, SVAL (yp -> yp_varexp),
						val2real (yp -> yp_default));
				break;

			case YP_NULL:
			default:
				break;

			case YP_BIT:
			case YP_BITLIST:
			case YP_OCT:
			case YP_SEQ:
			case YP_SEQTYPE:
			case YP_SEQLIST:
			case YP_SET:
			case YP_SETTYPE:
			case YP_SETLIST:
			case YP_CHOICE:
			case YP_ANY:
			case YP_OID:
			case YP_IDEFINED:
				printf (" <<%s%s>>", var, SVAL (yp -> yp_varexp));
				break;
			}
	}

	if (direction == YP_ENCODER
			&& yp -> yp_varexp
			&& (cp = index (yp -> yp_varexp, ' '))
			&& strncmp (cp + 1, "-> ", 3) == 0) {
		*cp = NULL;
		sprintf (buffer, "(*%s) -> %s", yp -> yp_varexp, cp + 4);
		yp -> yp_varexp = new_string (buffer);
	}
}

/*    TYPE HANDLING */

YP  lookup_type (mod, id)
char *mod,
	 *id;
{
	SY	    sy;

	for (sy = mysymbols; sy; sy = sy -> sy_next) {
		if (mod) {
			if (strcmp (sy -> sy_module, mod))
				continue;
		} else if (strcmp (sy -> sy_module, mymodule)
				   && strcmp (sy -> sy_module, "UNIV"))
			continue;

		if (strcmp (sy -> sy_name, id) == 0)
			return sy -> sy_type;
	}

	return NULLYP;
}

/*  */

pepsy (yp, top, level, id, val, var, arrayflg)
YP	yp;
int	top,
	level,
	arrayflg;
char   *id,
	   *val,
	   *var;
{
	int    i,
		   j;
	char  *bp;
	char   *cp,
		   *dp,
		   *ep,
		   *newid,
		   buf1[BUFSIZ],
		   buf2[BUFSIZ],
		   buf3[BUFSIZ];
	YP	    y;
	YV	    yv;

	strcpy (bp = buf2, var);
	bp += strlen (bp);
	/* Preserve the name of the field */
	yp->yp_varexp = new_string(yp -> yp_ptrname ? yp -> yp_ptrname : id);

	switch (yp -> yp_code) {
	case YP_BOOL:
		if (aflag)
			printag (yp, level + 4, NULLCP);
		fprintf (fdef, "%*schar    %s;\n", level * 4, "",
				 array(id, arrayflg));
		break;

	case YP_INT:
	case YP_INTLIST:
	case YP_ENUMLIST:
		if (aflag)
			printag (yp, level + 4, NULLCP);
		fprintf (fdef, "%*sinteger     %s;\n", level * 4, "",
				 array(id, arrayflg));
		if (yp -> yp_code == YP_INT)
			break;
		for (yv = yp -> yp_value; yv; yv = yv -> yv_next) {
			modsym_aux (yv -> yv_named, buf1);
			fprintf (fdef, "#define\t%s_%s\t%d\n",
					 modsym (mymodule, top ? eval : id, "int"),
					 buf1, val2int (yv));
		}
		break;

	case YP_BIT:
	case YP_BITLIST:
		if (!top) {
			if (aflag)
				printag (yp, level + 4, NULLCP);
			fprintf (fdef, "%*sPE      %s;\n", level * 4, "",
					 array(id, arrayflg));
		}
		if (yp -> yp_code != YP_BITLIST)
			break;
		i = -1;
		for (yv = yp -> yp_value; yv; yv = yv -> yv_next)
			if ((j = val2int (yv)) < 0)
				pyyerror (yp, "invalid bit number in BIT STRING");
			else if (j > i)
				i = j;
		if (i < sizeof (int) * 8) {		/* NBBY */
			fprintf (fdef, "#define\t%s\t\"\\020",
					 modsym (mymodule, top ? eval : id, "bits"));
			for (yv = yp -> yp_value; yv; yv = yv -> yv_next)
				if (yv -> yv_flags & YV_NAMED)
					fprintf (fdef, "\\0%o%s",
							 val2int (yv) + 1, yv -> yv_named);
				else
					fprintf (fdef, "\\0%oBIT%d",
							 val2int (yv) + 1, val2int (yv));
			fprintf (fdef, "\"\n");
		}
		for (yv = yp -> yp_value; yv; yv = yv -> yv_next) {
			modsym_aux (yv -> yv_named, buf1);
			fprintf (fdef, "#define\t%s_%s\t%d\n",
					 modsym (mymodule, top ? eval : id, "bit"),
					 buf1, val2int (yv));
		}
		break;

	case YP_REAL:
		if (aflag)
			printag (yp, level + 4, NULLCP);
		fprintf (fdef, "%*sdouble    %s;\n", level * 4, "",
				 array(id, arrayflg));
		/* yp -> yp_varexp = new_string (buf2); */
		break;

	case YP_OCT:
		if (!top) {
			if (aflag)
				printag (yp, level + 4, NULLCP);
			fprintf (fdef, "%*sstruct qbuf *%s;\n", level * 4, "",
					 array(id, arrayflg));
		}
		break;

	case YP_NULL:
		if (aflag)
			printag (yp, level + 4, NULLCP);
		fprintf (fdef, "%*schar    %s;\n", level * 4, "",
				 array(id, arrayflg));
		break;

	case YP_SEQ:
	case YP_SET:
	case YP_ANY:
		if (!top) {
			if (aflag)
				printag (yp, level + 4, NULLCP);
			fprintf (fdef, "%*sPE      %s;\n", level * 4, "",
					 array(id, arrayflg));
		}
		break;

	case YP_SEQTYPE:
	case YP_SETTYPE:
		ep = yp -> yp_code != YP_SETTYPE ? "element" : "member";
		if ((cp = rindex (buf2, ' ')) && *++cp) {
			if ((dp = rindex (cp, '.')) && *++dp)
				cp = dp;
			sprintf (dp = buf1, "%*.*s",
					 cp - buf2, cp - buf2, buf2);
			dp += strlen (dp);
		} else {
			strcpy (buf1, buf2);
			dp = NULL;
		}
		newid = yp -> yp_ptrname ? yp -> yp_ptrname : id;
		if (h2flag && top)
			fprintf (fdef, "%*sinteger\tnelem;\n", level * 4, "");
		if (!top) {
			if (yp -> yp_structname)
				id = yp -> yp_structname;
			else if (!Hflag)
				id = gensym (ep, NULLCP);
			if (aflag)
				printag (yp, level + 4, NULLCP);
			fprintf (fdef, "%*sstruct %s {\n", level * 4, "", id);
			if (h2flag)
				fprintf (fdef, "%*sinteger\tnelem;\n", (level + 1) * 4, "");
		}
		if (dp)
			strcpy (dp, newid);
		strcpy (bp = buf2, id);
		bp += strlen (bp);

		if (!top)
			yp -> yp_declexp = new_string (id);

		if (dp)
			strcpy (dp, newid);
		if ((y = yp -> yp_type) -> yp_code == YP_IDEFINED && hflag) {
			modsym_aux (y -> yp_identifier, cp = buf3);
			if (h2flag) {
				cp += strlen(cp);
				sprintf (cp, "[n_%s]", PARVAL (yp->yp_declexp));
				cp = buf3;
			}
		} else {
			switch (y -> yp_code) {
			case YP_SEQLIST:
			case YP_SETLIST:
			case YP_SETTYPE:
			case YP_SEQTYPE:
			case YP_CHOICE:
			case YP_IDEFINED:
				cp = gensym (ep, h2flag ? PARVAL(yp->yp_declexp) : NULLCP);
				break;
			default:
				cp = gensym (ep, NULLCP);
				break;
			}
		}
		sprintf (bp, " -> %s", cp);
		level++;
		pepsy (y, 0, level, cp, ep, buf2, h2flag);
		*bp = NULL;
		if (y -> yp_code != YP_IDEFINED)
			free (cp);
		if (!h2flag)
			fprintf (fdef, "\n%*sstruct %s *next;\n", level * 4, "",
					 top ? modsym (mymodule, val, "type") : id);

		level--;

		strcpy (bp = buf2, var);
		bp += strlen (bp);

		if (!top) {
			fprintf (fdef, "%*s} *%s;\n", level * 4, "",
					 array(newid, arrayflg));
			if (!Hflag)
				free (id);
		}
		break;

	case YP_SEQLIST:
	case YP_SETLIST:
		ep = yp -> yp_code != YP_SETLIST ? "element" : "member";
		if ((cp = rindex (buf2, ' ')) && *++cp) {
			if ((dp = rindex (cp, '.')) && *++dp)
				cp = dp;
			sprintf (dp = buf1, "%*.*s",
					 cp - buf2, cp - buf2, buf2);
			dp += strlen (dp);
		} else {
			strcpy (buf1, buf2);
			dp = NULL;
		}
		newid = yp -> yp_ptrname ? yp -> yp_ptrname : id;
		if (!top) {
			if (yp -> yp_structname)
				id = yp -> yp_structname;
			else if (!Hflag)
				id = gensym (ep, NULLCP);
			if (aflag)
				printag (yp, level + 4, NULLCP);
			fprintf (fdef, "%*sstruct %s {\n", level * 4, "", id);

			if (dp)
				strcpy (dp, newid);

			strcpy (bp = buf2, id);
			bp += strlen (bp);
			yp -> yp_declexp = new_string (id);

			level++;
		}
		if (dp)
			strcpy (dp, newid);
		for (y = yp -> yp_type, i = 0; y; y = y -> yp_next) {
			if (y -> yp_flags & YP_OPTIONAL)
				switch (y -> yp_code) {
				case YP_BOOL:
				case YP_INT:
				case YP_INTLIST:
				case YP_ENUMLIST:
				case YP_REAL:
				case YP_NULL: {
					char obuf[BUFSIZ];

					if (i == 0)
						fprintf (fdef, "%*sinteger     optionals;\n",
								 level * 4, "");
					if (y -> yp_flags & YP_ID)
						modsym_aux (y -> yp_id, cp = buf1);
					else {
						cp = gensym (ep, NULLCP);
						strcpy (buf1, cp);
						free (cp);
						cp = buf1;
					}
					sprintf (obuf, "%s_%s",
							 modsym (mymodule,
									 top ? eval : id,
									 "opt"), cp);
					fprintf (fdef, "#define\t%s (0%08o)\n", obuf,
							 1 << i);
#ifdef ASN1_OUTPUT
					y -> yp_optcontrol = new_string (obuf);
					y -> yp_flags |= YP_OPTCONTROL;
#endif

					i ++;
					if (i >= 8 * sizeof (int))
						yyerror ("too many optionals in structure");
				}
				break;
				}
		}
		if (i > 0)  fprintf (fdef, "\n");

		for (y = yp -> yp_type, i = 1; y; y = y -> yp_next, i++) {
			if (y -> yp_flags & YP_ID)
				modsym_aux (y -> yp_id, cp = buf1);
			else
				cp = gensym (ep, NULLCP);
			sprintf (bp, " -> %s", cp);
			pepsy (y, 0, level, cp, ep, buf2, 0);
			*bp = NULL;
			if (!(y -> yp_flags & YP_ID))
				free (cp);
			if (y -> yp_next)
				fprintf (fdef, "\n");
		}
		if (i == 1)
			fprintf (fdef, "%*schar    dummy;\n", level * 4, "");
		if (!top) {
			level--;

			strcpy (bp = buf2, var);
			bp += strlen (bp);

			fprintf (fdef, "%*s} *%s;\n", level * 4, "",
					 array(newid, arrayflg));
			if (!Hflag)
				free (id);
		}
		break;

	case YP_CHOICE:
		if ((cp = rindex (buf2, ' ')) && *++cp) {
			if ((dp = rindex (cp, '.')) && *++dp)
				cp = dp;
			sprintf (dp = buf1, "%*.*s",
					 cp - buf2, cp - buf2, buf2);
			dp += strlen (dp);
		} else {
			strcpy (buf1, buf2);
			dp = NULL;
		}
		newid = yp -> yp_ptrname ? yp -> yp_ptrname : id;
		if (!top) {
			if (yp -> yp_structname)
				id = yp -> yp_structname;
			else if (!Hflag)
				id = gensym ("choice", NULLCP);
			if (aflag)
				printag (yp, level + 4, NULLCP);
			fprintf (fdef, "%*sstruct %s {\n", level * 4, "", id);

			if (dp)
				strcpy (dp, newid);
			strcpy (bp = buf2, id);
			bp += strlen (bp);
			yp -> yp_declexp = new_string (id);

			level++;
		}
		if (dp)
			strcpy (dp, newid);
		fprintf (fdef, "%*sint         offset;\n", level * 4, "");
		if (top)
			cp = modsym (mymodule, val, "type");
		else
			cp = id;
		sprintf (ep = buf1, "%s_", cp);
		ep += strlen (ep);
		for (y = yp -> yp_type, i = 1; y; y = y -> yp_next, i++) {
			if (y -> yp_flags & YP_ID)
				modsym_aux (y -> yp_id, ep);
			else
				sprintf (ep, "%d", i);
			y -> yp_offset = new_string (buf1);
			fprintf (fdef, "#define\t%s\t%d\n", y -> yp_offset, i);
		}
		fprintf (fdef, "\n%*sunion {\n", level * 4, "");
		level++;
		for (y = yp -> yp_type; y; y = y -> yp_next) {
			char	*t;

			if (y -> yp_flags & YP_ID)
				modsym_aux (y -> yp_id, cp = buf1);
			else
				cp = gensym ("choice", NULLCP);
			sprintf (bp, " -> un.%s", cp);
			pepsy (y, 0, level, cp, "choice", buf2, 0);
			/* prefix yp_varexp (field name) with un. so we will generate offsets that
			 * allow for the union all the CHOICE fields are imbedded in
			 */
			t = y->yp_varexp;
			y->yp_varexp = my_strcat("un.",  t);
			free(t);

			*bp = NULL;
			if (!(y -> yp_flags & YP_ID))
				free (cp);
			if (y -> yp_next)
				fprintf (fdef, "\n");
		}
		level--;
		fprintf (fdef, "%*s}       un;\n", level * 4, "");
		if (!top) {
			level--;

			strcpy (bp = buf2, var);
			bp += strlen (bp);

			fprintf (fdef, "%*s} *%s;\n", level * 4, "",
					 array(newid, arrayflg));
			if (!Hflag)
				free (id);
		}
		break;

	case YP_OID:
		if (!top) {
			if (aflag)
				printag (yp, level + 4, NULLCP);
			fprintf (fdef, "%*sOID     %s;\n", level * 4, "",
					 array(id, arrayflg));
		}
		break;

	case YP_IDEFINED:
		if (aflag)
			printag (yp, level + 4, NULLCP);
		{
			/* Predefined Universal Type */
			struct univ_typ *p;
			if ((p = univtyp(yp->yp_identifier))) {
				fprintf (fdef, "%*s%s%s;\n", level * 4, "",
						 p->univ_data, array(id, arrayflg));
				/*
				        if (fflag)
				            ferr(1, "pepsy:YP_IDEFINED:fflag not implemented\n");
				*/
				break;
			}
		}
		fprintf (fdef, "%*sstruct %s *%s;\n", level * 4, "",
				 modsym (yp -> yp_module, yp -> yp_identifier, "type"),
				 array(id, arrayflg));
		break;

	default:
		myyerror ("unknown type: %d", yp -> yp_code);
	}
}

/*  */

static	printag (yp, level, pullup)
YP	yp;
int	level;
char   *pullup;
{
	fprintf (fdef, "%*s/* ", level * 4, "");
	switch (yp -> yp_code) {
	case YP_IDEFINED:
		if (yp -> yp_module && strcmp (yp -> yp_module, mymodule))
			fprintf (fdef, "%s.", yp -> yp_module);
		fprintf (fdef, "%s", yp -> yp_identifier);
		break;

	default:
		fprintf (fdef, "%s", tags[yp -> yp_code]);
		break;
	}
	if (pullup)
		fprintf (fdef, " pulled up from %s", pullup);
	fprintf (fdef, " */\n");
}

/*  */

static	xalloc (yp, top, level, arg, type, brackets)
YP	yp;
int	top,
	level,
	brackets;
char   *arg,
	   *type;
{
	int	    didone;
	YP	    y;

	if (hflag && !arg && !type)
		return;

	didone = 0;

	if (arg && type) {
		if (brackets && !didone) {
			printf ("%*s%%{\n", level * 4, "");
			level++, didone++;
		}

		if (h2flag && (yp -> yp_code == YP_SEQTYPE ||
					   yp -> yp_code == YP_SETTYPE)) {
			printf ("%*s{\n%*sPE      xx_pe = prim2%s ($$);\n\n",
					level * 4, "", (level +1) * 4, "",
					yp -> yp_code == YP_SEQTYPE ? "seq" : "set");
			printf ("%*sn_%s = xx_pe -> pe_cardinal > 0 ",
					(level + 1) * 4, "", arg);
			printf ("? xx_pe -> pe_cardinal : 0;\n%*s}\n", level * 4, "");
			printf ("%*sif ((*(%s) = (struct %s *)\n",
					level * 4, "", arg, type);
			printf ("%*scalloc (1 + (unsigned) n_%s, sizeof **(%s)",
					(level + 2) * 4, "", arg, arg);
			printf (")) == ((struct %s *) 0)) {\n", type);
			printf ("%*sadvise (NULLCP, \"%%s\", PEPY_ERR_NOMEM);\n",
					(level + 1) * 4, "");
			printf ("%*sreturn NOTOK;\n%*s}\n", (level + 1) * 4, "",
					level * 4, "");
			printf ("%*s(*%s) -> nelem = n_%s;\n", level * 4, "", arg, arg);
			printf ("%*sn_%s = 0;\n", level * 4, "", arg);
		} else {
			printf ("%*sif ((*(%s) = (struct %s *)\n",
					level * 4, "", arg, type);
			printf ("%*scalloc (1, sizeof **(%s))) == ((struct %s *) 0)) {\n",
					(level + 2) * 4, "", arg, type);
			printf ("%*sadvise (NULLCP, \"%%s\", PEPY_ERR_NOMEM);\n",
					(level + 1) * 4, "");
			printf ("%*sreturn NOTOK;\n%*s}\n", (level + 1) * 4, "",
					level * 4, "");
		}
	}
	switch (yp -> yp_code) {
	case YP_SEQTYPE:
	case YP_SETTYPE:
		if (top) break;
	case YP_CHOICE:
	case YP_SEQLIST:
	case YP_SETLIST:
		for (y = yp -> yp_type; y; y = y -> yp_next)
			switch (y -> yp_code) {
			case YP_SEQTYPE:
			case YP_SETTYPE:
				if (h2flag && (yp -> yp_code == YP_SETLIST ||
							   yp -> yp_code == YP_SEQLIST)) {
					/* include allocation here - no chance later */
					if (brackets && !didone) {
						printf ("%*s%%{\n", level * 4, "");
						level++, didone++;
					}
					if (y -> yp_declexp)
						printf ("%*s%s = &(%s);\n", level * 4, "",
								y -> yp_declexp,
								y -> yp_varexp);
					xalloc (y, top, level, y -> yp_declexp,
							y -> yp_declexp, 0);
				}
			/* and continue ... */
			case YP_SEQLIST:
			case YP_SETLIST:
			case YP_CHOICE:
				if (brackets && !didone) {
					printf ("%*s%%{\n", level * 4, "");
					level++, didone++;
				}
				printf ("%*s%s = &(%s);\n",
						level * 4, "", y -> yp_declexp,
						y -> yp_varexp);
				break;
			}
		break;
	}

	if (brackets && didone) {
		level--;
		printf ("%*s%%}\n", level * 4, "");
	}
}


static	balloc (yp, var, action2, level)
YP	yp;
char   *var, *action2;
int	level;
{
	printf ("\n%*s%%{\n", level * 4, "");
	level++;

	printf ("%*sif ((%s%s = prim2bit (pe_cpy ($$))) == NULLPE) {\n",
			level * 4, "", var, SVAL (yp -> yp_varexp));
	printf ("%*sadvise (NULLCP, \"%%s\", PEPY_ERR_NOMEM);\n", (level + 1) * 4, "");
	printf ("%*sreturn NOTOK;\n%*s}\n", (level + 1) * 4, "", level * 4, "");

	if (action2)
		printf ("\n%*s%s\n", level * 4, "", action2);

	level--;
	printf ("%*s%%}", level * 4, "");
}

#ifdef	notdef
static	qalloc (yp, var, action2, level)
YP	yp;
char   *var,
	   *action2;
int	level;
{
	printf ("\n%*s%%{\n", level * 4, "");
	level++;

	printf ("%*sif ((%s%s = str2qb ($$, $$_len, 1)) == ((struct qbuf *) 0)) {\n",
			level * 4, "", var, SVAL (yp -> yp_varexp));
	printf ("%*sadvise (NULLCP, \"%%s\", PEPY_ERR_NOMEM);\n", (level + 1) * 4, "");
	printf ("%*sreturn NOTOK;\n%*s}\n", (level + 1) * 4, "", level * 4, "");

	if (action2)
		printf ("\n%*s%s\n", level * 4, "", action2);

	level--;
	printf ("%*s%%}", level * 4, "");
}
#endif

/*  */

/*
 * now we don't pull up things if they have actions associated with them
 * this should be okay for those who want exact posy equivalence and
 * yet keep those who want the pepy support able to have multiple
 * actions in CHOICE { CHOICE {} } and such like
 */
static	choice_pullup (yp, partial)
YP	yp;
int	partial;
{
	YP	   *x,
	 y,
	 z,
	 *z1,
	 z2,
	 z3;

	for (x = &yp -> yp_type; y = *x; x = &y -> yp_next) {
		if (y -> yp_flags & (YP_TAG | YP_BOUND))
			continue;

		switch (y -> yp_code) {
		case YP_IDEFINED:
			if ((z = lookup_type (y -> yp_module, y -> yp_identifier))
					== NULLYP
					|| z -> yp_code != YP_CHOICE)
				continue;
			if (z -> yp_control_act || z ->yp_bef_alist || z-> yp_aft_alist)
				continue;

			choice_pullup (z2 = copy_type (z), CH_FULLY);
			if (y -> yp_flags & YP_OPTIONAL) {
				z2 -> yp_flags |= YP_OPTIONAL;
				partial = 1;
			}
			if (y -> yp_flags & YP_DEFAULT) {
				z2 -> yp_flags |= YP_DEFAULT;
				z2 -> yp_default = copy_value (y -> yp_default);
				partial = 1;
			}
			goto patch;

		case YP_CHOICE:
			if (y -> yp_control_act || y ->yp_bef_alist || y-> yp_aft_alist)
				continue;

			choice_pullup (z2 = copy_type (y), CH_FULLY);
patch:
			;
			if (partial) {
				*x = z2;
				z2 -> yp_next =  y -> yp_next;
				continue;
			}
			break;

		default:
			continue;
		}
		z = z3 = z2 -> yp_type;
		for (z1 = &z -> yp_next; z2 = *z1; z1 = &z2 -> yp_next)
			z3 = z2;
		*z1 = y -> yp_next;
		*x = z;
		y = z3;
	}
}

/*  */

static	components_pullup (yp)
YP	yp;
{
	YP    *x,
	y,
	z,
	z1,
	z2;

	for (x = &yp -> yp_type; y = *x; x = &y -> yp_next) {
		if (!(y -> yp_flags & YP_COMPONENTS))
			continue;

		switch (y -> yp_code) {
		case YP_SEQLIST:
		case YP_SETLIST:
			z = y;
			break;

		case YP_IDEFINED:
			if ((z = lookup_type (y -> yp_module, y -> yp_identifier))
					== NULLYP) {
				warning ("COMPONENTS OF target \"%s\" is undefined",
						 y -> yp_identifier);
				continue;
			}
			break;
		}
		if (yp -> yp_code != z -> yp_code) {
			warning ("COMPONENTS OF target \"%s\" is wrong type, should be %s",
					 y -> yp_code == YP_IDEFINED ? y -> yp_identifier
					 : y -> yp_id ? y -> yp_id
					 : "",
					 yp -> yp_code == YP_SEQLIST ? "SEQUENCE" : "SET");
			continue;
		}
		if (z -> yp_type == NULLYP)
			continue;
		components_pullup (z = copy_type (z));
		*x = z2 = z -> yp_type;
		for (x = &z -> yp_type; z1 = *x; x = &z1 -> yp_next)
			z2 = z1;
		*x = y -> yp_next;
		y = z2;
	}
}

/*    VALUE HANDLING */

static int  val2int (yv)
YV	yv;
{
	switch (yv -> yv_code) {
	case YV_BOOL:
	case YV_NUMBER:
		return yv -> yv_number;

	case YV_STRING:
		yyerror ("need an integer, not a string");

	case YV_IDEFINED:
	case YV_IDLIST:
		yyerror ("haven't written symbol table for values yet");

	case YV_NULL:
		yyerror ("need an integer, not NULL");

	default:
		myyerror ("unknown value: %d", yv -> yv_code);
	}
	/* NOTREACHED */
}

static double  val2real (yv)
YV	yv;
{
	switch (yv -> yv_code) {
	case YV_NUMBER:
		return yv -> yv_number;

	case YV_REAL:
		return yv -> yv_real;

	case YV_STRING:
		yyerror ("need an integer, not a string");

	case YV_IDEFINED:
	case YV_IDLIST:
		yyerror ("haven't written symbol table for values yet");

	case YV_NULL:
		yyerror ("need an integer, not NULL");

	default:
		myyerror ("unknown value: %d", yv -> yv_code);
	}
	/* NOTREACHED */
}

/*  */

static	val2prf (yv, level)
YV	yv;
int	level;
{
	YV    y;

	if (yv -> yv_flags & YV_ID)
		printf ("%s ", yv -> yv_id);

	if (yv -> yv_flags & YV_TYPE)	/* will this REALLY work??? */
		do_type1 (yv -> yv_type, 0, level, NULLCP, NULLCP, NULLCP, NULL);

	switch (yv -> yv_code) {
	case YV_BOOL:
		printf (yv -> yv_number ? "TRUE" : "FALSE");
		break;

	case YV_NUMBER:
		if (yv -> yv_named)
			printf ("%s", yv -> yv_named);
		else
			printf ("%d", yv -> yv_number);
		break;

	case YV_REAL:
		dump_real (yv -> yv_real);
		break;

	case YV_STRING:
		printf ("\"%s\"", yv -> yv_string);
		break;

	case YV_IDEFINED:
		if (yv -> yv_module)
			printf ("%s.", yv -> yv_module);
		printf ("%s", yv -> yv_identifier);
		break;

	case YV_IDLIST:
	case YV_VALIST:
		printf ("{");
		for (y = yv -> yv_idlist; y; y = y -> yv_next) {
			printf (" ");
			val2prf (y, level + 1);
			printf (y -> yv_next ? ", " : " ");
		}
		printf ("}");
		break;

	case YV_NULL:
		printf ("NULL");
		break;

	default:
		myyerror ("unknown value: %d", yv -> yv_code);
		/* NOTREACHED */
	}
}
static
dump_real (double r) {
#ifndef	BSD44
	extern char *ecvt ();
	char	*cp;
	char	sbuf[128];
	int	decpt, sign;

	cp = ecvt (r, 20, &decpt, &sign);
	strcpy (sbuf, cp);	/* cp gets overwritten by printf */
	printf ("{ %s%s, 10, %d }", sign ? "-" : "", sbuf,
			decpt - strlen (sbuf));
#else
	char   *cp,
		   *dp,
		   *sp;
	char    sbuf[128];

	sprintf (sbuf, "%.19e", r);
	if (*(dp = sbuf) == '-')
		sp = "-", dp++;
	else
		sp = "";

	if (dp[1] != '.' || (cp = index (dp, 'e')) == NULL) {
		printf ("{ 0, 10, 0 } -- %s --", sbuf);
		return;
	}
	*cp++ = NULL;
	printf ("{ %s%c%s, 10, %d }",
			sp, *dp, dp + 2, atoi (cp) - strlen (dp + 2));
#endif
}

/*  */

static int  dfl2int (yp)
YP	yp;
{
	YV	    yv,
	 y;

	yv = yp -> yp_default;
	switch (yv -> yv_code) {
	case YV_BOOL:
	case YV_NUMBER:
		return yv -> yv_number;

	case YV_STRING:
		yyerror ("need an integer, not a string");

	case YV_REAL:
		yyerror ("need an integer, not a real");

	case YV_IDEFINED:
		for (y = yp -> yp_value; y; y = y -> yv_next)
			if (y -> yv_code == YV_NUMBER
					&& (y -> yv_flags & YV_NAMED)
					&& strcmp (yv -> yv_identifier, y -> yv_named) == 0)
				return y -> yv_number;
	/* and fall */

	case YV_IDLIST:
		yyerror ("haven't written symbol table for values yet");

	case YV_NULL:
		yyerror ("need an integer, not NULL");

	default:
		myyerror ("unknown value: %d", yv -> yv_code);
	}
	/* NOTREACHED */
}

/*    DEBUG */

print_type (yp, level)
YP	yp;
int	level;
{
	YP	    y;
	YV	    yv;

	if (yp == NULLYP)
		return;

	fprintf (stderr, "%*scode=0x%x flags=%s direction=0x%x\n", level * 4, "",
			 yp -> yp_code, sprintb (yp -> yp_flags, YPBITS),
			 yp -> yp_direction);
	fprintf (stderr,
			 "%*sintexp=\"%s\" strexp=\"%s\" prfexp=0%o declexp=\"%s\" varexp=\"%s\"\n",
			 level * 4, "", yp -> yp_intexp, yp -> yp_strexp, yp -> yp_prfexp,
			 yp -> yp_declexp, yp -> yp_varexp);
	if (yp -> yp_param_type)
		fprintf (stderr, "%*sparameter type=\"%s\"\n", level * 4, "",
				 yp -> yp_param_type);
	if (yp -> yp_action0)
		fprintf (stderr, "%*saction0 at line %d=\"%s\"\n", level * 4, "",
				 yp -> yp_act0_lineno, yp -> yp_action0);
	if (yp -> yp_action05)
		fprintf (stderr, "%*saction05 at line %d=\"%s\"\n", level * 4, "",
				 yp -> yp_act05_lineno, yp -> yp_action05);
	if (yp -> yp_action1)
		fprintf (stderr, "%*saction1 at line %d=\"%s\"\n", level * 4, "",
				 yp -> yp_act1_lineno, yp -> yp_action1);
	if (yp -> yp_action2)
		fprintf (stderr, "%*saction2 at line %d=\"%s\"\n", level * 4, "",
				 yp -> yp_act2_lineno, yp -> yp_action2);
	if (yp -> yp_action3)
		fprintf (stderr, "%*saction3 at line %d=\"%s\"\n", level * 4, "",
				 yp -> yp_act3_lineno, yp -> yp_action3);

	if (yp -> yp_flags & YP_TAG) {
		fprintf (stderr, "%*stag class=0x%x value=0x%x\n", level * 4, "",
				 yp -> yp_tag -> yt_class, yp -> yp_tag -> yt_value);
		print_value (yp -> yp_tag -> yt_value, level + 1);
	}

	if (yp -> yp_flags & YP_DEFAULT) {
		fprintf (stderr, "%*sdefault=0x%x\n", level * 4, "", yp -> yp_default);
		print_value (yp -> yp_default, level + 1);
	}

	if (yp -> yp_flags & YP_ID)
		fprintf (stderr, "%*sid=\"%s\"\n", level * 4, "", yp -> yp_id);

	if (yp -> yp_flags & YP_BOUND)
		fprintf (stderr, "%*sbound=\"%s\"\n", level * 4, "", yp -> yp_bound);

	if (yp -> yp_offset)
		fprintf (stderr, "%*soffset=\"%s\"\n", level * 4, "", yp -> yp_offset);

	switch (yp -> yp_code) {
	case YP_INTLIST:
	case YP_ENUMLIST:
	case YP_BITLIST:
		fprintf (stderr, "%*svalue=0x%x\n", level * 4, "", yp -> yp_value);
		for (yv = yp -> yp_value; yv; yv = yv -> yv_next) {
			print_value (yv, level + 1);
			fprintf (stderr, "%*s----\n", (level + 1) * 4, "");
		}
		break;

	case YP_SEQTYPE:
	case YP_SEQLIST:
	case YP_SETTYPE:
	case YP_SETLIST:
	case YP_CHOICE:
		fprintf (stderr, "%*stype=0x%x\n", level * 4, "", yp -> yp_type);
		for (y = yp -> yp_type; y; y = y -> yp_next) {
			print_type (y, level + 1);
			fprintf (stderr, "%*s----\n", (level + 1) * 4, "");
		}
		break;

	case YP_IDEFINED:
		fprintf (stderr, "%*smodule=\"%s\" identifier=\"%s\"\n",
				 level * 4, "", yp -> yp_module ? yp -> yp_module : "",
				 yp -> yp_identifier);
		break;

	default:
		break;
	}
}

/*  */

static	print_value (yv, level)
YV	yv;
int	level;
{
	YV	    y;

	if (yv == NULLYV)
		return;

	fprintf (stderr, "%*scode=0x%x flags=%s\n", level * 4, "",
			 yv -> yv_code, sprintb (yv -> yv_flags, YVBITS));

	if (yv -> yv_action)
		fprintf (stderr, "%*saction at line %d=\"%s\"\n", level * 4, "",
				 yv -> yv_act_lineno, yv -> yv_action);

	if (yv -> yv_flags & YV_ID)
		fprintf (stderr, "%*sid=\"%s\"\n", level * 4, "", yv -> yv_id);

	if (yv -> yv_flags & YV_NAMED)
		fprintf (stderr, "%*snamed=\"%s\"\n", level * 4, "", yv -> yv_named);

	if (yv -> yv_flags & YV_TYPE) {
		fprintf (stderr, "%*stype=0x%x\n", level * 4, "", yv -> yv_type);
		print_type (yv -> yv_type, level + 1);
	}

	switch (yv -> yv_code) {
	case YV_NUMBER:
	case YV_BOOL:
		fprintf (stderr, "%*snumber=0x%x\n", level * 4, "",
				 yv -> yv_number);
		break;

	case YV_STRING:
		fprintf (stderr, "%*sstring=\"%s\"\n", level * 4, "",
				 yv -> yv_string);
		break;

	case YV_IDEFINED:
		if (yv -> yv_flags & YV_BOUND)
			fprintf (stderr, "%*smodule=\"%s\" identifier=\"%s\"\n",
					 level * 4, "", yv -> yv_module, yv -> yv_identifier);
		else
			fprintf (stderr, "%*sbound identifier=\"%s\"\n",
					 level * 4, "", yv -> yv_identifier);
		break;

	case YV_IDLIST:
	case YV_VALIST:
		for (y = yv -> yv_idlist; y; y = y -> yv_next) {
			print_value (y, level + 1);
			fprintf (stderr, "%*s----\n", (level + 1) * 4, "");
		}
		break;

	default:
		break;
	}
}

/*    SYMBOLS */

static SY  new_symbol (encpref, decpref, prfpref, mod, id, type)
char  *encpref,
	  *decpref,
	  *prfpref,
	  *mod,
	  *id;
YP	type;
{
	SY    sy;

	if ((sy = (SY) calloc (1, sizeof *sy)) == NULLSY)
		yyerror ("out of memory");
	sy -> sy_encpref = encpref;
	sy -> sy_decpref = decpref;
	sy -> sy_prfpref = prfpref;
	sy -> sy_module = mod;
	sy -> sy_name = id;
	sy -> sy_type = type;

	return sy;
}


static SY
add_symbol (SY s1, SY s2) {
	SY	    sy;

	if (s1 == NULLSY)
		return s2;

	for (sy = s1; sy -> sy_next; sy = sy -> sy_next)
		continue;
	sy -> sy_next = s2;

	return s1;
}

/*    MODULES */

MD  lookup_module (module, oid)
char   *module;
OID	oid;
{
	MD	    md;

	for (md = mymodules; md; md = md -> md_next) {
		if (module && md -> md_module && strcmp (md -> md_module, module) == 0)
			return md;
		if (oid && md -> md_oid && oid_cmp(oid, md->md_oid) == 0)
			return md;
	}

	read_ph_file (module, oid);

	if ((md = (MD) calloc (1, sizeof *md)) == NULLMD)
		yyerror ("out of memory");
	md -> md_module = new_string (module);
	if (oid)
		md -> md_oid = oid_cpy(oid);
	else
		md -> md_oid = NULLOID;

	if (mymodules != NULLMD)
		md -> md_next = mymodules;

	return (mymodules = md);
}

/*    TYPES */

YP	new_type (code, lineno)
int	code;
int	lineno;
{
	YP    yp;

	if ((yp = (YP) calloc (1, sizeof *yp)) == NULLYP)
		yyerror ("out of memory");
	yp -> yp_code = code;
	yp -> yp_lineno = lineno;

	return yp;
}


YP	add_type (y, z)
YP	y,
 z;
{
	YP	    yp;

	for (yp = y; yp -> yp_next; yp = yp -> yp_next)
		continue;
	yp -> yp_next = z;

	return y;
}

/*  */

YP	copy_type (yp)
YP	yp;
{
	YP	    y;

	if (yp == NULLYP)
		return NULLYP;

	y = new_type (yp -> yp_code, yp -> yp_lineno);
	y -> yp_direction = yp -> yp_direction;

	switch (yp -> yp_code) {
	case YP_IDEFINED:
		if (yp -> yp_module)
			y -> yp_module = new_string (yp -> yp_module);
		y -> yp_identifier = new_string (yp -> yp_identifier);
		y -> yp_modid = oid_cpy (yp -> yp_modid);
		break;

	case YP_SEQTYPE:
	case YP_SEQLIST:
	case YP_SETTYPE:
	case YP_SETLIST:
	case YP_CHOICE:
		y -> yp_type = copy_type (yp -> yp_type);
		break;

	case YP_INTLIST:
	case YP_ENUMLIST:
	case YP_BITLIST:
		y -> yp_value = copy_value (yp -> yp_value);
		break;

	default:
		break;
	}

	y -> yp_intexp = yp -> yp_intexp;
	y -> yp_strexp = yp -> yp_strexp;
	y -> yp_prfexp = yp -> yp_prfexp;

	y -> yp_declexp = yp -> yp_declexp;
	y -> yp_varexp = yp -> yp_varexp;

	if (yp -> yp_structname)
		y -> yp_structname = new_string (yp -> yp_structname);
	if (yp -> yp_ptrname)
		y -> yp_ptrname = new_string (yp -> yp_ptrname);

	if (yp -> yp_param_type)
		y -> yp_param_type = new_string (yp -> yp_param_type);

	if (yp -> yp_action0) {
		y -> yp_action0 = new_string (yp -> yp_action0);
		y -> yp_act0_lineno = yp -> yp_act0_lineno;
	}

	if (yp -> yp_action05) {
		y -> yp_action05 = new_string (yp -> yp_action05);
		y -> yp_act05_lineno = yp -> yp_act05_lineno;
	}

	if (yp -> yp_action1) {
		y -> yp_action1 = new_string (yp -> yp_action1);
		y -> yp_act1_lineno = yp -> yp_act1_lineno;
	}

	if (yp -> yp_action2) {
		y -> yp_action2 = new_string (yp -> yp_action2);
		y -> yp_act2_lineno = yp -> yp_act2_lineno;
	}

	if (yp -> yp_action3) {
		y -> yp_action3 = new_string (yp -> yp_action3);
		y -> yp_act3_lineno = yp -> yp_act3_lineno;
	}

	y -> yp_flags = yp -> yp_flags;

	if (yp -> yp_flags & YP_DEFAULT)
		y -> yp_default = copy_value (yp -> yp_default);

	if (yp -> yp_flags & YP_ID)
		y -> yp_id = new_string (yp -> yp_id);

	if (yp -> yp_flags & YP_TAG)
		y -> yp_tag = copy_tag (yp -> yp_tag);

	if (yp -> yp_flags & YP_BOUND)
		y -> yp_bound = new_string (yp -> yp_bound);

	if (yp -> yp_flags & YP_PARMVAL)
		y -> yp_parm = new_string (yp -> yp_parm);

	if (yp -> yp_flags & YP_CONTROLLED)
		y -> yp_control = new_string (yp -> yp_control);

	if (yp -> yp_flags & YP_OPTCONTROL)
		y -> yp_optcontrol = new_string (yp -> yp_optcontrol);

	if (yp -> yp_offset)
		y -> yp_offset = new_string (yp -> yp_offset);

	if (yp -> yp_control_act) {
		y -> yp_control_act = yp -> yp_control_act;
	}
	if (yp -> yp_optional_act) {
		y -> yp_optional_act = yp -> yp_optional_act;
	}
	if (yp -> yp_bef_alist) {
		y -> yp_bef_alist = yp -> yp_bef_alist;
	}
	if (yp -> yp_aft_alist) {
		y -> yp_aft_alist = yp -> yp_aft_alist;
	}


	if (yp -> yp_next)
		y -> yp_next = copy_type (yp -> yp_next);

	return y;
}

/*    VALUES */

YV	new_value (code)
int	code;
{
	YV    yv;

	if ((yv = (YV) calloc (1, sizeof *yv)) == NULLYV)
		yyerror ("out of memory");
	yv -> yv_code = code;

	return yv;
}


YV	add_value (y, z)
YV	y,
 z;
{
	YV	    yv;

	for (yv = y; yv -> yv_next; yv = yv -> yv_next)
		continue;
	yv -> yv_next = z;

	return y;
}

/*  */

YV	copy_value (yv)
YV	yv;
{
	YV	    y;

	if (yv == NULLYV)
		return NULLYV;

	y = new_value (yv -> yv_code);
	y -> yv_flags = yv -> yv_flags;

	if (yv -> yv_action) {
		y -> yv_action = new_string (yv -> yv_action);
		y -> yv_act_lineno = yv -> yv_act_lineno;
	}

	if (yv -> yv_flags & YV_ID)
		y -> yv_id = new_string (yv -> yv_id);

	if (yv -> yv_flags & YV_NAMED)
		y -> yv_named = new_string (yv -> yv_named);

	if (yv -> yv_flags & YV_TYPE)
		y -> yv_type = copy_type (yv -> yv_type);

	switch (yv -> yv_code) {
	case YV_NUMBER:
	case YV_BOOL:
		y -> yv_number = yv -> yv_number;
		break;

	case YV_STRING:
		y -> yv_string = new_string (yv -> yv_string);
		break;

	case YV_IDEFINED:
		if (yv -> yv_module)
			y -> yv_module = new_string (yv -> yv_module);
		y -> yv_identifier = new_string (yv -> yv_identifier);
		break;

	case YV_IDLIST:
	case YV_VALIST:
		y -> yv_idlist = copy_value (yv -> yv_idlist);
		break;

	default:
		break;
	}

	if (yv -> yv_next)
		y -> yv_next = copy_value (yv -> yv_next);

	return y;
}

/*    TAGS */

YT	new_tag (class)
PElementClass	class;
{
	YT    yt;

	if ((yt = (YT) calloc (1, sizeof *yt)) == NULLYT)
		yyerror ("out of memory");
	yt -> yt_class = class;

	return yt;
}

/*  */

YT	copy_tag (yt)
YT	yt;
{
	YT	    y;

	if (yt == NULLYT)
		return NULLYT;

	y = new_tag (yt -> yt_class);

	y -> yt_value = copy_value (yt -> yt_value);

	return y;
}

/*    STRINGS */

char *
new_string (char *s) {
	char  *p;

	if (s == NULLCP)
		return NULLCP;

	if ((p = malloc ((unsigned) (strlen (s) + 1))) == NULLCP)
		yyerror ("out of memory");

	strcpy (p, s);
	return p;
}

/*    SYMBOLS */

static struct triple {
	char	   *t_name;
	PElementClass   t_class;
	PElementID	    t_id;
}		triples[] = {
	"IA5String", PE_CLASS_UNIV,	PE_DEFN_IA5S,
	"ISO646String", PE_CLASS_UNIV, PE_DEFN_IA5S,
	"NumericString", PE_CLASS_UNIV, PE_DEFN_NUMS,
	"PrintableString", PE_CLASS_UNIV, PE_DEFN_PRTS,
	"T61String", PE_CLASS_UNIV, PE_DEFN_T61S,
	"TeletexString", PE_CLASS_UNIV, PE_DEFN_T61S,
	"VideotexString", PE_CLASS_UNIV, PE_DEFN_VTXS,
	"GeneralizedTime", PE_CLASS_UNIV, PE_DEFN_GENT,
	"GeneralisedTime", PE_CLASS_UNIV, PE_DEFN_GENT,
	"UTCTime", PE_CLASS_UNIV, PE_DEFN_UTCT,
	"UniversalTime", PE_CLASS_UNIV, PE_DEFN_UTCT,
	"GraphicString", PE_CLASS_UNIV, PE_DEFN_GFXS,
	"VisibleString", PE_CLASS_UNIV, PE_DEFN_VISS,
	"GeneralString", PE_CLASS_UNIV, PE_DEFN_GENS,
	"EXTERNAL", PE_CLASS_UNIV, PE_CONS_EXTN,
	"ObjectDescriptor", PE_CLASS_UNIV, PE_PRIM_ODE,

	NULL
};

/*  */

char *
modsym (char *module, char *id, char *prefix) {
	char    buf1[BUFSIZ],
			buf2[BUFSIZ],
			buf3[BUFSIZ];
	struct triple *t;
	static char buffer[BUFSIZ];

	if (module == NULLCP)
		for (t = triples; t -> t_name; t++)
			if (strcmp (t -> t_name, id) == 0) {
				module = "UNIV";
				break;
			}

	if (prefix)
		modsym_aux (prefix, buf1);
	modsym_aux (module ? module : mymodule, buf2);
	modsym_aux (id, buf3);
	if (prefix)
		sprintf (buffer, "%s_%s_%s", buf1, buf2, buf3);
	else
		sprintf (buffer, "%s_%s", buf2, buf3);

	return buffer;
}


static
modsym_aux (char *name, char *bp) {
	char   c;

	while (c = *name++)
		switch (c) {
		case '-':
			*bp++ = '_';
			*bp++ = '_';
			break;

		default:
			*bp++ = c;
			break;
		}

	*bp = NULL;
}

/*  */

static char *
gensym (char *s, char *a) {
	int     i;
	char  *p;
	char    buffer[BUFSIZ];
	static int  cP = 0;
	static int  eP = 0;
	static int  mP = 0;

	switch (*s) {
	case 'c':
		i = cP++;
		break;
	case 'e':
		i = eP++;
		break;
	case 'm':
		i = mP++;
		break;

	default:
		myyerror ("unknown gensym argument \"%s\"", s);
		/* NOTREACHED */
	}
	if (a)
		sprintf (buffer, "%s_%s_%d[n_%s]", s, modulename, i, a);
	else
		sprintf (buffer, "%s_%s_%d", s, modulename, i);

	if ((p = malloc ((unsigned) (strlen (buffer) + 11))) == NULLCP)
		yyerror ("out of memory");

	strcpy (p, buffer);
	return p;
}

/* pepy compatible routines - you know how it is ... */
int
init_new_file()  {
	;
}

int
end_file()  {
	;
}

static char *
array (char *s, int flg) {
	static char buf[BUFSIZ];
	char	*p;

	if (!flg) return s;

	if (p = index (s, '[')) {
		sprintf (buf, "%*.*s[1]", p - s, p - s, s);
		return buf;
	}
	return s;
}

static void prime_default (yp, level)
YP	yp;
int	level;
{
	switch (yp -> yp_code) {
	case YP_BOOL:
		printf ("%*s%s = %d;\n", level * 4, "",
				SVAL (yp->yp_varexp),
				val2int (yp -> yp_default) ? 1 : 0);
		break;

	case YP_INT:
		printf ("%*s%s = %d;\n", level * 4, "",
				SVAL (yp -> yp_varexp), val2int (yp -> yp_default));
		break;

	case YP_INTLIST:
	case YP_ENUMLIST:
		printf ("%*s%s = %d;\n", level * 4, "",
				SVAL (yp -> yp_varexp), dfl2int (yp));
		break;

	case YP_REAL:
		printf ("%*s%s = %g;\n", level * 4, "",
				SVAL (yp -> yp_varexp),
				val2real (yp -> yp_default));

	default:
		break;
	}
}
/* PH FILES */

/* really need much more information in the .ph file... */

static	read_ph_file (module, oid)
char *module;
OID	oid;
{
	int     class,
			value,
			direction;
	char    buffer[BUFSIZ],
			file[BUFSIZ],
			id[BUFSIZ],
			encpref[BUFSIZ],
			decpref[BUFSIZ],
			printpref[BUFSIZ];
	char    *p, *ep, *dp, *ppp;
	FILE  *fp;
	YP	    yp;
	YT	    yt;
	YV	    yv;

	sprintf (file, "%s.ph", module);
	if (oid)
		sprintf (p = buffer, "%s.ph", sprintoid(oid));
	else
		p = NULLCP;
	if ((fp = open_ph_file (file, p, "r")) == NULL) {
		warning ("Can't find file %s%s%s failed\n",
				 file, p ? "/" : "", p ? p : "");
		return;
	}

	if (strcmp (module, "UNIV"))
		yyprint (module, 1, 0);

	while (fgets (buffer, sizeof buffer, fp)) {
		if (sscanf (buffer, "%d/%d/%d: %s",
					&class, &value, &direction, id) !=4) {
			myyerror ("bad external definition in %s: %s",
					  file, buffer);
			continue;
		}
		ppp = dp = ep = NULLCP;
		if (p = index(buffer, '|')) {
			if( sscanf (p+1, "%s %s %s\n", encpref, decpref, printpref) == 3) {
				ppp = new_string (printpref);
				dp = new_string (decpref);
				ep = new_string (encpref);
			}
		}

		yp = new_type (class == -1 ? YP_ANY : YP_IMPTYPE, -1);
		yp -> yp_flags = YP_IMPORTED;
		if (class >= 0) {
			yp -> yp_flags |= YP_TAG;
			yp -> yp_tag = yt = new_tag ((PElementClass) class);
			yt -> yt_value = yv = new_value (YV_NUMBER);
			yv -> yv_number = value;
		}
		yp -> yp_direction = direction;
		pass1_type (ep, dp, ppp, new_string (module),
					new_string (id), yp);
	}

	fclose (fp);
}

/*  */

static
write_ph_file()  {
	int	    msave;
	char    file[BUFSIZ];
	char    fileoid[BUFSIZ];
	char	*cp;
	FILE  *fp;
	SY	    sy;
	YT	    yt;
	YP	    yp;

	sprintf (file, "%s.ph", mymodule);
	if (mymoduleid)
		sprintf (cp = fileoid, "%s.ph", sprintoid(mymoduleid));
	else
		cp = NULLCP;
	msave = mflag, mflag = 0;
	if ((fp = open_ph_file (file, cp, "w")) == NULL)
		myyerror ("unable to write %s", file);
	mflag = msave;

	for (sy = mysymbols; sy; sy = sy -> sy_next) {
		yp = sy -> sy_type;
		if (yp -> yp_flags & YP_IMPORTED)
			continue;
		if (doexternals == 0 && (yp->yp_flags & YP_EXPORTED) == 0)
			continue;

		if (is_any_type (yp)) {
			fprintf (fp, "-1/0/%d: %s", yp -> yp_direction, sy -> sy_name);
			fprintf (fp, " |%s %s %s\n", yyencpref, yydecpref, yyprfpref);
		} else if ((yt = lookup_tag (yp)) && yt -> yt_class != PE_CLASS_CONT) {
			fprintf (fp, "%d/%d/%d: %s", yt -> yt_class,
					 val2int (yt -> yt_value), yp -> yp_direction,
					 sy -> sy_name);
			fprintf (fp, " |%s %s %s\n", yyencpref, yydecpref, yyprfpref);
		}
	}

	fclose (fp);
}

/*  */

#ifndef	PEPSYPATH
#define	PEPSYPATH	""
#endif


static FILE *open_ph_file (fn, fnoid, mode)
char *fn,
	 *fnoid,
	 *mode;
{
	char  *dst,
		  *path;
	char    fnb[BUFSIZ];
	FILE  *fp;
	static char *pepypath = NULL;

	if (*fn == '/')
		return fopen (fn, mode);

	if (mflag) {	/* MOBY HACK */
		if ((fp = fopen (fn, mode)) != NULL)
			return fp;
		if (fnoid && (fp = fopen (fnoid, mode)) != NULL)
			return fp;

		sprintf (fnb, "../pepy/%s", fn);
		if ((fp = fopen (fnb, mode)) != NULL)
			return fp;
		if (fnoid) {
			sprintf (fnb, "../pepy/%s", fnoid);
			if ((fp = fopen (fnb, mode)) != NULL)
				return fp;
		}

		sprintf (fnb, "../../pepy/%s", fn);
		if((fp = fopen (fnb, mode)) != NULL)
			return fp;
		if (fnoid) {
			sprintf (fnb, "../../pepy/%s", fnoid);
			if ((fp = fopen (fnb, mode)) != NULL)
				return fp;
		}
		return NULL;
	}

	if (pepypath == NULL && (pepypath = getenv ("PEPSYPATH")) == NULL)
		pepypath = PEPSYPATH;
	path = pepypath;

	do {
		dst = fnb;
		while (*path && *path != ':')
			*dst++ = *path++;
		if (dst != fnb)
			*dst++ = '/';
		strcpy (dst, fn);
		if ((fp = fopen (fnb, mode)) != NULL)
			break;
		if (fnoid) {
			strcpy (dst, fnoid);
			if ((fp = fopen (fnb, mode)) != NULL)
				break;
		}
	} while (*path++);

	return fp;
}

YT  lookup_tag (yp)
YP     yp;
{
	struct tuple *t;
	static struct ypt ypts;
	YT     yt = &ypts;
	static struct ypv ypvs;
	YV     yv = &ypvs;
	YP     z;

	if (yp -> yp_flags & YP_TAG)
		return yp -> yp_tag;

	while (yp -> yp_code == YP_IDEFINED) {
		if (yp -> yp_module && strcmp (yp -> yp_module, mymodule))
			lookup_module (yp -> yp_module, yp -> yp_modid);

		if (z = lookup_type (yp -> yp_module, yp -> yp_identifier)) {
			yp = z;

			if (yp -> yp_flags & YP_TAG)
				return yp -> yp_tag;

			continue;
		}

		break;
	}

	for (t = tuples; t -> t_type != YP_UNDF; t++)
		if (t -> t_type == yp -> yp_code) {
			yt -> yt_class = t -> t_classnum;
			yt -> yt_value = yv;
			yv -> yv_code = YV_NUMBER;
			yv -> yv_number = t -> t_idnum;

			return yt;
		}

	return NULLYT;
}

int  is_any_type (yp)
YP     yp;
{
	YP z;

	while (yp -> yp_code == YP_IDEFINED) {
		if (yp -> yp_flags & YP_TAG)
			return 0;

		if (yp -> yp_module && strcmp (yp -> yp_module, mymodule))
			lookup_module (yp -> yp_module, yp -> yp_modid);

		if (z = lookup_type (yp -> yp_module, yp -> yp_identifier)) {
			yp = z;

			continue;
		}

		break;
	}

	return (yp -> yp_code == YP_ANY && !(yp -> yp_flags & YP_TAG));
}

/*
 * return a string with the leading pathname stripped off
 */
char *
pstrip (char *p) {
	char *p1;

	if (p1 = rindex(p, '/'))
		return (p1 + 1);
	return (p);
}


/*
 * produce a #include on the given file descriptor according to what ever
 * rules are in fashion today. Unfortunately these keep changing so to
 * minimise the effort involved in keeping up we put all the code to change
 * in the one place, here. -- amrw
 *
 * actually, the rules have never changed, andrew just can't figure them out.
 *	-- mtr
 */
doincl(fp, file)
FILE	*fp;		/* where #include is to be written to */
char	*file[];	/* files to be included */
{
	char	**p;

	if (mflag) {
		/* PEPYPATH version */

		fprintf (fp, "#ifndef	PEPYPATH\n");
		for (p = file; *p; p++) {
			if (is_stand(*p))
				fprintf (fp, "#include <isode/%s>\n", *p);
			else
				fprintf (fp, "#include \"%s\"\n", pstrip(*p));
		}
		fprintf (fp, "#else\n");
		for (p = file; *p; p++) {
			fprintf (fp, "#include \"%s\"\n", pstrip(*p));
		}
		fprintf (fp, "#endif\n\n");

	} else {
		for (p = file; *p; p++) {
			if (is_stand(*p))
				fprintf (fp, "#include <isode/%s>\n", *p);
			else
				fprintf (fp, "#include \"%s\"\n", pstrip(*p));
		}
	}
	fprintf (fp, "\n");
}

/* standard files  - that should be found in the <isode> directory */
static char *stand_f[] = {
	"psap.h",
	"pepsy.h",
	"UNIV-types.h",
	"UNIV_defs.h",
	"UNIV_pre_defs.h",

	(char *)0	/* terminating NULL pointer */
};


/*
 * determine if the given (after stripping any path) file is a standard
 * include file which should be in the include/isode directory.
 * return nonzero (true) if it is.
 */
int
is_stand (char *file) {
	char	**p;
	char	*f = pstrip (file);
	for (p = stand_f; *p; p++) {
		if (strcmp(f, pstrip(*p)) == 0)
			return (1);
	}

	return (0);
}

/* Number of different external modules that can referenced */
#define EXTMODSIZE	50

/* table of external modules we reference */
static char *extmodtab[EXTMODSIZE];
static int   nextmod = 0;	/* next free slot in external module table */

/*
 * build up a list of external modules we have referenced
 */
int
addextmod (
	char *p	/* name of external module */
) {

	if (nextmod >= EXTMODSIZE)
		ferr(1, "Too many external modules reference, table overflow\n");

	extmodtab[nextmod++] = p;
}

/*
 * process all the external modules collected to produce the includes
 * required
 */
proc_extmod(fp)
FILE	*fp;
{
	char	**p;
	char 	*files[EXTMODSIZE + 1];
	char 	*buf;
	char	*tail = "-types.h";
	char	*prefix = "pepsy/";
	int		last = 0;

	if (nextmod <= 0)
		return;		/* no external definitions */

	for (p = extmodtab; p < extmodtab + nextmod; p++) {
		if (last >= EXTMODSIZE)
			ferr(1, "proc_extmod: too many external modules\n");

		buf = concat(*p, tail);
		if (mflag || is_stand(buf)) /* need to prepend a "pepsy/" */
			files[last++] = my_strcat(prefix, buf);
		else
			files[last++] = my_new_str(buf);
	}
	files[last++] = (char *)0;

	doincl(fp, files);

	/* free up this memory */
	--last; /* don't free the NULL pointer - can core dump */
	while (last > 0)
		free(files[--last]);

}

/*
 * allocate a yfn structure and intialise it
 */
YFN
new_yfn(efn, dfn, pfn, ffn)
char	*efn, *dfn, *pfn, *ffn;
{

	YFN    fn;
	char	buf[STRSIZE];

	if ((fn = (YFN) calloc (1, sizeof *fn)) == NULLYFN)
		yyerror ("out of memory");

	if (efn) {
		if (getid(efn, buf, STRSIZE) == NULLCP)
			yyerror("Bad Encoding function\n");
		fn -> yfn_enc = strdup(buf);
		free(efn);
	}

	if (dfn) {
		if (getid(dfn, buf, STRSIZE) == NULLCP)
			yyerror("Bad Decoding function\n");
		fn -> yfn_dec = strdup(buf);
		free(dfn);
	}

	if (pfn) {
		if (getid(pfn, buf, STRSIZE) == NULLCP)
			yyerror("Bad Printing function\n");
		fn -> yfn_prt = strdup(buf);
		free(pfn);
	}

	if (ffn) {
		if (getid(ffn, buf, STRSIZE) == NULLCP)
			yyerror("Bad Printing function\n");
		fn -> yfn_fre = strdup(buf);
		free(ffn);
	}

	return fn;
}

/*
 * support routine for action_t = allocate space for it and fill it in with
 * the given yy_action field
 */
Action
new_action_t(text, lineno, num)
char	*text;
int	lineno;
{
	Action	act;

	if ((act = (Action) malloc(sizeof (action_t))) == NULLAction)
		yyerror("out of memory\n");

	act->a_data = text;
	act->a_line = lineno;
	act->a_num = num;

	return (act);
}

/*
 * support routine for YAL = allocate space for it and make sure it is
 * zero'd
 */
YAL
new_yal() {
	YAL	yal;

	if ((yal = (YAL) calloc(1, sizeof (*yal))) == NULLYAL)
		yyerror("out of memory\n");

	return (yal);
}
YAL
yal_join(yal1, yal2)
YAL	yal1, yal2;
{
	if (yal2 == NULLYAL)
		return (yal1);
	if (yal1 == NULLYAL)
		return (yal2);

	if (yal1->yal_enc == NULLAction && yal2->yal_enc != NULLAction)
		yal1->yal_enc = yal2->yal_enc;
	else if (yal1->yal_enc != NULLAction && yal2->yal_enc != NULLAction)
		yyerror("two encoding actions in the same place\n merge into one\n");

	if (yal1->yal_dec == NULLAction && yal2->yal_dec != NULLAction)
		yal1->yal_dec = yal2->yal_dec;
	else if (yal1->yal_dec != NULLAction && yal2->yal_dec != NULLAction)
		yyerror("two decoding actions in the same place\n merge into one\n");

	if (yal1->yal_prn == NULLAction && yal2->yal_prn != NULLAction)
		yal1->yal_prn = yal2->yal_prn;
	else if (yal1->yal_prn != NULLAction && yal2->yal_prn != NULLAction)
		yyerror("two printing actions in the same place\n merge into one\n");

	free((char *)yal2);
	return (yal1);
}
/*
 * join two yfn structures
 */
YFN
join_yfn(fn1, fn2)
YFN	fn1, fn2;
{

	if (fn2 == NULLYFN)
		return (fn1);
	if (fn1 == NULLYFN)
		return (fn2);

	if (fn1->yfn_enc == NULLCP && fn2->yfn_enc != NULLCP)
		fn1->yfn_enc = fn2->yfn_enc;
	else if (fn1->yfn_enc != NULLCP && fn2->yfn_enc != NULLCP)
		yyerror("Illegal: two encoding functions for the same type\n");

	if (fn1->yfn_dec == NULLCP && fn2->yfn_dec != NULLCP)
		fn1->yfn_dec = fn2->yfn_dec;
	else if (fn1->yfn_dec != NULLCP && fn2->yfn_dec != NULLCP)
		yyerror("Illegal: two decoding functions for the same type\n");

	if (fn1->yfn_prt == NULLCP && fn2->yfn_prt != NULLCP)
		fn1->yfn_prt = fn2->yfn_prt;
	else if (fn1->yfn_prt != NULLCP && fn2->yfn_prt != NULLCP)
		yyerror("Illegal: two printing functions for the same type\n");

	free((char *)fn2);

	return (fn1);
}

