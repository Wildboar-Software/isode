/* eis_select.c - */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/eis_select.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/RCS/eis_select.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: eis_select.c,v $
 * Revision 9.0  1992/06/16  12:34:01  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#include "quipu/util.h"
#include "quipu/commonarg.h"
#include "quipu/entry.h"

extern LLog * log_dsap;

#define EIS_SELECT eis.eis_select

static Attr_Sequence  cpy_as_comp_type ();
Attr_Sequence  cpy_as_comp ();
static Attr_Sequence  as_cpy_type ();
static Attr_Sequence  as_cpy_enc (Attr_Sequence, DN, DN, char);
extern AV_Sequence avs_cpy_enc ();
extern Attr_Sequence dsa_pseudo_attr;
extern AttributeType at_acl;

Attr_Sequence
eis_select (
	EntryInfoSelection eis,
	Entry entryptr,
	DN dn,
	char qctx,	/* If TRUE - it is a Quipu context association */
	DN node
) {
	Attr_Sequence result = NULLATTR;
	Attr_Sequence trail;
	Attr_Sequence temp;
	Attr_Sequence temp2;
	Attr_Sequence eptr;

	DLOG (log_dsap,LLOG_TRACE,("eis_select"));

	if (eis.eis_allattributes || qctx) {

		for(temp=entryptr->e_attributes; temp != NULLATTR; temp=temp->attr_link) {

			if ( ! (qctx && (AttrT_cmp (at_acl,temp->attr_type) == 0)))
				if ( (temp->attr_acl != NULLACL_INFO) &&
						(check_acl(dn,ACL_READ,temp->attr_acl,node ) != OK ))
					continue;

			if ((eis.eis_infotypes == EIS_ATTRIBUTETYPESONLY) && !qctx)
				temp2 = cpy_as_comp_type (temp);
			else
				temp2 = cpy_as_comp (temp);

			if (result == NULLATTR) {
				result = temp2;
				trail = result;
			} else {
				trail->attr_link = temp2;
				trail = temp2;
			}
		}

		if (entryptr->e_iattr) {
			for (temp = entryptr->e_iattr->i_default; temp != NULLATTR; temp=temp->attr_link) {
				if ( ! as_find_type (entryptr->e_attributes,temp->attr_type)) {

					if ( ! (qctx && (AttrT_cmp (at_acl,temp->attr_type) == 0)))
						if ( (temp->attr_acl != NULLACL_INFO) &&
								(check_acl(dn,ACL_READ,temp->attr_acl,node ) != OK ))
							continue;

					if ((eis.eis_infotypes == EIS_ATTRIBUTETYPESONLY) && !qctx)
						temp2 = cpy_as_comp_type (temp);
					else
						temp2 = cpy_as_comp (temp);

					if (result == NULLATTR) {
						result = temp2;
						trail = result;
					} else {
						trail->attr_link = temp2;
						trail = temp2;
					}
				}
			}

			if (entryptr->e_iattr->i_always) {
				if ((eis.eis_infotypes == EIS_ATTRIBUTETYPESONLY) && !qctx)
					temp = as_cpy_type (entryptr->e_iattr->i_always);
				else
					temp = as_cpy_enc (entryptr->e_iattr->i_always,dn,node,qctx);

				result = as_merge (result, temp);
			}
		}

	} else {
		for(eptr=EIS_SELECT; eptr != NULLATTR; eptr=eptr->attr_link) {

			if ((temp = as_find_type (entryptr->e_attributes,eptr->attr_type)) == NULLATTR) {
				if (entryptr->e_iattr) {
					if (entryptr->e_iattr->i_default)
						if ((temp = as_find_type (entryptr->e_iattr->i_default,eptr->attr_type)) != NULLATTR)
							goto got_default;
					if (entryptr->e_iattr->i_always)
						goto got_always;

				}
				continue;
			}

got_default:
			;
			if ( (temp->attr_acl != NULLACL_INFO)&& (check_acl(dn,ACL_READ,temp->attr_acl,node ) != OK ))
				continue;

			if (eis.eis_infotypes == EIS_ATTRIBUTETYPESONLY)
				temp2 = cpy_as_comp_type (temp);
			else
				temp2 = cpy_as_comp (temp);

			if (result == NULLATTR) {
				result = temp2;
				trail = result;
			} else {
				trail->attr_link = temp2;
				trail = temp2;
			}
got_always:
			;

			if (entryptr->e_iattr) {
				if ((temp = as_find_type (entryptr->e_iattr->i_always,eptr->attr_type)) != NULLATTR) {

					if ( (temp->attr_acl != NULLACL_INFO)&& (check_acl(dn,ACL_READ,temp->attr_acl,node ) != OK ))
						continue;

					if (eis.eis_infotypes == EIS_ATTRIBUTETYPESONLY)
						temp2 = cpy_as_comp_type (temp);
					else
						temp2 = cpy_as_comp (temp);

					if (result == NULLATTR) {
						result = temp2;
						trail = result;
					} else {
						trail->attr_link = temp2;
						trail = temp2;
					}
				}
			}

		}
	}
	return (result);
}


