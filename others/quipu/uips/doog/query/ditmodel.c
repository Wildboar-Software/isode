/* 

/*****************************************************************************

  dit_model.c - Model of DIT used by query engine.

*****************************************************************************/

#include "types.h"
#include "util.h"

#include "ditmodel.h"

#include "quipu/attr.h"
objectTypeList get_child_list(AttributeType parent_type);
objectTypeList get_search_attrs(AttributeType attr_type);
static void set_search_attr(char *type_label, AttributeType obj_type, AttributeType search_type);
static int test_init_ditmodel (void);
static QBool is_search_type(AttributeType at);
entryList get_default_path(AttributeType object_type);


ditRelation ditmodel = NULLDitRelation;
searchInfoList search_type_list = NULLSearchInfo;

/* Should be in ufsearch.c ! */
ufsPath ufs_paths = NULLUfsPath;

/*
 * Add a new parent/child relation (if not already existent).
 */
void set_relation(AttributeType parent_type, AttributeType child_type, QBool need_subtree) {
	ditRelation new_node, curr_node;
	objectType curr_child;

	for (curr_node = ditmodel;
			curr_node != NULLDitRelation;
			curr_node = curr_node->next) {
		if (curr_node->parent_type == parent_type) {
			for (curr_child = curr_node->child_types;
					curr_child != NULLObjectType;
					curr_child = curr_child->next)
				if (curr_child->object_type == child_type) return;
			break;
		}
	}
	if (curr_node != NULLDitRelation) {
		curr_child = object_type_alloc();
		curr_child->next = curr_node->child_types;
		curr_node->child_types = curr_child;
		curr_child->object_type = child_type;
	} else {
		new_node = dit_relation_alloc();
		new_node->next = ditmodel;
		ditmodel = new_node;
		new_node->parent_type = parent_type;
		curr_child = new_node->child_types = object_type_alloc();
		curr_child->use_subtree = need_subtree;
		curr_child->object_type = child_type;
		curr_child->next = NULLObjectType;
	}
}

objectTypeList get_child_list(AttributeType parent_type) {
	ditRelation curr_node;

	for (curr_node = ditmodel;
			curr_node != NULLDitRelation;
			curr_node = curr_node->next)
		if (curr_node->parent_type == parent_type)
			return curr_node->child_types;
	return NULLObjectType;
}

objectTypeList get_search_attrs(AttributeType attr_type) {
	searchInfo curr_info;

	for (curr_info = search_type_list;
			curr_info != NULLSearchInfo;
			curr_info = curr_info->next)
		if (attr_type == curr_info->object_type) return curr_info->search_on_list;
	return NULLObjectType;
}

void set_search_attr(char *type_label, AttributeType obj_type, AttributeType search_type) {
	searchInfo curr_info;
	objectType curr_type;

	for (curr_info = search_type_list;
			curr_info != NULLSearchInfo;
			curr_info = curr_info->next)
		if (obj_type == curr_info->object_type) {
			for (curr_type = curr_info->search_on_list;
					curr_type != NULLObjectType;
					curr_type = curr_type->next)
				if (curr_type->object_type == search_type) return;
			curr_type = object_type_alloc();
			curr_type->next = curr_info->search_on_list;
			curr_info->search_on_list = curr_type;
			curr_type->object_type = search_type;
			return;
		}
	curr_info = search_info_alloc();
	curr_info->next = search_type_list;
	search_type_list = curr_info;
	curr_info->object_type = obj_type;
	curr_info->type_label = copy_string(type_label);
	curr_type = curr_info->search_on_list = object_type_alloc();
	curr_type->next = NULLObjectType;
	curr_type->object_type = search_type;
}

