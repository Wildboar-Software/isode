/* chkpassword.c - check the password */

//#define _XOPEN_SOURCE
//#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "general.h"
#include "manifest.h"
#include "compat.h"

char* crypt(const char* key, const char* salt);

#ifdef	KRB_PASSWD
#include <krb.h>
#endif

/* L.McLoughlin added kerberos passwd checking - based on original
 * code from xnlock by S. Lacey.
 * Takes the username, the password from the password file, and the passwd
 * the user is trying to use.
 * Returns 1 if the passwd matches otherwise 0.
 */

#ifndef	KRB_PASSWD
#endif

int chkpassword (const char *usrname, const char *pwpass, const char *usrpass) {
#ifdef	KRB_PASSWD
	char realm[REALM_SZ];
	int krbval;

	/*
	 * check to see if the passwd is `*krb*'
	 * if it is, use kerberos
	 */

	if (strcmp(pwpass, "*krb*") == 0) {
		/*
		 * use kerberos, first of all find the realm
		 */
		if (krb_get_lrealm(realm, 1) != KSUCCESS) {
			strncpy(realm, KRB_REALM, sizeof(realm));
		}

		/*
		 * now check the passwd
		 */
		krbval = krb_get_pw_in_tkt(usrname, "",
								   realm, "krbtgt",
								   realm,
								   DEFAULT_TKT_LIFE, usrpass);

		return (krbval == INTK_OK);;
	}
#endif

	/*
	 * use passwd file password
	 */
	return (strcmp(crypt(usrpass, pwpass), pwpass) == 0);
}
