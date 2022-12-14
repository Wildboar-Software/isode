/* ftamrelease1.c - FPM: initiate release */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/ftam/RCS/ftamrelease1.c,v 9.0 1992/06/16 12:14:55 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/ftam/RCS/ftamrelease1.c,v 9.0 1992/06/16 12:14:55 isode Rel $
 *
 *
 * $Log: ftamrelease1.c,v $
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

static int FTerminateRequestAux (struct ftamblk *fsb, PE sharedASE, struct FTAMrelease *ftr, struct FTAMindication *fti);

/*    F-TERMINATE.REQUEST */
static int  FTerminateRequestAux ();

int
FTerminateRequest (int sd, PE sharedASE, struct FTAMrelease *ftr, struct FTAMindication *fti) {
	SBV	    smask;
	int     result;
	struct ftamblk *fsb;

	missingP (ftr);
	missingP (fti);

	smask = sigioblock ();

	ftamPsig (fsb, sd);

	result = FTerminateRequestAux (fsb, sharedASE, ftr, fti);

	sigiomask (smask);

	return result;
}

/*  */

static int FTerminateRequestAux (struct ftamblk *fsb, PE sharedASE, struct FTAMrelease *ftr, struct FTAMindication *fti) {
	int     result;
	PE	    pe;
	struct AcSAPrelease acrs;
	struct AcSAPrelease   *acr = &acrs;
	struct AcSAPindication  acis;
	struct AcSAPindication *aci = &acis;
	struct AcSAPabort *aca = &aci -> aci_abort;
	struct type_FTAM_PDU *pdu;
	struct type_FTAM_F__TERMINATE__response *rsp;

	if (!(fsb -> fsb_flags & FSB_INIT))
		return ftamlose (fti, FS_GEN (fsb), 0, NULLCP, "not initiator");
	if (fsb -> fsb_state != FSB_INITIALIZED)
		return ftamlose (fti, FS_GEN (fsb), 0, NULLCP,
						 "not in the initialized state");

	bzero ((char *) acr, sizeof *acr);
	bzero ((char *) ftr, sizeof *ftr);

	pe = NULLPE;
	if ((pdu = (struct type_FTAM_PDU *) calloc (1, sizeof *pdu)) == NULL) {
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
	pdu -> offset = type_FTAM_PDU_f__terminate__request;
	if (sharedASE
			&& (pdu -> un.f__terminate__request =
					shared2fpm (fsb, sharedASE, fti)) == NULL)
		goto out;

	if (encode_FTAM_PDU (&pe, 1, 0, NULLCP, pdu) == NOTOK) {
		ftamlose (fti, FS_GEN (fsb), 1, NULLCP,
				  "error encoding PDU: %s", PY_pepy);
		goto out;
	}

	pe -> pe_context = fsb -> fsb_id;

	fsbtrace (fsb, (fsb -> fsb_fd, "A-RELEASE.REQUEST", "F-TERMINATE-request",
					pe, 0));

	result = AcRelRequest (fsb -> fsb_fd, ACF_NORMAL, &pe, 1, NOTOK, acr, aci);

	pe_free (pe);
	pe = NULLPE;
	free_FTAM_PDU (pdu);
	pdu = NULL;

	if (result == NOTOK) {
		if (aca -> aca_source == ACA_USER)
			return acs2ftamabort (fsb, aca, fti);

		acs2ftamlose (fsb, fti, "AcRelRequest", aca);
		goto out;
	}

	if (!acr -> acr_affirmative) {
		result = fpktlose (fsb, fti, FS_ACS_MGMT, NULLCP,
						   "other side refused to release association");
		goto done;
	}
	fsb -> fsb_fd = NOTOK;

	if (acr -> acr_ninfo < 1 || (pe = acr -> acr_info[0]) == NULLPE) {
		result = fpktlose (fsb, fti, FS_PRO_ERR, NULLCP, NULLCP);
		goto done;
	}

	if (decode_FTAM_PDU (pe, 1, NULLIP, NULLVP, &pdu) == NOTOK)
		result = fpktlose (fsb, fti, FS_PRO_ERRMSG, NULLCP,
						   "unable to parse PDU: %s", PY_pepy);
	else {
		if (pdu -> offset != type_FTAM_PDU_f__terminate__response) {
			result = fpktlose (fsb, fti, FS_PRO_ERRPROC, NULLCP,
							   "expecting F-TERMINATE-response, got %d",
							   pdu -> offset);
			goto done;
		}
		rsp = pdu -> un.f__terminate__response;

		fsbtrace (fsb,
				  (fsb -> fsb_fd, "A-RELEASE.CONFIRMATION",
				   "F-TERMINATE-response", pe, 1));

		if (rsp -> shared__ASE__information
				&& fpm2shared (fsb, rsp -> shared__ASE__information,
							   &ftr -> ftr_sharedASE, fti) == NOTOK)
			goto done;
		if (rsp -> charging
				&& fpm2chrg (fsb, rsp -> charging, &ftr -> ftr_charges, fti)
				== NOTOK)
			goto done;

		result = OK;
	}

done:
	;
	if (pdu)
		free_FTAM_PDU (pdu);
	ACRFREE (acr);
	freefsblk (fsb);

	return result;
}
