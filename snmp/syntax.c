/* syntax.c - SMI syntax handling */

/*
 * 
 *
 * Contributed by NYSERNet Inc.  This work was partially supported by the
 * U.S. Defense Advanced Research Projects Agency and the Rome Air Development
 * Center of the U.S. Air Force Systems Command under contract number
 * F30602-88-C-0016.
 *
 *
 * 
 *
 *
 *
 */

#include <stdio.h>
#include <stdint.h>
#include "SNMP-types.h"
#include "objects.h"
#include "tailor.h"

#include "internet.h"
#include "clns.h"

#define	MAXSYN	50

static object_syntax syntaxes[MAXSYN + 1];
static OS    synlast = syntaxes;

static int integer_encode (integer *x, PE *pe) {
	if ((*pe = int2prim (*x)) == NULLPE)
		return NOTOK;
	return OK;
}

static int integer_decode (integer **x, PE pe) {
	integer	i = prim2num (pe);
	if (i == NOTOK && pe -> pe_errno != PE_ERR_NONE)
		return NOTOK;
	if ((*x = (integer *) malloc (sizeof **x)) == NULL)
		return NOTOK;
	**x = i;
	return OK;
}

static void integer_free (integer *x) {
	free ((char *) x);
}

static int integer_parse (integer **x, char *s) {
	long    l;

	if (sscanf (s, "%ld", &l) != 1)
		return NOTOK;
	if ((*x = (integer *) malloc (sizeof **x)) == NULL)
		return NOTOK;
	**x = (integer) l;
	return OK;
}

static void integer_print (integer *x, OS os) {
	printf ("%d", *x);
}

static void services_print (integer *x, OS os) {
	printf ("%s", sprintb ((int) *x,
						   "\020\01physical\02datalink/subnetwork\03internet\04transport\05session\06presentation\07application"));
}

static void privs_print (integer *x, OS os) {
	printf ("%s", sprintb ((int) *x,
						   "\020\01get\02get-next\03get-response\04set\05trap"));
}

static void add_integer (void) {
	add_syntax ("INTEGER", (EncoderFunction)integer_encode, (DecoderFunction)integer_decode, (FreeFunction)integer_free,
				(ParseFunction)integer_parse, (PrintFunction)integer_print);
	add_syntax ("Services", (EncoderFunction)integer_encode, (DecoderFunction)integer_decode, (FreeFunction)integer_free,
				(ParseFunction)integer_parse, (PrintFunction)services_print);
	add_syntax ("Privileges", (EncoderFunction)integer_encode, (DecoderFunction)integer_decode, (FreeFunction)integer_free,
				(ParseFunction)integer_parse, (PrintFunction)privs_print);
}

static int string_encode (struct qbuf *x, PE *pe) {
	if ((*pe = qb2prim_aux (x, PE_CLASS_UNIV, PE_PRIM_OCTS, 0)) == NULLPE)
		return NOTOK;
	return OK;
}

static int string_decode (struct qbuf **x, PE pe) {
	struct qbuf *qb = prim2qb (pe);
	if (qb == NULL)
		return NOTOK;
	*x = qb;
	return OK;
}

static int string_parse (struct qbuf **x, char *s) {
	struct qbuf *qb;

	if (strncmp (s, "0x", 2) == 0) {
		int	len;
		char   *p;

		s += 2;
		if ((len = strlen (s)) % 3 != 2)
			return NOTOK;
		len /= 3, len++;
		if ((qb = str2qb (NULLCP, len, 1)) == NULL)
			return NOTOK;
		p = qb -> qb_forw -> qb_data;
		while (*s) {
			int	    i;
			if (sscanf (s, "%x", &i) != 1) {
oops:
				;
				qb_free (qb);
				return NOTOK;
			}
			*p++ = i & 0xff;
			s += 2;
			if (*s && *s++ != ':')
				goto oops;
		}
	} else if ((qb = str2qb (s, strlen (s), 1)) == NULL)
		return NOTOK;
	*x = qb;
	return OK;
}

static void string_print (struct qbuf *x, OS os) {
	char *cp,
		 *ep;
	char   *p;
	struct qbuf *qb;

	p = "0x";
	for (qb = x -> qb_forw; qb != x; qb = qb -> qb_forw)
		for (ep = (cp = qb -> qb_data) + qb -> qb_len; cp < ep; cp++) {
			printf ("%s%02x", p, *cp & 0xff);
			p = ":";
		}
}

static void string_display (struct qbuf *x, OS os) {
	struct qbuf *qb;

	printf ("\"");
	for (qb = x -> qb_forw; qb != x; qb = qb -> qb_forw)
		printf ("%*.*s", qb -> qb_len, qb -> qb_len, qb -> qb_data);
	printf ("\"");
}

