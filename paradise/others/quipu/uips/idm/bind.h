#ifndef _xdua_h_
#define _xdua_h_

#include "types.h"

typedef enum {
	EXTERNAL_AUTH,
	STRONG_AUTH,
	PROTECTED_AUTH,
	SIMPLE_AUTH
} auth_bind_type;

int bind_to_ds();

#endif /* _xdua_h_ */

#define INV_PSWD -2
#define PSWD_TRY_LIMIT 3
#define USERNAME_TRY_LIMIT 3
#define INV_USERNAME -3
