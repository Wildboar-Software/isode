/* vprint.c - pepy printer support */

#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "UNIV-types.h"
#include "psap.h"
#include "pepsy.h"
#include "logger.h"

#ifndef __STDC__
int	fprintf ();
#endif

static void vprint1 (void);
static void vprint2 (void);
static void vwrite (char *s);

#define	VPRINT(s)	vprint1 (), vwrite ((s)), vprint2 ()

static int vlevel = 0;

static int didname = 0;
static int didvpop = 0;
static int didvpush = 0;
static int docomma = 0;

static char  *py_classlist[] = {
	"UNIVERSAL", "APPLICATION", "", "PRIVATE"
};

static char *vbp = NULLCP;
static char *vsp;

IFP   vfnx = (IFP) fprintf;
FILE *vfp = (FILE*) NULL;
static PS    vps = NULLPS;

#ifdef __GNUC__
__attribute__((constructor))
static void _init_vfp ()
{
	vfp = stdout;
}
#endif

static char *oct2str (char *s, int len);
static char *newbuf (int i);

void vpush(void)  {
	if (didvpush)
		vwrite ("\n"), didvpush = 0;
	else if (!didname && docomma)
		vwrite (",\n");
	if (didname)
		vwrite (" ");
	else if (vfp && vlevel > 0)
		(*vfnx) (vfp, "%*s", vlevel * 3, "");
	vwrite ("{");
	vlevel++;
	didname = didvpop = docomma = 0, didvpush = 1;
}

void vpop(void)  {
	if (didname || docomma)
		vwrite ("\n");
	vlevel--;
	if (!didvpush && vfp && vlevel > 0)
		(*vfnx) (vfp, "%*s", vlevel * 3, "");
	vwrite ("}");
	if (vlevel == 0)
		vwrite ("\n");
	didname = didvpush = 0, didvpop = docomma = vlevel ? 1 : 0;
}

void vname (char *name) {
	if (didvpush)
		vwrite ("\n"), didvpush = 0;
	else if (docomma)
		vwrite (",\n");

	if (vfp && vlevel > 0)
		(*vfnx) (vfp, "%*s", vlevel * 3, "");
	vwrite (name);
	didname = 1;
}

void vtag (int class, int id) {
	char *bp;
	static char buffer[BUFSIZ];

	if (didname)
		return;

	bp = buffer;
	*bp++ = '[';
	switch (class) {
	case PE_CLASS_UNIV:
	case PE_CLASS_APPL:
	case PE_CLASS_PRIV:
		sprintf (bp, "%s ", py_classlist[class]);
		bp += strlen (bp);
		break;

	case PE_CLASS_CONT:
	default:
		break;
	}
	sprintf (bp, "%d]", id);

	vname (buffer);
}

#ifndef	lint
void vprint (char *fmt, ...) {
	char    buffer[BUFSIZ];
	va_list ap;
	vprint1 ();
	va_start (ap, fmt);
	_asprintf (buffer, NULLCP, fmt, ap);
	va_end (ap);
	vwrite (buffer);
	vprint2 ();
}
#else
/* VARARGS */

int vprint (char *fmt) {
	vprint (fmt);
}
#endif

static void vprint1(void) {
	if (didvpush) {
		vwrite ("\n"), didvpush = 0;
		goto indent;
	} else if (didname)
		vwrite (" ");
	else {
		if (docomma)
			vwrite (",\n");
indent:
		;
		if (vfp && vlevel > 0)
			(*vfnx) (vfp, "%*s", vlevel * 3, "");
	}
}

static void vprint2(void)  {
	if (vlevel == 0)
		vwrite ("\n");
	didname = didvpop = 0, docomma = vlevel ? 1 : 0;
}

static void vwrite (char *s) {
	if (vfp)
		(*vfnx) (vfp, "%s", s);
	else {
		char   c,
			   *cp;

		if (vps)
			for (cp = s; *cp; cp++) {
				if (*cp == '\n' )
					ps_write (vps, (PElementData) " ", 1);
				else
					ps_write (vps, (PElementData) cp, 1);
			}
		else
			for (cp = s; *cp; )
				*vbp++ = (c = *cp++) != '\n' ? c : ' ';
	}
}

