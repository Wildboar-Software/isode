/* std_sntx.c - invoke standard syntax handlers */
extern void string_syntaxes (void);
extern void cilist_syntax (void);
extern void dn_syntax (void);
extern void psap_syntax (void);
extern void objectclass_syntax (void);
extern void oid_syntax (void);
extern void time_syntax (void);
extern void boolean_syntax (void);
extern void integer_syntax (void);
extern void fax_syntax (void);
extern void post_syntax (void);
extern void telex_syntax (void);
extern void teletex_syntax (void);
extern void pref_deliv_syntax (void);
extern void guide_syntax (void);
extern void certificate_syntax (void);
extern void certificate_pair_syntax (void);

void standard_syntaxes (void);

void standard_syntaxes (void) {
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
