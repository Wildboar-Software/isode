/* entryinfo.c - Entry Information routines */

#include <string.h>
#include "quipu/util.h"
#include "quipu/commonarg.h"

void entryinfo_comp_free (const EntryInfo *a, const int state)
{
	EntryInfo * einfo, *e2;
	Attr_Sequence as;

	if (a == NULLENTRYINFO)
		return;
	dn_free (a->ent_dn);
	if (state == 1) {
		Attr_Sequence as_tmp = NULLATTR;
		for ( as=a->ent_attr; as!= NULLATTR; as=as_tmp) {
			as_tmp = as -> attr_link;
			free ((char *) as);
		}
	} else
		as_free (a->ent_attr);
	for (einfo=a->ent_next; einfo!=NULLENTRYINFO; einfo=e2) {
		dn_free (einfo->ent_dn);
		if (state == 1) {
			Attr_Sequence as_tmp = NULLATTR;
			for ( as=einfo->ent_attr; as!= NULLATTR; as=as_tmp) {
				as_tmp = as -> attr_link;
				free ((char *) as);
			}
		} else
			as_free (einfo->ent_attr);
		e2 = einfo -> ent_next;
		free ((char *) einfo);
	}
}

void entryinfo_free (EntryInfo *a, const int state)
{
	if (a == NULLENTRYINFO)
		return;
	entryinfo_comp_free (a,state);
	free ((char *) a);
}

void entryinfo_cpy (EntryInfo *a, const EntryInfo *b)
{
	a->ent_dn        = dn_cpy (b->ent_dn);
	a->ent_attr      = as_cpy (b->ent_attr);
	a->ent_iscopy    = b->ent_iscopy;
	a->ent_age       = b->ent_age;
	a->ent_next      = b->ent_next;
}

void entryinfo_append (const EntryInfo *a, const EntryInfo *b)
{
	EntryInfo *ptr;
	if ( a  == NULLENTRYINFO )
		return;
	for (ptr=a; ptr->ent_next != NULLENTRYINFO; ptr=ptr->ent_next)
		;  /* noop */
	ptr->ent_next = b;
}

void entryinfo_merge (const EntryInfo *a, const EntryInfo *b, const int fast)
{
	EntryInfo *ptr;
	EntryInfo *tmp, *prev, *trail;

	if (( a == NULLENTRYINFO )
			|| (b == NULLENTRYINFO ))
		return;
	/* Use fast mode if you know there are no duplicates */
	if (fast) {
		for (ptr=a; ptr->ent_next != NULLENTRYINFO; ptr=ptr->ent_next)
			;
		ptr->ent_next = b;
		return;
	}
	for (ptr=a; ptr != NULLENTRYINFO; ptr=ptr->ent_next) {
		prev = NULLENTRYINFO;
		for (tmp=b; tmp != NULLENTRYINFO; tmp=tmp->ent_next) {
			if (dn_cmp (ptr->ent_dn, tmp->ent_dn) == OK) {
				/* already got it - throw it away */
				if (prev == NULLENTRYINFO)
					b = tmp->ent_next;
				else
					prev->ent_next = tmp->ent_next;
				as_free (tmp->ent_attr);
				dn_free (tmp->ent_dn);
				free ((char *)tmp);
				break;
			}
			prev = tmp;
		}
		trail = ptr;
	}
	trail->ent_next = b;
}

void entryinfo_print (PS ps, const EntryInfo *entryinfo, const int format)
{
	EntryInfo *einfo;

	for (einfo= entryinfo; einfo!=NULLENTRYINFO; einfo=einfo->ent_next) {
		dn_print(ps,einfo->ent_dn,EDBOUT);
		ps_print (ps,"\n");
		if (einfo->ent_attr)
			as_print (ps,einfo->ent_attr,format);
		ps_print (ps,"\n");
	}
}
