#ifndef _query_attr_type_list_h_
#define _query_attr_type_list_h_

#include "types.h"
#include "quipu/attr.h"

/*
 *	Storage cell for a list of AttributeTypes
 */
typedef struct _attr_type_cell {
	AttributeType type;

	struct _attr_type_cell *next;
} attr_type_cell, *attrTypeList;

#define NULLAttrTypeList (attrTypeList) NULL
#define at_cell_alloc() (attrTypeList) smalloc(sizeof(attr_type_cell))

/*
 *	Procs
 */
QBool add_type_to_list(AttributeType type, attrTypeList *list_ptr), delete_type_from_list(AttributeType type, attrTypeList *list_ptr);
void free_at_list(attrTypeList *list_ptr);

#endif
