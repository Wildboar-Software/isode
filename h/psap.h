/* psap.h - include file for presentation users (PS-USER) */

#ifndef	_PSAP_
#define	_PSAP_

#ifdef LINUX
#include <sys/types.h>
#endif
#include <stdint.h>
#ifndef	_MANIFEST_
#include "manifest.h"
#endif
#ifndef	_GENERAL_
#include "general.h"
#endif
#include <math.h>

#ifndef USE_BUILTIN_OIDS
#define USE_BUILTIN_OIDS	1
#endif

typedef struct OIDentifier {
	int	    oid_nelem;		/* number of sub-identifiers */

	unsigned int *oid_elements;	/* the (ordered) list of sub-identifiers */
}			OIDentifier, *OID;
#define	NULLOID	((OID) 0)

OID	ode2oid (char *descriptor);
int	oid_cmp (OID p, OID q), elem_cmp (unsigned int *ip, int i, unsigned int *jp, int j);
OID	oid_cpy (OID oid);
void oid_free (OID oid);
#define	oid2ode(i)	oid2ode_aux ((i), 1)
char *oid2ode_aux (OID identifier, int quoted);
char *sprintoid (OID oid);
OID	str2oid (char *s);

typedef	uint8_t	   PElementClass;

typedef	uint8_t	   PElementForm;

typedef uint16_t    PElementID;	/* 0..16383 are meaningful (14 bits) */
#define	PE_ID_BITS	14

#define	PE_ID(class,code) \
	((int) ((((code) & 0x3fff) << 2) | ((class) & 0x0003)))

typedef	int	   PElementLen;

typedef uint8_t	  byte, *PElementData;
#define	NULLPED	((PElementData) 0)

#define	PEDalloc(s)		((PElementData) malloc_int (s))

#define	PEDrealloc(p, s)	((PElementData) realloc_int ((char *) (p), (s)))

#define	PEDfree(p)		free ((char *) (p))

#define	PEDcmp(b1, b2, length)	\
			bcmp_int ((char *) (b1), (char *) (b2), (length))

#define	PEDcpy(b1, b2, length)	\
			((void) bcopy_int ((char *) (b1), (char *) (b2), (length)))

