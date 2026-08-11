/* common.h - */

#ifndef COMH
#define COMH

#include "quipu/attrvalue.h"
#include "quipu/dsp.h"
#include "quipu/commonarg.h"
#include "quipu/bind.h"
#include "quipu/read.h"
#include "quipu/compare.h"
#include "quipu/abandon.h"
#include "quipu/list.h"
#include "quipu/ds_search.h"
#include "quipu/add.h"
#include "quipu/modify.h"
#include "quipu/modifyrdn.h"
#include "quipu/remove.h"
#include "quipu/ds_error.h"

typedef struct simple_creds {
	DN      usc_dn;
	int     usc_passwd_len;
	char    *usc_passwd;
	char    *usc_time1;
	char    *usc_time2;
} USC;

#endif
