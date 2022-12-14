/* ryinitiator.c - remote association initiator */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/idist/RCS/ryinitiator.c,v 9.0 1992/06/16 14:38:53 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/others/idist/RCS/ryinitiator.c,v 9.0 1992/06/16 14:38:53 isode Rel $
 *
 * Severely hacked to give embedded functionality for client.
 *
 * Julian Onions <jpo@cs.nott.ac.uk>
 * Nottingham University Computer Science
 *
 *
 * $Log: ryinitiator.c,v $
 * Revision 9.0  1992/06/16  14:38:53  isode
 * Release 8.0
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include "Idist-types.h"
#include "Idist-ops.h"
#include "defs.h"

/*    DATA */

void	adios (char *, char *, ...);
void	advise (char *, char *, ...);
void	ros_adios (), ros_advise (), acs_advise (), acs_adios ();

char *getstring ();

static char *myservice = "isode idist";

static char *mycontext = "isode idist";
static char *mypci = "isode idist pci";


extern char *isodeversion;

static int ryconnect ();

/*    INITIATOR */

int
makeconn (char *thehost) {
	int	result;
	PE	data;
	struct type_Idist_Initiate *initial;
	char	*cp;
	char	buf[BUFSIZ];
	char	ruser[128], *rhost;
	static char lasthost[BUFSIZ];

	if (lasthost[0] != 0) {
		if (strcmp (thehost, lasthost) == 0)
			return 1;
		closeconn ();
	}

	strcpy (lasthost, thehost);

	if ((initial = (struct type_Idist_Initiate *)
				   malloc (sizeof *initial)) == NULL)
		adios ("memory", "out of");

	initial -> version = VERSION;

	if (cp = index(thehost, '@')) {
		rhost = cp + 1;
		strncpy (ruser, thehost, cp - thehost);
		ruser[cp - thehost] = 0;
	} else {
		strcpy (ruser, user);
		rhost = thehost;
	}
	if (!qflag)
		printf ("updating host %s\n", rhost);
	sprintf (buf, "user (%s:%s): ", rhost, ruser);
	cp = getstring (buf);
	if (cp == NULL)
		cp = ruser;
	initial -> user = str2qb (cp, strlen (cp), 1);

	sprintf (buf, "password (%s:%s): ", rhost, cp);
	cp = getpassword (buf);

	initial -> passwd = str2qb (cp, strlen(cp), 1);
	bzero (cp, strlen (cp));	/* in case of cores */

	if (encode_Idist_Initiate (&data, 1, 0, NULLCP, initial) == NOTOK) {
		advise (NULLCP, "Error encoding data");
		return 0;
	}
	data -> pe_context = 3;	/* hack */

	result = ryconnect (rhost, data, myservice, mycontext, mypci);

	free_Idist_Initiate (initial);

	return result == OK ? 1 : 0;
}


static	int	ry_sd = NOTOK;

static int ryconnect (thehost, data, theservice, thecontext, thepci)
char   *thehost,
	   *theservice,
	   *thecontext,
	   *thepci;
PE	data;
{
	struct SSAPref sfs;
	struct SSAPref *sf;
	struct PSAPaddr *pa;
	struct AcSAPconnect accs;
	struct AcSAPconnect   *acc = &accs;
	struct AcSAPindication  acis;
	struct AcSAPindication *aci = &acis;
	struct AcSAPabort *aca = &aci -> aci_abort;
	AEI	    aei;
	OID	    ctx,
			pci;
	struct PSAPctxlist pcs;
	struct PSAPctxlist *pc = &pcs;
	struct RoSAPindication rois;
	struct RoSAPindication *roi = &rois;
	struct RoSAPpreject *rop = &roi -> roi_preject;

	if ((aei = _str2aei (thehost, theservice, thecontext, 0, NULLCP, NULLCP))
			== NULLAEI)
		adios (NULLCP, "unable to resolve service: %s", PY_pepy);
	if ((pa = aei2addr (aei)) == NULLPA)
		adios (NULLCP, "address translation failed");

	if ((ctx = ode2oid (thecontext)) == NULLOID)
		adios (NULLCP, "%s: unknown object descriptor", thecontext);
	if ((ctx = oid_cpy (ctx)) == NULLOID)
		adios (NULLCP, "out of memory");
	if ((pci = ode2oid (thepci)) == NULLOID)
		adios (NULLCP, "%s: unknown object descriptor", thepci);
	if ((pci = oid_cpy (pci)) == NULLOID)
		adios (NULLCP, "out of memory");
	pc -> pc_nctx = 1;
	pc -> pc_ctx[0].pc_id = 1;
	pc -> pc_ctx[0].pc_asn = pci;
	pc -> pc_ctx[0].pc_atn = NULLOID;

	if ((sf = addr2ref (PLocalHostName ())) == NULL) {
		sf = &sfs;
		bzero ((char *) sf, sizeof *sf);
	}

	if (AcAssocRequest (ctx, NULLAEI, aei, NULLPA, pa, pc, NULLOID,
						0, ROS_MYREQUIRE, SERIAL_NONE, 0, sf, &data, 1, NULLQOS,
						acc, aci)
			== NOTOK)
		acs_adios (aca, "A-ASSOCIATE.REQUEST");

	if (acc -> acc_result != ACS_ACCEPT) {
		int slen;
		char *str;

		if (acc -> acc_ninfo > 0 && (str = prim2str(acc->acc_info[0], &slen)))
			adios (NULLCP, "association rejected: [%s] %*.*s",
				   AcErrString (acc -> acc_result),
				   slen, slen, str);
		else
			adios (NULLCP, "association rejected: [%s]",
				   AcErrString (acc -> acc_result));
	}

	ry_sd = acc -> acc_sd;
	ACCFREE (acc);

	if (RoSetService (ry_sd, RoPService, roi) == NOTOK)
		ros_adios (rop, "set RO/PS fails");
	return OK;
}