typedef struct PElement {
	int	    pe_errno;		/* Error codes */
#define	PE_ERR_NONE	0	/*   No error */
#define	PE_ERR_OVER	1	/* Overflow */
#define	PE_ERR_NMEM	2	/*   Out of memory */
#define	PE_ERR_BIT	3	/*   No such bit */
#define	PE_ERR_UTCT	4	/*   Malformed universal timestring */
#define	PE_ERR_GENT	5	/*   Malformed generalized timestring */
#define	PE_ERR_MBER	6	/*   No such member */
#define	PE_ERR_PRIM	7	/*   Not a primitive form */
#define	PE_ERR_CONS	8	/*   Not a constructor form */
#define	PE_ERR_TYPE	9	/*   Class/ID mismatch in constructor */
#define	PE_ERR_OID	10	/*   Malformed object identifier */
#define	PE_ERR_BITS	11	/*   Malformed bitstring */
#define PE_ERR_NOSUPP	12	/*   Type not supported */
#define	PE_ERR_SIGNED	13	/*   Signed integer not expected */
#define PE_ERR_DUPLICATE 14	/*   duplicate member */
#define PE_ERR_SYNTAX	15	/*   syntax error */

	/* for the PSAP */
	int	    pe_context;		/* indirect reference */
#define	PE_DFLT_CTX	0	/*   the default context */

#define	PE_ID_XTND	0x1f	/* distinguished ID for extension bits */
#define	PE_ID_MORE	0x80	/* more to come flag */
#define	PE_ID_MASK	0x7f	/* value in extension byte */
#define	PE_ID_SHIFT	7

#define	PE_CLASS_MASK	0xc0	/* identifier class bits (8-7) */
#define	PE_CLASS_SHIFT	6
#define	PE_FORM_MASK	0x20	/* identifier form bit (6) */
#define	PE_FORM_SHIFT	5
#define	PE_CODE_MASK	0x1f	/* identifier code bits (5-1) + ... */
#define	PE_CODE_SHIFT	0

	PElementClass	pe_class;
#define	PE_CLASS_UNIV	0x0	/* Universal */
#define	PE_CLASS_APPL	0x1	/* Application-wide */
#define	PE_CLASS_CONT	0x2	/* Context-specific */
#define	PE_CLASS_PRIV	0x3	/* Private-use */

	PElementForm	pe_form;
#define	PE_FORM_PRIM	0x0	/* PRIMitive */
#define	PE_FORM_CONS	0x1	/* CONStructor */
#define	PE_FORM_ICONS	0x2	/*   internal: Inline CONStructor */

	PElementID pe_id;		/* should be extensible, 14 bits for now */
	/* Pseudo Types */
#define	PE_UNIV_EOC	0x000	/* End-of-contents */

#define	PE_PRIM_BOOL		0x001	/* Boolean */
#define	PE_PRIM_INT			0x002	/* Integer */
#define	PE_PRIM_BITS		0x003	/* Bitstring */
#define	PE_PRIM_OCTS		0x004	/* Octetstring */
#define	PE_PRIM_NULL		0x005	/* Null */
#define	PE_PRIM_OID			0x006	/* Object identifier */
#define	PE_PRIM_ODE			0x007	/* Object descriptor */
#define	PE_CONS_EXTN		0x008	/* External */
#define PE_PRIM_REAL		0x009	/* Real */
#define PE_PRIM_ENUM		0x00a	/* Enumerated type */
#define PE_PRIM_ENCR		0x00b	/* Encrypted */
#define PE_DEFN_UTF8		0x00c 	/* UTF8String */
#define PE_PRIM_ROID		0x00d	/* Relative OID */
#define PE_PRIM_TIME		0x00e	/* TIME */
#define PE_PRIM_RESV		0x00f	/* Reserved 15 */
#define	PE_CONS_SEQ			0x010	/* Sequence */
#define	PE_CONS_SET			0x011	/* Set */
#define	PE_DEFN_NUMS		0x012	/* Numeric String */
#define	PE_DEFN_PRTS		0x013	/* Printable String */
#define	PE_DEFN_T61S		0x014	/* T.61 String */
#define	PE_DEFN_VTXS		0x015	/* Videotex String */
#define	PE_DEFN_IA5S		0x016	/* IA5 String */
#define	PE_DEFN_UTCT		0x017	/* UTC Time */
#define	PE_DEFN_GENT		0x018	/* Generalized Time */
#define	PE_DEFN_GFXS		0x019	/* Graphics string (ISO2375) */
#define	PE_DEFN_VISS		0x01a	/* Visible string */
#define	PE_DEFN_GENS		0x01b	/* General string */
#define PE_DEFN_USTR		0x01c	/* UniversalString */
#define PE_DEFN_CSTR		0x01d	/* CHARACTER STRING */
#define PE_DEFN_CHRS		0x01d
#define PE_DEFN_BSTR		0x01e	/* BMPString */
#define PE_DEFN_DATE		0x01f	/* DATE */
#define PE_DEFN_TOD			0x020	/* TIME-OF-DAY */
#define PE_DEFN_DTIM		0x021	/* DATE-TIME */
#define PE_DEFN_DURN		0x022	/* DURATION */
#define PE_DEFN_OID_IRI		0x023	/* OID-IRI */
#define PE_DEFN_ROID_IRI	0x024	/* RELATIVE-OID-IRI */

	PElementLen	pe_len;
#define	PE_LEN_XTND	0x80	/* long or indefinite form */
#define	PE_LEN_SMAX	127	/* largest short form */
#define	PE_LEN_MASK	0x7f	/* mask to get number of bytes in length */
#define	PE_LEN_INDF	(-1)	/* indefinite length */

	PElementLen	pe_ilen;

	union {
		PElementData	 un_pe_prim;	/* PRIMitive value */
		struct PElement *un_pe_cons;	/* CONStructor head */
	}                       pe_un1;
#define	pe_prim	pe_un1.un_pe_prim
#define	pe_cons	pe_un1.un_pe_cons

	union {
		int	    un_pe_cardinal;	/* cardinality of list */
		int	    un_pe_nbits;	/* number of bits in string */
	}			    pe_un2;
#define	pe_cardinal	pe_un2.un_pe_cardinal
#define	pe_nbits	pe_un2.un_pe_nbits

	int	    pe_inline;		/* for "ultra-efficient" PElements */
	char   *pe_realbase;	/* .. */

	int	    pe_offset;		/* offset of element in sequence */

	struct PElement *pe_next;

	int	    pe_refcnt;		/* hack for ANYs in pepy */

#ifdef	DEBUG
	struct PElement *pe_link;	/* malloc debugging... */
#endif
}			PElement, *PE;
#define	NULLPE	((PE) 0)
#define	NULLPEP	((PE *) 0)

