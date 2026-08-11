/*
 * 
 */

/* This file contains code to implement the list storage facilities
 * in the modified widget program (renamed SD 5/1/90).
 */

/*    This file was written by Damanjit Mahl @ Brunel University
 *    as part of the modifications made to
 *    the Quipu X.500 widget interface written by Paul Sharpe
 *    at GEC Research, Hirst Research Centre.
 */



#ifndef STRINGSEQ
#define STRINGSEQ

#include "general.h"

typedef struct string_seq {
	char *dname;
	unsigned s_strlen;
	int seq_num;
	struct string_seq *next;
} string_seq, *str_seq;

#define NULLDS ((str_seq) 0)
char *get_from_seq ();
void free_seq(), add_seq();

#endif
