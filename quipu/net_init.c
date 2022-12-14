/* net_init.c - Init network section of DSA process */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/net_init.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/RCS/net_init.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: net_init.c,v $
 * Revision 9.0  1992/06/16  12:34:01  isode
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

#include <unistd.h>
#define getdtablesize() (sysconf (_SC_OPEN_MAX))
#include "rosap.h"
#include "tsap.h"
#include "quipu/util.h"
#include "quipu/connection.h"
#include "quipu/oid.h"
#include "tailor.h"

extern	LLog	* log_dsap;
extern	char	* mydsaname;

extern	OID	acse_pci;
extern	OID	x500_da_ac;
extern	OID	x500_ds_ac;
extern	OID	quipu_ds_ac;
extern	OID	internet_ds_ac;
extern	OID	x500_da_as;
extern	OID	x500_ds_as;
extern	OID	quipu_ds_as;
extern	OID	internet_ds_as;
extern	struct PSAPctxlist	* x500_da_pcdl;
extern	struct PSAPctxlist	* x500_ds_pcdl;
extern	struct PSAPctxlist	* quipu_ds_pcdl;
extern	struct PSAPctxlist	* internet_ds_pcdl;
extern  AttributeType at_listen;
extern  Entry local_find_entry_aux();

extern	DN			  mydsadn;
extern  struct PSAPaddr		* mydsaaddr;
extern  struct PSAPaddr		* dsaladdr;

extern	struct PSAPaddr		* psap_cpy();
int	max_conns;

static int
TMagic (int *vecp, char **vec, struct TSAPdisconnect *td) {
	int	    sd;
	struct TSAPstart tss;
	struct TSAPstart  *ts = &tss;

	if (TInit (*vecp, vec, ts, td) == NOTOK)
		return NOTOK;
	sd = ts -> ts_sd;

	if (TConnResponse (sd, &ts -> ts_called, ts -> ts_expedited, NULLCP, 0,
					   NULLQOS, td) == NOTOK)
		return NOTOK;

	if (TSaveState (sd, vec + 1, td) == NOTOK)
		return NOTOK;
	vec[*vecp = 2] = NULL;

	return OK;
}