#define	pe_seterr(pe, e, v)	((pe) -> pe_errno = (e), (v))

extern	int	pe_allocs;
extern	int	pe_frees;
extern	int	pe_most;
extern	PE	pe_list;
#ifdef	DEBUG
extern	PE	pe_active;
#endif

PE	pe_alloc (PElementClass, PElementForm, PElementID);
void pe_free (PE pe);
int	pe_cmp (PE p, PE q);
PE	pe_cpy (PE pe);
int	pe_pullup (PE pe);
PE	pe_expunge (PE pe, PE r);
int	pe_extract (PE pe, PE r);

PE str2pe (char *s, int len, int *advance, int *result);
PE qb2pe (struct qbuf *qb, int len, int depth, int *result);

extern int    pe_maxclass;
extern char  *pe_classlist[];

extern int    pe_maxuniv;
extern char  *pe_univlist[];

extern int    pe_maxappl;
extern char **pe_applist;

extern int    pe_maxpriv;
extern char **pe_privlist;

typedef struct UTCtime {
	int	    ut_year;
	int	    ut_mon;
	int	    ut_mday;
	int	    ut_hour;
	int	    ut_min;
	int	    ut_sec;

	int	    ut_usec;

	int	    ut_zone;

	int	    ut_flags;
#define	UT_NULL		0x00
#define	UT_ZONE		0x01
#define	UT_SEC		0x02
#define	UT_USEC		0x04
}			UTCtime, *UTC;
#define	NULLUTC	((UTC) 0)

void	tm2ut (struct tm *tm, UTC ut);
long	gtime (struct tm *tm);
struct tm *ut2tm (UTC ut);

extern char *psapversion;

int	prim2flag (PE pe);
PE	flag2prim (int b, int class, int id);
#define	bool2prim(b)		flag2prim ((b), PE_CLASS_UNIV, PE_PRIM_BOOL)

integer	prim2num (PE pe);
PE	num2prim (integer i, int class, int id);
#define	int2prim(i)		num2prim ((integer) (i), PE_CLASS_UNIV, PE_PRIM_INT)

#define	prim2enum(i)		prim2num((i))
#define enum2prim(a,b,c)	num2prim((a), (b), (c))
#define enumint2prim(i)		enum2prim ((i), PE_CLASS_UNIV, PE_PRIM_ENUM)

/* psap REAL parameters - tons of 'em */
#define PE_REAL_FLAGS	0300	/* flag bits of real */
#define 	PE_REAL_BINENC	0200	/* binary encoding */
#define 	PE_REAL_DECENC	0000	/* decimal encoding */
#define 	PE_REAL_SPECENC	0100	/* special encoding */

#define PE_REAL_B_S	0100	/* sign bit */
#define PE_REAL_B_BASE	0060	/* base bits */
#define 	PE_REAL_B_B2	0000
#define 	PE_REAL_B_B8	0020
#define 	PE_REAL_B_B16	0040

#define PE_REAL_B_F	0014	/* factor bits */
#define PE_REAL_B_EXP	0003	/* exponent type bits */
#define		PE_REAL_B_EF1	0000
#define 	PE_REAL_B_EF2	0001
#define		PE_REAL_B_EF3	0002
#define		PE_REAL_B_EF4	0003

#define	PE_REAL_PLUSINF		0200
#define PE_REAL_MINUSINF	0201

