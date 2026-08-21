/*
 * 
 */

#ifndef FILT
#define FILT

#include "quipu/ds_search.h"

typedef struct stroid_list {
	int fi_type;
	char *stroid;
	char *name;
} filt_item;

typedef struct filter_struct {
	int flt_type;
	union pod_ftype {
		filt_item item;
		struct filter_struct *sub_filt;
	} fu_cont;
	struct filter_struct *next;
} filt_struct;

void make_type(char *name_val, filt_struct *filt);
filt_struct *make_item_filter(char *oid, int match, char *value);
filt_struct *link_filters(filt_struct *filt1, filt_struct *filt2);
filt_struct *make_parent_filter(int filt_type, filt_struct *filt1, filt_struct *filt2, filt_struct *filt3);
Filter make_filter(filt_struct *filt);
Filter make_attr_filter(void);
void free_filt(filt_struct *filt);

#endif
