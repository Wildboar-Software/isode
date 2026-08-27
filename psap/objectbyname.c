/* objectbyname.c - getisobjectbyname */

#include <string.h>
#include "psap.h"
#include "tailor.h"

struct isobject *
getisobjectbyname (const char *descriptor) {
	struct isobject   *io;

	isodetailor (NULLCP, 0);
#ifdef	DEBUG
	SLOG (addr_log, LLOG_TRACE, NULLCP,
		  ("getisobjectbyname \"%s\"", descriptor));
#endif

	setisobject (0);
	while (io = getisobject ())
		if (strcmp (descriptor, io -> io_descriptor) == 0)
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
			  ("lookup of object \"%s\" failed", descriptor));

	return io;
}
