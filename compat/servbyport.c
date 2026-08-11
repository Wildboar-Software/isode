/* servbyport.c - getisoserventbyport */

#include <string.h>
#include "general.h"
#include "manifest.h"
#include "isoservent.h"
#include "tailor.h"
#include "internet.h"

struct isoservent *
getisoserventbyport (char *provider, unsigned short port) {
	struct isoservent *is;

	isodetailor (NULLCP, 0);
	DLOG (addr_log, LLOG_TRACE,
		  ("getisoserventbyport \"%s\" %d", provider, (int) ntohs (port)));

	setisoservent (0);
	while (is = getisoservent ())
		if (sizeof (port) == is -> is_selectlen
				&& port == is -> is_port
				&& strcmp (provider, is -> is_provider) == 0)
			break;
	endisoservent ();

	if (is) {
#ifdef	DEBUG
		if (addr_log -> ll_events & LLOG_DEBUG)
			_printsrv (is);
#endif
	} else
		SLOG (addr_log, LLOG_EXCEPTIONS, NULLCP,
			  ("lookup of local service %s/%d failed",
			   provider, (int) ntohs (port)));

	return is;
}
