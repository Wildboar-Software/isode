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
	unsigned result = 0;

	while (str && (*str != '\0')) {
		cp = index(str, '$');
		if (cp != NULLCP)
			*cp++ = '\0';
		result |= cmd_srch(str, permtab);
		str = cp;
	}
	return (result);
}