static void add_string (void) {
	add_syntax ("OctetString", (EncoderFunction)string_encode, (DecoderFunction)string_decode, (FreeFunction)qb_free,
				(ParseFunction)string_parse, (PrintFunction)string_print);
	add_syntax ("DisplayString", (EncoderFunction)string_encode, (DecoderFunction)string_decode, (FreeFunction)qb_free,
				(ParseFunction)string_parse, (PrintFunction)string_display);
	add_syntax ("PhysAddress", (EncoderFunction)string_encode, (DecoderFunction)string_decode, (FreeFunction)qb_free,
				(ParseFunction)string_parse, (PrintFunction)string_print);
}

static int object_encode (OID x, PE *pe) {
	if ((*pe = oid2prim (x)) == NULLPE)
		return NOTOK;
	return OK;
}

static int object_decode (OID *x, PE pe) {
	OID oid = prim2oid (pe);
	if (oid == NULLOID || (*x = oid_cpy (oid)) == NULLOID)
		return NOTOK;
	return OK;
}

static int object_parse (OID *x, char *s) {
	OID	    oid = text2oid (s);
	if (oid == NULL)
		return NOTOK;
	*x = oid;
	return OK;
}

static void object_print (OID x, OS os) {
	char  *cp,
		  ode[BUFSIZ];

	strcpy (ode, oid2ode (x));
	printf ("%s", ode);
	if (strcmp (ode, cp = sprintoid (x)))
		printf (" (%s)", cp);
}

static void add_object (void) {
	add_syntax ("ObjectID", (EncoderFunction)object_encode, (DecoderFunction)object_decode, (FreeFunction)oid_free,
				(ParseFunction)object_parse, (PrintFunction)object_print);
}

static int null_encode (char *x, PE *pe) {
	if ((*pe = pe_alloc (PE_CLASS_UNIV, PE_FORM_PRIM, PE_PRIM_NULL)) == NULLPE)
		return NOTOK;
	return OK;
}

static int null_decode (char **x, PE pe) {
	if ((*x = (char *) calloc (1, sizeof **x)) == NULL)
		return NOTOK;
	return OK;
}

static void null_free (char *x) {
	free ((char *) x);
}

static int null_parse (char **x, char *s) {
	if (lexequ (s, "NULL"))
		return NOTOK;
	if ((*x = (char *) calloc (1, sizeof **x)) == NULL)
		return NOTOK;
	return OK;
}

static void null_print (char *x, OS os) {
	printf ("NULL");
}

static void add_null (void) {
	add_syntax ("NULL", (EncoderFunction)null_encode, (DecoderFunction)null_decode, (FreeFunction)null_free, (ParseFunction)null_parse, (PrintFunction)null_print);
}

static int ipaddr_encode (struct sockaddr_in *x, PE *pe) {
	if ((*pe = str2prim ((char *) &x -> sin_addr, 4, PE_CLASS_APPL, 0))
			== NULLPE)
		return NOTOK;
	return OK;
}

static int ipaddr_decode (struct sockaddr_in **x, PE pe) {
	struct type_SNMP_IpAddress *ip;
	struct qbuf *qb;
	struct sockaddr_in *isock;

	if (decode_SNMP_IpAddress (pe, 1, NULLIP, NULLVP, &ip) == NOTOK)
		return NOTOK;
	if (qb_pullup (ip) == NOTOK
			|| (isock = (struct sockaddr_in *) calloc (1, sizeof *isock))
			== NULL) {
		free_SNMP_IpAddress (ip);
		return NOTOK;
	}
	if ((qb = ip -> qb_forw) -> qb_len != 4) {
		free ((char *) isock);
		free_SNMP_IpAddress (ip);
		return NOTOK;
	}
	isock -> sin_family = AF_INET;
	bcopy (qb -> qb_data,
		   (char *) &isock -> sin_addr,
		   sizeof isock -> sin_addr);
	*x = isock;
	free_SNMP_IpAddress (ip);
	return OK;
}

static void ipaddr_free (struct sockaddr_in *x) {
	free ((char *) x);
}

static int  ipaddr_parse (struct sockaddr_in **x, char *s) {
	struct hostent *hp = gethostbystring (s);
	struct sockaddr_in *isock;

	if (hp == NULL)
		return NOTOK;
	if ((isock = (struct sockaddr_in *) calloc (1, sizeof *isock)) == NULL)
		return NOTOK;
	isock -> sin_family = AF_INET;
	inaddr_copy (hp, isock);
	*x = isock;
	return OK;
}

static void ipaddr_print (struct sockaddr_in *x, OS os) {
	printf ("%s", inet_ntoa (x -> sin_addr));
}

