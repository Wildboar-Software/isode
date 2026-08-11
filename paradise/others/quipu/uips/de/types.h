/* template.c - your comments here */




#ifndef _query_types_h_
#define _query_types_h_

#include <stdio.h>

#define bool unsigned char
#define cardinal unsigned int

typedef enum {
	yes,
	no,
	unknown
} known;

typedef enum {
	RQ_processing,
	RQ_results_returned,
	RQ_error_returned
} request_state;

#endif
