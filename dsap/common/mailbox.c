/* mailbox.c - otherMailbox attribute */

/*
	SYNTAX:
		mailbox ::= <printablestring> '$' <IA5String>

	EXAMPLE:
		internet $ quipu-support@cs.ucl.ac.uk
*/
#include <string.h>
#include "quipu/util.h"
#include "quipu/entry.h"
#include "quipu/syntaxes.h"

static void mailbox_free (void *value) {
	struct mailbox *ptr = (struct mailbox *) value;

	free (ptr->mbox);
	free (ptr->mtype);
	free ((char *) ptr);
}

static void *mailbox_cpy (void *value) {
	struct mailbox *a = (struct mailbox *) value;
	struct mailbox * result;

	result = (struct mailbox *) smalloc (sizeof (struct mailbox));
	result->mbox = strdup (a->mbox);
	result->mtype = strdup (a->mtype);
	return (result);
}

static int mailbox_cmp (void *value1, void *value2) {
	struct mailbox *a = (struct mailbox *) value1;
	struct mailbox *b = (struct mailbox *) value2;
	int res;

	if (a == (struct mailbox *) NULL)
		if (b == (struct mailbox *) NULL)
			return (0);
		else
			return (-1);
	if ( (res = lexequ(a->mbox,b->mbox)) != 0)
		return (res);
	if ( (res = lexequ(a->mtype,b->mtype)) != 0)
		return (res);
	return (0);
}

static void mailbox_print (PS ps, void *value, int format) {
	struct mailbox* mail = (struct mailbox *) value;

	if (format == READOUT)
		ps_printf (ps,"%s: %s",mail->mtype, mail->mbox);
	else
		ps_printf (ps,"%s $ %s",mail->mtype, mail->mbox);
}

static void *str2mailbox (char *str) {
	struct mailbox * result;
	char * ptr;
	char * mark = NULLCP;

	if ( (ptr=index (str,'$')) == NULLCP) {
		parse_error ("seperator missing in mailbox '%s'",str);
		return ((struct mailbox *) NULL);
	}
	result = (struct mailbox *) smalloc (sizeof (struct mailbox));
	*ptr--= 0;
	if (isspace (*ptr)) {
		*ptr = 0;
		mark = ptr;
	}
	ptr++;
	result->mtype = strdup (str);
	*ptr++ = '$';
	result->mbox = strdup (SkipSpace(ptr));
	if (mark != NULLCP)
		*mark = ' ';
	return (result);
}

static PE mail_enc (void *value) {
	struct mailbox * m = (struct mailbox *) value;
	PE ret_pe;

	encode_Thorn_MailBox (&ret_pe,0,0,NULLCP,m);
	return (ret_pe);
}

static void * mail_dec (PE pe) {
	struct mailbox * m;

	if (decode_Thorn_MailBox (pe,1,NULL,NULLVP,&m) == NOTOK) {
		return ((struct mailbox *) NULL);
	}
	return (m);
}

void mailbox_syntax (void) {
	add_attribute_syntax ("Mailbox",
						  mail_enc,		mail_dec,
						  str2mailbox,	mailbox_print,
						  mailbox_cpy,	mailbox_cmp,
						  mailbox_free,		NULLCP,
						  NULL,			TRUE);
}
