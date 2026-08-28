/* aetseq.c - application entity titles -- sequential lookup */

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "psap.h"
#include "isoaddrs.h"
#include "acsap.h"

extern int _startisoentity (char *descriptor);
extern int _stopisoentity (const char *descriptor, struct isoentity *iep);

static char objent[BUFSIZ];
static struct isoentity ies;

int str2aet_seq (char *designator, char *qualifier, struct isoentity *iep) {
	int     hitdes,
			hitqual;
	char    descriptor[BUFSIZ],
			desdflt[BUFSIZ],
			qualdflt[BUFSIZ];
	struct isoentity  *ie;
	struct isoentity ids,
			   iqs;

	sprintf (objent, "%s-%s", designator, qualifier);
	sprintf (desdflt, "%s-%s", designator, "default");
	sprintf (qualdflt, "%s-%s", "default", qualifier);
	hitdes = hitqual = 0;
	bzero ((char *) &ids, sizeof ids);
	bzero ((char *) &iqs, sizeof iqs);

	ie = NULL;

	if (!setisoentity (0))
		return NOTOK;
	while (_startisoentity (descriptor) == OK) {
		if (strcmp (descriptor, objent) == 0) {
			if (_stopisoentity (descriptor, &ies) != OK)
				continue;

			ie = &ies;
			break;
		}

		if (!hitdes && strcmp (descriptor, desdflt) == 0) {
			if (_stopisoentity (descriptor, &ies) != OK)
				continue;
			ies.ie_descriptor = objent;

			hitdes++;
			ids = ies;		/* struct copy */
			continue;
		}

		if (!hitqual && strcmp (descriptor, qualdflt) == 0) {
			if (_stopisoentity (descriptor, &ies) != OK)
				continue;
			ies.ie_descriptor = objent;

			hitqual++;
			iqs = ies;		/* struct copy */
			continue;
		}
	}
	endisoentity ();

	if (!ie && hitqual) {
		ie = &ies;
		*ie = iqs;		/* struct copy */

		if (hitdes) {
			bcopy ((char *) ids.ie_addr.pa_addr.sa_addr.ta_addrs,
				   (char *) ie -> ie_addr.pa_addr.sa_addr.ta_addrs,
				   sizeof ie -> ie_addr.pa_addr.sa_addr.ta_addrs);
			ie -> ie_addr.pa_addr.sa_addr.ta_naddr =
				ids.ie_addr.pa_addr.sa_addr.ta_naddr;
		}
	}

	if (ie) {
		*iep = *ie;	/* struct copy */
		return OK;
	}

	return NOTOK;
}
