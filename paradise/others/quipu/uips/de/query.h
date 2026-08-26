/* query.h - main information structure */
#ifndef _query_h
#define _query_h

#include "quipu/util.h"

struct query {
	char defvalue [LINESIZE];
	char entered [LINESIZE];
	char displayed [LINESIZE];
	struct namelist * lp;
	int listlen;
};

#endif /* _query_h */
