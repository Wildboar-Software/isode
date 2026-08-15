#include <string.h>
#include <stdlib.h>
#include "general.h"
#include "symtab.h"

void put_symbol_value(table_entry table, char *name, char *val) {
	if (!name) return;
	while(table && strcmp(name, table->name)) {
		table = table->next;
	}
	if (table) {
		free(table->val);
		if (val) {
			table->val =
				(char *) malloc((unsigned) strlen(val) + 1);
			strcpy(table->val, val);
		} else
			table->val = (char *) 0;
	} else {
		table = (table_entry) malloc(sizeof(table_entry));
		table->next = NULLSYM;
		table->name = (char *) malloc((unsigned) strlen(name) + 1);
		strcpy(table->name, name);
		if (val) {
			table->val =
				(char *) malloc((unsigned) strlen(val) + 1);
			strcpy(table->val, val);
		} else
			table->val = 0;
	}
}

char *get_symbol_value(table_entry table, char *name) {
	while(table && strcmp(name, table->name)) table = table->next;
	if (table)
		return table->val;
	return (char *) 0;
}

void free_table(table_entry table) {
	table_entry  entry;

	while(table) {
		if (table->val)
			free(table->val);
		free(table->name);
		entry = table;
		table = table->next;
		free((char *) entry);
	}
}