void vstring (PE pe) {
	PE p;

	switch (pe -> pe_form) {
	case PE_FORM_PRIM:
	case PE_FORM_ICONS:
		VPRINT (oct2str ((char *) pe -> pe_prim, (int) pe -> pe_len));
		break;

	case PE_FORM_CONS:
		vpush ();
		for (p = pe -> pe_cons; p; p = p -> pe_next)
			vstring (p);
		vpop ();
		break;
	}
}

static char *oct2str (char *s, int len) {
	int     ia5ok;
	int    k;
	char  *bp, *cp, *dp, *zp;

	ia5ok = 1, k = 0;
	for (dp = (cp = s) + len; cp < dp; cp++) {
		switch (*cp) {
		case ' ':
			continue;

		case '"':
			break;

		case '\b':
		case '\f':
		case '\n':
		case '\r':
		case '\t':
		case '\\':
			ia5ok = -1, k++;
			continue;

		case '-':
			if (cp > s && *(cp + 1) == '-')
				break;
			continue;

		default:
			if (iscntrl (*cp) || isspace (*cp) || (*cp & 0x80))
				break;
			continue;
		}
		ia5ok = 0;
		break;
	}

	switch (ia5ok) {
	case 1:
		zp = newbuf (len + 2);
		sprintf (zp, "\"%*.*s\"", len, len, s);
		break;

	case -1:
		bp = zp = newbuf (len + k + 2);
		*bp++ = '"';
		for (cp = s; cp < dp; cp++)
			if (*cp >= ' ' && *cp != '\\')
				*bp++ = *cp;
			else {
				*bp++ = '\\';
				switch (*cp) {
				case '\b':
					*bp++ = 'b';
					break;
				case '\f':
					*bp++ = 'f';
					break;
				case '\n':
					*bp++ = 'n';
					break;
				case '\r':
					*bp++ = 'r';
					break;
				case '\t':
					*bp++ = 't';
					break;

				case '\\':
					*bp++ =  '\\';
					break;
				}
			}
		sprintf (bp, "\"");
		break;

	case 0:
	default:
		bp = zp = newbuf (len * 2 + 3);
		*bp++ = '\'';
		for (cp = s; cp < dp; cp++) {
			sprintf (bp, "%02x", *cp & 0xff);
			bp += strlen (bp);
		}
		sprintf (bp, "'H");
		break;
	}

	return zp;
}

char *bit2str (PE pe, char *s) {
	int ia5ok;
	int hit, i, j, k;
	char *bp, *cp, *zp;

	j = pe -> pe_nbits;
	if ((cp = s) && *++cp) {
		ia5ok = 1, hit = 0;
		for (i = 0; i < j;)
			if (bit_test (pe, i++) == 1) {
				do {
					if (!(k = *cp++ & 0xff))
						break;
					if (k == i) {
						hit += hit ? 2 : 1;
						for (; *cp > ' '; cp++)
							hit++;
					} else
						for (; *cp > ' '; cp++)
							continue;
				} while (k != 0 && k < i);
				if (k == 0 || k > i) {
					ia5ok = 0;
					break;
				}
			}
	} else
		ia5ok = 0;

	if (ia5ok) {
		bp = zp = newbuf (hit + 3);
		*bp++ = '{';

		cp = s, cp++;
		for (i = hit = 0; i < j;)
			if (bit_test (pe, i++) == 1) {
				do {
					if (!(k = *cp++ & 0xff))
						break;
					if (k == i) {
						if (hit)
							*bp++ = ',';
						*bp++ = ' ';
						for (; *cp > ' '; cp++)
							*bp++ = *cp;
					} else
						for (; *cp > ' '; cp++)
							continue;
				} while (k != 0 && k < i);
				if (k == 0 || k > i)
					break;
				hit++;
			}

		sprintf (bp, "%s}", hit ? " " : "");
	} else {
		bp = zp = newbuf (j + 3);
		*bp++ = '\'';
		for (i = 0; i < j; i++)
			*bp++ = bit_test (pe, i) ? '1' : '0';
		sprintf (bp, "'B");
	}

	return zp;
}

