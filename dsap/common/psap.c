/* psap.c - General PSAP utility routines */

#include "quipu/util.h"
#include "quipu/attr.h"
#include "psap.h"
#include "isoaddrs.h"
#include "acsap.h"
#include "../x500as/DO-types.h"

extern LLog * log_dsap;

void psap_free (struct PSAPaddr *psap) {
	free ((char *)psap) ;
}

struct PSAPaddr *psap_cpy (struct PSAPaddr *a) {
	struct PSAPaddr * r;
	r = (struct PSAPaddr *) smalloc (sizeof (struct PSAPaddr));
	bzero ((char *) r,sizeof (struct PSAPaddr));
	*r = *a;        /* struct copy */
	return (r);
}

static void *psap_cpy_void (void *value)
{
	struct PSAPaddr *a = (struct PSAPaddr *) value;
	return psap_cpy (a);
}

static void psap_free_void (void *value)
{
	struct PSAPaddr *psap = (struct PSAPaddr *) value;
	psap_free (psap);
}

void psap_dup (struct PSAPaddr *r, struct PSAPaddr *a) {
	*r = *a;    /* struct copy */
}

static int psap_cmp (void *value1, void *value2) {
	struct PSAPaddr *r = (struct PSAPaddr *) value1;
	struct PSAPaddr *a = (struct PSAPaddr *) value2;

	return (bcmp ((char *) r, (char *) a, sizeof *a) ? (-1) : 0);
}

static PE psap_enc (void *value) {
	struct PSAPaddr *p = (struct PSAPaddr *) value;
	PE ret_pe;

	if (build_DSE_PSAPaddr (&ret_pe,0,0,NULLCP,(char *)p) == NOTOK ) {
		ret_pe = NULLPE;
		LLOG (log_dsap,LLOG_EXCEPTIONS, ("Failed to encode PSAP"));
	}
	return (ret_pe);
}

static void *psap_dec (PE pe) {
	struct PSAPaddr *psap;

	psap = (struct PSAPaddr *) smalloc (sizeof *psap);
	if (parse_DSE_PSAPaddr (pe, 1, NULL, NULLVP, (char *)psap) == NOTOK) {
		free ((char *)psap);
		return (NULLPA);
	}
	return (psap);
}

static void *psap_parse (char *s) {
	struct PSAPaddr *pa;
	struct PSAPaddr *psap;

	psap = (struct PSAPaddr *) calloc (1,sizeof (struct PSAPaddr));
	if (pa=str2paddr(s)) {
		*psap = *pa;  /* struct copy */
		return (psap);
	} else {
		parse_error ("invalid presentation address %s",s);
		free ((char *)psap);
		return (NULLPA);
	}
}

static void psap_print (PS ps, void *value, int format) {
	struct PSAPaddr *p = (struct PSAPaddr *) value;

	if (format != READOUT)
		ps_printf (ps, "%s", _paddr2str(p,NULLNA,-1));
	else
		ps_printf (ps, "%s", paddr2str(p,NULLNA));
}

void psap_syntax (void) {
	add_attribute_syntax ("presentationAddress",
						  psap_enc,		psap_dec,
						  psap_parse,	psap_print,
						  psap_cpy_void,	psap_cmp,
						  psap_free_void,  	NULLCP,
						  NULL,		TRUE );
}
