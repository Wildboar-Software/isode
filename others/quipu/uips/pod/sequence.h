/*
 * 
 */

#ifndef SEQ
#define SEQ

#include "general.h"
#include "defs.h"

typedef struct str_seq {
	char *dname;
	struct str_seq *next;
} strSeq, *str_seq;

#define NULLDS ((str_seq) 0)
char *get_from_seq(int seq_num, str_seq seq_ptr);
void add_seq(str_seq *seq, char *str);
void free_seq(str_seq seq_ptr);
#endif