#ifdef vunknown
#undef vunknown
#endif

void vunknown (PE pe) {
	int     i;
#ifdef	notyet	    /* could comment this in, but then all programs need -lm */
	double  j;
#endif
	OID	    oid;
	PE	    p;

	switch (pe -> pe_form) {
	case PE_FORM_PRIM:
		switch (PE_ID (pe -> pe_class, pe -> pe_id)) {
		case PE_ID (PE_CLASS_UNIV, PE_PRIM_BOOL):
			if ((i = prim2flag (pe)) == NOTOK)
				goto bad_pe;
			VPRINT (i ? "TRUE" : "FALSE");
			break;

		case PE_ID (PE_CLASS_UNIV, PE_PRIM_INT):
		case PE_ID (PE_CLASS_UNIV, PE_PRIM_ENUM):
			if ((i = prim2num (pe)) == NOTOK
					&& pe -> pe_errno != PE_ERR_NONE)
				goto bad_pe;
			vprint ("%d", i);
			break;

#ifdef	notyet
		case PE_ID (PE_CLASS_UNIV, PE_PRIM_REAL):
			if ((j = prim2real (pe)) == NOTOK
					&& pe -> pe_errno != PE_ERR_NONE)
				goto bad_pe;
			vprint ("%g", j);
			break;
#endif

		case PE_ID (PE_CLASS_UNIV, PE_PRIM_BITS):
			if ((p = prim2bit (pe)) == NULLPE)
				goto bad_pe;
			if (p -> pe_nbits < LOTSOFBITS) {
				VPRINT (bit2str (p, "\020"));
				break;
			}
		/* else fall... */

		default:
bad_pe:
			;
			vtag ((int) pe -> pe_class, (int) pe -> pe_id);
		/* fall */

		case PE_ID (PE_CLASS_UNIV, PE_PRIM_OCTS):
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_IA5S):
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_NUMS):
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_PRTS):
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_T61S):
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_VTXS):
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_GENT):
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_UTCT):
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_GFXS):
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_VISS):
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_GENS):
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_CHRS):
		case PE_ID (PE_CLASS_UNIV, PE_PRIM_ODE):
			vstring (pe);
			break;

		case PE_ID (PE_CLASS_UNIV, PE_PRIM_NULL):
			VPRINT ("0");
			break;

		case PE_ID (PE_CLASS_UNIV, PE_PRIM_OID):
			if ((oid = prim2oid (pe)) == NULLOID)
				goto bad_pe;
			VPRINT (oid2ode (oid));
			break;
		}
		break;

	case PE_FORM_CONS:
		switch (PE_ID (pe -> pe_class, pe -> pe_id)) {
		case PE_ID (PE_CLASS_UNIV, PE_CONS_SEQ):
		case PE_ID (PE_CLASS_UNIV, PE_CONS_SET):
			break;

		case PE_ID (PE_CLASS_UNIV, PE_CONS_EXTN):
			print_UNIV_EXTERNAL (pe, 1, NULL, NULLVP,
								 NULLCP);
			return;

		default:
			vtag ((int) pe -> pe_class, (int) pe -> pe_id);
			break;
		}
		vpush ();
		for (p = pe -> pe_cons; p; p = p -> pe_next)
			vunknown (p);
		vpop ();
		break;

	case PE_FORM_ICONS:
		vtag ((int) pe -> pe_class, (int) pe -> pe_id);
		vstring (pe);
		break;
	}
}

void vpushfp (FILE *fp, PE pe, char *s, int rw) {
	vpushpp (fp, pe, s, rw);
}

