/* isohost.c - getlocalhost */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "general.h"
#include "manifest.h"
#ifdef	TCP
#include "internet.h"
#endif
#include "tailor.h"
#ifdef	SYS5
#include <sys/utsname.h>
#endif

char *getlocalhost (void) {
	char   *cp;
#ifdef	TCP
	struct hostent *hp;
#endif
#ifdef	SYS5
	struct utsname uts;
#endif
	static char buffer[BUFSIZ];

	if (buffer[0])
		return buffer;
	isodetailor (NULLCP, 0);
	if (*isodename)
		strcpy (buffer, isodename);
	else {
#if	!defined(SOCKETS) && !defined(SYS5)
		strcpy (buffer, "localhost");
#endif
#ifdef	SOCKETS
		gethostname (buffer, sizeof buffer);
#endif
#ifdef	SYS5
		uname (&uts);
		strcpy (buffer, uts.nodename);
#endif
#ifdef	TCP
		if (hp = gethostbyname (buffer))
			strcpy (buffer, hp -> h_name);
		else
			SLOG (addr_log, LLOG_EXCEPTIONS, NULLCP,
				  ("%s: unknown host", buffer));
#endif
		if (cp = index (buffer, '.'))
			*cp = 0;
	}
	return buffer;
}