#ifndef HUGE
#ifdef MAXFLOAT
#define PE_REAL_INFINITY	MAXFLOAT
#else
#define	PE_REAL_INFINITY	99.e99
#endif
#else
#define PE_REAL_INFINITY	HUGE
#endif

double	prim2real (PE pe);
PE	real2prim (double, PElementClass, PElementID);
#define double2prim(i)		real2prim ((i), PE_CLASS_UNIV, PE_PRIM_REAL)

char   *prim2str (PE pe, int *len);
PE	str2prim (char *, int, PElementClass, PElementID);
struct qbuf *prim2qb (PE pe);
PE	qb2prim_aux (struct qbuf *, PElementClass, PElementID, int);		/* really should be qb2pe () */
#define	qb2prim(q,c,i)		qb2prim_aux ((q), (c), (i), 0)
#define	oct2prim(s,len)		str2prim ((s), (len), PE_CLASS_UNIV, PE_PRIM_OCTS)
#define	ode2prim(s,len)		str2prim ((s), (len), PE_CLASS_UNIV, PE_PRIM_ODE)
#define utf2prim(s,len)		str2prim ((s), (len), PE_CLASS_UNIV, PE_DEFN_UTF8)
// #define time2prim(s,len)	str2prim ((s), (len), PE_CLASS_UNIV, PE_DEFN_TIME)
#define	nums2prim(s,len)	str2prim ((s), (len), PE_CLASS_UNIV, PE_DEFN_NUMS)
#define	prts2prim(s,len)	str2prim ((s), (len), PE_CLASS_UNIV, PE_DEFN_PRTS)
#define	t61s2prim(s,len)	str2prim ((s), (len), PE_CLASS_UNIV, PE_DEFN_T61S)
#define	vtxs2prim(s,len)	str2prim ((s), (len), PE_CLASS_UNIV, PE_DEFN_VTXS)
#define	ia5s2prim(s,len)	str2prim ((s), (len), PE_CLASS_UNIV, PE_DEFN_IA5S)
#define	utc2prim(s,len)		str2prim ((s), (len), PE_CLASS_UNIV, PE_DEFN_UTCT)
// #define gent2prim(s,len)	str2prim ((s), (len), PE_CLASS_UNIV, PE_DEFN_GENT)
#define	gfxs2prim(s,len)	str2prim ((s), (len), PE_CLASS_UNIV, PE_DEFN_GFXS)
#define	viss2prim(s,len)	str2prim ((s), (len), PE_CLASS_UNIV, PE_DEFN_VISS)
#define	gens2prim(s,len)	str2prim ((s), (len), PE_CLASS_UNIV, PE_DEFN_GENS)
// UniversalString cannot have a macro like above.
// BMPString cannot have a macro like above.
#define date2prim(s,len)	str2prim ((s), (len), PE_CLASS_UNIV, PE_DEFN_DATE)
#define tod2prim(s,len)	str2prim ((s), (len), PE_CLASS_UNIV, PE_DEFN_TIME_OF_DAY)
#define dt2prim(s,len)	str2prim ((s), (len), PE_CLASS_UNIV, PE_DEFN_DTIM)
#define dur2prim(s,len)	str2prim ((s), (len), PE_CLASS_UNIV, PE_DEFN_DURN)
#define oidiri2prim(s,len)	str2prim ((s), (len), PE_CLASS_UNIV, PE_DEFN_OID_IRI)
#define roidiri2prim(s,len)	str2prim ((s), (len), PE_CLASS_UNIV, PE_DEFN_ROID_IRI)

PE	prim2bit (PE pe);
PE	bit2prim (PE pe);

int	bit_on (PE pe, int i), bit_off (PE pe, int i);
int	bit_test (PE pe, int i);

OID	prim2oid (PE pe);
PE	obj2prim (OID, PElementClass, PElementID);
#define	oid2prim(o)		obj2prim ((o), PE_CLASS_UNIV, PE_PRIM_OID)

