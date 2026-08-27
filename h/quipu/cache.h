#ifndef CACHEH
#define CACHEH

#include "quipu/entry.h"
#include "quipu/ds_error.h"
#include "quipu/commonarg.h"

int check_schema (Entry eptr, Attr_Sequence as, struct DSError *error);
int unravel_attribute (Entry eptr, struct DSError *error);
void cache_entry (const EntryInfo *ptr, const char complete, const char vals);
void delete_cache (DN adn);
void delete_list_cache (DN adn);
struct subordinate;
struct subordinate *subord_cpy (struct subordinate *x);
int free_all_list_cache (void);

#endif
