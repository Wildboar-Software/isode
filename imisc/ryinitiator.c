/* ryinitiator.c - generic interactive initiator */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/time.h>
#include "pepsycodec.h"
#include "ryinitiator.h"
#include "tailor.h"

#undef	TIMER
#undef	TMS
#ifdef	BSD42
#define	TIMER
#endif
#ifdef	SYS5
#define	TIMER
#ifndef	HPUX
#include <sys/times.h>
#define	TMS
#endif
#endif

static int getlines (const char *buffer);
static void invoke (int sd, struct RyOperation *ops, struct dispatch *ds, char **args);
#ifdef	TIMER
static int timing_result (int sd, const int id, const int dummy, caddr_t result, struct RoSAPindication *roi);
static void timer (const int bytes, const int pkts);
#ifndef	TMS
static void tvsub (struct timeval *tdiff, const struct timeval *t1, const struct timeval *t0);
#endif
#endif
#ifndef	lint
static void _advise (char* what, const char* fmt, va_list ap);
#endif

static int count = 1;
int	length = 536;

#ifdef	TIMER
#define	DS_RESULT(ds)	(timing ? timing_result : (ds) -> ds_result)

static int timing = 0;
#else
#define	DS_RESULT(ds)	((ds) -> ds_result)
#endif

static char *myname = "ryinitiator";

extern char *isodeversion;

void ryinitiator (const int argc, char **argv, char *myservice, char *mycontext, const char *mypci, struct RyOperation *ops, const struct dispatch *dispatches, ds_argument_t quit) {
	int	    iloop,
			sd;
	char  *cp,
		  **ap;
	char    buffer[BUFSIZ],
			*vec[NVEC + 1];
	struct dispatch   *ds;
	struct QOStype qos;
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

	if (myname = rindex (argv[0], '/'))
		myname++;
	if (myname == NULL || *myname == NULL)
		myname = argv[0];

	isodetailor (myname, 1);

	qos.qos_reliability = HIGH_QUALITY;

	for (ap = argv + 1; cp = *ap; ap++) {
		if (*cp != '-')
			break;

		if (strcmp (cp, "-low") == 0) {
			qos.qos_reliability = LOW_QUALITY;
			continue;
		}
		if (strcmp (cp, "-high") == 0) {
			qos.qos_reliability = HIGH_QUALITY;
			continue;
		}
		if (strcmp (cp, "-c") == 0) {
			if ((cp = *++ap) == NULL
					|| sscanf (cp, "%d", &count) != 1
					|| count < 1)
				adios (NULLCP, "usage: %s -c count", myname);
#ifdef	TIMER
			timing++;
#endif
			continue;
		}
		if (strcmp (cp, "-l") == 0) {
			if ((cp = *++ap) == NULL
					|| sscanf (cp, "%d", &length) != 1
					|| length < 0)
				adios (NULLCP, "usage: %s -l length", myname);
			continue;
		}

		adios (NULLCP, "%s: unknown switch", cp);
	}

	if ((cp = *ap++) == NULL)
		adios (NULLCP, "usage: %s host [operation [ arguments ... ]]", myname);

	if ((aei = _str2aei (cp, myservice, mycontext, *ap == NULLCP, NULLCP,
						 NULLCP)) == NULLAEI)
		adios (NULLCP, "unable to resolve service: %s", PY_pepy);
	if ((pa = aei2addr (aei)) == NULLPA)
		adios (NULLCP, "address translation failed");

	if ((ctx = ode2oid (mycontext)) == NULLOID)
		adios (NULLCP, "%s: unknown object descriptor", mycontext);
	if ((ctx = oid_cpy (ctx)) == NULLOID)
		adios (NULLCP, "out of memory");
	if ((pci = ode2oid (mypci)) == NULLOID)
		adios (NULLCP, "%s: unknown object descriptor", mypci);
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

	if (*ap == NULLCP) {
		printf ("%s", myname);
		if (sf -> sr_ulen > 2)
			printf (" running on host %s", sf -> sr_udata + 2);
		if (sf -> sr_clen > 2)
			printf (" at %s", sf -> sr_cdata + 2);
		printf (" [%s, ", oid2ode (ctx));
		printf ("%s]\n", oid2ode (pci));
		printf ("using %s\n", isodeversion);

		printf ("%s... ", cp);
		fflush (stdout);

		iloop = 1;
	} else {
		cp = *ap++;
		for (ds = dispatches; ds -> ds_name; ds++)
			if (strcmp (ds -> ds_name, cp) == 0)
				break;
		if (ds -> ds_name == NULL)
			adios (NULLCP, "unknown operation \"%s\"", cp);

		iloop = 0;
	}

	if (AcAssocRequest (ctx, NULLAEI, aei, NULLPA, pa, pc, NULLOID,
						0, ROS_MYREQUIRE, SERIAL_NONE, 0, sf, NULLPEP, 0, &qos,
						acc, aci)
			== NOTOK)
		acs_adios (aca, "A-ASSOCIATE.REQUEST");

	if (acc -> acc_result != ACS_ACCEPT) {
		if (iloop)
			printf ("failed\n");

		adios (NULLCP, "association rejected: [%s]",
			   AcErrString (acc -> acc_result));
	}
	if (iloop) {
		printf ("connected\n");
		fflush (stdout);
	}

	sd = acc -> acc_sd;
	ACCFREE (acc);

	if (RoSetService (sd, RoPService, roi) == NOTOK)
		ros_adios (rop, "set RO/PS fails");

	if (iloop) {
		for (;;) {
			if (getlines (buffer) == NOTOK)
				break;

			if (str2vec (buffer, vec) < 1)
				continue;

			for (ds = dispatches; ds -> ds_name; ds++)
				if (strcmp (ds -> ds_name, vec[0]) == 0)
					break;
			if (ds -> ds_name == NULL) {
				advise (NULLCP, "unknown operation \"%s\"", vec[0]);
				continue;
			}

			invoke (sd, ops, ds, vec + 1);
		}
	} else
		invoke (sd, ops, ds, ap);

	(*quit) (sd, (struct dispatch *) NULL, (char **) NULL, (caddr_t *) NULL);
}

