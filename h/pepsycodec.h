#include "pepsy.h"
#include "psap.h"

int enc_f (
	const int typ,
	modtyp *mod,
	PE *pe,
	const int explicit,
	const int len,
	const char *buf,
	char *parm
);

int dec_f (
	const int typ,
	modtyp *mod,
	PE pe,
	const int explicit,
	const int *len,
	char **buf,
	char **parm
);

void vpushquipu(PS ps);
void vpopquipu(void);
