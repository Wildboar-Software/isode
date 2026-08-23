/* soundex.c - compare a filter and attribute value */

#include <ctype.h>
#include <string.h>
#include "quipu/util.h"
#include "quipu/attrvalue.h"
#include "quipu/ds_search.h"
#include "quipu/common.h"

char *first_word (char *ptr);
char *next_word (char *ptr);
static int match_word (char *a);
static int soundex_cmp (char *a, char *b);
int soundex_match (struct filter_item *fitem, AV_Sequence avs);


extern char chrcnv[];
extern LLog *log_dsap;

#ifndef SOUNDEX_LEN
#define SOUNDEX_LEN	-1
#endif

#define iswordbreak(x)	(!isascii(x) || isspace((unsigned char) (x)) || \
	ispunct((unsigned char) (x)) || \
	isdigit((unsigned char) (x)) || x == '\0')

char *first_word (char *ptr) {
	if ( ptr == NULLCP )
		return NULLCP;
	while (iswordbreak(*ptr))
		if (*ptr == 0)
			return NULLCP;
		else
			ptr++;
	return(ptr);
}

char *next_word (char *ptr) {
	if ( ptr == NULLCP )
		return NULLCP;
	for (;;) {
		if (*ptr == 0)
			return NULLCP;
		else if (iswordbreak(*ptr)) {
			while (*ptr && iswordbreak(*ptr))
				ptr++;
			if (*ptr == 0)
				return NULLCP;
			else
				return(ptr);
		} else
			ptr++;
	}
	/* NOTREACHED */
}

void soundex (char *s, char **c) {
	char code, adjacent, ch, *p;
	int i, cmax;

	p = s;
	if ( *p == '\0' ) {
		*c = (char *) malloc(sizeof(char));
		**c = '\0';
		return;
	}
	/* assume at least four...realloc if we need to */
	*c = (char *) malloc(5*sizeof(char));
	cmax = 4;
	adjacent = '0';
	if ( islower( *p ) )
		(*c)[0] = toupper(*p);
	else
		(*c)[0] = *p;
	(*c)[1]  = '\0';
	for ( i = 0; i < 99 && (! iswordbreak(*p)); p++ ) {
		if ( islower( *p ) )
			ch = toupper (*p);
		else
			ch = *p;
		code = '0';
		switch (ch) {
		case 'B':
		case 'F':
		case 'P':
		case 'V':
			code = (adjacent != '1') ? '1' : '0';
			break;
		case 'S':
		case 'C':
		case 'G':
		case 'J':
		case 'K':
		case 'Q':
		case 'X':
		case 'Z':
			code = (adjacent != '2') ? '2' : '0';
			break;
		case 'D':
		case 'T':
			code = (adjacent != '3') ? '3' : '0';
			break;
		case 'L':
			code = (adjacent != '4') ? '4' : '0';
			break;
		case 'M':
		case 'N':
			code = (adjacent != '5') ? '5' : '0';
			break;
		case 'R':
			code = (adjacent != '6') ? '6' : '0';
			break;
		default:
			adjacent = '0';
		}
		if ( i == 0 ) {
			adjacent = code;
			i++;
		} else if (code != '0' ) {
			if (i == SOUNDEX_LEN)
				break;
			if (i == cmax) {
				{
					int n = cmax;
					char *np;
					if (add_int_to_int (&n, cmax) != 0
							|| add_int_to_int (&n, 1) != 0)
						return;
					np = realloc_int (*c, n);
					if (np == NULL)
						return;
					*c = np;
				}
				cmax *= 2;
			}
			adjacent = (*c)[i] = code;
			i++;
		}
	}
	if (i > 0)
		(*c)[i] = '\0';
	return;
}

static char	*g_bcode;
static int	g_bcodelen;

static int match_word (char *a) {
	char	*as;
	int	cmp;

	soundex(a, &as);
#ifdef SOUNDEX_PREFIX
	cmp = strncmp_int(as, g_bcode, g_bcodelen);
#else
	cmp = strcmp(as, g_bcode);
#endif
	free(as);
	return(cmp == 0);
}

int soundex_cmp (char *a, char *b) {
	char result = FALSE;
	char * ptr;

	for( ; a && b ; b = next_word (b) ) {
		soundex(b, &g_bcode);
		g_bcodelen = strlen(g_bcode);
		for (ptr=first_word(a); ptr; ptr=next_word(ptr) )  {
			if (match_word (ptr)) {
				a = next_word (ptr);
				result = TRUE;
				break;
			}
		}
		free(g_bcode);
		if (ptr == NULLCP)
			return FALSE;
	}
	if ((a == NULL) && (b != NULL))
		return FALSE;
	return (result);
}

int soundex_match (struct filter_item *fitem, AV_Sequence avs)
{
	for (; avs != NULLAV; avs=avs->avseq_next)
		if (soundex_cmp ((char *)avs->avseq_av.av_struct, (char *)fitem->UNAVA.ava_value->av_struct))
			return (OK);
	return (NOTOK);
}