Attr_Sequence
attr_eis_select (EntryInfoSelection eis, Attr_Sequence as, DN dn, DN node) {
	Attr_Sequence result = NULLATTR;
	Attr_Sequence trail;
	Attr_Sequence temp;
	Attr_Sequence temp2;
	Attr_Sequence eptr;

	DLOG (log_dsap,LLOG_TRACE,("attr_eis_select"));

	if (eis.eis_allattributes) {

		for(temp=as; temp != NULLATTR; temp=temp->attr_link) {

			if ( (temp->attr_acl != NULLACL_INFO) &&
					(check_acl(dn,ACL_READ,temp->attr_acl,node ) != OK ))
				continue;
			if (eis.eis_infotypes == EIS_ATTRIBUTETYPESONLY)
				temp2 = cpy_as_comp_type (temp);
			else
				temp2 = cpy_as_comp (temp);

			if (result == NULLATTR) {
				result = temp2;
				trail = result;
			} else {
				trail->attr_link = temp2;
				trail = temp2;
			}
		}

	} else {
		for(eptr=EIS_SELECT; eptr != NULLATTR; eptr=eptr->attr_link) {

			if ((temp = as_find_type (as,eptr->attr_type)) == NULLATTR)
				continue;

			if ( (temp->attr_acl != NULLACL_INFO) &&
					(check_acl(dn,ACL_READ,temp->attr_acl,node ) != OK ))
				continue;

			if (eis.eis_infotypes == EIS_ATTRIBUTETYPESONLY)
				temp2 = cpy_as_comp_type (temp);
			else
				temp2 = cpy_as_comp (temp);

			if (result == NULLATTR) {
				result = temp2;
				trail = result;
			} else {
				trail->attr_link = temp2;
				trail = temp2;
			}
		}
	}
	return (result);
}


Attr_Sequence
dsa_eis_select (
	EntryInfoSelection eis,
	Entry entryptr,
	DN dn,
	char qctx,	/* If TRUE - it is a Quipu context association */
	DN node
) {
	Attr_Sequence result = NULLATTR;
	Attr_Sequence trail;
	Attr_Sequence temp;
	Attr_Sequence temp2;
	Attr_Sequence eptr;
	extern AttributeType at_acl;

	DLOG (log_dsap,LLOG_TRACE,("dsa_eis_select"));

	/* Only send attributes asked for - even if Quipu context. */
	/* Don't want this stuff cached */

	if (eis.eis_allattributes)
		return eis_select (eis,entryptr,dn, qctx, node);

	update_pseudo_attr ();

	for(eptr=EIS_SELECT; eptr != NULLATTR; eptr=eptr->attr_link) {

		if ((temp = as_find_type (entryptr->e_attributes,
								  eptr->attr_type)) == NULLATTR)
			if ((temp = as_find_type (dsa_pseudo_attr,
									  eptr->attr_type)) == NULLATTR)
				continue;

		if ( (temp->attr_acl != NULLACL_INFO)&& (check_acl(dn,ACL_READ,temp->attr_acl,node ) != OK ))
			continue;

		if (eis.eis_infotypes == EIS_ATTRIBUTETYPESONLY)
			temp2 = cpy_as_comp_type (temp);
		else
			temp2 = cpy_as_comp (temp);

		if (result == NULLATTR) {
			result = temp2;
			trail = result;
		} else {
			trail->attr_link = temp2;
			trail = temp2;
		}
	}
	return (result);
}

