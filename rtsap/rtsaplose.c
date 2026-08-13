/* rtsaplose.c - RTPM: you lose */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "rtpkt.h"
#include "tailor.h"

#ifndef	lint
static int  _rtsaplose (struct RtSAPindication *rti, int reason, va_list ap);
#endif

#ifndef	lint
int	rtpktlose (struct assocblk*acb, ...) {
	int	    reason,
			result,
			value;
	struct RtSAPindication *rti;
	struct RtSAPabort *rta;
	va_list ap;

	va_start (ap, acb);
	rti = va_arg (ap, struct RtSAPindication *);
	reason = va_arg (ap, int);
	result = _rtsaplose (rti, reason, ap);
	va_end (ap);
	if ((rta = &rti -> rti_abort) -> rta_cc > 0) {
		SLOG (rtsap_log, LLOG_EXCEPTIONS, NULLCP,
			  ("rtpktlose [%s] %*.*s", RtErrString (rta -> rta_reason),
			   rta -> rta_cc, rta -> rta_cc, rta -> rta_data));
	} else
		SLOG (rtsap_log, LLOG_EXCEPTIONS, NULLCP,
			  ("rtpktlose [%s]", RtErrString (rta -> rta_reason)));
	if (acb == NULLACB
			|| acb -> acb_fd == NOTOK
			|| acb -> acb_rtpktlose == NULLIFP)
		return result;
	switch (reason) {
	case RTS_PROTOCOL:
		value = ABORT_PROTO;
		break;
	case RTS_CONGEST:
		value = ABORT_TMP;
		break;
	default:
		value = ABORT_LSP;
		break;
	}
	(*acb -> acb_rtpktlose) (acb, value);
	return result;
}
#else
int rtpktlose (struct assocblk *acb, struct RtSAPindication *rti, int reason, char *what, char *fmt) {
	return rtpktlose (acb, rti, reason, what, fmt);
}
#endif

#ifndef	lint
int	rtsaplose (struct RtSAPindication*rti, ...) {
	int	    reason,
			result;
	va_list ap;
	va_start (ap, rti);
	reason = va_arg (ap, int);
	result = _rtsaplose (rti, reason, ap);
	va_end (ap);
	return result;
}
#else
int rtsaplose (struct RtSAPindication *rti, int reason, char *what, char *fmt) {
	return rtsaplose (rti, reason, what, fmt);
}
#endif

#ifndef	lint
static int _rtsaplose (  /* what, fmt, args ... */
	struct RtSAPindication *rti,
	int reason,
	va_list ap
) {
	char  *bp;
	char    buffer[BUFSIZ];
	struct RtSAPabort *rta;
    char   *what, *fmt;
	if (rti) {
		bzero ((char *) rti, sizeof *rti);
		rti -> rti_type = RTI_ABORT;
		rta = &rti -> rti_abort;
		bp = buffer;
		what = va_arg (ap, char *);
		fmt  = va_arg (ap, char *);
		_asprintf (bp, what, fmt, ap);
		bp += strlen (bp);
		rta -> rta_peer = 0;
		rta -> rta_reason = reason;
		copyRtSAPdata (buffer, bp - buffer, rta);
	}

	return NOTOK;
}
#endif