int
closeconn () {
	struct AcSAPrelease acrs;
	struct AcSAPrelease   *acr = &acrs;
	struct AcSAPindication  acis;
	struct AcSAPindication *aci = &acis;
	struct AcSAPabort *aca = &aci -> aci_abort;

	if (ry_sd == NOTOK)
		return;

	if (AcRelRequest (ry_sd, ACF_NORMAL, NULLPEP, 0, NOTOK, acr, aci) == NOTOK)
		acs_adios (aca, "A-RELEASE.REQUEST");

	if (!acr -> acr_affirmative) {
		AcUAbortRequest (ry_sd, NULLPEP, 0, aci);
		adios (NULLCP, "release rejected by peer: %d", acr -> acr_reason);
	}

	ACRFREE (acr);
}

/*  */

invoke (op, arg, mod, ind, rfx, efx)
int	op;
modtyp	*mod;		/* encoding/decoding table for Idist */
int	ind;		/* index of this type in tables */
caddr_t	arg;
IFP	rfx, efx;
{
	int	    result;
	struct RoSAPindication  rois;
	struct RoSAPindication *roi = &rois;
	struct RoSAPpreject   *rop = &roi -> roi_preject;
	extern int result_value;

	switch (result = RyStub (ry_sd, table_Idist_Operations, op,
							 RyGenID (ry_sd), NULLIP, arg, rfx, efx,
							 ROS_SYNC, roi)) {
	case NOTOK:		/* failure */
		if (ROS_FATAL (rop -> rop_reason))
			ros_adios (rop, "STUB");
		ros_advise (rop, "STUB");
		break;

	case OK:		/* got a result/error response */
		break;

	case DONE:		/* got RO-END? */
		adios (NULLCP, "got RO-END.INDICATION");
	/* NOTREACHED */

	default:
		adios (NULLCP, "unknown return from RyStub=%d", result);
		/* NOTREACHED */
	}

	if (mod  && ind >= 0 && arg)
		fre_obj (arg, mod -> md_dtab[ind], mod, 1);

	return result_value;
}

/*  */

/*  */

SFD cleanup ();

void
ros_adios (struct RoSAPpreject *rop, char *event) {
	ros_advise (rop, event);

	cleanup ();

	_exit (1);
}


void
ros_advise (struct RoSAPpreject *rop, char *event) {
	char    buffer[BUFSIZ];

	if (rop -> rop_cc > 0)
		sprintf (buffer, "[%s] %*.*s", RoErrString (rop -> rop_reason),
				 rop -> rop_cc, rop -> rop_cc, rop -> rop_data);
	else
		sprintf (buffer, "[%s]", RoErrString (rop -> rop_reason));

	advise (NULLCP, "%s: %s", event, buffer);
}

/*  */

void
acs_adios (struct AcSAPabort *aca, char *event) {
	acs_advise (aca, event);

	cleanup ();
	_exit (1);
}


void
acs_advise (struct AcSAPabort *aca, char *event) {
	char    buffer[BUFSIZ];

	if (aca -> aca_cc > 0)
		sprintf (buffer, "[%s] %*.*s",
				 AcErrString (aca -> aca_reason),
				 aca -> aca_cc, aca -> aca_cc, aca -> aca_data);
	else
		sprintf (buffer, "[%s]", AcErrString (aca -> aca_reason));

	advise (NULLCP, "%s: %s (source %d)", event, buffer,
			aca -> aca_source);
}

/*  */

#ifndef	lint
static void	_advise ();


void	adios (char *what, char *fmt, ...) {
	va_list ap;

	va_start (ap, fmt);

	_advise (what, fmt, ap);

	cleanup ();

	va_end (ap);

	_exit (1);
}
#else
/* VARARGS */

void
adios (char *what, char *fmt) {
	adios (what, fmt);
}
#endif


#ifndef	lint
void	advise (char *what, char *fmt, ...) {
	va_list ap;

	va_start (ap, fmt);

	_advise (what, fmt, ap);

	va_end (ap);
}


static void  _advise (char *what, char *fmt, va_list ap) {
	char    buffer[BUFSIZ];

	_asprintf (buffer, what, fmt, ap);

	fflush (stdout);

	fprintf (stderr, "%s: ", myname);
	fputs (buffer, stderr);
	fputc ('\n', stderr);

	fflush (stderr);
}
#else
/* VARARGS */

void
advise (char *what, char *fmt) {
	advise (what, fmt);
}
#endif


#ifndef	lint
void	ryr_advise (char *what, char *fmt, ...) {
	va_list ap;

	va_start (ap, fmt);

	_advise (what, fmt, ap);

	va_end (ap);
}
#else
/* VARARGS */

void
ryr_advise (char *what, char *fmt) {
	ryr_advise (what, fmt);
}
#endif

char *
getstring (char *prompt) {
	static char buffer[BUFSIZ];
	char	*cp;

	fputs (prompt, stdout);
	fflush (stdout);

	if (fgets (buffer, sizeof buffer, stdin) == NULL)
		return NULLCP;

	if (cp = index (buffer, '\n'))
		*cp = '\0';
	if (buffer[0] == '\0')
		return NULLCP;
	return buffer;
}
