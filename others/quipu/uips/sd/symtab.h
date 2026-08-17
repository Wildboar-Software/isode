#ifndef SYMTAB
#define SYMTAB

typedef struct tab_entry {
	char *val;
	char *name;
	struct tab_entry *next;
} *table_entry;

#define NULLSYM (table_entry) 0

#endif
