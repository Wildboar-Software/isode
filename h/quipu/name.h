/* name.h - */

#ifndef QUIPUNAME
#define QUIPUNAME

#include "quipu/attr.h"

typedef struct rdncomp {        /* RDN is sequence of attribute value   */
	/* assertions                           */
	/* represents RelativeDistinguishedName */
	attrType       	rdn_at;
	attrVal      	rdn_av;
	struct rdncomp      *rdn_next;
} rdncomp, *RDN;

#define NULLRDN ((RDN) 0)
#define rdn_comp_alloc()          (RDN) smalloc(sizeof(rdncomp))
RDN rdn_comp_new (AttributeType at, AttributeValue av);
RDN str2rdn(char *str);
RDN rdn_cpy (RDN rdn);
RDN rdn_comp_cpy (RDN rdn);
RDN rdn_merge (RDN a, RDN b);
int rdn_cmp (RDN a, RDN b);
void rdn_free (RDN rdn);
void rdn_comp_free (RDN rdn);
int rdn_cmp_comp (RDN a, RDN b);
int rdn_cmp_reverse (RDN a, RDN b);

typedef struct dncomp {         /* DN is sequence of RDNs.              */
	/* represents RDNSequence which is      */
	/* equivalent to DistinguishedName      */
	RDN                 dn_rdn;
	struct dncomp       *dn_parent;
} dncomp, *DN;

#define NULLDN ((DN) 0)

#define dn_comp_alloc()        (DN) smalloc(sizeof(dncomp))
#define dn_comp_print(x,y,z)   if (y!=NULLDN) rdn_print(x,y->dn_rdn,z)
#define dn_comp_fill(x,y)     x -> dn_rdn = y
#define dn_comp_cmp(x,y)      ((rdn_cmp (x->dn_rdn ,y->dn_rdn) == OK) ? OK : NOTOK )

DN dn_comp_new (RDN rdn);
DN dn_comp_cpy (DN dn);
DN dn_cpy (DN dn);
DN str2dn (char *str);
int dn_cmp (DN a, DN b);
int dn_cmp_prefix (DN a, DN b);
int dn_order_cmp (DN a, DN b);
void dn_free (DN dn);
void dn_comp_free (DN dn);
void dn_append (DN a, DN b);
char *dn2edbfile (DN dn);

char *dn2str (DN dn);
char *dn2ufn (DN dn, const int multiline);
char *dn2rfc (DN dn, const char *sep);

void rdn_print (PS ps, RDN rdn, int format);
void dn_print (PS ps, DN dn, int format);
void dn_rfc_print (PS ps, DN dn, const char *sep);
void ufn_dn_print (PS ps, DN dn, const int multiline);
void ufn_rdn_print (PS ps, RDN rdn);

int ufn_dn_print_aux (PS ps, DN dn, DN marker, const int multiline);

extern int ufn_indent;

#endif
