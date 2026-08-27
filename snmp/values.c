/* values.c - encode values */
/*
 * Contributed by NYSERNet Inc.  This work was partially supported by the
 * U.S. Defense Advanced Research Projects Agency and the Rome Air Development
 * Center of the U.S. Air Force Systems Command under contract number
 * F30602-88-C-0016.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "SNMP-types.h"
#include "objects.h"
#include "logger.h"

#define	ADVISE	if (o_advise) (*o_advise)

void (*o_advise)(int code, char *what, char *fmt, ...) = NULL;

int	o_generic (OI oi, struct type_SNMP_VarBind *v, int offset) {
	OID    oid = oi -> oi_name;
	OT	    ot = oi -> oi_type;
	OS	    os = ot -> ot_syntax;

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
	if (os == NULLOS) {
		ADVISE (LLOG_EXCEPTIONS, NULLCP,
				"no syntax defined for object \"%s\"", ot -> ot_text);
		return (offset == type_SNMP_PDUs_get__next__request ? NOTOK
				: int_SNMP_error__status_genErr);
	}
	if (ot -> ot_info == NULL) {
		ADVISE (LLOG_EXCEPTIONS, NULLCP,
				"no value defined for object \"%s\"", ot -> ot_text);
		return (offset == type_SNMP_PDUs_get__next__request ? NOTOK
				: int_SNMP_error__status_noSuchName);
	}
	if (v -> value)
		free_SNMP_ObjectSyntax (v -> value), v -> value = NULL;
	if ((*os -> os_encode) (ot -> ot_info, &v -> value) == NOTOK) {
		ADVISE (LLOG_EXCEPTIONS, NULLCP,
				"encoding error for variable \"%s\"",
				oid2ode (oi -> oi_name));
		return (offset == type_SNMP_PDUs_get__next__request ? NOTOK
				: int_SNMP_error__status_genErr);
	}
	return int_SNMP_error__status_noError;
}

int	s_generic (OI oi, struct type_SNMP_VarBind *v, int offset)
{
	OID    oid = oi -> oi_name;
	OT	    ot = oi -> oi_type;
	OS	    os = ot -> ot_syntax;

	switch (offset) {
	case type_SNMP_PDUs_set__request:
	case type_SNMP_PDUs_commit:
	case type_SNMP_PDUs_rollback:
		if (oid -> oid_nelem != ot -> ot_name -> oid_nelem + 1
				|| oid -> oid_elements[oid -> oid_nelem - 1] != 0)
			return int_SNMP_error__status_noSuchName;
		break;

	default:
		return int_SNMP_error__status_genErr;
	}
	if (os == NULLOS) {
		ADVISE (LLOG_EXCEPTIONS, NULLCP,
				"no syntax defined for object \"%s\"", ot -> ot_text);
		return int_SNMP_error__status_genErr;
	}
	switch (offset) {
	case type_SNMP_PDUs_set__request:
		if (ot -> ot_save)
			(*os -> os_free) (ot -> ot_save), ot -> ot_save = NULL;
		if ((*os -> os_decode) ((void **)&ot -> ot_save, v -> value) == NOTOK)
			return int_SNMP_error__status_badValue;
		if (os -> os_data2) {
			const integer	value = *((integer *) ot -> ot_save);
			if (value <= 0 || value > os -> os_data2)
				return int_SNMP_error__status_badValue;
		}
		break;

	case type_SNMP_PDUs_commit:
		if (ot -> ot_info)
			(*os -> os_free) (ot -> ot_info);
		ot -> ot_info = ot -> ot_save, ot -> ot_save = NULL;
		break;

	case type_SNMP_PDUs_rollback:
		if (ot -> ot_save)
			(*os -> os_free) (ot -> ot_save), ot -> ot_save = NULL;
		break;
	}
	return int_SNMP_error__status_noError;
}

int	o_longword (OI oi, struct type_SNMP_VarBind *v, const integer number) {
	return o_number (oi, v, (caddr_t) &number);
}

int	o_number (OI oi, struct type_SNMP_VarBind *v, caddr_t number) {
	int	    result;
	OT	    ot = oi -> oi_type;
	OS	    os = ot -> ot_syntax;

	if (os == NULLOS) {
		ADVISE (LLOG_EXCEPTIONS, NULLCP,
				"no syntax defined for object \"%s\"", ot -> ot_text);
		return int_SNMP_error__status_genErr;
	}
	if (v -> value)
		free_SNMP_ObjectSyntax (v -> value), v -> value = NULL;
	result = (*os -> os_encode) (number, &v -> value);
	if (result == NOTOK) {
		ADVISE (LLOG_EXCEPTIONS, NULLCP,
				"encoding error for variable \"%s\"",
				oid2ode (oi -> oi_name));
		return int_SNMP_error__status_genErr;
	}
	return int_SNMP_error__status_noError;
}

int	o_string (OI oi, struct type_SNMP_VarBind *v, char *base, const int len) {
	int	    result;
	struct qbuf *value;
	OT	    ot = oi -> oi_type;
	OS	    os = ot -> ot_syntax;

	if (os == NULLOS) {
		ADVISE (LLOG_EXCEPTIONS, NULLCP,
				"no syntax defined for object \"%s\"", ot -> ot_text);
		return int_SNMP_error__status_genErr;
	}
	if ((value = str2qb (base, len, 1)) == NULL) {
		ADVISE (LLOG_EXCEPTIONS, NULLCP, "out of memory");
		return int_SNMP_error__status_genErr;
	}
	if (v -> value)
		free_SNMP_ObjectSyntax (v -> value), v -> value = NULL;
	result = (*os -> os_encode) (value, &v -> value);
	qb_free (value);
	if (result == NOTOK) {
		ADVISE (LLOG_EXCEPTIONS, NULLCP,
				"encoding error for variable \"%s\"",
				oid2ode (oi -> oi_name));
		return int_SNMP_error__status_genErr;
	}
	return int_SNMP_error__status_noError;
}

int	o_string_s (OI oi, struct type_SNMP_VarBind *v, char *base) {
	int	len;

	if (strlen2int (base, &len) != 0) {
		ADVISE (LLOG_EXCEPTIONS, NULLCP,
				"string too long for object \"%s\"",
				oi && oi -> oi_type ? oi -> oi_type -> ot_text : "?");
		return int_SNMP_error__status_genErr;
	}
	return o_string (oi, v, base, len);
}

int	o_qbstring (OI oi, struct type_SNMP_VarBind *v, struct qbuf *value) {
	int	    result;
	OT	    ot = oi -> oi_type;
	OS	    os = ot -> ot_syntax;

	if (os == NULLOS) {
		ADVISE (LLOG_EXCEPTIONS, NULLCP,
				"no syntax defined for object \"%s\"", ot -> ot_text);
		return int_SNMP_error__status_genErr;
	}
	if (v -> value)
		free_SNMP_ObjectSyntax (v -> value), v -> value = NULL;
	result = (*os -> os_encode) (value, &v -> value);
	if (result == NOTOK) {
		ADVISE (LLOG_EXCEPTIONS, NULLCP,
				"encoding error for variable \"%s\"",
				oid2ode (oi -> oi_name));
		return int_SNMP_error__status_genErr;
	}
	return int_SNMP_error__status_noError;
}

int	o_specific (OI oi, struct type_SNMP_VarBind *v, caddr_t value) {
	int	    result;
	OT	    ot = oi -> oi_type;
	OS	    os = ot -> ot_syntax;

	if (os == NULLOS) {
		ADVISE (LLOG_EXCEPTIONS, NULLCP,
				"no syntax defined for object \"%s\"", ot -> ot_text);
		return int_SNMP_error__status_genErr;
	}
	if (v -> value)
		free_SNMP_ObjectSyntax (v -> value), v -> value = NULL;
	result = (*os -> os_encode) (value, &v -> value);
	if (result == NOTOK) {
		ADVISE (LLOG_EXCEPTIONS, NULLCP,
				"encoding error for variable \"%s\"",
				oid2ode (oi -> oi_name));
		return int_SNMP_error__status_genErr;
	}
	return int_SNMP_error__status_noError;
}

int	mediaddr2oid (unsigned int *ip, const uint8_t *addr, const int len, const int islen) {
	int   i;

	if (islen)
		*ip++ = len & 0xff;
	for (i = len; i > 0; i--)
		*ip++ = *addr++ & 0xff;
	return (len + (islen ? 1 : 0));
}

OID	oid_extend (OID q, const int howmuch) {
	int	nelem,
			nalloc;
	unsigned int   *ip,
			 *jp;
	OID	    oid;

	if (q == NULLOID)
		return NULLOID;
	nelem = q -> oid_nelem;
	if (nelem < 1)
		return NULLOID;
	if (howmuch >= 0) {
		if (nelem > INT_MAX - howmuch)
			return NULLOID;
		nalloc = nelem + howmuch;
	} else {
		if (howmuch == INT_MIN || nelem < -howmuch)
			return NULLOID;
		nalloc = nelem + howmuch;
	}
	{
		int extra = nalloc;

		if (add_int_to_int (&extra, 1) != 0)
			return NULLOID;
		if ((oid = (OID) malloc (sizeof *oid)) == NULLOID)
			return NULLOID;
		if ((ip = (unsigned int *) calloc_int (extra, sizeof *ip))
				== NULL) {
			free ((char *) oid);
			return NULLOID;
		}
	}
	oid -> oid_elements = ip;
	oid -> oid_nelem = nalloc;
	{
		int k;

		for (k = 0, jp = q -> oid_elements; k < nelem; k++, jp++)
			*ip++ = *jp;
	}
	return oid;
}

OID	oid_normalize (OID q, const int howmuch, const int bigvalue) {
	int	i;
	unsigned int   *ip,
			 *jp;
	OID	    oid;

	if ((oid = oid_extend (q, howmuch)) == NULL)
		return NULLOID;
	for (jp = (ip = oid -> oid_elements + q -> oid_nelem) - 1;
			jp >= oid -> oid_elements;
			jp--)
		if (*jp > 0) {
			*jp -= 1;
			break;
		}
	for (i = howmuch; i > 0; i--)
		*ip++ = (unsigned int) bigvalue;
	return oid;
}
