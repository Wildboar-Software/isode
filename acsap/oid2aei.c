/* oid2aei.c - application entity titles -- OID to AE info  */

#include <strings.h>
#include "psap.h"
#include "isoaddrs.h"

AEI	oid2aei (OID oid) {
	static AEInfo aeinfo;
	AEI	    aei = &aeinfo;
	static PE pe = NULLPE;

	if (pe)
		pe_free (pe);
	bzero ((char *) aei, sizeof *aei);
	aei -> aei_ap_title = pe = obj2prim (oid, PE_CLASS_UNIV, PE_PRIM_OID);
	return aei;
}
