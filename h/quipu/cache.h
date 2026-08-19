#ifndef CACHEH
#define CACHEH

#include "quipu/entry.h"
#include "quipu/ds_error.h"
#include "quipu/commonarg.h"

int check_schema (Entry eptr, Attr_Sequence as, struct DSError *error);
int unravel_attribute (Entry eptr, struct DSError *error);
void cache_entry (EntryInfo *ptr, char complete, char vals);

#endif
