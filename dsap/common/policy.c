/* policy.c - SecurityPolicy abstract syntax  */

#include "cmd_srch.h"
#include "quipu/policy.h"

/*
 * This syntax is not yet implemented. However, the following routines for
 * handling security policies are used by some applications.
 */

static CMD_TABLE permtab[] = {
	"detect", POLICY_ACCESS_DETECT,
	"read", POLICY_ACCESS_READ,
	"add", POLICY_ACCESS_ADD,
	"write", POLICY_ACCESS_WRITE,
	"all", POLICY_ACCESS_ALL,
	"none", 0,
	NULLCP, 0
};

unsigned
str2permission (char *str) {
	char *cp;
	const unsigned result = 0;

	while (str && (*str != '\0')) {
		unsigned u;
		int v;

		cp = index(str, '$');
		if (cp != NULLCP)
			*cp++ = '\0';
		v = cmd_srch(str, permtab);
		if (int2uint (v, &u) != 0)
			return 0;
		result |= u;
		str = cp;
	}
	return (result);
}
