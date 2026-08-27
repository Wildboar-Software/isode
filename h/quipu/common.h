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

int sub_string (const short sntx);
int telephone_match (const short sntx);
int case_exact_match (const short sntx);
int approx_string (const short sntx);
int check_3166 (char *a);
void soundex (const char *s, char **c);
int	dn_in_dnseq(DN dn, const struct dn_seq *dnseq);
int check_guard (
	const char *pwd,
	const int pwd_len,
	const char *salt,
	const char *hval,
	const int hlen
);

#endif
