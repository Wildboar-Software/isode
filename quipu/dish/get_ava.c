/* get_ava.c - */

#include "quipu/util.h"
#include "quipu/commonarg.h"

#define	OPT	(!frompipe || rps -> ps_byteno == 0 ? opt : rps)
#define	RPS	(!frompipe || opt -> ps_byteno == 0 ? rps : opt)
extern	char	frompipe;
extern	PS	opt, rps;

int get_ava (AVA *avasert, char *type, char *value) {
	char           *TidyString ();

	if (type == NULLCP || *type == 0)
		return (NOTOK);
	avasert->ava_type = AttrT_new (TidyString (type));
	if (avasert->ava_type == NULLAttrT) {
		ps_printf (OPT, "Invalid at %s\n", type);
		return (NOTOK);
	}
	if ((avasert->ava_value = str_at2AttrV (TidyString (value), avasert->ava_type)) == NULLAttrV) {
		ps_print (OPT, "Invalid attribute value value\n");
		AttrT_free (avasert->ava_type);
		return (NOTOK);
	}
	return (OK);
}
