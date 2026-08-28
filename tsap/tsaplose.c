/* tsaplose.c - TPM: you lose */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include "tpkt.h"
#include "mpkt.h"
#include "tailor.h"
static int _tsaplose (
	struct TSAPdisconnect *td,
	int reason,
	va_list ap
);


#ifdef	LPP
#undef	MGMT
#endif

static int _tsaplose (
	struct TSAPdisconnect *td,
	int reason,
	va_list ap
);

int	tpktlose (struct tsapblk* tb, ...) {
	int	    reason,
			result;
	char	*what, *fmt, *bp;
	struct TSAPdisconnect   tds;
	struct TSAPdisconnect  *td;
	va_list ap;
	char    buffer[BUFSIZ];

	va_start (ap, tb);
	// tb = va_arg (ap, struct tsapblk *);
	td = va_arg (ap, struct TSAPdisconnect *);
	if (td == NULL)
		td = &tds;
	reason = va_arg (ap, int);
	result = _tsaplose (td, reason, ap);
	what = va_arg(ap, char *);
	fmt = va_arg(ap, char *);
	if (fmt)
		_asprintf (bp = buffer, what, fmt, ap);
	va_end (ap);
	if (td -> td_cc > 0) {
		SLOG (tsap_log, LLOG_EXCEPTIONS, NULLCP,
			  ("tpktlose [%s] %*.*s", TErrString (td -> td_reason), td -> td_cc,
			   td -> td_cc, td -> td_data));
	} else
		SLOG (tsap_log, LLOG_EXCEPTIONS, NULLCP,
			  ("tpktlose [%s]", TErrString (td -> td_reason)));

#ifdef  MGMT
	if (tb -> tb_manfnx)
		switch (reason) {
		case DR_REMOTE:
		case DR_CONGEST:
			(*tb -> tb_manfnx) (CONGEST, tb);
			break;
		case DR_PROTOCOL:
		case DR_MISMATCH:
			(*tb -> tb_manfnx) (PROTERR, tb);
			break;
		case DR_SESSION:
		case DR_ADDRESS:
		case DR_CONNECT:
		case DR_DUPLICATE:
		case DR_OVERFLOW:
		case DR_REFUSED:
			(*tb -> tb_manfnx) (CONFIGBAD, tb);
			break;
		default:
			(*tb -> tb_manfnx) (OPREQINBAD, tb);
		}
#endif
	(*tb -> tb_losePfnx) (tb, reason, td);
	return result;
}

int	tsaplose (struct TSAPdisconnect*td, ...) {
	int	    reason,
			result;
	va_list ap;
	va_start (ap, td);
	reason = va_arg (ap, int);
	result = _tsaplose (td, reason, ap);
	va_end (ap);
	return result;
}

static int _tsaplose (
	struct TSAPdisconnect *td,
	int reason,
	va_list ap
) {
	char  *bp;
	char  *what;
	const char  *fmt;
	char    buffer[BUFSIZ];

	what = va_arg(ap, char*);
	fmt = va_arg(ap, char*);
	if (td) {
		bzero ((char *) td, sizeof *td);
		_asprintf (bp = buffer, what, fmt, ap);
		bp += strlen (bp);
		td -> td_reason = reason;
		copyTSAPdata (buffer, bp - buffer, td);
	}
	return NOTOK;
}
