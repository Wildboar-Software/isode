/* 

#ifndef _query_ds_util_h_
#define _query_ds_util_h_

#include "types.h"
#include "util.h"
#include "error.h"

void qy_dn_print(PS ps, DN dn, int format);
char *qy_dn2str(DN dn);
QBool qy_in_hierarchy(objectclass *a, objectclass *b);

QE_error_code make_filter_items(AttributeType attr_type, char *search_value, Filter *ex_filter_ptr, Filter *ap_filter_ptr), make_typed_filter_items(char *filter_str, Filter *ex_filter_ptr, Filter *ap_filter_ptr);
QBool is_good_match(char *match_str, char *dn_str);

char *get_entry_type_name(char *entry_name);

#endif
