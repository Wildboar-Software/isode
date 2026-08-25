/* attr_sntx.c - Attribute Attribute syntax! */

/*
 *	SYNTAX:
 *
 *		attributeSyntax ::= '(' attributeSequence ')'
 *
 *		Written by :-	Kuan Siew Weng
 */
#include <string.h>
#include "quipu/util.h"
#include "quipu/common.h"
#include "psap.h"
static char *find_nest (char *str);
static void attrSntx_print (PS ps, void *value, int format);
static void * str2attrSntx (char *str);
static Attr_Sequence str2attrSeq(char * buf);
static PE avs_enc(AV_Sequence avs);
static AV_Sequence avs_dec(PE pe, AttributeType at);
static PE attr_enc(Attr_Sequence a);
static Attr_Sequence attr_dec(PE pe);
static PE attrSntx_enc (Attr_Sequence a);
static Attr_Sequence attrSntx_dec (PE pe);
void attribute_syntax (void);


#define AS_START_DELIMITER 	'('
#define AS_END_DELIMITER 	')'

extern PE dn_enc(DN dn);
extern DN dn_dec(PE pe);
extern DN str2dnX(char *str);

char * find_nest(char *str);

static int indent = 0;
short as_sntx;
extern short oc_sntx;
extern void (*oc_hier)(void);

#ifdef TURBO_DISK
char fromfile;
#endif
char *find_nest (char *str) {
	char *cp, *ptr1, *ptr2;

	if(!(cp = index(str,AS_START_DELIMITER)))
		return(NULL);
	ptr1 = ++cp;
	if(!(ptr2 = index(cp,AS_END_DELIMITER)))
		return(NULL);
	*ptr2 = 0;
	while((ptr1 = index(ptr1,AS_START_DELIMITER))) {
		*ptr2 = AS_END_DELIMITER;
		ptr2++;
		ptr1++;
		if(!(ptr2 = index(ptr2,AS_END_DELIMITER)))
			return(NULL);
		*ptr2 = 0;
	}
	*ptr2 = AS_END_DELIMITER;
	return(ptr2);
}

static void attrSntx_print (PS ps, void *value, int format) {
	Attr_Sequence a = (Attr_Sequence) value;
	char buf[LINESIZE];
	Attr_Sequence  atl;
	AV_Sequence avs;
	extern int oidformat;
	int i;

	if (a) {
		if (format == READOUT) {
			indent++;
			for ( atl = a ; atl != NULL; atl = atl->attr_link) {
				sprintf(buf,"%s",attr2name(atl->attr_type,oidformat));
				for (avs= atl->attr_value; avs != NULLAV; avs = avs->avseq_next) {
					ps_printf(ps,"\n");
					for ( i = 0; i< indent; i++)
						ps_printf(ps,"  ");
					ps_printf(ps,"%-21s - ",buf);
					avs_comp_print(ps,avs,format);
				}
			}
			indent--;
		} else {
			ps_printf(ps,"%c\n",AS_START_DELIMITER);
			as_print(ps,a,format);
			ps_printf(ps,"%c",AS_END_DELIMITER);
		}
	}
}

static void * str2attrSntx (char *str) {
	char * ptr;
	Attr_Sequence as = NULLATTR, tas;
	char * getnextline(void);
#ifdef TURBO_DISK
	char * fgetnextline(void);
#endif

	if ( *str != AS_START_DELIMITER)
		parse_error ("Starting delimeter missing",NULLCP);
	for (;;) { /* break out */
#ifdef TURBO_DISK
		if (fromfile) {
			if ((ptr = fgetnextline ()) == NULLCP) {
				parse_error ("Attribute sntx EOF unexpected",NULLCP);
				return (NULLATTR);
			}
		} else
#endif
			if ((ptr = getnextline ()) == NULLCP) {
				parse_error ("Attribute sntx EOF unexpected(2)",NULLCP);
				return (NULLATTR);
			}
		if ( *ptr == AS_END_DELIMITER )
			break;
		if ((tas = str2as(ptr)) == NULLATTR) {
			parse_error ("attr sntx attr parse failed",NULLCP);
			return (NULLATTR);
		}
		as = as_merge (as, tas);
	}
	return as;
}

