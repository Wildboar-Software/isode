/* ftambulk2.c - FPM: respond to bulk data transfer */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/ftam/RCS/ftambulk2.c,v 9.0 1992/06/16 12:14:55 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/ftam/RCS/ftambulk2.c,v 9.0 1992/06/16 12:14:55 isode Rel $
 *
 *
 * $Log: ftambulk2.c,v $
 * Revision 9.0  1992/06/16  12:14:55  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


/* LINTLIBRARY */

#include <stdio.h>
#include <signal.h>
#include "fpkt.h"

static int FTransEndRequestAux (struct ftamblk *fsb, PE sharedASE, struct FTAMindication *fti);
static int FTransEndResponseAux (struct ftamblk *fsb, int action, PE sharedASE, struct FTAMdiagnostic diag[], int ndiag, struct FTAMindication *fti);

/*    F-TRANSFER-END.RESPONSE */
static int  FTransEndResponseAux ();

int
FTransEndResponse (int sd, int action, PE sharedASE, struct FTAMdiagnostic diag[], int ndiag, struct FTAMindication *fti) {
	SBV	    smask;
	int     result;
	struct ftamblk *fsb;

	switch (action) {
	case FACTION_SUCCESS:
	case FACTION_TRANS:
	case FACTION_PERM:
		break;

	default:
		return ftamlose (fti, FS_GEN_NOREASON, 0, NULLCP,
						 "bad value for action parameter");
	}
	toomuchP (diag, ndiag, NFDIAG, "diagnostic");
	missingP (fti);

	smask = sigioblock ();

	ftamPsig (fsb, sd);

	result = FTransEndResponseAux (fsb, action, sharedASE, diag, ndiag, fti);

	sigiomask (smask);

	return result;
}

/*  */

static int FTransEndResponseAux (struct ftamblk *fsb, int action, PE sharedASE, struct FTAMdiagnostic diag[], int ndiag, struct FTAMindication *fti) {
	int	    result;
	PE	    pe;
	struct PSAPindication   pis;
	struct PSAPindication  *pi = &pis;
	struct PSAPabort   *pa = &pi -> pi_abort;
	struct type_FTAM_PDU *pdu;
	struct type_FTAM_F__TRANSFER__END__response *rsp;

	if (fsb -> fsb_flags & FSB_INIT)
		return ftamlose (fti, FS_GEN (fsb), 0, NULLCP, "not responder");
	if (fsb -> fsb_state != FSB_DATAFIN2)
		return ftamlose (fti, FS_GEN (fsb), 0, NULLCP, "wrong state");

	pe = NULLPE;
	if ((pdu = (struct type_FTAM_PDU *) calloc (1, sizeof *pdu)) == NULL) {
no_mem:
		;
		ftamlose (fti, FS_GEN (fsb), 1, NULLCP, "out of memory");
out:
		;
		if (pe)
			pe_free (pe);
		if (pdu)
			free_FTAM_PDU (pdu);
		if (fti -> fti_abort.fta_action == FACTION_PERM)
			freefsblk (fsb);
		return NOTOK;
	}
	pdu -> offset = type_FTAM_PDU_f__transfer__end__response;
	if ((rsp = (struct type_FTAM_F__TRANSFER__END__response *)
			   calloc (1, sizeof *rsp)) == NULL)
		goto no_mem;
	pdu -> un.f__transfer__end__response = rsp;
	if ((rsp -> action__result =
				(struct type_FTAM_Action__Result *)
				calloc (1, sizeof *rsp -> action__result)) == NULL)
		goto no_mem;
	rsp -> action__result -> parm = action;
	if (sharedASE
			&& (rsp -> shared__ASE__information =
					shared2fpm (fsb, sharedASE, fti)) == NULL)
		goto out;
	if (ndiag > 0
			&& (rsp -> diagnostic = diag2fpm (fsb, 0, diag, ndiag, fti))
			== NULL)
		goto out;

	if (encode_FTAM_PDU (&pe, 1, 0, NULLCP, pdu) == NOTOK) {
		ftamlose (fti, FS_GEN (fsb), 1, NULLCP,
				  "error encoding PDU: %s", PY_pepy);
		goto out;
	}

	pe -> pe_context = fsb -> fsb_id;

	fsbtrace (fsb, (fsb -> fsb_fd, "P-DATA.REQUEST", "F-TRANSFER-END-response",
					pe, 0));

	result = PDataRequest (fsb -> fsb_fd, &pe, 1, pi);

	pe_free (pe);
	pe = NULLPE;
	free_FTAM_PDU (pdu);
	pdu = NULL;

	if (result == NOTOK) {
		ps2ftamlose (fsb, fti, "PDataRequest", pa);
		goto out;
	}

	fsb -> fsb_state = FSB_DATAIDLE;

	return OK;
}
