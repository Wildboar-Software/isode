/* tsapstate.c - TPM: hack state */

#include <signal.h>
#include <string.h>
#include <stdint.h>
#include "tpkt.h"
#include "manifest.h"

int TSaveState (int sd, char **vec, struct TSAPdisconnect *td) {
	SBV     smask;
	struct tsapblk *tb;
	static char buffer[sizeof *tb * 2 + 1];

	missingP (vec);

	smask = sigioblock ();

	tsapPsig (tb, sd);

	if (tb -> tb_len > 0) {
		sigiomask (smask);

		return tsaplose (td, DR_WAITING, NULLCP, NULLCP);
	}

	buffer[explode (buffer, (uint8_t *) tb, sizeof *tb)] = 0;
	*vec++ = buffer;
	*vec = NULL;

	tb -> tb_fd = NOTOK;

	freetblk (tb);

	sigiomask (smask);

	return OK;
}

int TRestoreState (char *buffer, struct TSAPstart *ts, struct TSAPdisconnect *td) {
	struct tsapblk  tbs;
	struct tsapblk *tb;

	missingP (buffer);
	missingP (ts);

	if ((tb = newtblk ()) == NULL)
		return tsaplose (td, DR_CONGEST, NULLCP, "out of memory");

	if (implode ((uint8_t *) &tbs, buffer, strlen (buffer)) != sizeof tbs) {
		tsaplose (td, DR_PARAMETER, NULLCP, "bad state vector");
		goto out1;
	}

	if (findtblk (tbs.tb_fd)) {
		tsaplose (td, DR_PARAMETER, NULLCP, "transport descriptor active");
		goto out1;
	}
	tb -> tb_fd = tbs.tb_fd;
	tb -> tb_flags = tbs.tb_flags & (TB_CONN | TB_EXPD | TB_TP0 | TB_TP4);
	tb -> tb_srcref = tbs.tb_srcref;
	tb -> tb_dstref = tbs.tb_dstref;
	tb -> tb_initiating = tbs.tb_initiating;	/* struct copy */
	tb -> tb_responding = tbs.tb_responding;	/* struct copy */

	switch (tb -> tb_flags & (TB_TP0 | TB_TP4)) {
#ifdef	TCP
	case TB_TCP:
		TTService (tb);
		break;
#endif

#ifdef	X25
	case TB_X25:
		XTService (tb);
		break;
#endif

#ifdef	TP4
	case TB_TP4:
		tp4init (tb);
		break;
#endif

	default:
		tsaplose (td, DR_PARAMETER, NULLCP, "network type not set");
		tb -> tb_fd = NOTOK;
		goto out1;
	}

	tb -> tb_tsdusize = tbs.tb_tsdusize;

	bzero ((char *) ts, sizeof *ts);
	ts -> ts_sd = tb -> tb_fd;
	copyTSAPaddrX (&tb -> tb_initiating, &ts -> ts_calling);
	copyTSAPaddrX (&tb -> tb_responding, &ts -> ts_called);
	if (tb -> tb_flags & TB_EXPD)
		ts -> ts_expedited = 1;
	ts -> ts_tsdusize = tb -> tb_tsdusize;
	ts -> ts_qos = tb -> tb_qos;	/* struct copy */

	return OK;

out1:
	;
	freetblk (tb);

	return NOTOK;
}
