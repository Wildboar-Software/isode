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

int sub_string (short sntx);
int telephone_match (short sntx);
int case_exact_match (short sntx);
int approx_string (short sntx);
int check_3166 (char *a);
void soundex (char *s, char **c);
int	dn_in_dnseq(DN dn, struct dn_seq *dnseq);
int check_guard (
	char *pwd,
	int pwd_len,
	char *salt,
	char *hval,
	int hlen
);

#endif