UTC	prim2time (PE pe, int generalized);
#define	prim2utct(pe)		prim2time ((pe), 0)
#define	prim2gent(pe)		prim2time ((pe), 1)
PE	time2prim (UTC, int, PElementClass, PElementID);
#define	utct2prim(u)		time2prim ((u), 0, PE_CLASS_UNIV, PE_DEFN_UTCT)
#define	gent2prim(u)		time2prim ((u), 1, PE_CLASS_UNIV, PE_DEFN_GENT)
char   *time2str (UTC u, int generalized);
#define	utct2str(u)		time2str ((u), 0)
#define	gent2str(u)		time2str ((u), 1)
UTC	str2utct (char *cp, int len), str2gent (char *cp, int len);

PE	prim2set (PE pe);
#define	set2prim(pe)		(pe)
int	set_add (PE pe, PE r), set_addon (PE pe, PE last, PE new);
int set_del (PE, PElementClass, PElementID);
PE	set_find (PE, PElementClass, PElementID);
#define	first_member(pe)	((pe) -> pe_cons)
#define	next_member(pe,p)	((p) -> pe_next)

#define	prim2seq(pe)		(prim2set (pe))
#define	seq2prim(pe)		(pe)
int	seq_add (PE pe, PE r, int i), seq_addon (PE pe, PE last, PE new), seq_del (PE pe, int i);
PE	seq_find (PE pe, int i);

char   *pe_error (int c);

#ifdef SVR4_UCB
#ifdef PS	/* ucb define PS in sys/sparc/reg.h for "portability" !?! */
#undef PS
#endif
#endif

typedef struct PStream PStream, *PS;

struct PStream {
	int	    ps_errno;		/* Error codes */
#define	PS_ERR_NONE	 0	/*   No error */
#define	PS_ERR_OVERID	 1	/*   Overflow in ID */
#define	PS_ERR_OVERLEN	 2	/*   Overflow in length */
#define	PS_ERR_NMEM	 3	/*   Out of memory */
#define	PS_ERR_EOF	 4	/*   End of file */
#define	PS_ERR_EOFID	 5	/*   End of file reading extended ID */
#define	PS_ERR_EOFLEN	 6	/*   End of file reading extended length */
#define	PS_ERR_LEN	 7	/*   Length mismatch */
#define	PS_ERR_TRNC	 8	/* Truncated */
#define	PS_ERR_INDF	 9	/*   Indefinite length in primitive form */
#define	PS_ERR_IO	10	/*   I/O error */
#define	PS_ERR_EXTRA	11	/*   Extraneous octets */
#define	PS_ERR_XXX	12	/* XXX */

	union {
		char *un_ps_addr;
		struct {
			char   *st_ps_base;
			int	    st_ps_cnt;
			char   *st_ps_ptr;
			int	    st_ps_bufsiz;
		}			un_ps_st;
		struct {
			struct udvec *uv_ps_head;
			struct udvec *uv_ps_cur;
			struct udvec *uv_ps_end;
			int	    uv_ps_elems;
			int	    uv_ps_slop;
			int	    uv_ps_cc;
		}			un_ps_uv;
	}                       ps_un;
#define	ps_addr	ps_un.un_ps_addr
#define	ps_base	ps_un.un_ps_st.st_ps_base
#define	ps_cnt	ps_un.un_ps_st.st_ps_cnt
#define	ps_ptr	ps_un.un_ps_st.st_ps_ptr
#define	ps_bufsiz	ps_un.un_ps_st.st_ps_bufsiz
#define	ps_head	ps_un.un_ps_uv.uv_ps_head
#define	ps_cur	ps_un.un_ps_uv.uv_ps_cur
#define	ps_end	ps_un.un_ps_uv.uv_ps_end
#define	ps_elems	ps_un.un_ps_uv.uv_ps_elems
#define	ps_slop	ps_un.un_ps_uv.uv_ps_slop
#define	ps_cc	ps_un.un_ps_uv.uv_ps_cc

	char *ps_extra;		/* for George's recursive PStreams */

	int	    ps_inline;		/* for "ultra-efficient" PStreams */

	int	    ps_scratch;		/* XXX */

	int	    ps_byteno;		/* byte position */

