/* quipu_sntx.c - invoke quipu syntax handlers */

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