void vsetfp (FILE *fp, char *s) {
	vfp = fp;
	vfnx = (IFP) fprintf;

	if(s != NULLCP)
		(*vfnx) (vfp, "%s\n", s);

	vlevel = didname = didvpush = didvpop = docomma = 0;
}

void vpopfp(void)  {
	(*vfnx) (vfp, "-------\n");
	fflush (vfp);
	vpopp ();
}

void vpushstr (char *cp) {
	vfp = NULL;
	vbp = vsp = cp;
	vlevel = didname = didvpush = didvpop = docomma = 0;
}

void vpopstr(void)  {
	while (--vbp >= vsp)
		if (*vbp != ' ')
			break;
	*++vbp = 0;
	vfp = stdout;
}

void vpushpp (
	FILE *vfp,
	PE pe,
	char *text,
	int rw
) {
	fprintf(vfp, "%s %s", rw ? "read" : "wrote", text ? text : "pdu");
	if (pe -> pe_context != PE_DFLT_CTX)
		fprintf(vfp, ", context %d", pe -> pe_context);
	fprintf(vfp, "\n");
	vlevel = didname = didvpush = didvpop = docomma = 0;
}

void vpopp(void)  {
	vfp = stdout, vfnx = (IFP) fprintf;
}

void vpushquipu (PS ps) {
	vps = ps;
	vfp = NULL;
	vlevel = didname = didvpush = didvpop = docomma = 0;
}

void vpopquipu(void)  {
	vpopp();
	vps = NULLPS;
}

#undef	pvpdu

/**
 * ind: index into tables
 * mod: pointer to tables
 */
void pvpdu (LLog *lp, int ind, modtyp *mod, PE pe, char *text, int rw) {
	char   *bp;
	char   buffer[BUFSIZ];
	vfp = (FILE *) lp, vfnx = (IFP)ll_printf;
	bp = buffer;
	sprintf (bp, "%s %s", rw ? "read" : "wrote",
			 text ? text : "pdu");
	bp += strlen (bp);
	if (pe -> pe_context != PE_DFLT_CTX) {
		sprintf (bp, ", context %d", pe -> pe_context);
		bp += strlen (bp);
	}
	LLOG (lp, LLOG_ALL, ("%s", buffer));
	vlevel = didname = didvpush = didvpop = docomma = 0;
	if (mod == NULL)
		vunknown (pe);
	else
		prnt_f (ind, mod, pe, 1, NULL, NULLVP);
	ll_printf (lp, "-------\n");
	ll_sync (lp);
	vfp = stdout, vfnx = (IFP) fprintf;
}

static char *bufp = NULL;

static char *newbuf (int i) {
	static unsigned int len = 0;
	if (i++ < len)
		return bufp;
	if (bufp)
		free (bufp);
	if ((bufp = malloc ((unsigned int) i)))
		len = i;
	else
		len = 0;
	return bufp;
}

/*  VPDU - support for backwards compatibility */

void _vpdu (LLog *lp, IFP fnx, PE pe, char *text, int rw) {
	char   *bp;
	char   buffer[BUFSIZ];
	vfp = (FILE *) lp, vfnx = (IFP)ll_printf;
	bp = buffer;
	sprintf (bp, "%s %s", rw ? "read" : "wrote",
			 text ? text : "pdu");
	bp += strlen (bp);
	if (pe -> pe_context != PE_DFLT_CTX) {
		sprintf (bp, ", context %d", pe -> pe_context);
		bp += strlen (bp);
	}
	LLOG (lp, LLOG_ALL, ("%s", buffer));
	vlevel = didname = didvpush = didvpop = docomma = 0;
	(*fnx) (pe, 1, NULLIP, NULLVP, NULLCP);
	ll_printf (lp, "-------\n");
	ll_sync (lp);
	vfp = stdout, vfnx = (IFP) fprintf;
}

#ifdef DEBUG
void free_pepsy_bp(void)  {
	if (bufp)
		free(bufp);
}
#endif
