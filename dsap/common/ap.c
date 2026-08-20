/* ap.c - Quipu access point syntax  */

#include "quipu/util.h"
#include "quipu/attr.h"
#include "psap.h"
#include "isoaddrs.h"
#include "../x500as/DO-types.h"
#include "pepsycodec.h"

extern LLog * log_dsap;
extern struct PSAPaddr * psap_cpy ();
extern void aps_free (struct access_point *app);

void aps_free_void (void *value) {
	aps_free ((struct access_point *) value);
}

static void *qap_cpy (void *value) {
	struct access_point *a = (struct access_point *) value;
	struct access_point * r;

	r = (struct access_point *) smalloc (sizeof (struct access_point));
	bzero ((char *) r,sizeof (struct access_point));
	r -> ap_name = dn_cpy (a -> ap_name);
	if (a -> ap_address)
		r -> ap_address = psap_cpy ( a -> ap_address );
	return (r);
}

static int qap_cmp (void *value1, void *value2) {
	struct access_point *r = (struct access_point *) value1;
	struct access_point *a = (struct access_point *) value2;
	int res;

	if (( res = dn_cmp (r -> ap_name, a -> ap_name)) == 0)
		if ( r -> ap_address && a -> ap_address )
			return (bcmp ((char *) r -> ap_address,
						  (char *) a -> ap_address,
						  sizeof *a -> ap_address) ? (-1) : 0);
		else
			return ( r -> ap_address == a -> ap_address ? 0 :
					 r -> ap_address > a -> ap_address ? 1 : (-1));
	return res;
}

static PE qap_enc (void *value) {
	struct access_point *p = (struct access_point *) value;
	PE ret_pe;

	if (encode_DO_QAccessPoint (&ret_pe,0,0,NULLCP,p) == NOTOK )
		return NULLPE;
	return (ret_pe);
}

static void *qap_dec (PE pe) {
	struct access_point *qap;

	if (decode_DO_QAccessPoint (pe,1,NULL,NULLVP,&qap) == NOTOK) {
		return (NULLACCESSPOINT);
	}
	return (qap);
}

static void *qap_parse (char *s) {
	struct PSAPaddr *pa;
	struct access_point *qap;
	char * p;

	qap = (struct access_point *) calloc (1,sizeof (struct access_point));
	if ((p = index (s,'#')) != NULLCP) {
		*p++ = 0;
		if (pa=str2paddr(SkipSpace(p))) {
			qap->ap_address = (struct PSAPaddr *) calloc (1,sizeof (struct PSAPaddr));
			*qap->ap_address = *pa;  /* struct copy */
		} else {
			parse_error ("invalid presentation address in access point %s",p);
			free ((char *)qap);
			*(--p) = '#';
			return (NULLACCESSPOINT);
		}
	}
	if ((qap -> ap_name = str2dn (s)) == NULLDN) {
		if (qap->ap_address)
			free ((char *)qap->ap_address);
		free ((char *)qap);
		return NULLACCESSPOINT;
	}
	if (p)
		*--p = '#';
	return qap;
}

static void qap_print (PS ps, void *value, int format) {
	struct access_point *p = (struct access_point *) value;

	dn_print (ps, p -> ap_name, format);
	if ( p -> ap_address )
		if (format != READOUT)
			ps_printf (ps, " # %s", _paddr2str(p->ap_address,NULLNA,-1));
		else
			ps_printf (ps, " # %s", paddr2str(p->ap_address,NULLNA));
}

void ap_syntax (void) {
	add_attribute_syntax ("AccessPoint",
						  qap_enc,		qap_dec,
						  qap_parse,	qap_print,
						  qap_cpy,		qap_cmp,
						  aps_free_void,		NULLCP,
						  NULL,			TRUE );
}