static void invoke (int sd, struct RyOperation *ops, struct dispatch *ds, char **args) {
	int    i;
	int	    cc,
			result;
	caddr_t in;
	struct RoSAPindication  rois;
	struct RoSAPindication *roi = &rois;
	struct RoSAPpreject   *rop = &roi -> roi_preject;

	in = NULL;
	if (ds -> ds_argument && (*ds -> ds_argument) (sd, ds, args, &in) == NOTOK)
		return;

#ifdef	TIMER
	if (timing) {
		struct RyOperation *ryo = ops;
		PE	pe;

		cc = 0;

		for (; ryo -> ryo_name; ryo++)
			if (ryo -> ryo_op == ds -> ds_operation)
				break;
		if (!ryo -> ryo_name || !ryo -> ryo_arg_mod)
			goto nope;

		pe = NULLPE;
		if (enc_f (ryo -> ryo_arg_index, ryo -> ryo_arg_mod, &pe, 1, 0,
				   NULLCP, in) != NOTOK)
			cc = ps_get_abs (pe);
		if (pe)
			pe_free (pe);

nope:
		;
		timer (0, 0);
	}
#endif
	for (i = 0; i < count; i++)
		switch (result = RyStub (sd, ops, ds -> ds_operation, RyGenID (sd),
								 NULL, in, DS_RESULT (ds), (void (*)(int, int, int, caddr_t, struct RoSAPindication *))ds -> ds_error,
								 ROS_SYNC, roi)) {
		case NOTOK:		/* failure */
			if (ROS_FATAL (rop -> rop_reason))
				ros_adios (rop, "STUB");
			ros_advise (rop, "STUB");
			goto out;

		case OK:		/* got a result/error response */
			break;

		case DONE:		/* got RO-END? */
			adios (NULLCP, "got RO-END.INDICATION");
		/* NOTREACHED */

		default:
			adios (NULLCP, "unknown return from RyStub=%d", result);
			/* NOTREACHED */
		}

#ifdef	TIMER
	if (timing)
		timer (cc, count);
#endif

out:
	;
	if (ds -> ds_fr_mod && in)
		fre_obj (in, ds -> ds_fr_mod -> md_dtab[ds -> ds_fr_index],
				 ds -> ds_fr_mod, 1);
}

static int getlines (const char *buffer) {
	int    i;
	char  *cp, *ep;
	static int  sticky = 0;

	if (sticky) {
		sticky = 0;
		return NOTOK;
	}

	printf ("%s> ", myname);
	fflush (stdout);

	for (ep = (cp = buffer) + BUFSIZ - 1; (i = getchar ()) != '\n';) {
		if (i == EOF) {
			printf ("\n");
			clearerr (stdin);
			if (cp != buffer) {
				sticky++;
				break;
			}

			return NOTOK;
		}

		if (cp < ep) {
			if (int2octet (i, cp) != 0)
				return NOTOK;
			cp++;
		}
	}
	*cp = 0;
	return OK;
}

#ifdef	TIMER

#ifndef	NBBY
#define	NBBY	8
#endif

