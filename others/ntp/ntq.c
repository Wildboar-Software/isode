#ifndef	lint
static char *RCSid = "$Header: /xtel/isode/isode/others/ntp/RCS/ntq.c,v 9.0 1992/06/16 12:42:48 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/others/ntp/RCS/ntq.c,v 9.0 1992/06/16 12:42:48 isode Rel $
 * NTP query program - useful for debugging, Specific to OSI.
 *
 * $Log: ntq.c,v $
 * Revision 9.0  1992/06/16  12:42:48  isode
 * Release 8.0
 *
 */

#include "ntp.h"
#include <stdarg.h>
#include "NTP-ops.h"
#include "NTP-types.h"

char	*myname;
int	sleeptime = 30;

char	*mycontext = "ntp";
char	*mypci = "ntp pci";

int	query_result (), query_error ();

void	ros_adios (), ros_advise (), acs_adios (), acs_advise ();
static void	adios (char *, char *, ...);
static void	advise (char *, char *, ...);
PE	build_bind_arg ();

int
main (int argc, char **argv) {
	extern char	*optarg;
	extern int	optind;
	int	opt;

	myname = argv[0];
	while((opt = getopt(argc, argv, "-s")) != EOF)
		switch (opt) {
		case 's':
			sleeptime = atoi (optarg);
			break;
		default:
			fprintf (stderr, "Usage: %s [-s] host\n", myname);
			break;
		}

	argc -= optind;
	argv += optind;
	isodetailor (myname, 0);

	if (argc < 1)
		adios (NULLCP, "Usage: %s [-s] host", myname);

	ntp_monitor (*argv);
	exit (0);
	return 0;
}


int
ntp_monitor (char *host) {
	int	sd;

	sd = mk_connect (host);

	for (;;) {

		send_request (sd);

		sleep ((unsigned)sleeptime);
	}
}

int
mk_connect (char *addr) {
	int sd;
	struct SSAPref sfs;
	struct SSAPref *sf;
	struct PSAPaddr *pa;
	struct AcSAPconnect accs;
	struct AcSAPconnect   *acc = &accs;
	struct AcSAPindication  acis;
	struct AcSAPindication *aci = &acis;
	struct AcSAPabort *aca = &aci -> aci_abort;
	OID     ctx,
			pci;
	struct PSAPctxlist pcs;
	struct PSAPctxlist *pc = &pcs;
	struct RoSAPindication rois;
	struct RoSAPindication *roi = &rois;
	struct RoSAPpreject *rop = &roi -> roi_preject;

	PE pep[1];

	if ((pa = str2paddr (addr)) == NULLPA) {
		advise ( NULLCP, "Can't translate %s", addr);
		return NOTOK;
	}

	pep[0] = build_bind_arg ();

	if ((ctx = ode2oid (mycontext)) == NULLOID) {
		advise ( NULLCP,
				 "%s: unknown object descriptor", mycontext);
		return NOTOK;
	}
	if ((ctx = oid_cpy (ctx)) == NULLOID) {
		advise ( "memory", "out of");
		return NOTOK;
	}
	if ((pci = ode2oid (mypci)) == NULLOID) {
		advise ( NULLCP,
				 "%s: unknown object descriptor", mypci);
		return NOTOK;
	}
	if ((pci = oid_cpy (pci)) == NULLOID) {
		advise ( "memory", "out of");
		return NOTOK;
	}
	pc -> pc_nctx = 1;
	pc -> pc_ctx[0].pc_id = 1;
	pc -> pc_ctx[0].pc_asn = pci;
	pc -> pc_ctx[0].pc_atn = NULLOID;
	if ((sf = addr2ref (PLocalHostName ())) == NULL) {
		sf = &sfs;
		bzero ((char *) sf, sizeof *sf);
	}

	if (AcAssocRequest (ctx, NULLAEI, NULLAEI, NULLPA, pa,
						pc, NULLOID,
						0, ROS_MYREQUIRE, SERIAL_NONE, 0, sf,
						pep, 1, NULLQOS,
						acc, aci) == NOTOK)
		acs_adios (aca, "A-ASSOCIATE.REQUEST");

	pe_free (pep[0]);

	if (acc -> acc_result != ACS_ACCEPT)
		ac_failed (acc);

	sd = acc -> acc_sd;
	ACCFREE (acc);

	if (RoSetService (sd, RoPService, roi) == NOTOK)
		ros_adios (rop, "set RO/PS fails");

	return sd;
}

int
ac_failed (struct AcSAPconnect *acc) {
	if (acc -> acc_ninfo > 0) {
		struct type_NTP_BindError *binderr;
		char	*cp = NULLCP;

		if (decode_NTP_BindError (acc -> acc_info[0], 1,
								  NULLIP, NULLVP,
								  &binderr) != NOTOK) {
			if (binderr -> supplementary)
				cp = qb2str (binderr -> supplementary);
			switch (binderr -> reason) {
			case int_NTP_reason_refused:
				adios (NULLCP,
					   "connection refused: %s",
					   cp ? cp : "");
				break;
			case int_NTP_reason_validation:
				adios (NULLCP,
					   "validation failure: %s",
					   cp ? cp : "");
				break;
			case int_NTP_reason_version:
				adios (NULLCP,
					   "version mismatch: %s",
					   cp ? cp : "");
				break;
			case int_NTP_reason_badarg:
				adios (NULLCP,
					   "bad connect argument: %s",
					   cp ? cp : "");
				break;
			case int_NTP_reason_congested:
				adios (NULLCP,
					   "congested: %s",
					   cp ? cp : "");
				break;
			default:
				adios (NULLCP,	"Unknown reason (%d) %s",
					   binderr -> reason,
					   cp ? cp : "");
				break;
			}
			free_NTP_BindError (binderr);
		}
	}
	adios (NULLCP, "Connection failed: %s",
		   AcErrString (acc -> acc_result));
}


