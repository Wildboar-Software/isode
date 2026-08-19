/* oid2ode.c - object identifier to object descriptor  */

#include <stdio.h>
#include "psap.h"
#include "tailor.h"

char   *oid2ode_aux (OID identifier, int quoted) {
	int	    events;
	struct isobject *io;
	static char buffer[BUFSIZ];

	events = addr_log -> ll_events;
	addr_log -> ll_events = LLOG_FATAL;

	io = getisobjectbyoid (identifier);

	addr_log -> ll_events = events;

	if (io) {
		sprintf (buffer, quoted ? "\"%s\"" : "%s",
				 io -> io_descriptor);
		return buffer;
	}

	return sprintoid (identifier);
}
