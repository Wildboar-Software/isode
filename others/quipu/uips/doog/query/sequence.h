/* 

#ifndef _query_sequence_h_
#define _query_sequence_h_

#include "types.h"

typedef struct _string_cell {
	char *string;
	struct _string_cell *next;
} string_cell, *stringCell, *stringList;

#define NULLStrCell (stringCell) NULL
#define cell_alloc() (stringCell) smalloc(sizeof(string_cell))

void add_string_to_seq(char *string, stringCell *sequence),
	 delete_from_seq(char *string, stringCell *sequence),
	 free_string_seq(stringCell *sequence);

stringCell copy_string_seq();

#endif /* _query_sequence_h_ */
