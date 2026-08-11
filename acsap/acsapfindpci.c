/* acsapfindpci.c - find PCI for ACSE */







#include <stdio.h>
#include <signal.h>
#define	ACSE
#include "acpkt.h"

int AcFindPCI (int sd, int *pci, struct AcSAPindication *aci) {
	SBV     smask;
	struct assocblk  *acb;

	missingP (pci);
	missingP (aci);

	smask = sigioblock ();

	if ((acb = findacblk (sd)) == NULL) {
		sigiomask (smask);
		return acsaplose (aci, ACS_PARAMETER, NULLCP,
						  "invalid association descriptor");
	}

	*pci = acb -> acb_id;

	sigiomask (smask);

	return OK;
}