int test_init_ditmodel (void) {
	entryList search_path;
	AttributeType country, org, ou, loc, person;

	country = AttrT_new("2.5.6.2");
	org = AttrT_new("2.5.6.4");
	ou = AttrT_new("2.5.6.5");
	loc = AttrT_new("2.5.6.3");
	person = AttrT_new("2.5.6.6");
	/* Root goes to country and locality */
	set_relation(NULLAttrT, country, FALSE);
	set_relation(NULLAttrT, loc, FALSE);
	/* country goes to organization */
	set_relation(country, org, FALSE);
	set_relation(country, loc, FALSE);
	/* locality goes to org or ou */
	set_relation(loc, org, FALSE);
	set_relation(loc, ou, TRUE);
	set_relation(loc, person, TRUE);
	/* Organization goes to ou and cn */
	set_relation(org, ou, TRUE);
	set_relation(org, loc, FALSE);
	set_relation(org, person, TRUE);
	/* ou goes to person */
	set_relation(ou, person, TRUE);
	set_relation(ou, ou, FALSE);
	/* Default search paths */
	/* Search attrs */
	search_path = NULLEntryList;
	dn_list_insert("c=us", &search_path, country);
	dn_list_insert("c=gb", &search_path, country);
	set_default_path(country, search_path);
	set_search_attr("Country",
	country,
	AttrT_new("0.9.2342.19200300.99.1.8"));
	search_path = NULLEntryList;
	dn_list_insert("c=gb@o=nottingham university", &search_path, org);
	dn_list_insert("c=gb@o=university college london", &search_path, org);
	dn_list_insert("c=gb@o=edinburgh university", &search_path, org);
	dn_list_insert("c=gb@o=joint network team", &search_path, org);
	dn_list_insert("c=gb@o=brunel university", &search_path, org);
	set_default_path(loc, search_path);
	set_search_attr("Place",
	loc,
	AttrT_new("2.5.4.7"));
	set_default_path(org, search_path);
	set_search_attr("Organization",
	org,
	AttrT_new("2.5.4.10"));
	set_search_attr("Organization",
	org,
	AttrT_new("2.5.4.7"));
	search_path = NULLEntryList;
	set_default_path(ou, search_path);
	set_search_attr("Department",
	ou,
	AttrT_new("2.5.4.11"));
	set_search_attr("Department",
	ou,
	AttrT_new("2.5.4.7"));
	search_path = NULLEntryList;
	dn_list_insert("c=gb@o=nottingham university", &search_path, org);
	dn_list_insert("c=gb@o=university college london", &search_path, org);
	dn_list_insert("c=gb@o=edinburgh university", &search_path, org);
	dn_list_insert("c=gb@o=brunel university", &search_path, org);
	dn_list_insert("c=gb@o=joint network team", &search_path, org);
	set_default_path(person, search_path);
	set_search_attr("Person",
	person,
	AttrT_new("2.5.4.3"));
}

QBool is_search_attr(AttributeType octype, AttributeType srchtype) {
	return (octype == srchtype);
}

/*
 * Check if the given attribute is in the given objectClass' must list.
 */
QBool is_oc_must(objectclass *ocp, AttributeType at) {
	table_seq optr;

	for (optr = ocp->oc_must; optr != NULLTABLE_SEQ; optr = optr->ts_next)
		if (optr->ts_oa == at)
			return TRUE;
	return FALSE;
}

/*
 * Check if the given attribute is in the given objectClass' may list.
 */
QBool is_oc_may(objectclass *ocp, AttributeType at) {
	table_seq optr;

	for (optr = ocp->oc_may; optr != NULLTABLE_SEQ; optr = optr->ts_next)
		if (optr->ts_oa == at)
			return TRUE;
	return FALSE;
}

/*
 * Check if given object class is in search type list.
 */
QBool is_search_type(AttributeType at) {
	searchInfoList srchlist = search_type_list;

	while (srchlist != NULLSearchInfo && srchlist->object_type != at)
		;
	return (srchlist == NULLSearchInfo? FALSE : TRUE);
}

/*
 * Return the label for a given search type.
 */
char *get_type_label(AttributeType at) {
	searchInfoList srchlist;

	for (srchlist = search_type_list;
			srchlist != NULLSearchInfo && srchlist->object_type != at;
			srchlist = srchlist->next)
		;
	return (srchlist == NULLSearchInfo? NULLCP : srchlist->type_label);
}

/*
 * Set a Ufs Path.
 */
void set_default_path(AttributeType object_type, entryList path) {
	ufsPath curr_path;

	for (curr_path = ufs_paths;
			curr_path != NULLUfsPath;
			curr_path = curr_path->next) {
		if (curr_path->object_type == object_type)
			break;
	}
	if (curr_path == NULLUfsPath) {
		curr_path = ufs_path_alloc();
		curr_path->next = ufs_paths;
		curr_path->object_type = object_type;
		curr_path->path = path;
		ufs_paths = curr_path;
	}
}

entryList get_default_path(AttributeType object_type) {
	ufsPath curr_path;

	for (curr_path = ufs_paths;
			curr_path != NULLUfsPath;
			curr_path = curr_path->next) {
		if (curr_path->object_type == object_type)
			return curr_path->path;
	}
	return NULLEntryList;
}
