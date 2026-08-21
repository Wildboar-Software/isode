/* telex.c - Telex attribute */

/*
	SYNTAX:
		telex ::= <printablestring> '$' <printablestring>
				'$' <printablestring>

	REPRESENTING
		number $ country $ answerback
*/

#include <string.h>
#include "quipu/util.h"
#include "quipu/entry.h"
#include "quipu/syntaxes.h"
#include "pepsycodec.h"

extern LLog * log_dsap;

static void telex_free (void *value) {
	struct telex *ptr = (struct telex *) value;

	free (ptr->telexnumber);
	free (ptr->countrycode);
	free (ptr->answerback);
	free ((char *) ptr);
}

static void *telex_cpy (void *value) {
	struct telex *a = (struct telex *) value;
	struct telex * result;

	result = (struct telex *) smalloc (sizeof (struct telex));
	result->telexnumber = strdup (a->telexnumber);
	result->countrycode = strdup (a->countrycode);
	result->answerback  = strdup (a->answerback);
	return (result);
}

static int telex_cmp (void *value1, void *value2) {
	struct telex *a = (struct telex *) value1;
	struct telex *b = (struct telex *) value2;
	int res;

	if (a == (struct telex *) NULL)
		if (b == (struct telex *) NULL)
			return (0);
		else
			return (-1);
	if ( (res = lexequ(a->telexnumber,b->telexnumber)) != 0)
		return (res);
	if ( (res = lexequ(a->countrycode,b->countrycode)) != 0)
		return (res);
	if ( (res = lexequ(a->answerback,b->answerback)) != 0)
		return (res);
	return (0);
}

static void telex_print (PS ps, void *value, int format) {
	struct telex* telex = (struct telex *) value;

	if (format == READOUT)
		ps_printf (ps,"number: %s, country: %s, answerback: %s",telex->telexnumber, telex->countrycode, telex->answerback);
	else
		ps_printf (ps,"%s $ %s $ %s",telex->telexnumber, telex->countrycode, telex->answerback);
}

static void *str2telex (char *str) {
	struct telex * result;
	char * ptr;
	char * mark = NULLCP;
	char * prtparse (char *str);

	if ( (ptr=index (str,'$')) == NULLCP) {
		parse_error ("seperator missing in telex '%s'",str);
		return ((struct telex *) NULL);
	}
	result = (struct telex *) smalloc (sizeof (struct telex));
	*ptr--= 0;
	if (isspace (*ptr)) {
		*ptr = 0;
		mark = ptr;
	}
	ptr++;
	if ((result->telexnumber = prtparse(str)) == NULLCP)
		return ((struct telex *) NULL);
	if ((int)strlen (result->telexnumber) > UB_TELEX_NUMBER) {
		parse_error ("telexnumber too big",NULLCP);
		return ((struct telex *) NULL);
	}
	*ptr++ = '$';
	if (mark != NULLCP)
		*mark = ' ';
	str = SkipSpace(ptr);
	if ( (ptr=index (str,'$')) == NULLCP) {
		parse_error ("2nd seperator missing in telex '%s'",str);
		return ((struct telex *) NULL);
	}
	*ptr--= 0;
	if (isspace (*ptr)) {
		*ptr = 0;
		mark = ptr;
	} else
		mark = NULLCP;
	ptr++;
	if ((result->countrycode = prtparse(str)) == NULLCP)
		return ((struct telex *) NULL);
	if ((int)strlen (result->countrycode) > UB_COUNTRY_CODE) {
		parse_error ("countrycode too big",NULLCP);
		return ((struct telex *) NULL);
	}
	*ptr++ = '$';
	if (mark != NULLCP)
		*mark = ' ';
	if ((result->answerback = prtparse(SkipSpace(ptr))) == NULLCP)
		return ((struct telex *) NULL);
	return (result);
}

static PE telex_enc (void *value) {
	struct telex * m = (struct telex *) value;
	PE ret_pe;

	encode_SA_TelexNumber (&ret_pe,0,0,NULLCP,m);
	return (ret_pe);
}

static void * telex_dec (PE pe) {
	struct telex * m;

	if (decode_SA_TelexNumber (pe,1,NULL,NULLVP,&m) == NOTOK) {
		return ((struct telex *) NULL);
	}
	if ((int)strlen (m->telexnumber) > UB_TELEX_NUMBER) {
		LLOG(log_dsap,LLOG_EXCEPTIONS,("telexnumber too big"));
		return ((struct telex *) NULL);
	}
	if ((int)strlen (m->countrycode) > UB_COUNTRY_CODE) {
		LLOG(log_dsap,LLOG_EXCEPTIONS,("telex country code too big"));
		return ((struct telex *) NULL);
	}
	if ((int)strlen (m->answerback) > UB_ANSWERBACK) {
		LLOG(log_dsap,LLOG_EXCEPTIONS,("telex answer back too big"));
		return ((struct telex *) NULL);
	}
	return (m);
}

void telex_syntax (void) {
	add_attribute_syntax ("TelexNumber",
						  telex_enc,		telex_dec,
						  str2telex,		telex_print,
						  telex_cpy,		telex_cmp,
						  telex_free,		NULLCP,
						  NULL,			TRUE);
}
