#ifndef QUIPU_SCHEMA_H
#define QUIPU_SCHEMA_H

#include "quipu/attr.h"

int check_oc_hierarchy (AV_Sequence avs);
int check_avs_schema (AttributeType at, AV_Sequence avs_oc);

/** see if b in oc a */
int test_hierarchy (
	const objectclass *a,
	const objectclass *b
);

int test_schema (AV_Sequence tree, AV_Sequence oc);

#endif