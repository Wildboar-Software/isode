/* isoentity.c - application entity titles -- sequential lookup utilities  */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "psap.h"
#include "isoaddrs.h"
#include "tailor.h"

static char *isoentities = "isoentities";

static FILE *servf = NULL;
static int   stayopen = 0;

static int   mask;
static int   vecp;
static char  buffer[BUFSIZ + 1];
static char *vec[NVEC + NSLACK + 1];

static struct isoentity ies;

int setisoentity (int f) {
	if (servf == NULL)
		servf = fopen (isodefile (isoentities, 0), "r");
	else
		rewind (servf);
	stayopen |= f;

	return (servf != NULL);
}

int endisoentity(void) {
	if (servf && !stayopen) {
		fclose (servf);
		servf = NULL;
	}

	return 1;
}

struct isoentity *getisoentity(void) {
	static char  descriptor[BUFSIZ];

	if (servf == NULL
			&& (servf = fopen (isodefile (isoentities, 0), "r")) == NULL)
		return NULL;

	while (_startisoentity (descriptor) == OK)
		if (_stopisoentity (descriptor, (struct isoentity *) NULL) == OK)
			return (&ies);

	return NULL;
}

int _startisoentity (char *descriptor) {
	char  *cp,
		  *dp;
	char   *ep;

	ep = (dp = buffer) + sizeof buffer;
	while (fgets (dp, ep - dp, servf) != NULL) {
		if (*buffer == '#')
			continue;
		if (*dp == '\n' && dp == buffer)
			continue;
		if (cp = index (buffer, '\n')) {
			*cp-- = 0;
			if (*cp == '\\') {
				if ((dp = cp) + 1 >= ep)
					dp = buffer;
				continue;
			}
		}

		switch (vecp = str2vecX (buffer, vec, 5 + 1, &mask, 0, 1)) {
		case 3:			/* no address */
		case 4:			/* new-style */
			break;

		default:
			continue;
		}

		sprintf (descriptor, "%s-%s", vec[0], vec[1]);

		return OK;
	}

	return DONE;
}

int _stopisoentity (char *descriptor, struct isoentity *iep) {
	int    i;
	struct isoentity  *ie = &ies;
	OID		oid = &ie -> ie_identifier;
	struct PSAPaddr *pa = &ie -> ie_addr;
	struct PSAPaddr *pz;
	static unsigned int elements[NELEM + 1];
	bzero ((char *) ie, sizeof *ie);
	if (strcmp (vec[2], "NULL") == 0)
		elements[i = 0] = 0;
	else if ((i = str2elem (vec[2], elements)) <= 1)
		return NOTOK;
	oid -> oid_elements = elements;
	oid -> oid_nelem = i;
	ie -> ie_descriptor = descriptor;
	switch (vecp) {
	case 3:		/* no address */
		break;

	case 4:		/* new-style */
		if (pz = str2paddr (vec[3]))
			*pa = *pz;		/* struct copy */
		break;
	}
	if (iep)
		*iep = *ie;	/* struct copy */
	return OK;
}

int _printent (struct isoentity *ie) {
	LLOG (addr_log, LLOG_DEBUG,
		  ("Entity:  %s (%s)", ie -> ie_descriptor,
		   oid2ode (&ie -> ie_identifier)));
	ll_printf (addr_log, "Address: %s\n",
			   paddr2str (&ie -> ie_addr, NULLNA));
	ll_printf (addr_log, "///////\n");
	ll_sync (addr_log);
}
