/* protected.c - ProtectedPassword attribute syntax */
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "logger.h"
#include "quipu/util.h"
#include "quipu/attr.h"
#include "quipu/authen.h"
#include "quipu/syntaxes.h"
#include "pepsycodec.h"
#include "quipu/common.h"

static PE prot_enc (void *value);
static void * prot_dec (PE pe);
static void *
str2prot (char *str);
static void prot_print (PS ps, void *value, int format);
char *hash2str (unsigned long hash, int *len);
unsigned long hash_passwd (unsigned long seed, const char *str, int len);
static int prot_cmp (void *value1, void *value2);
static void *
prot_cpy (void *value);
static void prot_free (void *value);
void protected_password_syntax (void);


extern LLog *log_dsap;
extern char dsa_mode;
char *cryptparse(char *str);

static PE prot_enc (void *value) {
	struct protected_password *x = (struct protected_password *) value;
	PE result = NULLPE;

	encode_Quipu_ProtectedPassword (&result, 0, 0, NULLCP, x);
	return (result);
}

static void * prot_dec (PE pe) {
	struct protected_password *result;

	if (decode_Quipu_ProtectedPassword (pe, 0, NULL, NULLVP, &result)
			== NOTOK)
		return ((struct protected_password *) 0);
	return (result);
}

static void *
str2prot (char *str) {
	struct protected_password *result;
	char *octparse(char *str);

	result = (struct protected_password *)
			 calloc(1, sizeof(*result));
	if (result == (struct protected_password *) 0)
		return (result);
	/* Using strlen means can't have zeros in the password */
	result->passwd = cryptparse(str);
	if (strlen2int (result->passwd, &result->n_octets) != 0) {
		free ((char *) result);
		return NULL;
	}
	result->is_protected[0] = '\0';
	result->time1 = NULLCP;
	result->time2 = NULLCP;
	result->random1 = (struct random_number *) 0;
	result->random2 = (struct random_number *) 0;
	return (result);
}

static void prot_print (PS ps, void *value, int format) {
	struct protected_password *parm = (struct protected_password *) value;
	char *cp;
	extern char * cryptstring(char *str);

	/* Make a null-terminated copy */
	{
		int n = parm->n_octets;

		if (add_int_to_int (&n, 1) != 0)
			return;
		cp = malloc_int (n);
	}
	if (cp == NULL)
		return;
	if (bcopy_int(parm->passwd, cp, parm->n_octets) != 0) {
		free (cp);
		return;
	}
	cp[parm->n_octets] = '\0';
	if (dsa_mode == FALSE) {
		if (format == READOUT)
			ps_print (ps,"Read but not displayed");
		else
			octprint(ps, cp, format);
	} else {
		ps_print(ps, "{CRYPT}");
		octprint(ps, cryptstring(cp), format);
	}
	free(cp);
}

/* Portable conversion from OCTET STRING to whatever structure is
 * used to hold a hash. This is currently an unsigned long, which limits the
 * length of a hash.
 */

/* The reverse operation. Currently, hashes are always 4 octets long. */

char *hash2str (unsigned long hash, int *len) {
	char *result;
	int i;

	result = malloc(5);
	if (result == NULLCP)
		return (result);
	for (i=0; i<4; i++) {
		result[i] = (char) (hash & 255);
		hash = hash >> 8;
	}
	*len = 4;
	return (result);
}

/* insecure hash function for testing purposes */

unsigned long
hash_passwd (unsigned long seed, const char *str, int len) {
	seed = 0;
	DLOG(log_dsap, LLOG_DEBUG, ("Hash = %D", seed));
	return (seed);
}

int check_guard (
	const char *pwd, /* This string is not null-terminated */
	const int pwd_len,
	const char *salt, /* Null-terminated salt */
	const char *hval, /* This string is not null-terminated */
	const int hlen
) {
	return (2);
}

static int prot_cmp (void *value1, void *value2) {
	struct protected_password *a = (struct protected_password *) value1;
	struct protected_password *b = (struct protected_password *) value2;
	int retval;

	if (a->is_protected[0] == (char) 0) {
		if (b->is_protected[0] == (char) 0) {
			/* Both are unencrypted. Do a direct compare. */
			if (a->n_octets != b->n_octets)
				retval = 2;
			else
				retval = (strncmp_int(a->passwd, b->passwd, a->n_octets) == 0)? 0:2;
		} else
			retval = check_guard(a->passwd, a->n_octets, b->time1, b->passwd, b->n_octets);
	} else {
		if (b->is_protected[0] == (char) 0)
			retval = check_guard(b->passwd, b->n_octets, a->time1, a->passwd, a->n_octets);
		else {
			/* Both are encrypted.
			 * This case does not occur with sane usage of this syntax.
			 * However, we have to handle it in case a DUA tries it.
			 * To preserve semantics of `equals', should check whether a & b
			 * are both guarded versions of the same thing, BUT the encryption
			 * mechanism prevents us doing this check.
			 *
			 * To make evrything mathematically correct, should re-write it
			 * to use '>=' rather than '='. Unfortunately, can't check '>='
			 * with a directory COMPARE operation ...
			 */
			if (a->n_octets != b->n_octets)
				retval = 2;
			else
				retval = (strncmp_int(a->passwd, b->passwd, a->n_octets) == 0)? 0:2;
		}
	}
	return (retval);
}

static void *
prot_cpy (void *value) {
	struct protected_password *parm = (struct protected_password *) value;
	struct protected_password *result;

	result = (struct protected_password *)
			 calloc(1, sizeof(*result));
	result->passwd = malloc_int(parm->n_octets);
	if (result->passwd == NULLCP)
		return ((struct protected_password *) 0);
	if (bcopy_int(parm->passwd, result->passwd, parm->n_octets) != 0) {
		free (result->passwd);
		free ((char *) result);
		return ((struct protected_password *) 0);
	}
	result->n_octets = parm->n_octets;
	if (parm->time1 == NULLCP)
		result->time1 = NULLCP;
	else
		result->time1 = strdup(parm->time1);
	if (parm->time2 == NULLCP)
		result->time2 = NULLCP;
	else
		result->time2 = strdup(parm->time2);
	result->random1 = (struct random_number *) 0;
	result->random2 = (struct random_number *) 0;
	result->is_protected[0] = parm->is_protected[0];
	return (result);
}

static void prot_free (void *value) {
	struct protected_password *parm = (struct protected_password *) value;

	if (parm->passwd != NULLCP)
		free(parm->passwd);
	if (parm->time1 != NULLCP)
		free(parm->time1);
	if (parm->time2 != NULLCP)
		free(parm->time2);
	free((char *) parm);
}

void protected_password_syntax (void) {
	add_attribute_syntax ("ProtectedPassword",
						  prot_enc,	prot_dec,
						  str2prot,	prot_print,
						  prot_cpy,	prot_cmp,
						  prot_free,	NULLCP,
						  NULL,		FALSE);
}
