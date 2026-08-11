#include "quipu/util.h"
#include "quipu/name.h"

extern LLog * log_dsap;

void dn_append (DN a, DN b)
{
	DN ptr;
	DN eptr;

	if (a == NULLDN)
		DLOG (log_dsap,LLOG_DEBUG,("appending to null dn!"));
	else {
		for (eptr = a; eptr != NULLDN; eptr = eptr->dn_parent)
			ptr = eptr;
		ptr->dn_parent = b;
	}
}

