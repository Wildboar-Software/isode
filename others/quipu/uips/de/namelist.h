/* template.c - your comments here */





#include <stdio.h>
#include "quipu/attrvalue.h"
#include "util.h"

struct namelist {
	char *name;
	struct namelist *next;
	Attr_Sequence ats;
};

#define NULLLIST (struct namelist *)NULL
#define list_alloc()   (struct namelist *)smalloc(sizeof (struct namelist))
