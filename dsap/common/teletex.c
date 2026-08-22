/* teletex.c - Teletex attribute */

/*
	SYNTAX:
		teletex ::= <printablestring> '$' <printablestring>
				'$' <printablestring>...

	REPRESENTING:
		terminal $ graphic $ control $ page $ misc $ private
*/
#include <string.h>
#include "quipu/util.h"
#include "quipu/entry.h"
#include "quipu/syntaxes.h"
#include "pepsycodec.h"
static void teletex_free (void *value);
static char *xstrdup (char *a);
static void *teletex_cpy (void *value);
static int teletex_cmp (void *value1, void *value2);
static void teletex_print (PS ps, void *value, int format);
static void *str2teletex (char *str);
static PE teletex_enc (void *value);
static void * teletex_dec (PE pe);
void teletex_syntax (void);


#define nfree(x) if (x != NULLCP) free (x)

extern LLog * log_dsap;

static void teletex_free (void *value) {
	struct teletex *ptr = (struct teletex *) value;

	nfree (ptr->terminal);
	nfree (ptr->graphic);
	nfree (ptr->control);
	nfree (ptr->page);
	nfree (ptr->misc);
	nfree (ptr->t_private);
	free ((char *) ptr);
}

static char *xstrdup (char *a) {
	if (( a == NULLCP) || (*a == NULL))
		return (NULLCP);
	else
		return (strdup (a));
}

static void *teletex_cpy (void *value) {
	struct teletex *a = (struct teletex *) value;
	struct teletex * result;

	result = (struct teletex *) smalloc (sizeof (struct teletex));
	result->terminal = strdup (a->terminal);
	result->graphic  = xstrdup (a->graphic);
	result->control  = xstrdup (a->control);
	result->page     = xstrdup (a->page);
	result->misc     = xstrdup (a->misc);
	result->t_private  = xstrdup (a->t_private);
	return (result);
}

static int teletex_cmp (void *value1, void *value2) {
	struct teletex *a = (struct teletex *) value1;
	struct teletex *b = (struct teletex *) value2;
	int res;

	if (a == (struct teletex *) NULL)
		if (b == (struct teletex *) NULL)
			return (0);
		else
			return (-1);
	if ( (res = lexequ(a->terminal,b->terminal)) != 0)
		return (res);
	if ( (res = lexequ(a->graphic,b->graphic)) != 0)
		return (res);
	if ( (res = lexequ(a->control,b->control)) != 0)
		return (res);
	if ( (res = lexequ(a->page,b->page)) != 0)
		return (res);
	if ( (res = lexequ(a->misc,b->misc)) != 0)
		return (res);
	if ( (res = lexequ(a->t_private,b->t_private)) != 0)
		return (res);
	return (0);
}

static void teletex_print (PS ps, void *value, int format) {
	struct teletex* teletex = (struct teletex *) value;

	if (format == READOUT)
		ps_print (ps,"terminal: ");
	ps_print (ps,teletex->terminal);
	if (format != READOUT)
		ps_print (ps," $ ");
	if (teletex->graphic != NULLCP) {
		if (format == READOUT)
			ps_print (ps,", graphic: ");
		ps_print (ps,teletex->graphic);
	}
	if (format != READOUT)
		ps_print (ps," $ ");
	if (teletex->control != NULLCP) {
		if (format == READOUT)
			ps_print (ps,", control: ");
		ps_print (ps,teletex->control);
	}
	if (format != READOUT)
		ps_print (ps," $ ");
	if (teletex->page != NULLCP) {
		if (format == READOUT)
			ps_print (ps,", page: ");
		ps_print (ps,teletex->page);
	}
	if (format != READOUT)
		ps_print (ps," $ ");
	if (teletex->misc != NULLCP) {
		if (format == READOUT)
			ps_print (ps,", misc: ");
		ps_print (ps,teletex->misc);
	}
	if (format != READOUT)
		ps_print (ps," $ ");
	if (teletex->t_private != NULLCP) {
		if (format == READOUT)
			ps_print (ps,", private: ");
		ps_print (ps,teletex->t_private);
	}
}