static void add_ipaddr (void) {
	add_syntax ("IpAddress", (EncoderFunction)ipaddr_encode, (DecoderFunction)ipaddr_decode, (FreeFunction)ipaddr_free,
				(ParseFunction)ipaddr_parse, (PrintFunction)ipaddr_print);
}

/* good enough for now (and probably forever)... */

static void add_netaddr (void) {
	add_syntax ("NetworkAddress", (EncoderFunction)ipaddr_encode, (DecoderFunction)ipaddr_decode,
				(FreeFunction)ipaddr_free, (ParseFunction)ipaddr_parse, (PrintFunction)ipaddr_print);
}

/* also used in SNMP-capable gawk... */
uint32_t prim2ulong (PE pe) {
	uint32_t   i;
	PElementData dp, ep;

	if (pe -> pe_form != PE_FORM_PRIM || (dp = pe -> pe_prim) == NULLPED)
		return pe_seterr (pe, PE_ERR_PRIM, 0);
	if (pe -> pe_len > sizeof (i) + 1)
		return pe_seterr (pe, PE_ERR_OVER, 0);
	if (pe -> pe_len == sizeof (i) + 1 && (*dp & 0x7f))
		return pe_seterr (pe, PE_ERR_OVER, 0);
	if (*dp & 0x80)
		return pe_seterr (pe, PE_ERR_SIGNED, 0);
	pe -> pe_errno = PE_ERR_NONE;	/* in case result is ZERO-valued */
	i = 0L;
	for (ep = dp + pe -> pe_len; dp < ep;)
		i = (i << 8) | (*dp++ & 0xff);
	return i;
}

/* also used in SNMP-capable gawk... */
PE  ulong2prim (uint32_t i, PElementClass class, PElementID id) {
	int	    extend;
	int    n;
	uint32_t mask;
	PElementData dp;
	PE	    pe;

	if ((pe = pe_alloc (class, PE_FORM_PRIM, id)) == NULLPE)
		return NULLPE;
	mask = 0xff << (((n = sizeof i) - 1) * 8);
	while (n > 1 && (i & mask) == 0)
		mask >>= 8, n--;
	extend = (i & (0x80 << ((n - 1) * 8))) ? 1 : 0;
	if ((pe -> pe_prim = PEDalloc (n + extend)) == NULLPED) {
		pe_free (pe);
		return NULLPE;
	}
	for (dp = pe -> pe_prim + (pe -> pe_len = n + extend); n-- > 0; i >>= 8)
		*--dp = i & 0xff;
	if (extend)
		*--dp = 0x00;
	return pe;
}

static int  counter_encode (uint32_t *x, PE *pe) {
	if ((*pe = ulong2prim (*x, PE_CLASS_APPL, 1)) == NULLPE)
		return NOTOK;

	return OK;
}

static int counter_decode (uint32_t **x, PE pe) {
	uint32_t	i = prim2ulong (pe);

	if (i == 0 && pe -> pe_errno != PE_ERR_NONE)
		return NOTOK;
	if ((*x = (uint32_t *) malloc (sizeof **x)) == NULL)
		return NOTOK;
	**x = i;
	return OK;
}

static void counter_free (uint32_t *x) {
	free ((char *) x);
}

static int counter_parse (uint32_t **x, char *s) {
	uint32_t  i;

	if (sscanf (s, "%U", &i) != 1)
		return NOTOK;
	if ((*x = (uint32_t *) malloc (sizeof **x)) == NULL)
		return NOTOK;
	**x = i;
	return OK;
}

static void counter_print (uint32_t *x, OS os) {
	printf ("%U", *x);
}

static void add_counter (void) {
	add_syntax ("Counter", (EncoderFunction)counter_encode, (DecoderFunction)counter_decode, (FreeFunction)counter_free,
				(ParseFunction)counter_parse, (PrintFunction)counter_print);
}

static int  gauge_encode (uint32_t *x, PE *pe) {
	if ((*pe = ulong2prim (*x, PE_CLASS_APPL, 2)) == NULLPE)
		return NOTOK;
	return OK;
}

static void add_gauge (void) {
	add_syntax ("Gauge", (EncoderFunction)gauge_encode, (DecoderFunction)counter_decode, (FreeFunction)counter_free,
				(ParseFunction)counter_parse, (PrintFunction)counter_print);
}

static int timeticks_encode (uint32_t *x, PE *pe) {
	if ((*pe = ulong2prim (*x, PE_CLASS_APPL, 3)) == NULLPE)
		return NOTOK;
	return OK;
}

