/* objectbyname.c - getisobjectbyoid */







#include <stdio.h>
#include "psap.h"
#include "tailor.h"

struct isobject *getisobjectbyoid (oid)
OID	oid;
{
	struct isobject   *io;

	isodetailor (NULLCP, 0);
#ifdef	DEBUG
	SLOG (addr_log, LLOG_TRACE, NULLCP,
		  ("getisobjectbyoid %s", sprintoid (oid)));
#endif

	setisobject (0);
	while (io = getisobject ())
		if (oid_cmp (oid, &io -> io_identity) == 0)
			break;
	endisobject ();

	if (io) {
#ifdef	DEBUG
		SLOG (addr_log, LLOG_DEBUG, NULLCP,
			  ("\tODE: \"%s\"\tOID: %s",
			   io -> io_descriptor, sprintoid (&io -> io_identity)));
#endif
	} else
		SLOG (addr_log, LLOG_EXCEPTIONS, NULLCP,
			  ("lookup of object %s failed", sprintoid (oid)));

	return io;
}
