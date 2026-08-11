/* 

#ifndef _query_types_h_
#define _query_types_h_

#include <stdio.h>

#define QBool char
#define QCardinal unsigned int

typedef enum {
	yes,
	no,
	unknown,
	might_not
} known;

typedef enum {
	RQ_processing,
	RQ_results_returned,
	RQ_error_returned
} request_state;

#ifndef TRUE
#define TRUE (1)
#define FALSE (0)
#endif

#endif
