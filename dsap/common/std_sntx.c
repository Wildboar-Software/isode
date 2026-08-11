/* std_sntx.c - invoke standard syntax handlers */

int standard_syntaxes (void) {
	string_syntaxes ();
	cilist_syntax ();
	dn_syntax ();
	psap_syntax ();
	objectclass_syntax ();
	oid_syntax ();
	time_syntax ();
	boolean_syntax ();
	integer_syntax ();
	fax_syntax ();
	post_syntax ();
	telex_syntax ();
	teletex_syntax ();
	pref_deliv_syntax ();
	guide_syntax ();
	certificate_syntax ();
	certificate_pair_syntax ();
}
