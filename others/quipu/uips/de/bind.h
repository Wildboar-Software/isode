/* template.c - your comments here */

#ifndef _xdua_h_
#define _xdua_h_

#include "types.h"

typedef enum {
	EXTERNAL_AUTH,
	STRONG_AUTH,
	PROTECTED_AUTH,
	SIMPLE_AUTH
} auth_bind_type;

int bind_to_ds(int argc, char *argv[], auth_bind_type auth_type);

#endif _xdua_h_
