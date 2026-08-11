/* policy.h - representation of security policy */

#ifndef QUIPUPOLICY
#define QUIPUPOLICY
#include "psap.h"

struct security_policy {
	OID oid;
	int p_type;
#define POLICY_PARM_ABSENT      0
#define POLICY_PARM_UNKNOWN     1
#define POLICY_PARM_NUMERIC     2
#define POLICY_PARM_ACCESS      3
	union {
		int numeric;
		unsigned access;
	} un;
};

#define NULLPOLICY ((struct security_policy *) 0)

#define POLICY_ACCESS_DETECT	1
#define POLICY_ACCESS_READ 	2
#define POLICY_ACCESS_ADD	4
#define POLICY_ACCESS_WRITE	8
#define POLICY_ACCESS_ALL	15

#endif
