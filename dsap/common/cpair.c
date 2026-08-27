/* cpair.c - CertificatePair attribute syntax */

#include "quipu/util.h"
#include "quipu/authen.h"
#include "quipu/syntaxes.h"
#include "pepsycodec.h"
static PE cpair_enc(void *value);
static void *cpair_dec(PE pe);
static void *str2cpair (char *str);
static void printcpair(PS ps, void *value, int format);
struct certificate_list *cpair_cpy (const struct certificate_list *parm);
static void *cpair_cpy_void (void *value);
static int cpair_cmp (void *value1, void *value2);
void certificate_pair_syntax (void);


/* We import these functions, which ought to be internal to certificate.c */
struct certificate *cert_cpy(struct certificate *parm);
struct certificate *str2cert(char *str);

static PE cpair_enc(void *value) {
	PE pe;
	struct certificate_list *parm = (struct certificate_list *) value;
	encode_AF_CertificatePair(&pe, 0, 0, NULLCP, parm);
	return (pe);
}

static void *cpair_dec(PE pe) {
	struct certificate_list *result;

	if (decode_AF_CertificatePair(pe, 0, NULL, NULLVP, &result) == NOTOK)
		return (struct certificate_list *) 0;
	return (result);
}

static void *str2cpair (char *str) {
	struct certificate_list *result;
	char *ptr;

	result = (struct certificate_list *) calloc(1, sizeof(*result));
	if (result == (struct certificate_list *) 0)
		return (result);
	/* If there isn't a '|', the pair is technically illegal. However,
	 * allow this case to mean : "the string contains a certificate,
	 * which is the forward cross certificate".
	 */
	ptr = index(str, '|');
	if (ptr != NULLCP) {
		*ptr = '\0';
		ptr++;
	}
	/* Need to cook up a quick test for whether a string contains a certificate
	 * or whitespace. A certificate will always contain a '#', so use this.
	 */
	if (index(str, '#') != NULLCP) {
		result->cert = str2cert(str);
		if (result->cert == (struct certificate *) 0) {
			cpair_free(result);
			return ((struct certificate_list *) 0);
		}
	}
	str = ptr;
	if ((str != NULLCP) && (index(str, '#') != NULLCP)) {
		result->reverse = str2cert(str);
		if (result->reverse == (struct certificate *) 0) {
			cpair_free(result);
			return ((struct certificate_list *) 0);
		}
	}
	return (result);
}

void printcpair(PS ps, void *value, int format) {
	struct certificate_list *parm = (struct certificate_list *) value;
	if (parm->cert)
		printcert(ps, parm->cert, format);
	ps_printf(ps, "|\\\n");
	if (parm->reverse)
		printcert(ps, parm->reverse, format);
}

struct certificate_list *cpair_cpy (const struct certificate_list *parm) {
	struct certificate_list *result;

	result = (struct certificate_list *) calloc(1, sizeof(*result));
	if (result == (struct certificate_list *) 0)
		return (result);
	if (parm->cert)
		result->cert = cert_cpy(parm->cert);
	if (parm->reverse)
		result->reverse = cert_cpy(parm->reverse);
	return (result);
}

void *cpair_cpy_void (void *value) {
	return cpair_cpy ((struct certificate_list *) value);
}

static int cpair_cmp (void *value1, void *value2) {
	struct certificate_list *a = (struct certificate_list *) value1;
	struct certificate_list *b = (struct certificate_list *) value2;
	int retval;

	if (a->cert == (struct certificate *) 0) {
		if (b->cert == (struct certificate *) 0)
			retval = 0;
		else
			retval = 1;
	} else {
		if (b->cert == (struct certificate *) 0)
			retval = -1;
		else
			retval = cert_cmp(a->cert, b->cert);
	}
	if (retval != 0)
		return (retval);
	if (a->reverse == (struct certificate *) 0) {
		if (b->reverse == (struct certificate *) 0)
			retval = 0;
		else
			retval = 1;
	} else {
		if (b->reverse == (struct certificate *) 0)
			retval = -1;
		else
			retval = cert_cmp(a->reverse, b->reverse);
	}
	return (retval);
}

void cpair_free (void *value) {
	struct certificate_list *parm = (struct certificate_list *) value;
	if (parm->cert)
		cert_free(parm->cert);
	if (parm->reverse)
		cert_free(parm->reverse);
	free((char *) parm);
}

void certificate_pair_syntax (void) {
	add_attribute_syntax(
		"CertificatePair",
		cpair_enc,	cpair_dec,
		str2cpair,	printcpair,
		cpair_cpy_void,	cpair_cmp,
		cpair_free,	NULLCP,
		NULL,		TRUE);
}
