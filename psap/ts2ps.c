/* ts2ps.c - TSDU-backed abstraction for PStreams
 		(really just a refinement of datagram-backed PStreams) */

#include <stdio.h>
#include "psap.h"
#include "tsap.h"
#include "tailor.h"

int ts_read (int fd, struct qbuf **q) {
	struct qbuf *qb;
	struct TSAPdata  txs;
	struct TSAPdata *tx = &txs;
	struct TSAPdisconnect  tds;
	struct TSAPdisconnect *td = &tds;

	if (TReadRequest (fd, tx, NOTOK, td) == NOTOK) {
		if (td -> td_reason == DR_NORMAL) {
			*q = NULL;
			return OK;
		}

		SLOG (psap_log, LLOG_EXCEPTIONS, NULLCP,
			  (td -> td_cc > 0 ? "ts_read: [%s] %*.*s" : "ts_read: [%s]",
			   TErrString (td -> td_reason), td -> td_cc, td -> td_cc,
			   td -> td_data));

		return NOTOK;
	}

	qb = &tx -> tx_qbuf;
	if (qb -> qb_forw -> qb_forw != qb && qb_pullup (qb) == NOTOK) {
		SLOG (psap_log, LLOG_EXCEPTIONS, NULLCP,
			  ("ts_read: qb_pullup fails"));
		TXFREE (tx);

		return NOTOK;
	}

	remque (qb = tx -> tx_qbuf.qb_forw);
	qb -> qb_forw = qb -> qb_back = qb;

	*q = qb;

	TXFREE (tx);

	return qb -> qb_len;
}

int ts_write (int fd, struct qbuf *qb) {
	struct TSAPdisconnect  tds;
	struct TSAPdisconnect *td = &tds;

	if (TDataRequest (fd, qb -> qb_data, qb -> qb_len, td) == NOTOK) {
		SLOG (psap_log, LLOG_EXCEPTIONS, NULLCP,
			  (td -> td_cc > 0 ? "ts_write: [%s] %*.*s" : "ts_write: [%s]",
			   TErrString (td -> td_reason), td -> td_cc, td -> td_cc,
			   td -> td_data));

		return NOTOK;
	}

	return qb -> qb_len;
}
