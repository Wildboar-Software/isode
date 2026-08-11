

/* 

/*****************************************************************************

  attrt_list.c - Strorage for a list of AttributeTypes

*****************************************************************************/

#include "attrt_list.h"

/*
 * - add_type_to_list() -
 */
QBool add_type_to_list(type, list_ptr)
AttributeType type;
attrTypeList *list_ptr;
{
	attrTypeList curr_cell = *list_ptr;

	while (curr_cell != NULLAttrTypeList && curr_cell->type != type)
		curr_cell = curr_cell->next;

	if (curr_cell == NULLAttrTypeList) {
		curr_cell = at_cell_alloc();

		curr_cell->next = *list_ptr;
		curr_cell->type = type;

		*list_ptr = curr_cell;

		return TRUE;
	}

	return FALSE;
} /* add_type_to_list */

/*
 * - delete_type_from_list() -
 */
QBool delete_type_from_list(type, list_ptr)
AttributeType type;
attrTypeList *list_ptr;
{
	attrTypeList curr_cell = *list_ptr, last_cell = NULLAttrTypeList;

	while (curr_cell != NULLAttrTypeList && curr_cell->type != type) {
		last_cell = curr_cell;
		curr_cell = curr_cell->next;
	}

	if (curr_cell == NULLAttrTypeList)
		return FALSE;
	else {
		if (last_cell == NULLAttrTypeList)
			*list_ptr = curr_cell->next;
		else {
			last_cell->next = curr_cell->next;
			free((char *) curr_cell);
		}

		return TRUE;
	}
} /* delete_type_from_list */

/*
 * - free_at_list() -
 */
void free_at_list(list_ptr)
attrTypeList *list_ptr;
{
	attrTypeList curr_cell, next_cell;

	for (curr_cell = *list_ptr;
			curr_cell != NULLAttrTypeList;
			curr_cell = next_cell) {
		next_cell = curr_cell->next;

		free((char *) curr_cell);
	}

	*list_ptr = NULLAttrTypeList;
} /* free_at_list */
