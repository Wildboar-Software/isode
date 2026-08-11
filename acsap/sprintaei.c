/* sprintaei.c - manage AE info */







#include <stdio.h>
#include <string.h>
#include "psap.h"
#include "isoaddrs.h"

char *sprintaei (AEI aei) {
	char *cp;
	char   *bp;
	static int    i;
	static char buffer1[BUFSIZ],
		   buffer2[BUFSIZ];

	bp = cp = (i++ % 2) ? buffer1 : buffer2;

	*cp++ = '<';

	if (aei -> aei_ap_title) {
		vpushstr (cp);
		vunknown (aei -> aei_ap_title);
		vpopstr ();
		cp += strlen (cp);
	}
	*cp++ = ',';

	if (aei -> aei_ae_qualifier) {
		vpushstr (cp);
		vunknown (aei -> aei_ae_qualifier);
		vpopstr ();
		cp += strlen (cp);
	}
	*cp++ = ',';

	if (aei -> aei_flags & AEI_AE_ID) {
		sprintf (cp, "%d", aei -> aei_ae_id);
		cp += strlen (cp);
	}
	*cp++ = ',';

	if (aei -> aei_flags & AEI_AP_ID) {
		sprintf (cp, "%d", aei -> aei_ap_id);
		cp += strlen (cp);
	}
	strcpy (cp, ">");

	return bp;
}
