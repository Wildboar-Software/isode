/* attr.h - directory service interface definition */

#ifndef QUIPUATTR
#define QUIPUATTR

#include "quipu/oid.h"
#include "manifest.h"
#include "quipu/util.h"

#ifndef TRUE
#define FALSE 0
#define TRUE 1
#endif

/* FIRST SECTION DEFINES STRUCTURES FOR ADDRESSING */

typedef oid_table_attr * attrType;
typedef oid_table_attr * AttributeType;

#define NULLAttrT (AttributeType) NULL
#define AttrT_alloc()
#define AttrT_cmp(x,y)	( x == y ? 0 : ( x > y ? -1 : 1 ))
#define AttrT_decode(x)
#define AttrT_free(x)
#define AttrT_cpy(x) x
#define grab_oid(x)  (x ? x->oa_ot.ot_oid : NULLOID)

AttributeType AttrT_new();

#define str2AttrT(x) AttrT_new(x)

typedef struct {
	short    av_syntax;             /* Specifies the syntax of the      */
	/* attribute  that follows.         */
	caddr_t  av_struct;
} attrVal, * AttributeValue;

typedef struct ava {            /* represents AttributeValueAssertion */
	AttributeType ava_type;
	AttributeValue ava_value;
} ava, AVA;

#define AV_WRITE_FILE		256
#define AV_FILE			128
#define MAX_AV_SYNTAX 		100

#define NULLAttrV (AttributeValue) NULL
#define AttrV_alloc()   (AttributeValue) smalloc (sizeof (attrVal));

AttributeValue AttrV_cpy(AttributeValue x);
AttributeValue str_at2AttrV(char * str, AttributeType at);
AttributeValue str2AttrV(char * str, short syntax);
short str2syntax(char * str);

typedef struct avseqcomp {      /* attribute may have multiple values   */
	/* respresents SET OF AttributeValue    */
	attrVal      	avseq_av;
	struct avseqcomp    *avseq_next;
} avseqcomp, *AV_Sequence;

typedef struct {
	AttributeType fi_sub_type;
	AV_Sequence fi_sub_initial;
	AV_Sequence fi_sub_any;
	AV_Sequence fi_sub_final;
	/* initial and final should be zero or  */
	/* one components only                  */
	char       *fi_sub_match; /* for DSA use */
} Filter_Substrings;

struct filter_item {
	int         fi_type;
#define FILTERITEM_EQUALITY 1
#define FILTERITEM_SUBSTRINGS 2
#define FILTERITEM_GREATEROREQUAL 3
#define FILTERITEM_LESSOREQUAL 4
#define FILTERITEM_PRESENT 5
#define FILTERITEM_APPROX 6
	union {
		AttributeType fi_un_type;
		AVA fi_un_ava;
		Filter_Substrings fi_un_substrings;
	} fi_un;
	/* field for DSA use - no need to fill if using DUA */
	IFP	    fi_ifp;
};

typedef PE (*AttributeValueEncoder)(void *value);
typedef void* (*AttributeValueDecoder)(PE pe);
typedef void* (*AttributeValueParser)(char *str);
typedef void (*AttributeValuePrinter)(PS ps, void *value, int format);
typedef void* (*AttributeValueCopier)(void *value);
typedef int (*AttributeValueComparator)(void *value1, void *value2);
typedef void (*AttributeValueFree)(void *value);
typedef int (*AttributeValueApproximator)(struct filter_item *fitem, AV_Sequence avs);

short add_attribute_syntax (char *sntx,
	AttributeValueEncoder enc,
	AttributeValueDecoder dec,
	AttributeValueParser parse,
	AttributeValuePrinter print,
	AttributeValueCopier cpy,
	AttributeValueComparator cmp,
	AttributeValueFree sfree,
	char *print_pe,
	AttributeValueApproximator approx,
	char multiline);
short modify_av_printer ();

struct file_syntax {
	short	fs_real_syntax;
	char *  fs_name;
	char	fs_mode;
	char	fs_ref;
	AttributeValue fs_attr;
};
#define FS_DEFAULT 0x01 	/* default file name */
#define FS_CREATE  0x02		/* created thus remove file */
#define FS_TMP     0x04		/* DSA created tmp file */

#define EDBOUT  1
#define FILEOUT 2   /* for writing to files only */
#define READOUT 3
#define DIROUT  4   /* for dn and rdn print only */
#define	RDNOUT	5
#define	UFNOUT	6   /* user-friendly naming */

#define EDB_LINEWRAP	38	/* Wrap very long lines after 38 characters */

#define ps_print(ps,data) (void)ps_write(ps,(PElementData)data,strlen(data))

typedef struct {
	char *s_sntx;		/* String defining syntax */
	AttributeValueEncoder s_encode;
	AttributeValueDecoder s_decode;
	AttributeValueParser s_parse;
	AttributeValuePrinter s_print;
	AttributeValueCopier s_copy;
	/* return 0 if equal, -1 or 1 if not equal (not sure which way though) */
	AttributeValueComparator s_compare;
	AttributeValueFree s_free;
	char *s_pe_print;	/* process to handle raw PE */
	/* Approximate match routine. The first argument is a filter_item, and
	the second is an AV_Sequence. I just couldn't actually use those types in
	this header without recursive includes. */
	AttributeValueApproximator s_approx;
	char s_multiline;	/* if true print each value on new line */
} sntx_table;

extern char quipu_faststart;
extern char * TidyString();
extern char * TidyString2();

#define FAST_TIDY(x) if (quipu_faststart) while (isascii(*x) && isspace (*x)) x++; else x = TidyString(x)

#endif