	int(*ps_primeP)(PS ps, int waiting);
	int(*ps_readP)(PS ps, PElementData data, PElementLen n, int in_line);
	int(*ps_writeP)(PS ps, PElementData data, PElementLen n, int in_line);
	int(*ps_flushP)(PS ps);
	int(*ps_closeP)(PS ps);
};
#define	NULLPS	((PS) 0)

#define	ps_seterr(ps, e, v)	((ps) -> ps_errno = (e), (v))

PS	ps_alloc (int (*io)(PS ps));
void	ps_free (PS ps);

int	ps_io (PS ps, int (*io)(PS ps, PElementData data, PElementLen n, int in_line), PElementData data, PElementLen n, int in_line);
#define	ps_read(ps, data, cc)	ps_io ((ps), (ps) -> ps_readP, (data), (cc), 0)
#define	ps_write(ps, data, cc)	ps_write_aux ((ps), (data), (cc), 0)
#define	ps_write_aux(ps, data, cc, in_line) \
    	ps_io ((ps), (ps) -> ps_writeP, (data), (cc), (in_line))

static inline int
ps_print_s (PS ps, char *data)
{
	int n;

	if (data == NULL || strlen2int (data, &n) != 0)
		return NOTOK;
	return ps_write (ps, (PElementData) data, n);
}

static inline int
ps_write_span (PS ps, char *start, char *end)
{
	int n;

	if (start == NULL || ptrdiff2int (end - start, &n) != 0)
		return NOTOK;
	return ps_write (ps, (PElementData) start, n);
}

int	ps_flush (PS ps);

int ps_prime (PS ps, int waiting);

int ps_get_plen (PE pe);
int ps_read_id (PS ps, int top, PElementClass *class, PElementForm *form, PElementID *id);
int ps_read_cons (PS ps, PE *pe, PElementLen len);
int ps_read_len (PS ps, PElementLen *len);

int	std_open (PS ps);
#define	std_setup(ps, fp)	((ps) -> ps_addr = (char *) (fp), OK)

int	str_open (PS ps);
int	str_setup (PS ps, char *cp, int cc, int in_line);

int	dg_open (PS ps);
int	dg_setup (PS ps, int fd, int size, int (*rfx)(int fd, struct qbuf **q), int (*wfx)(int fd, struct qbuf *qb), int (*cfx)(int fd));

int	fdx_open (PS ps);
int	fdx_setup (PS ps, int fd);

int	qbuf_open (PS ps);
#define	qbuf_setup(ps, qb)	((ps) -> ps_addr = (char *) (qb), OK)

#define	ts_open	dg_open
#define	ts_setup(p,f,s)		dg_setup ((p), (f), (s), ts_read, ts_write)
int	ts_read (int fd, struct qbuf **q), ts_write (int fd, struct qbuf *qb);

int	uvec_open (PS ps);
int	uvec_setup (PS ps, int len);

#define	ps2pe(ps)		ps2pe_aux ((ps), 1, 1)
PE	ps2pe_aux (PS ps, int top, int all);
#define	pe2ps(ps, pe)		pe2ps_aux ((ps), (pe), 1)
int	pe2ps_aux (PS ps, PE pe, int eval);

PE	pl2pe (PS ps);
int	pe2pl (PS ps, PE pe);

extern int    ps_len_strategy;
#define	PS_LEN_SPAG	0
#define	PS_LEN_INDF	1
#define	PS_LEN_LONG	2

int	ps_get_abs (PE pe);

char   *ps_error (int c);

struct isobject {
	char   *io_descriptor;

	OIDentifier io_identity;
};

int	setisobject (int f),	endisobject (void);

struct isobject *getisobject (void);

struct isobject *getisobjectbyname (char *descriptor);
struct isobject *getisobjectbyoid (OID oid);

extern	int	Len;
extern	char   *Qcp;
extern	char   *Ecp;

int	pe2qb_f (PE pe);

extern	int	Byteno;
extern	int	Qbrefs;
extern struct qbuf *Hqb;
extern struct qbuf *Fqb;
extern struct qbuf *Qb;

#define qbuf2pe(qb, len, result) (Byteno = 0, Hqb = qb, \
                                        Fqb = (Qb = (qb) -> qb_forw), \
                                        qbuf2pe_f (result))
