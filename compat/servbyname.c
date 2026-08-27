/* servbyname.c - getisoserventbyname */

#include <string.h>
#include "general.h"
#include "manifest.h"
#include "isoservent.h"
#include "tailor.h"

struct isoservent *
getisoserventbyname (const char *entity, const char *provider) {
	struct isoservent *is;

	isodetailor (NULLCP, 0);
	DLOG (addr_log, LLOG_TRACE,
		  ("getisoserventbyname \"%s\" \"%s\"", entity, provider));

	setisoservent (0);
	while (is = getisoservent ())
		if (strcmp (entity, is -> is_entity) == 0
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
			  ("lookup of local service %s/%s failed", provider, entity));

	return is;
}
