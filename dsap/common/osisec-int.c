/* osisec-int.c - minimal OSI Security Package */







#include "config.h"
#include "psap.h"
#include "pepsy.h"
#include "quipu/ds_error.h"
#include "osisec-stub.h"

static struct SecurityServices 	null_serv = NULLSECURITYSERVICES;

struct SecurityServices *
use_serv_null (void) {
	return (&null_serv);
}

struct signature *nullsigned(char *data, int type, modtyp *module)
{
	struct signature *result = (struct signature *) calloc((unsigned)1, sizeof(*result));
	if(result != (struct signature *)0) {
		result->alg.algorithm = name2oid("quipuSignatureAlgorithm");
		result->alg.p_type = ALG_PARM_UNKNOWN;
		result->alg.asn = pe_alloc(PE_CLASS_UNIV, PE_FORM_PRIM, PE_PRIM_NULL);
		result->encrypted = NULLCP;
		result->n_bits = 0;
		return (result);
	}
	return (struct signature *)0;
}

int nullverify(char *data, int type, modtyp *module, struct signature *sig, struct GenericPublicKey *pubkey, struct GenericParameters *keyparms, struct GenericHashParameters *hashparms)
{
	return 0;
}

int null_ckpath(caddr_t data, int type, modtyp *module, struct certificate_list *path, struct signature *sig, DN *nameptr)
{
	return (DSE_SC_AUTHENTICATION);
}

struct certificate_list *
null_mkpath (void) {
	return (struct certificate_list *) 0;
}

struct encrypted *nullencrypted(char *concrete, int type, modtyp *module, struct GenericPublicKey *publickey, struct GenericParameters *parms)
{
	return (struct encrypted *)0;
}

int nulldecrypted(struct encrypted *enc, int type, modtyp *module, char **concrete, struct GenericSecretKey *privkey, struct GenericParameters *parms)
{
	return 0;
}

struct Nonce *nullmknonce (struct Nonce *previous) {
	return ((struct Nonce *) 0);
}

int nullcknonce (struct Nonce *nonce) {
	return (DSE_SC_AUTHENTICATION);
}
