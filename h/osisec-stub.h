/* osisec-stub.h - minimal OSI Security Package Interface Definitions */

#ifndef SECURITYSERVICES
#define SECURITYSERVICES

/* x509-concrete.h -  concrete structures for X.509 protocol elements */

#ifndef _X509_CONCRETE_
#define _X509_CONCRETE_

#ifdef OSISEC
#include "osisec/alg_rsa.h"
#include "osisec/alg_md2.h"
#include "osisec/alg_md4.h"
#endif

#include "quipu/authen.h"

/* NEW for OSISEC encryption service */
struct encrypted {
	struct alg_id alg;
	int n_bits;
	char *encrypted;
};

/* NEW for OSISEC bind token service */
struct Nonce {
	char *non_time1;
	char *non_time2;
	struct random_number non_r1;
	struct random_number non_r2;
	struct alg_id non_alg;
};

#endif

#ifndef GENERICALGTYPES
#define GENERICALGTYPES

struct GenericPublicKey;
struct GenericSecretKey;
struct GenericParameters;

struct GenericHash;
struct GenericHashParameters;
struct GenericHashKey;
#endif

struct SecurityServices {
	char           			*serv_name;
	struct signature *	 	(*serv_sign)(char *data, int type, modtyp *module);
	int				(*serv_verify)(char *data, int type, modtyp *module, struct signature *sig, struct GenericPublicKey *pubkey, struct GenericParameters *keyparms, struct GenericHashParameters *hashparms);
	int				(*serv_ckpath)(caddr_t data, int type, modtyp *module, struct certificate_list *path, struct signature *sig, DN *nameptr);
	int				(*serv_ckfpath)(caddr_t data, int type, modtyp *module, struct certificate_list *path, struct signature *sig, DN *nameptr);
	struct certificate_list *	(*serv_mkpath)(void);
	struct encrypted * 		(*serv_encrypt)(char *concrete, int type, modtyp *module, struct GenericPublicKey *publickey, struct GenericParameters *parms);
	int				(*serv_decrypt)(struct encrypted *enc, int type, modtyp *module, char **concrete, struct GenericSecretKey *privkey, struct GenericParameters *parms);
	struct Nonce *			(*serv_mknonce)(struct Nonce *previous);
	int				(*serv_cknonce)(struct Nonce *nonce);
};

extern struct signature        *nullsigned(char *data, int type, modtyp *module);
extern int      		nullverify(char *data, int type, modtyp *module, struct signature *sig, struct GenericPublicKey *pubkey, struct GenericParameters *keyparms, struct GenericHashParameters *hashparms);
extern struct certificate_list *null_mkpath(void);
extern int			null_ckpath(caddr_t data, int type, modtyp *module, struct certificate_list *path, struct signature *sig, DN *nameptr);
extern struct encrypted	       *nullencrypted(char *concrete, int type, modtyp *module, struct GenericPublicKey *publickey, struct GenericParameters *parms);
extern int      		nulldecrypted(struct encrypted *enc, int type, modtyp *module, char **concrete, struct GenericSecretKey *privkey, struct GenericParameters *parms);
extern struct Nonce *		nullmknonce(struct Nonce *previous);
extern int			nullcknonce(struct Nonce *nonce);

#define NULLSECURITYSERVICES { \
	"quipusecurityservices", \
	nullsigned, \
	nullverify, \
	null_ckpath, \
	null_ckpath, \
	null_mkpath, \
	nullencrypted, \
	nulldecrypted, \
	nullmknonce, \
	nullcknonce \
}

extern struct SecurityServices	*use_serv_null(void);
#define SECSERV sec_serv
extern struct SecurityServices *SECSERV;

#define SIGNED_MACRO(serv,a,b,c)	(serv->serv_sign)((a),(b),(c))
#define VERIFY_MACRO(serv,a,b,c,d,e,f,g) (serv->serv_verify)((a),(b),(c),(d),(e),(f),(g))

#define CHECKPATH(serv,a,b,c,d,e,f) 	(serv->serv_ckpath)((a),(b),(c),(d),(e),(f))
#define CHECKFPATH(serv,a,b,c,d,e,f) 	(serv->serv_ckfpath)((a),(b),(c),(d),(e),(f))
#define MAKEPATH(serv) 			(serv->serv_mkpath)()

#define ENCRYPTED_MACRO(serv,a,b,c,d,e)	(serv->serv_encrypt)((a),(b),(c),(d),(e))
#define DECRYPTED_MACRO(serv,a,b,c,d,e,f) (serv->serv_decrypt)((a),(b),(c),(d),(e),(f))

#define MAKE_NONCE(serv,a)	(serv->serv_mknonce)((a))
#define CHECK_NONCE(serv,a) (serv->serv_cknonce)((a))
#endif
