/* servbysel.c - getisoserventbyselector */







#include <string.h>
#include "general.h"
#include "manifest.h"
#include "isoservent.h"
#include "tailor.h"

struct isoservent *
getisoserventbyselector (char *provider, char *selector, int selectlen) {
	struct isoservent *is;

	isodetailor (NULLCP, 0);
	DLOG (addr_log, LLOG_TRACE,
		  ("getisoserventbyselector \"%s\" %s",
		   provider, sel2str (selector, selectlen, 1)));

	setisoservent (0);
	while (is = getisoservent ())
		if (selectlen == is -> is_selectlen
				&& bcmp (selector, is -> is_selector, is -> is_selectlen) == 0
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
			  ("lookup of local service %s %s failed",
			   provider, sel2str (selector, selectlen, 1)));

	return is;
}