PE qbuf2pe_f (int *result);
char *qb2str (struct qbuf *q);
struct qbuf *str2qb (char *s, int len, int head) ;
static inline struct qbuf *
str2qb_s (char *s)
{
	int n;

	if (s == NULL || strlen2int (s, &n) != 0)
		return NULL;
	return str2qb (s, n, 1);
}

static inline PE
str2prim_s (char *s, PElementClass cl, PElementID id)
{
	int n;

	if (s == NULL || strlen2int (s, &n) != 0)
		return NULLPE;
	return str2prim (s, n, cl, id);
}
void qb_free (struct qbuf *qb);

int	pe2ssdu (PE pe, char **base, int *len);
PE	ssdu2pe (char *base, int len, char *realbase, int *result);

struct ll_struct;
void	pe2text (struct ll_struct *lp, PE pe, int rw, int cc), text2pe (void);

int	pe2uvec (PE pe, struct udvec **uv);

char   *int2strb (int n, int len);
int	strb2int (char *cp, int len);

PE	strb2bitstr (char *, int, PElementClass, PElementID);
char   *bitstr2strb (PE pe, int *k);

extern char PY_pepy[];

void	PY_advise (char *, char *, ...);
int	PY_pp (int argc, char **argv, char **envp, int (*pfx)(PE pe, int explicit, int *len, char **buffer, char *parm));

int	testdebug (PE pe, char *s);

void vpush (void);
void vpop (void);
void vname (char *name);
void vtag (int class, int id);
void vstring (PE pe);
void vunknown (PE pe);
void vprint (char*, ...);
char *bit2str (PE pe, char *s);
void vpushfp (FILE *fp, PE pe, char *s, int rw);
void vpopfp (void);
void vpushstr (char *cp);
void vpopstr (void);
void vpushpp (FILE *vfp, PE pe, char *text, int rw);
void vpopp (void);

/* handle calls to the vunknown print routine */
#define vunknown_P 	0, ((modtyp *) 0)

/* pepy string definitions */
extern char *pepy_strings[];

#define	 PEPY_ERR_BAD 			(pepy_strings[0])
#define	 PEPY_ERR_BAD_BITS 		(pepy_strings[1])
#define	 PEPY_ERR_BAD_BOOLEAN 		(pepy_strings[2])
#define	 PEPY_ERR_BAD_CLASS 		(pepy_strings[3])
#define	 PEPY_ERR_BAD_CLASS_FORM_ID 	(pepy_strings[4])
#define	 PEPY_ERR_BAD_FORM 		(pepy_strings[5])
#define	 PEPY_ERR_BAD_INTEGER 		(pepy_strings[6])
#define	 PEPY_ERR_BAD_OID 		(pepy_strings[7])
#define	 PEPY_ERR_BAD_OCTET 		(pepy_strings[8])
#define	 PEPY_ERR_BAD_REAL 		(pepy_strings[9])
#define	 PEPY_ERR_BAD_SEQ 		(pepy_strings[10])
#define	 PEPY_ERR_BAD_SET 		(pepy_strings[11])
#define	 PEPY_ERR_TOO_MANY_BITS 	(pepy_strings[12])
#define	 PEPY_ERR_TOO_MANY_ELEMENTS 	(pepy_strings[13])
#define	 PEPY_ERR_UNKNOWN_CHOICE 	(pepy_strings[14])
#define	 PEPY_ERR_UNK_COMP 		(pepy_strings[15])
#define	 PEPY_ERR_INIT_FAILED 		(pepy_strings[16])
#define	 PEPY_ERR_INVALID_CHOICE 	(pepy_strings[17])
#define	 PEPY_ERR_MISSING 		(pepy_strings[18])
#define	 PEPY_ERR_NOMEM  		(pepy_strings[19])
#define	 PEPY_ERR_TOO_MANY_TAGGED 	(pepy_strings[20])
#define	 PEPY_ERR_EXTRA_MEMBERS 	(pepy_strings[21])

void ps_printf (PS ps, char *fmt, ...);
int std_open (PS ps);
int std_flush (PS ps);
int free_static_oid (void);

#endif