#ifndef	TMS
static void timer (const int bytes, const int pkts) {
	long    ms;
	float   bs,
			ps;
	struct timeval  stop,
			   td;
	static struct timeval   start;

	if (pkts == 0) {
		gettimeofday (&start, (struct timezone *) 0);
		return;
	} else
		gettimeofday (&stop, (struct timezone  *) 0);
	tvsub (&td, &stop, &start);
	ms = (td.tv_sec * 1000) + (td.tv_usec / 1000);
	{
		int ims;
		float fbytes,
		      fpkts,
		      fms;

		if (int2float (bytes, &fbytes) != 0
				|| int2float (pkts, &fpkts) != 0
				|| long2int (ms ? ms : 1L, &ims) != 0
				|| int2float (ims, &fms) != 0)
			return;
		bs = (fbytes * fpkts * NBBY * 1000 / fms) / NBBY;
		ps = (fpkts * 1000) / fms;
	}
	printf ("%d operations in %d.%02d seconds (%.2f ops/s)",
			pkts, td.tv_sec, td.tv_usec / 10000, ps);
	if (bytes > 0)
		printf ("; %d bytes/op for %.2f Kbytes/s", bytes, bs / 1024);
	printf ("\n");
}

static void tvsub (struct timeval *tdiff, const struct timeval *t1, const struct timeval *t0) {
	tdiff -> tv_sec = t1 -> tv_sec - t0 -> tv_sec;
	tdiff -> tv_usec = t1 -> tv_usec - t0 -> tv_usec;
	if (tdiff -> tv_usec < 0)
		tdiff -> tv_sec--, tdiff -> tv_usec += 1000000;
}
#else
long times (struct tms *);

static void timer (int bytes, int pkts) {
	long    ms;
	float   bs,
			ps;
	long    stop,
			td,
			secs,
			msecs;
	struct tms tm;
	static long start;

	if (pkts == 0) {
		start = times (&tm);
		return;
	} else
		stop = times (&tm);

	td = stop - start;
	secs = td / 60, msecs = (td % 60) * 1000 / 60;
	ms = (secs * 1000) +  msecs;
	{
		int ims;
		float fbytes,
		      fpkts,
		      fms;

		if (int2float (bytes, &fbytes) != 0
				|| int2float (pkts, &fpkts) != 0
				|| long2int (ms ? ms : 1L, &ims) != 0
				|| int2float (ims, &fms) != 0)
			return;
		bs = (fbytes * fpkts * NBBY * 1000 / fms) / NBBY;
		ps = (fpkts * 1000) / fms;
	}

	printf ("%d operations in %d.%02d seconds (%.2f ops/s)",
			pkts, secs, msecs / 10, ps);
	if (bytes > 0)
		printf ("; %d bytes/op for %.2f Kbytes/s", bytes, bs / 1024);
	printf ("\n");
}
#endif
#endif

static int timing_result (int sd, const int id, const int dummy, caddr_t result, struct RoSAPindication *roi) {
	return OK;
}

void ros_adios (struct RoSAPpreject *rop, char *event) {
	ros_advise (rop, event);
	_exit (1);
}

void ros_advise (struct RoSAPpreject *rop, char *event) {
	char    buffer[BUFSIZ];
	if (rop -> rop_cc > 0)
		sprintf (buffer, "[%s] %*.*s", RoErrString (rop -> rop_reason),
				 rop -> rop_cc, rop -> rop_cc, rop -> rop_data);
	else
		sprintf (buffer, "[%s]", RoErrString (rop -> rop_reason));
	advise (NULLCP, "%s: %s", event, buffer);
}

void acs_adios (struct AcSAPabort *aca, const char *event) {
	acs_advise (aca, event);
	_exit (1);
}

void acs_advise (struct AcSAPabort *aca, char *event) {
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

#ifndef	lint
void adios (char* what, const char* fmt, ...) {
	va_list ap;
	va_start (ap, fmt);
	_advise (what, fmt, ap);
	va_end (ap);
	_exit (1);
}
#else
/* VARARGS */
void adios (char *what, const char *fmt) {
	adios (what, fmt);
}
#endif

#ifndef	lint
void advise (char* what, const char* fmt, ...) {
	va_list ap;
	va_start (ap, fmt);
	_advise (what, fmt, ap);
	va_end (ap);
}

static void _advise (char* what, const char* fmt, va_list ap) {
	char buffer[BUFSIZ];
	_asprintf (buffer, what, fmt, ap);
	fflush (stdout);
	fprintf (stderr, "%s: ", myname);
	fputs (buffer, stderr);
	fputc ('\n', stderr);
	fflush (stderr);
}
#else
/* VARARGS */
void advise (char *what, const char *fmt) {
	advise (what, fmt);
}
#endif