int
net_init (void) {
	int	    ntries,
			ontty;
	struct TSAPdisconnect	  td_s;
	struct TSAPdisconnect	* td = &(td_s);
	Entry			  my_entry;
	Attr_Sequence 		  as, entry_find_type();
	struct NSAPaddr *na;
	int				  x, *ip;


	DLOG(log_dsap, LLOG_TRACE, ("Net Starting"));
	isodetailor("server", 0);

	/*
	*  ds_init should have been called already to cover the possibility of
	*  the entry for this dsa being held remotely.
	*/
	if((my_entry = local_find_entry_aux(mydsadn, TRUE)) == NULLENTRY) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("net_init - can't find own entry"));
		return(NOTOK);
	}

	/* mydsaaddr == externally visable address, used for calculations */
	/* dsaladdr == actual address used to listen on */

	mydsaaddr = psap_cpy(my_entry->e_dsainfo->dsa_addr);
	if ( ( as = entry_find_type (my_entry, at_listen)) != NULLATTR)
		dsaladdr = psap_cpy ( (struct PSAPaddr *) as->attr_value->avseq_av.av_struct);
	else
		dsaladdr = mydsaaddr;

	max_conns = getdtablesize() - 3 - 3 - 1;
	max_conns -= dsaladdr -> pa_addr.sa_addr.ta_naddr;
	/* allow 3 fd's for stdio, 2 for logging, 1 emergency, and
	one for each listen.
	*/

	if((acse_pci = oid_cpy(DIR_ACSE)) == NULLOID) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("acse pci version 1 OID not found"));
		return NOTOK;
	}

	x500_da_ac = oid_cpy (DIR_ACCESS_AC);
	x500_ds_ac = oid_cpy (DIR_SYSTEM_AC);
	quipu_ds_ac = oid_cpy (DIR_QUIPU_AC);
	internet_ds_ac = oid_cpy (DIR_INTERNET_AC);
	x500_da_as = oid_cpy (DIR_ACCESS_AS);
	x500_ds_as = oid_cpy (DIR_SYSTEM_AS);
	quipu_ds_as = oid_cpy (DIR_QUIPU_AS);
	internet_ds_as = oid_cpy (DIR_INTERNET_AS);

	x500_da_pcdl->pc_nctx = 2;
	x500_da_pcdl->pc_ctx[0].pc_id = DIR_ACCESS_PC_ID;
	x500_da_pcdl->pc_ctx[0].pc_asn = oid_cpy(x500_da_as);
	x500_da_pcdl->pc_ctx[0].pc_atn = NULLOID;
	x500_da_pcdl->pc_ctx[1].pc_id = DIR_ACSE_PC_ID;
	x500_da_pcdl->pc_ctx[1].pc_asn = oid_cpy(acse_pci);
	x500_da_pcdl->pc_ctx[1].pc_atn = NULLOID;

	x500_ds_pcdl->pc_nctx = 2;
	x500_ds_pcdl->pc_ctx[0].pc_id = DIR_SYSTEM_PC_ID;
	x500_ds_pcdl->pc_ctx[0].pc_asn = oid_cpy(x500_ds_as);
	x500_ds_pcdl->pc_ctx[0].pc_atn = NULLOID;
	x500_ds_pcdl->pc_ctx[1].pc_id = DIR_ACSE_PC_ID;
	x500_ds_pcdl->pc_ctx[1].pc_asn = oid_cpy(acse_pci);
	x500_ds_pcdl->pc_ctx[1].pc_atn = NULLOID;

	quipu_ds_pcdl->pc_nctx = 2;
	quipu_ds_pcdl->pc_ctx[0].pc_id = DIR_QUIPU_PC_ID;
	quipu_ds_pcdl->pc_ctx[0].pc_asn = oid_cpy(quipu_ds_as);
	quipu_ds_pcdl->pc_ctx[0].pc_atn = NULLOID;
	quipu_ds_pcdl->pc_ctx[1].pc_id = DIR_ACSE_PC_ID;
	quipu_ds_pcdl->pc_ctx[1].pc_asn = oid_cpy(acse_pci);
	quipu_ds_pcdl->pc_ctx[1].pc_atn = NULLOID;

	internet_ds_pcdl->pc_nctx = 2;
	internet_ds_pcdl->pc_ctx[0].pc_id = DIR_INTERNET_PC_ID;
	internet_ds_pcdl->pc_ctx[0].pc_asn = oid_cpy(internet_ds_as);
	internet_ds_pcdl->pc_ctx[0].pc_atn = NULLOID;
	internet_ds_pcdl->pc_ctx[1].pc_id = DIR_ACSE_PC_ID;
	internet_ds_pcdl->pc_ctx[1].pc_asn = oid_cpy(acse_pci);
	internet_ds_pcdl->pc_ctx[1].pc_atn = NULLOID;

	ntries = 6, ontty = isatty (2);
	while  (TNetListenAux (&dsaladdr -> pa_addr.sa_addr, TMagic, td)
			== NOTOK) {
		LLOG (log_dsap, LLOG_EXCEPTIONS,
			  ("TNetListen failed on address %s",
			   paddr2str (dsaladdr, NULLNA)));
		if (td -> td_cc > 0) {
			if (ontty)
				fprintf (stderr,
						 "TNetListen: [%s] %*.*s\nAddress: %s\n",
						 TErrString (td -> td_reason), td -> td_cc,
						 td -> td_cc, td -> td_data,
						 paddr2str (dsaladdr, NULLNA));
			LLOG (log_dsap, LLOG_FATAL,
				  ("TNetListen: [%s] %*.*s", TErrString (td -> td_reason),
				   td -> td_cc, td -> td_cc, td -> td_data));
		} else {
			if (ontty)
				fprintf (stderr, "TNetListen: [%s]\nAddress: %s\n",
						 TErrString (td -> td_reason),
						 paddr2str (dsaladdr, NULLNA));
			LLOG (log_dsap, LLOG_FATAL,
				  ("TNetListen: [%s]", TErrString (td -> td_reason)));
		}

		if (ontty || td -> td_reason != DR_CONGEST || --ntries < 1)
			return NOTOK;

		LLOG (log_dsap, LLOG_FATAL,
			  ("sleeping for 5 minutes, will continue retrying %d more time%s",
			   ntries, ntries != 1 ? "s" : ""));

		stop_listeners ();
		sleep ((unsigned) 300);

	}

	/* compare ta and ts_communities to see if they have a network in common */
	for (ip = ts_communities; *ip; ip++) {

		for (na=mydsaaddr->pa_addr.sa_addr.ta_addrs,
				x = mydsaaddr->pa_addr.sa_addr.ta_naddr - 1 ;
				x >= 0;
				na++, x-- )

			if (na->na_community == *ip)
				break;

		if (x < 0)
			LLOG (log_dsap,LLOG_NOTICE,("Using community %d for external access",*ip));
	}

	return OK;
}
