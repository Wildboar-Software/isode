/* quipu_sntx.c - invoke quipu syntax handlers */

extern void standard_syntaxes (void);
extern void acl_syntax (void);
extern void edbinfo_syntax (void);
extern void schema_syntax (void);
extern void photo_syntax (void);
extern void protected_password_syntax(void);
extern void inherit_syntax (void);
extern void audio_syntax (void);
extern void ap_syntax (void);
extern void attribute_syntax (void);
extern void mailbox_syntax (void);
extern void documentStore_syntax (void);
extern void QoS_syntax (void);
extern void sacl_syntax(void);
extern void lacl_syntax(void);
extern void authp_syntax(void);
extern void dsa_control_syntax(void);
extern void quipu_call_syntax(void);

void quipu_syntaxes (void) {
	static char done = 0;

	if (done++)
		return;
	standard_syntaxes ();
	acl_syntax ();
	edbinfo_syntax ();
	schema_syntax ();
	photo_syntax ();
	protected_password_syntax();
	inherit_syntax ();
	audio_syntax ();
	ap_syntax ();
	attribute_syntax ();
	/* Thorn syntaxes */
	mailbox_syntax ();
	documentStore_syntax ();
	QoS_syntax ();
	sacl_syntax();
	lacl_syntax();
	authp_syntax();
	/* DSA control */
	dsa_control_syntax();
	quipu_call_syntax();
}
