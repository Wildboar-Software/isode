/* system.c - MIB realization of the System group */
/*
 * Contributed by NYSERNet Inc.  This work was partially supported by the
 * U.S. Defense Advanced Research Projects Agency and the Rome Air Development
 * Center of the U.S. Air Force Systems Command under contract number
 * F30602-88-C-0016.
 */

#include <stdio.h>
#include <unistd.h>
#include "mib.h"
#include "tailor.h"
#include <sys/time.h>
static int  o_sysUpTime (OI oi, struct type_SNMP_VarBind *v, int offset);
void init_system (void);


static int  o_sysUpTime (OI oi, struct type_SNMP_VarBind *v, int offset) {
	struct timeval now;
	OID    oid = oi -> oi_name;
	OT	    ot = oi -> oi_type;
	static   int lastq = -1;
	static   integer diff;

	switch (offset) {
	case type_SNMP_PDUs_get__request:
		if (oid -> oid_nelem != ot -> ot_name -> oid_nelem + 1
				|| oid -> oid_elements[oid -> oid_nelem - 1] != 0)
			return int_SNMP_error__status_noSuchName;
		break;

	case type_SNMP_PDUs_get__next__request:
		if (oid -> oid_nelem == ot -> ot_name -> oid_nelem) {
			OID	new;
			if ((new = oid_extend (oid, 1)) == NULLOID)
				return NOTOK;
			new -> oid_elements[new -> oid_nelem - 1] = 0;
			if (v -> name)
				free_SNMP_ObjectName (v -> name);
			v -> name = new;
		} else
			return NOTOK;
		break;

	default:
		return int_SNMP_error__status_genErr;
	}
	if (quantum != lastq) {
		lastq = quantum;
		if (gettimeofday (&now, (struct timezone *) 0) == NOTOK) {
			advise (LLOG_EXCEPTIONS, "failed", "gettimeofday");
			return generr (offset);
		}
		diff = (now.tv_sec - my_boottime.tv_sec) * 100
			   + ((now.tv_usec - my_boottime.tv_usec) / 10000);
	}
	return o_number (oi, v, (caddr_t) &diff);
}

static struct sys_pair {
	char   *s_name;
	char   *s_text;
	int	    (*s_getfnx)(OI oi, struct type_SNMP_VarBind *v, int offset);
	int	    (*s_setfnx)(OI oi, struct type_SNMP_VarBind *v, int offset);
}    pairs[] = {
	"sysDescr",    sysDescr,	o_generic,	NULL,
	"sysObjectID", sysObjectID, o_generic,	NULL,
	"sysUpTime",   NULL,	o_sysUpTime,	NULL,
	"sysContact",  NULL,	o_generic,	s_generic,
#define	SYS_NAME	4
	"sysName",     NULL,	o_generic,	s_generic,
	"sysLocation", NULL,	o_generic,	s_generic,
	"sysServices", "72",	o_generic,	NULL,

	NULL
};

void init_system (void) {
	char    buffer[BUFSIZ];
	OT	    ot;
	struct sys_pair *sp;

	gethostname (buffer, sizeof buffer);
	pairs[SYS_NAME].s_text = buffer;
	for (sp = pairs; sp -> s_name; sp++)
		if (ot = text2obj (sp -> s_name)) {
			ot -> ot_getfnx = sp -> s_getfnx;
			ot -> ot_setfnx = sp -> s_setfnx;
			if (sp -> s_text)
				if (ot -> ot_syntax)
					(*ot -> ot_syntax -> os_parse) ((void **)&ot -> ot_info,
													sp -> s_text);
				else
					advise (LLOG_EXCEPTIONS, NULLCP, "%s: no syntax",
							sp -> s_name);
		} else
			advise (LLOG_EXCEPTIONS, NULLCP, "%s: unknown object",
					sp -> s_name);
}
