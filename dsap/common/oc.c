/* oc.c - Object Class routines */







#include "quipu/util.h"
#include "quipu/entry.h"
#include "tailor.h"

extern LLog * log_dsap;
extern short oc_sntx;
extern IFP oc_hier;
extern IFP oc_avsprint;

static void add_hierarchy (objectclass *oc, AV_Sequence *avsp);

objectclass * oc_add (OID oid)
{
	oid_table * Current;
	extern objectclass ocOIDTable[];
	extern int ocNumEntries;

	Current = &ocOIDTable[ocNumEntries].oc_ot;
	if (oid == NULLOID)
		Current->ot_oid = NULLOID;
	else
		Current->ot_oid = oid_cpy (oid);
	Current->ot_name = strdup(oid2ode_aux(oid,0));
	Current->ot_stroid = strdup(sprintoid(oid));
	add_entry_aux (Current->ot_name,(caddr_t)&ocOIDTable[ocNumEntries],3,NULLCP);
	ocOIDTable[ocNumEntries].oc_hierachy = NULLOCSEQ;
	ocOIDTable[ocNumEntries].oc_may  = NULLTABLE_SEQ;
	ocOIDTable[ocNumEntries].oc_must = NULLTABLE_SEQ;
	return (&ocOIDTable[ocNumEntries++]);
}

objectclass * str2oc (char *str)
{
	char * ptr;
	char * get_oid (char *str);
	objectclass *oc;

	if ((oc = name2oc (str)) != NULLOBJECTCLASS)
		return (oc);

	/* unknown object class -- need to add to table */
	if ((ptr = get_oid (str)) == NULLCP) {
		parse_error ("Object class %s unknown",str);
		return (NULLOBJECTCLASS);
	}

	return (oc_add (str2oid(ptr)));
}

static AV_Sequence new_oc_avs (objectclass *oc)
{
	AV_Sequence avs;

	avs = avs_comp_alloc();
	avs->avseq_next = NULLAV;
	avs->avseq_av.av_syntax = oc_sntx;
	avs->avseq_av.av_struct = (caddr_t) oc;
	return (avs);
}

static AV_Sequence str2oc_hier (char *str)
{
	AV_Sequence avs = NULLAV;
	objectclass * oc;
	char * ptr, *save, val;

	str = SkipSpace (str);

	while ((ptr = index (str,'&')) != 0) {
		save = ptr++;
		save--;
		if (! isspace (*save))
			save++;
		val = *save;
		*save = 0;

		if ((oc = str2oc (str)) == NULLOBJECTCLASS)
			return (NULLAV);
		if (avs == NULLAV)
			avs = new_oc_avs (oc);
		else
			add_oc_avs (oc,&avs);
		add_hierarchy (oc,&avs);

		*save = val;
		str = SkipSpace(ptr);
	}

	if ((oc = str2oc (str)) == NULLOBJECTCLASS)
		return (NULLAV);
	if (avs == NULLAV)
		avs = new_oc_avs (oc);
	else
		add_oc_avs (oc,&avs);
	add_hierarchy (oc,&avs);

	return (avs);
}

void add_oc_avs (objectclass *oc, AV_Sequence *avsp)
{
	AV_Sequence loop;
	objectclass *ocp;

	/* see if we already have oc in heirarchy ... */

	for (loop = *avsp; loop != NULLAV; loop = loop->avseq_next) {
		ocp = (objectclass *)loop->avseq_av.av_struct;
		if (oc == ocp)
			return;
	}
	*avsp = avs_merge (*avsp,new_oc_avs(oc));
}

static void add_hierarchy (objectclass *oc, AV_Sequence *avsp)
{
	struct oc_seq * oidseq;

	for (oidseq = oc->oc_hierachy;
			oidseq != NULLOCSEQ; oidseq = oidseq->os_next) {
		add_oc_avs (oidseq->os_oc,avsp);
		add_hierarchy (oidseq->os_oc,avsp);
	}
}

