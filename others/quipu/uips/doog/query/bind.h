/* 

#ifndef _query_bind_h_
#define _query_bind_h_

#include "types.h"

typedef enum {
	EXTERNAL_AUTH,
	STRONG_AUTH,
	PROTECTED_AUTH,
	SIMPLE_AUTH
} auth_bind_type;

QBool bind_to_ds(int argc, char *argv[], auth_bind_type auth_type);
int get_association_descriptor(QCardinal request_id);

#endif /* _query_bind_h_ */