static void *str2teletex (char *str) {
	struct teletex * result;
	char * ptr;
	char * mark = NULLCP;
	char * prtparse (char *str);

	if ( (ptr=index (str,'$')) == NULLCP) {
		parse_error ("seperator missing in teletex '%s'",str);
		return ((struct teletex *) NULL);
	}
	result = (struct teletex *) smalloc (sizeof (struct teletex));
	*ptr--= 0;
	if (isspace (*ptr)) {
		*ptr = 0;
		mark = ptr;
	}
	ptr++;
	if ((result->terminal = prtparse(str)) == NULLCP)
		return ((struct teletex *) NULL);
	if ((int)strlen (result->terminal) > UB_TELETEX_TERMINAL_ID) {
		parse_error ("teletex string too long",NULLCP);
		return ((struct teletex *) NULL);
	}
	*ptr++ = '$';
	if (mark != NULLCP)
		*mark = ' ';
	str = SkipSpace(ptr);
	if ( (ptr=index (str,'$')) == NULLCP) {
		parse_error ("2nd seperator missing in teletex '%s'",str);
		return ((struct teletex *) NULL);
	}
	*ptr--= 0;
	if (isspace (*ptr)) {
		*ptr = 0;
		mark = ptr;
	} else
		mark = NULLCP;
	ptr++;
	result->graphic = xstrdup(str);
	*ptr++ = '$';
	if (mark != NULLCP)
		*mark = ' ';
	str = SkipSpace(ptr);
	if ( (ptr=index (str,'$')) == NULLCP) {
		parse_error ("3rd seperator missing in teletex '%s'",str);
		return ((struct teletex *) NULL);
	}
	*ptr--= 0;
	if (isspace (*ptr)) {
		*ptr = 0;
		mark = ptr;
	} else
		mark = NULLCP;
	ptr++;
	result->control = xstrdup(str);
	*ptr++ = '$';
	if (mark != NULLCP)
		*mark = ' ';
	str = SkipSpace(ptr);
	if ( (ptr=index (str,'$')) == NULLCP) {
		parse_error ("4th seperator missing in teletex '%s'",str);
		return ((struct teletex *) NULL);
	}
	*ptr--= 0;
	if (isspace (*ptr)) {
		*ptr = 0;
		mark = ptr;
	} else
		mark = NULLCP;
	ptr++;
	result->page = xstrdup(str);
	*ptr++ = '$';
	if (mark != NULLCP)
		*mark = ' ';
	str = SkipSpace(ptr);
	if ( (ptr=index (str,'$')) == NULLCP) {
		parse_error ("5th seperator missing in teletex '%s'",str);
		return ((struct teletex *) NULL);
	}
	*ptr--= 0;
	if (isspace (*ptr)) {
		*ptr = 0;
		mark = ptr;
	} else
		mark = NULLCP;
	ptr++;
	result->misc = xstrdup(str);
	*ptr++ = '$';
	if (mark != NULLCP)
		*mark = ' ';
	result->t_private = xstrdup(SkipSpace(ptr));
	return (result);
}

static PE teletex_enc (void *value) {
	struct teletex * m = (struct teletex *) value;
	PE ret_pe;

	encode_SA_TeletexTerminalIdentifier (&ret_pe,0,0,NULLCP,m);
	return (ret_pe);
}

static void * teletex_dec (PE pe) {
	struct teletex * m;

	if (decode_SA_TeletexTerminalIdentifier (pe,1,NULL,NULLVP,&m) == NOTOK) {
		return ((struct teletex *) NULL);
	}
	if ((int)strlen (m->terminal) > UB_TELETEX_TERMINAL_ID) {
		LLOG(log_dsap,LLOG_EXCEPTIONS,("teletex string too big"));
		return ((struct teletex *) NULL);
	}
	return (m);
}

void teletex_syntax (void) {
	add_attribute_syntax ("TeletexTerminalIdentifier",
						  teletex_enc,		teletex_dec,
						  str2teletex,		teletex_print,
						  teletex_cpy,		teletex_cmp,
						  teletex_free,		NULLCP,
						  NULL,			TRUE);
}
