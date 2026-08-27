#include "pepsy.h"
#include "psap.h"

int enc_f (
	int typ,
	modtyp *mod,
	PE *pe,
	int explicit,
	int len,
	char *buf,
	char *parm
);

int dec_f (
	int typ,
	modtyp *mod,
	PE pe,
	int explicit,
	int *len,
	char **buf,
	char **parm
);

void vpushquipu(PS ps);
void vpopquipu(void);
