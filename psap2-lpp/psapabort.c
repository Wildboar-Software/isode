/* psapabort.c - PPM: user abort */
/* Contributed by The Wollongong Group, Inc. */
#include <stdlib.h>
#include <signal.h>
#define	LPP
#include "PS-types.h"
#include "ppkt.h"
#include "tailor.h"
#include "pvpdu.h"

/* P-U-ABORT.REQUEST */

int	PUAbortRequest (int sd, PE *data, int ndata, struct PSAPindication *pi) {
	int	    smask;
	int	    result;
	struct psapblk *pb;
	PE	    pe;
	PS	    ps;
	struct type_PS_Abort__PDU *pdu;

	toomuchP (data, ndata, NPDATA_PS, "abort");
	if (ndata > 0) {
		if ((pe = data[0]) -> pe_context != PCI_ACSE)
			return psaplose (pi, PC_PARAMETER, NULLCP,
							 "wrong context for abort user data");
	} else
		pe = NULLPE;
	missingP (pi);
	smask = sigioblock ();
	if ((pb = findpblk (sd)) == NULL) {
		sigiomask (smask);
		return psaplose (pi, PC_PARAMETER, NULLCP,
						 "invalid presentation descriptor");
	}
	if ((pdu = (struct type_PS_Abort__PDU *) malloc (sizeof *pdu)) == NULL) {
		psaplose (pi, PC_CONGEST, NULLCP, "out of memory");
		goto out;
	}
	pdu -> reference = pb -> pb_reliability == LOW_QUALITY
		? pb -> pb_reference
		: NULLRF;
	pdu -> user__data = pe;
	pdu -> reason = NULL;
	pe = NULLPE;
	result = encode_PS_Abort__PDU (&pe, 1, 0, NULLCP, pdu);
	pdu -> reference = NULL;
	pdu -> user__data = NULLPE;
	free_PS_Abort__PDU (pdu);
	if (result != NOTOK) {
		PLOGP (psap2_log,PS_PDUs, pe, "Abort-PDU", 0);
		if ((result = pe2ps (ps = pb -> pb_stream, pe)) == NOTOK)
			pslose (pi, ps -> ps_errno);
		else
			result = OK;
	} else
		psaplose (pi, PC_CONGEST, NULLCP, "error encoding PDU: %s",
				  PY_pepy);
	if (pe)
		pe_free (pe);
out:
	;
	freepblk (pb);
	sigiomask (smask);
	return result;
}
