/* ryfind.c - ROSY: find operations and errors by numbers and names */

#include <string.h>
#include "rosy.h"

struct RyOperation *findopbyop (struct RyOperation *ryo, int op)
{
	if (!ryo)
		return NULL;
	for (; ryo -> ryo_name; ryo++)
		if (ryo -> ryo_op == op)
			return ryo;
	return NULL;
}

struct RyOperation *findopbyname (struct RyOperation *ryo, char *name)
{
	if (!ryo)
		return NULL;
	for (; ryo -> ryo_name; ryo++)
		if (strcmp (ryo -> ryo_name, name) == 0)
			return ryo;
	return NULL;
}

struct RyError *finderrbyerr (struct RyError *rye, int err)
{
	if (!rye)
		return NULL;
	for (; rye -> rye_name; rye++)
		if (rye -> rye_err == err)
			return rye;
	return NULL;
}

struct RyError *finderrbyname (struct RyError *rye, char *name)
{
	if (!rye)
		return NULL;
	for (; rye -> rye_name; rye++)
		if (strcmp (rye -> rye_name, name) == 0)
			return rye;
	return NULL;
}