Attr_Sequence str2attrSeq(char * buf) {
	char *cp, cp1[3];

	{
		int n;

		if (strlen2int (buf, &n) != 0 || n > INT_MAX - 4)
			return NULLATTR;
		cp = smalloc(n+4);
	}
	sprintf(cp,"%c\n", AS_START_DELIMITER);
	strcat(cp,buf);
	sprintf(cp1,"%c\n",AS_END_DELIMITER);
	strcat(cp,cp1);
	return(str2attrSntx(cp));
}
#define str2AttrList(buf)	str2attrSeq(buf)

static PE avs_enc(AV_Sequence avs) {
	AV_Sequence avl;
	attrVal av;
	PE	    pe;

	pe = pe_alloc(PE_CLASS_UNIV,PE_FORM_CONS,PE_CONS_SEQ);
	for (avl = avs; avl; avl=avl->avseq_next) {
		AttrV_cpy_enc(&(avl->avseq_av),&av);
		seq_add(pe,(PE) av.av_struct,-1);
	}
	return pe;
}

static AV_Sequence avs_dec(PE pe, AttributeType at) {
	AV_Sequence avl,av;
	PE r;

	avl = NULLAV;
	for (r = first_member(pe); r; r=next_member(pe,r)) {
		av = avs_comp_alloc();
		av->avseq_next = NULLAV;
		av->avseq_av.av_syntax = 0;
		av->avseq_av.av_struct = (caddr_t) pe_cpy(r);
		AttrV_decode(at,&(av->avseq_av));
		avl = avs_merge(avl,av);
	}
	return avl;
}

static PE attr_enc(Attr_Sequence a) {
	PE pe, r;

	pe = pe_alloc(PE_CLASS_UNIV,PE_FORM_CONS,PE_CONS_SEQ);
	if ((r = oid2prim(a->attr_type->oa_ot.ot_oid)) == NULLPE) {
		pe_free(pe);
		return(NULLPE);
	} else
		seq_add(pe,r,0);
	if ((r = avs_enc(a->attr_value)) == NULLPE) {
		pe_free(pe);
		return(NULLPE);
	} else
		seq_add(pe,r,1);
	return(pe);
}

static Attr_Sequence attr_dec(PE pe) {
	Attr_Sequence a;
	AttributeType at;
	PE r;

	a = NULLATTR;
	for (r = first_member(pe); r; r=next_member(pe,r)) {
		switch(r->pe_offset) {
		case 0:
			if (r->pe_form == PE_FORM_PRIM) {
				if (at = oid2attr(prim2oid(r))) {
					a = as_comp_new(at,NULLAV,NULLACL_INFO);
					if (((r=next_member(pe,r)) == NULLPE) ||
							((a->attr_value = avs_dec(r,at)) == NULLAV)) {
						as_free(a);
						return NULLATTR;
					}
				} else
					return NULLATTR;
				break;
			}
		default:
			if ((a = attr_dec(r)) == NULLATTR)
				return NULLATTR;
		}
	}
	return a;
}

/*
 * attrSntx_enc and attrSntx_dec must be defined as non static for tests
 */

PE attrSntx_enc (Attr_Sequence a) {
	Attr_Sequence atl;
	PE	    pe, r;

	pe = pe_alloc(PE_CLASS_UNIV,PE_FORM_CONS,PE_CONS_SEQ);
	for (atl = a; atl; atl=atl->attr_link) {
		if ((r = attr_enc(atl)) == NULLPE) {
			pe_free(pe);
			return(NULLPE);
		} else
			seq_add(pe,r,-1);
	}
	return pe;
}

Attr_Sequence attrSntx_dec (PE pe) {
	Attr_Sequence a, atl;
	PE r;

	atl = NULLATTR;
	for (r = first_member(pe); r; r=next_member(pe,r)) {
		if (((a = attr_dec(r)) == NULLATTR) ||
				((atl = as_merge(atl,a)) == NULLATTR)) {
			as_free(atl);
			return(NULLATTR);
		}
	}
	return atl;
}

void attribute_syntax (void) {
	as_sntx = add_attribute_syntax ("AttributeSyntax",
									(AttributeValueEncoder)attrSntx_enc,
									(AttributeValueDecoder)attrSntx_dec,
									str2attrSntx,
									attrSntx_print,
									(AttributeValueCopier)as_cpy,
									(AttributeValueComparator)as_cmp,
									(AttributeValueFree)as_free,
									NULLCP, NULL, TRUE);
}