Attr_Sequence
cpy_as_comp (Attr_Sequence as) {
	Attr_Sequence ptr;

	if (as==NULLATTR) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("copy of null as"));
		return (NULLATTR);
	}
	ptr = as_comp_alloc();
	ptr->attr_type = AttrT_cpy (as->attr_type);
	ptr->attr_value = avs_cpy_enc (as->attr_value);
	ptr->attr_link = NULLATTR;
	ptr->attr_acl = NULLACL_INFO;

	return (ptr);
}


static Attr_Sequence
cpy_as_comp_type (Attr_Sequence as) {
	Attr_Sequence ptr;

	if (as==NULLATTR) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("copy of null as"));
		return (NULLATTR);
	}
	ptr = as_comp_alloc();
	ptr->attr_type = AttrT_cpy (as->attr_type);
	ptr->attr_value = NULLAV;
	ptr->attr_link = NULLATTR;
	ptr->attr_acl = NULLACL_INFO;
	return (ptr);
}



int
eis_check (EntryInfoSelection eis, Entry entryptr, DN dn) {
	Attr_Sequence temp;
	Attr_Sequence as;
	DN node;

	DLOG (log_dsap,LLOG_TRACE,("eis_check"));

	node = get_copy_dn (entryptr);

	if (eis.eis_allattributes) {
		for(temp=entryptr->e_attributes; temp != NULLATTR; temp=temp->attr_link)
			if (temp->attr_acl != NULLACL_INFO)
				if (check_acl(NULLDN,ACL_READ,temp->attr_acl,node ) != OK )
					if (check_acl(dn,ACL_READ,temp->attr_acl,node ) == OK ) {
						dn_free (node);
						return NOTOK;
					}
	} else {
		for(temp=EIS_SELECT; temp != NULLATTR; temp=temp->attr_link) {
			if ((as = as_find_type (entryptr->e_attributes,temp->attr_type)) == NULLATTR)
				continue;
			if (as->attr_acl != NULLACL_INFO)
				if (check_acl(NULLDN,ACL_READ,as->attr_acl,node ) != OK )
					if (check_acl(dn,ACL_READ,as->attr_acl,node ) == OK ) {
						dn_free (node);
						return NOTOK;
					}
		}
	}
	dn_free (node);
	return OK;
}

static Attr_Sequence
as_cpy_type (Attr_Sequence as) {
	Attr_Sequence start;
	Attr_Sequence ptr,ptr2;
	Attr_Sequence eptr;

	start = cpy_as_comp_type (as);
	ptr2 = start;

	for(eptr = as->attr_link; eptr != NULLATTR; eptr=eptr->attr_link) {
		ptr = cpy_as_comp_type (eptr);
		ptr2->attr_link = ptr;
		ptr2 = ptr;
	}
	return (start);
}

static Attr_Sequence
as_cpy_enc (Attr_Sequence as, DN dn, DN node, char qctx) {
	Attr_Sequence start;
	Attr_Sequence ptr,ptr2;
	Attr_Sequence eptr;
	extern AttributeType at_acl;

	start = cpy_as_comp (as);
	ptr2 = start;

	for(eptr = as->attr_link; eptr != NULLATTR; eptr=eptr->attr_link) {
		if ( ! (qctx && (AttrT_cmp (at_acl,eptr->attr_type) == 0)))
			if ( (eptr->attr_acl != NULLACL_INFO) &&
					(check_acl(dn,ACL_READ,eptr->attr_acl,node) != OK ))
				continue;

		ptr = cpy_as_comp (eptr);
		ptr2->attr_link = ptr;
		ptr2 = ptr;
	}
	return (start);
}