int
send_request (int sd) {
	struct RoSAPindication rois;
	struct RoSAPindication *roi = &rois;
	struct RoSAPpreject *rop = &roi -> roi_preject;

	switch (RyStub (sd, table_NTP_Operations, operation_NTP_query,
					RyGenID (sd), NULLIP, NULLCP,
					query_result, query_error, ROS_SYNC, roi)) {
	case NOTOK:
		if (ROS_FATAL (rop -> rop_reason))
			ros_adios (rop, "STUB");
		ros_advise (rop, "STUB");
		break;

	case OK:            /* got a result/error response */
		break;

	case DONE:          /* got RO-END? */
		adios (NULLCP, "got RO-END.INDICATION");
	/* NOTREACHED */

	default:
		adios (NULLCP, "unknown return from RyStub");
		/* NOTREACHED */
	}
}

/* ARGSUSED */
int
query_result (int sd, int id, int dummy, struct type_NTP_ClockInfoList *result, struct RoSAPindication *roi) {
	struct type_NTP_ClockInfo *clock;
	char	*p, c;
	int 	i;

	printf (" %4.4s %4.4s %5.5s %8.8s %8.8s %8.8s %s\n",
			"Stratum", "Poll", "Reach", "Delay", "Offset", "Disp", "host");
	for (i = 0; i < 44; i++)
		putchar('-');
	putchar ('\n');
	for (; result; result = result -> next) {
		clock = result -> ClockInfo;

		c = ' ';
		if (bit_test (clock -> flags, bit_NTP_flags_configured))
			c = '-';
		if (bit_test (clock -> flags, bit_NTP_flags_sane))
			c = '.';
		if (bit_test (clock -> flags, bit_NTP_flags_candidate))
			c = '+';
		if (bit_test (clock -> flags, bit_NTP_flags_selected))
			c = '>';
		if (bit_test (clock -> flags, bit_NTP_flags_inactive))
			c = '!';
		putchar (c);
		printf ("%4d", clock -> stratum);
		printf (" %4d", clock -> timer);
		printf ("   %03o", clock -> reachability);
		printf (" %8d", clock -> estdelay);
		printf (" %8d", clock -> estoffset);
		printf (" %8d", clock -> estdisp);
		p = qb2str (clock -> remoteAddress);
		printf (" %s", p);
		free(p);
		if (c == '>') {
			struct type_NTP_ClockIdentifier *ci =
					clock -> reference;
			p = qb2str(ci -> un.referenceClock);
			printf (" (%s)", p);
			free (p);
		}
		putchar ('\n');
	}
	return OK;
}


/* ARGSUSED */
int
query_error (int sd, int id, int error, struct type_IMISC_IA5List *parameter, struct RoSAPindication *roi) {
	struct RyError *rye;

	if (error == RY_REJECT) {
		advise (NULLCP, "%s", RoErrString ((int) parameter));
		return OK;
	}

	if (rye = finderrbyerr (table_NTP_Errors, error))
		advise (NULLCP, "%s",  rye -> rye_name);
	else
		advise (NULLCP, "Error %d", error);

	return OK;
}
static PE build_bind_arg () {
	struct type_NTP_BindArgument *bindarg;
	PE	pe;

	bindarg = (struct type_NTP_BindArgument *)
			  calloc (1, sizeof *bindarg);
	bindarg -> version =
		pe_alloc (PE_CLASS_UNIV, PE_FORM_PRIM,
				  PE_PRIM_BITS);
	bit_on (bindarg -> version,
			bit_NTP_version_version__1);
	bit_on (bindarg -> version,
			bit_NTP_version_version__2);

	bindarg -> mode = (struct type_NTP_BindMode *)
					  calloc (1, sizeof *bindarg->mode);
	bindarg -> mode -> parm =
		int_NTP_BindMode_query;
	if (encode_NTP_BindArgument (&pe, 1, 0, NULLCP,
								 bindarg) == NOTOK)
		pe = NULLPE;
	else pe -> pe_context = 3;
	free_NTP_BindArgument (bindarg);
	return pe;
}

void
ros_adios (struct RoSAPpreject *rop, char *event) {
	ros_advise (rop, event);

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

/* ^L */

void
acs_adios (struct AcSAPabort *aca, char *event) {
	acs_advise (aca, event);

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

#ifndef lint
void    _advise ();


static void    adios (char *what, char *fmt, ...) {
	va_list ap;

	va_start (ap, fmt);

	_advise (what, fmt, ap);

	va_end (ap);

	_exit (1);
}
#else
/* VARARGS */

static void    adios (char *what, char *fmt) {
    adios (what, fmt);
}
#endif

#ifndef lint
static void    advise (char *what, char *fmt, ...) {
    va_list ap;

    va_start (ap, fmt);

    _advise (what, fmt, ap);

	va_end (ap);
}


static void  _advise (char *what, char *fmt, va_list ap)
{
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

static void    advise (char *what, char *fmt) {
    advise (what, fmt);
}
#endif


#ifndef lint
void    ryr_advise (char *what, char *fmt, ...) {
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