static void timeticks_print (uint32_t *x, OS os) {
	uint32_t  d,
			h,
			m,
			s,
			ds;

	ds = *x;
	s = ds / 100, ds = ds % 100;
	m = s / 60, s = s % 60;
	h = m / 60, m = m % 60;
	d = h / 24, h = h % 24;

	if (d > 0)
		printf ("%d days, ", d);
	if (d > 0 || h > 0)
		printf ("%d hours, ", h);
	if (d > 0 || h > 0 || m > 0)
		printf ("%d minutes, ", m);
	printf ("%d", s);
	if (ds > 0)
		printf (".%02d", ds);
	printf (" seconds (%U timeticks)", *x);
}

static void add_timeticks (void) {
	add_syntax ("TimeTicks", (EncoderFunction)timeticks_encode, (DecoderFunction)counter_decode, (FreeFunction)counter_free,
				(ParseFunction)counter_parse, (PrintFunction)timeticks_print);
}

static int clnpaddr_encode (x, pe)
struct sockaddr_iso *x;
PE     *pe;
{
	char    buffer[sizeof x -> siso_data + 1];

	buffer[0] = x -> siso_nlen & 0xff;
	bcopy (x -> siso_data, buffer + 1, (int) x -> siso_nlen);

	if ((*pe = str2prim (buffer, (int) (x -> siso_nlen + 1), PE_CLASS_APPL,
						 5)) == NULLPE)
		return NOTOK;

	return OK;
}

static int clnpaddr_decode (x, pe)
struct sockaddr_iso **x;
PE	pe;
{
	int	    len;
	struct type_SNMP_ClnpAddress *clnp;
	struct qbuf *qb;
	struct sockaddr_iso *isock;

	if (decode_SNMP_ClnpAddress (pe, 1, NULLIP, NULLVP, &clnp) == NOTOK)
		return NOTOK;
	if (qb_pullup (clnp) == NOTOK
			|| (isock = (struct sockaddr_iso *) calloc (1, sizeof *isock))
			== NULL) {
		free_SNMP_ClnpAddress (clnp);
		return NOTOK;
	}
	qb = clnp -> qb_forw;
	isock -> siso_family = AF_ISO;
	if ((len = qb -> qb_data[0] & 0xff) >= qb -> qb_len)
		len = qb -> qb_len - 1;
	bcopy (qb -> qb_data + 1, isock -> siso_data,
		   (int) (isock -> siso_nlen = len));

	*x = isock;

	free_SNMP_ClnpAddress (clnp);
	return OK;
}

static void clnpaddr_free (struct sockaddr_iso *x) {
	free ((char *) x);
}

static int clnpaddr_parse (x, s)
struct sockaddr_iso **x;
char   *s;
{
	struct sockaddr_iso *isock;

	if ((isock = (struct sockaddr_iso *) calloc (1, sizeof *isock)) == NULL)
		return NOTOK;
	isock -> siso_family = AF_ISO;
	isock -> siso_nlen = implode ((uint8_t *) isock -> siso_data, s,
								  strlen (s));
	*x = isock;
	return OK;
}

static void clnpaddr_print (struct sockaddr_iso *x, OS os) {
	char    buffer[sizeof x -> siso_data * 2 + 1];
	buffer[explode (buffer, (uint8_t *) x -> siso_data, (int) x -> siso_nlen)] = 0;
	printf ("NS+%s", buffer);
}

static void add_clnpaddr (void) {
	add_syntax ("ClnpAddress", (EncoderFunction)clnpaddr_encode, (DecoderFunction)clnpaddr_decode, (FreeFunction)clnpaddr_free,
				(ParseFunction)clnpaddr_parse, (PrintFunction)clnpaddr_print);
}

void readsyntax (void) {
	add_integer ();
	add_string ();
	add_object ();
	add_null ();
	add_ipaddr ();
	add_netaddr ();
	add_counter ();
	add_gauge ();
	add_timeticks ();
	add_clnpaddr ();
}

int	add_syntax (
	char *name,
	EncoderFunction f_encode,
	DecoderFunction f_decode,
	FreeFunction f_free,
	ParseFunction f_parse,
	PrintFunction f_print
) {
	int	    i;
	OS	    os = synlast++;

	if ((i = synlast - syntaxes) >= MAXSYN)
		return NOTOK;
	bzero ((char *) os, sizeof *os);
	os -> os_name = name;
	os -> os_encode = f_encode;
	os -> os_decode = f_decode;
	os -> os_free = f_free;
	os -> os_parse = f_parse;
	os -> os_print = f_print;
	return i;
}

OS	text2syn (name)
char   *name;
{
	OS	    os;

	for (os = syntaxes; os < synlast; os++)
		if (strcmp (os -> os_name, name) == 0)
			return os;

	return NULLOS;
}

void flsyntax (OS *first, OS *last) {
	if (first)
		*first = syntaxes;
	if (last)
		*last = synlast - 1;
}
