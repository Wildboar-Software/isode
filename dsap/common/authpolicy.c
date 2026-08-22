/* authpolicy.c - authentication policy routines */
#include <stdlib.h>
#include <strings.h>
#include "quipu/util.h"
#include "quipu/entry.h"
#include "quipu/syntaxes.h"
#include "pepsycodec.h"
static int authp_cmp( void *value1, void *value2 );
static void * authp_cpy( void *value );
static void * authp_decode( PE pe );
static PE authp_enc( void *value );
static int get_policy (char *str);
static void * str2authp( char *str );
static void authp_print (PS ps, void *value, int format);
void authp_syntax (void);


extern void sfree(char *x);

/*
 * These routines implement the AuthenticationPolicySyntax.  The EDB
 * representation of this syntax is as follows:
 *
 *  AuthenticationPolicySyntax ::= <modpolicy> '#' <readandcomparepolicy>
 *					'#' <listandsearchpolicy>
 *
 *  <modpolicy> ::= <authpolicy>
 *
 *  <readandcomparepolicy> ::= <authpolicy>
 *
 *  <listandsearchpolicy> ::= <authpolicy>
 *
 *  <authpolicy> ::= 'TRUST' | 'SIMPLE' | 'STRONG'
 */

short authp_sntx;

static int authp_cmp( void *value1, void *value2 ) {
	Authpolicy a = (Authpolicy) value1;
	Authpolicy b = (Authpolicy) value2;
	if ( a == NULLAUTHP && b == NULLAUTHP )
		return( 0 );
	if ( a == NULLAUTHP )
		return( 1 );
	if ( b == NULLAUTHP )
		return( -1 );
	if ( a->ap_modification != b->ap_modification )
		return( a->ap_modification > b->ap_modification ? 1 : -1 );
	if ( a->ap_readandcompare != b->ap_readandcompare )
		return( a->ap_readandcompare > b->ap_readandcompare ? 1 : -1 );
	if ( a->ap_listandsearch != b->ap_listandsearch )
		return( a->ap_listandsearch > b->ap_listandsearch ? 1 : -1 );
	return( 0 );
}

static void * authp_cpy( void *value ) {
	Authpolicy ap = (Authpolicy) value;
	Authpolicy new = authp_alloc();
	*new = *ap;
	return new;
}

static void * authp_decode( PE pe ) {
	Authpolicy	ap;

	if ( decode_Quipu_AuthenticationPolicySyntax( pe, 1, NULL, NULLVP,
			&ap ) == NOTOK ) {
		return( NULLAUTHP );
	}
	return( ap );
}

static PE authp_enc( void *value ) {
	Authpolicy ap = (Authpolicy) value;
	PE ret_pe;
	encode_Quipu_AuthenticationPolicySyntax( &ret_pe, 0, 0, NULLCP, ap );
	return( ret_pe );
}

static int get_policy (char *str) {
	/* get modification policy */
	if ( lexnequ( str, "trust", 5 ) == 0 ) {
		return( AP_TRUST );
	} else if ( lexnequ( str, "simple", 6 ) == 0 ) {
		return( AP_SIMPLE );
	} else if ( lexnequ( str, "strong", 6 ) == 0 ) {
		return( AP_STRONG );
	} else {
		parse_error( "unknown authentication policy '%s'", str );
		return( NOTOK );
	}
}

static void * str2authp( char *str ) {
	Authpolicy	new;
	char		save, *s;

	if ( (s = index( str, '#' )) == NULL ) {
		parse_error( "# missing in authentication policy '%s'", str );
		return( NULLAUTHP );
	}
	save = *s;
	*s = '\0';
	while ( isspace( *str ) )
		str++;
	new = authp_alloc();
	new->ap_modification = get_policy( str );
	/* skip past next # */
	*s++ = save;
	str = s;
	if ( (s = index( str, '#' )) == NULL ) {
		parse_error( "# missing in authentication policy '%s'", str );
		free( (char *) new );
		return( NULLAUTHP );
	}
	save = *s;
	*s = '\0';
	while ( isspace( *str ) )
		str++;
	new->ap_readandcompare = get_policy( str );
	/* skip past next # */
	*s++ = save;
	str = s;
	while ( isspace( *str ) )
		str++;
	new->ap_listandsearch = get_policy( str );
	return( new );
}

static char *policy[] = {
	"trust",
	"simple",
	"strong"
};

static void authp_print (PS ps, void *value, int format) {
	Authpolicy ap = (Authpolicy) value;
	if ( format == READOUT ) {
		ps_printf( ps, "modification policy: %s\n",
				   policy[ap->ap_modification] );
		ps_printf( ps, "\t\t\tread and compare policy: %s\n",
				   policy[ap->ap_readandcompare] );
		ps_printf( ps, "\t\t\tlist and search policy: %s",
				   policy[ap->ap_listandsearch] );
	} else {
		ps_printf( ps, "%s # %s # %s", policy[ap->ap_modification],
				   policy[ap->ap_readandcompare],
				   policy[ap->ap_listandsearch] );
	}
}

void authp_syntax (void) {
	authp_sntx = add_attribute_syntax ("AuthenticationPolicySyntax",
									   authp_enc,	authp_decode,
									   str2authp,	authp_print,
									   authp_cpy,	authp_cmp,
									   free,			NULLCP,
									   NULL,		TRUE);
}