static int in_hierarchy (AV_Sequence a, AV_Sequence b)
{
	struct oc_seq * oidseq;
	objectclass *oca, *ocb;

	if ((a == NULLAV) || (a->avseq_av.av_syntax != oc_sntx) || (a->avseq_av.av_struct == NULL))
		return (FALSE);

	if ((b == NULLAV) || (b->avseq_av.av_syntax != oc_sntx) || (b->avseq_av.av_struct == NULL))
		return (FALSE);

	oca = (objectclass *) a->avseq_av.av_struct;
	ocb = (objectclass *) b->avseq_av.av_struct;

	for (oidseq = ocb->oc_hierachy;
			oidseq != NULLOCSEQ; oidseq = oidseq->os_next)
		if (objclass_cmp(oca,oidseq->os_oc) == 0)
			return (TRUE);

	return (FALSE);
}

static void oc_print_avs (PS ps, AV_Sequence avs, int format)  /* need to use this somehow !!! */
      
                
           
{
	AV_Sequence newavs;
	char found;
	char printed = FALSE;

	if (avs == NULLAV)
		return;

	if (format != READOUT)
		DLOG (log_dsap,LLOG_EXCEPTIONS,("invalid call to oc_print"));

	for ( ; avs->avseq_next != NULLAV ; avs=avs->avseq_next) {
		found = FALSE;
		for (newavs = avs->avseq_next; newavs != NULLAV; newavs=newavs->avseq_next)
			if (in_hierarchy(avs,newavs) == TRUE) {
				found = TRUE;
				break;
			}

		if (found == FALSE) {
			if (printed == TRUE)
				ps_print (ps," & ");
			AttrV_print (ps,&avs->avseq_av,format);
			printed = TRUE;
		}
	}

	if (printed == TRUE)
		ps_print (ps," & ");
	AttrV_print (ps,&avs->avseq_av,format);
}

int objectclass_cmp (objectclass *a, objectclass *b)
{
	/* macro ! */

	return objclass_cmp(a,b);
}

static objectclass * oc_cpy (objectclass *oc)
{
	return (oc);	/* static table !!! */
}

int check_in_oc (OID oid, AV_Sequence avs)
{
	objectclass * oc;

	for (; avs != NULLAV; avs = avs->avseq_next) {
		oc = (objectclass *) avs->avseq_av.av_struct;
		if (oc == NULLOBJECTCLASS)
			continue;
		if (oid_cmp(oid,oc->oc_ot.ot_oid) == 0)
			return (TRUE);
	}

	return (FALSE);
}

static void oc_free (objectclass *oc)
{
	;	/* static table !!! */
}

static PE oc_enc (objectclass *oc)
{
	return (oid2prim(oc->oc_ot.ot_oid));
}

static objectclass * oc_dec (PE pe)
{
	OID oid;
	objectclass *oc;

	if (!test_prim_pe (pe,PE_CLASS_UNIV,PE_PRIM_OID))
		return NULLOBJECTCLASS;

	if ((oid = prim2oid (pe)) == NULLOID)
		return NULLOBJECTCLASS;

	if ((oc = oid2oc (oid)) != NULLOBJECTCLASS)
		return (oc);

	return (oc_add(oid));
}

void oc_print (PS ps, objectclass *oc, int format)
{
	extern int oidformat;

	if ( format != READOUT)
		ps_printf (ps,"%s",oc2name (oc,OIDPART));
	else
		ps_printf (ps,"%s",oc2name (oc,oidformat));
}

void objectclass_syntax (void) {
	oc_sntx = add_attribute_syntax ("objectclass",
									oc_enc,		oc_dec,
									str2oc,		oc_print,
									oc_cpy,		objectclass_cmp,
									oc_free,	NULLCP,
									NULLIFP,	FALSE );
	oc_hier = (IFP) str2oc_hier;
	oc_avsprint = (IFP) oc_print_avs;
	want_oc_hierarchy ();
}
