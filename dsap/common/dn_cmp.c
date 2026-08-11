#include "quipu/util.h"
#include "quipu/name.h"

int dn_cmp (DN a, DN b)
{
	int res;

	for (; (a != NULLDN) && (b != NULLDN) ; a = a->dn_parent, b = b->dn_parent)
		if ( (res = rdn_cmp (a->dn_rdn,b->dn_rdn)) != OK) {
			return res;
		}

	if (( a == NULLDN) && (b == NULLDN)) {
		return OK;
	} else {
		return ( a == NULLDN ? 1 : -1 );
	}

}

int dn_cmp_prefix (DN a, DN b)
{
	for (; a != NULLDN && b != NULLDN ; a = a->dn_parent, b = b->dn_parent)
		if ( dn_comp_cmp (a,b) == NOTOK) {
			return NOTOK;
		}
	if ( a == NULLDN) {
		return OK;
	} else {
		return NOTOK;
	}
}

int dn_order_cmp (DN a, DN b)
{
	int     i;

	for ( ; (a != NULLDN) && (b != NULLDN); a = a->dn_parent,
			b = b->dn_parent ) {
		if ( (i = rdn_cmp( a->dn_rdn, b->dn_rdn )) != 0 ) {
			return( i );
		}
	}

	if ( ( a == NULLDN) && (b == NULLDN) ) {
		return( 0 );
	} else if ( b ) {       /* b longer, so a is less */
		return( -1 );
	} else {                /* a must be longer */
		return( 1 );
	}
	/* NOTREACHED */
}
