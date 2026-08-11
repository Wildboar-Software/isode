/*
 * 
 */

#ifndef FILT
#define FILT

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

#endif
