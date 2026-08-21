/* 

#ifndef _query_entry_list_h_
#define _query_entry_list_h_

#include "types.h"
#include "quipu/attr.h"

typedef struct ent_list {
	char *string_dn;
	char *sort_key;
	AttributeType object_class;
	struct ent_list *next;
} entry_list, *entryList;

#define NULLEntryList (entryList) NULL
#define entry_list_alloc() (entryList) smalloc(sizeof(entry_list))

void dn_list_free(entryList *entry_list_ptr), dn_list_copy(entryList original, entryList *copy);
QBool dn_list_add(char *dn, entryList *entry_list_ptr, AttributeType object_type), dn_list_merge(), dn_list_insert(char *dn, entryList *entry_list_ptr, AttributeType object_type);

#endif _query_entry_list_h_
