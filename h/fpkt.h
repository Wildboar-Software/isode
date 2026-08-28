/* fpkt.h - include file for FTAM provider (FS-PROVIDER) */

#include "FTAM-types.h"

#ifndef	_FTAM_
#include "ftam.h"		/* definitions for FS-USERs */
#endif
#ifndef	_AcSAP_
#include "acsap.h"		/* definitions for AcS-USERs */
#endif

#define	FS_GEN(fsb) \
	((fsb -> fsb_flags & FSB_INIT) ? FS_GEN_INITIATOR : FS_GEN_RESPONDER)

#define	ftamPsig(fsb, sd) \
{ \
    if ((fsb = findfsblk (sd)) == NULL) { \
	(void) sigiomask (smask); \
	return ftamlose (fti, FS_GEN_NOREASON, 0, NULLCP, \
			    "invalid ftam descriptor"); \
    } \
    if (!(fsb -> fsb_flags & FSB_CONN)) { \
	(void) sigiomask (smask); \
	return ftamlose (fti, FS_GEN (fsb), 0, NULLCP, \
			    "ftam descriptor not connected"); \
    } \
    if (fsb -> fsb_flags & FSB_FINN) { \
	(void) sigiomask (smask); \
	return ftamlose (fti, FS_GEN (fsb), 0, NULLCP, \
			    "ftam descriptor finishing"); \
    } \
}

#define	ftamFsig(fsb, sd) \
{ \
    if ((fsb = findfsblk (sd)) == NULL) { \
	(void) sigiomask (smask); \
	return ftamlose (fti, FS_GEN_NOREASON, 0, NULLCP, \
			    "invalid ftam descriptor"); \
    } \
    if (!(fsb -> fsb_flags & FSB_CONN)) { \
	(void) sigiomask (smask); \
	return ftamlose (fti, FS_GEN (fsb), 0, NULLCP, \
			    "ftam descriptor not connected"); \
    } \
    if (!(fsb -> fsb_flags & FSB_FINN)) { \
	(void) sigiomask (smask); \
	return ftamlose (fti, FS_GEN (fsb), 0, NULLCP, \
			    "ftam descriptor not finishing"); \
    } \
}

#define toomuchP(b,n,m,p) \
{ \
    if (b == NULL) \
	n = 0; \
    else \
	if (n > m) \
	    return ftamlose (fti, FS_GEN_NOREASON, 0, NULLCP, \
			    "too many %ss", p); \
}

#ifdef __STDC__
#define missingP(p) \
{ \
    if (p == NULL) \
        return ftamlose (fti, FS_GEN_NOREASON, 0, NULLCP, \
                            "mandatory parameter \"%s\" missing", #p); \
}
#else
#define	missingP(p) \
{ \
    if (p == NULL) \
	return ftamlose (fti, FS_GEN_NOREASON, 0, NULLCP, \
			    "mandatory parameter \"%s\" missing", "p"); \
}
#endif
#ifndef	lint
#ifndef	__STDC__
#define	copyFTAMdata(base,len,d) \
{ \
    (void) copy_capped (base, d -> d/**/_data, (ptrdiff_t) (len), \
			sizeof d -> d/**/_data, &d -> d/**/_cc); \
}
#else
#define	copyFTAMdata(base,len,d) \
{ \
    (void) copy_capped (base, d -> d##_data, (ptrdiff_t) (len), \
			sizeof d -> d##_data, &d -> d##_cc); \
}
#endif
#else
#define	copyFTAMdata(base,len,d)	bcopy (base, (char *) d, len)
#endif

struct ftamblk;
int	ftamlose (struct FTAMindication *fti, ...);
int fpktlose (struct ftamblk *fsb, ...);
int ftamoops (struct FTAMindication *fti, ...);

struct ftamblk {
	struct ftamblk *fsb_forw;	/* doubly-linked list */
	struct ftamblk *fsb_back;	/* .. */

	int	    fsb_fd;		/* association descriptor */

	short   fsb_flags;		/* our state */
#define	FSB_NULL	0x0000
#define	FSB_CONN	0x0001	/* connected */
#define	FSB_FINN	0x0002	/* other side wants to finish */
#define	FSB_INIT	0x0004	/* this side initiated the association */
#define	FSB_ASYN	0x0008	/* asynchronous */
#define	FSB_DECHARGE	0x0010	/* responder can using charging on deselect */
#define	FSB_CANCEL	0x0020	/* this side started F-CANCEL */
#define	FSB_COLLIDE	0x0040	/* collision */

	short   fsb_state;		/* more state */
#define	FSB_INITIALIZED	0	/* initialized */
#define	FSB_MANAGEMENT	1	/* management */
#define	FSB_BULKBEGIN	2	/*   bulk data begin */
#define	FSB_BULKEND	3	/*   bulk data end */
#define	FSB_DATAIDLE	4	/*   data transfer idle */
#define	FSB_LOCATE	5	/*   locate in progress */
#define	FSB_ERASE	6	/*   erase in progress */
#define	FSB_DATAREAD	7	/*   data transfer read */
#define	FSB_DATAWRITE	8	/*   data transfer write */
#define	FSB_DATAFIN1	9	/*   data transfer done */
#define	FSB_DATAFIN2	10	/* .. */
#define	FSB_DATACANCEL	11	/*   cancel in progress */

	int	    fsb_group;		/* group flags */

	int	    fsb_srequirements;	/* session requirements */
	int	    fsb_owned;		/* session tokens we own */
	int	    fsb_avail;		/* session tokens available */
	int	    fsb_settings;	/* initial and resync settings */
	long    fsb_ssn;		/* serial number */
	struct SSAPref fsb_connect;	/* session connection reference */
	int	    fsb_ssdusize;	/* largest atomic SSDU */

	int	    fsb_id;		/* FTAM context id */
	int	    fsb_prequirements;	/* presentation requirements */
	struct PSAPctxlist fsb_contexts;/* presentation contexts */
	struct FTAMcontentlist fsb_contents; /* FTAM document types */

	OID	    fsb_context;	/* application context */

	int	    fsb_fqos;		/* ftam-QoS */
	int	    fsb_class;		/* service-class */
	int	    fsb_units;		/* functional-units */
	/* mandatory functional-units */
#define	FUNITS_TRANSFER	(FUNIT_GROUPING)
#define	FUNITS_ACCESS	(FUNIT_READ | FUNIT_WRITE | FUNIT_ACCESS)
#define	FUNITS_MANAGE	(FUNIT_LIMITED | FUNIT_GROUPING)
#define	FUNITS_TM	(FUNIT_LIMITED | FUNIT_GROUPING)
#define	FUNITS_UNCONS	(0)

	int	    fsb_attrs;		/* attribute-groups */

	void	  (*fsb_indication)(int sd, struct FTAMindication *fti);	/* event handler */

	struct PSAPdata fsb_data;	/* for screwy BDT stuff */

	int	    fsb_cancelaction;	/* handle CANCEL collisions */
	PE	    fsb_cancelshared;
	struct FTAMdiagnostic *fsb_canceldiags;
	int	    fsb_cancelndiag;

	void	  (*fsb_trace)(int sd, char *event, char *fpdu, PE pe, int rw);	/* user-defined tracing function */
};
#define	NULLFSB		((struct ftamblk *) 0)

void freefsblk (struct ftamblk *fsb);
struct ftamblk *newfsblk (void), *findfsblk (int sd);

int	ftamlose (struct FTAMindication* fti, ...);
int	ftamoops (struct FTAMindication* fti, ...);
int	fpktlose (struct ftamblk *fsb, ...);

#ifndef	lint
#define	fsbtrace(fsb,a)	if ((fsb) -> fsb_trace) (*((fsb) -> fsb_trace)) a
#else
#define	fsbtrace(fsb,a)	FTraceHook a
#endif

struct pair {
	int	    p_mask;
	int	    p_bitno;
};

extern struct pair fclass_pairs[],
		funit_pairs[],
		fattr_pairs[],
		fname_pairs[],
		fmode_pairs[],
		frequested_pairs[],
		fpermitted_pairs[];

struct type_FTAM_Access__Control__List *acl2fpm (const struct ftamblk *fsb, struct FTAMacelement *fe, struct FTAMindication *fti);
int	fpm2acl (const struct ftamblk *fsb, const struct type_FTAM_Access__Control__List *fpm, struct FTAMacelement **fe, struct FTAMindication *fti);

struct type_FTAM_Read__Attributes *attr2fpm (const struct ftamblk *fsb, struct FTAMattributes *fa, struct FTAMindication *fti);
int	fpm2attr (const struct ftamblk *fsb, struct type_FTAM_Read__Attributes *fpm, struct FTAMattributes *fa, struct FTAMindication *fti);

PE	bits2fpm (const struct ftamblk *fsb, struct pair pairs[], const int actions, struct FTAMindication *fti);
int	fpm2bits (const struct ftamblk *fsb, struct pair pairs[], PE fpm, int *actions, struct FTAMindication *fti);

struct type_FTAM_Charging *chrg2fpm (const struct ftamblk *fsb, struct FTAMcharging *charging, struct FTAMindication *fti);
int	fpm2chrg (const struct ftamblk *fsb, const struct type_FTAM_Charging *fpm, struct FTAMcharging *charging, struct FTAMindication *fti);

struct type_FTAM_Concurrency__Access *conacc2fpm (const struct ftamblk *fsb, struct FTAMconcurrency *fc, struct FTAMindication *fti);
int	fpm2conacc (const struct ftamblk *fsb, const struct type_FTAM_Concurrency__Access *fpm, struct FTAMconcurrency *fc, struct FTAMindication *fti);

struct type_FTAM_Concurrency__Control *conctl2fpm (const struct ftamblk *fsb, struct FTAMconcurrency *fc, struct FTAMindication *fti);
int	fpm2conctl (const struct ftamblk *fsb, const struct type_FTAM_Concurrency__Control *fpm, struct FTAMconcurrency *fc, struct FTAMindication *fti);
#define	conctl_present(fc) \
    ((fc) -> fc_readlock != FLOCK_NOTREQD \
	|| (fc) -> fc_insertlock != FLOCK_NOTREQD \
	|| (fc) -> fc_replacelock != FLOCK_NOTREQD \
	|| (fc) -> fc_extendlock != FLOCK_NOTREQD \
	|| (fc) -> fc_eraselock != FLOCK_NOTREQD \
	|| (fc) -> fc_readattrlock != FLOCK_NOTREQD \
	|| (fc) -> fc_chngattrlock != FLOCK_NOTREQD \
	|| (fc) -> fc_deletelock != FLOCK_NOTREQD) \
 

struct type_FTAM_Diagnostic *diag2fpm (const struct ftamblk *fsb, const int magic, struct FTAMdiagnostic diag[], int ndiag, struct FTAMindication *fti);
int	fpm2diag (const struct ftamblk *fsb, const struct type_FTAM_Diagnostic *fpm, struct FTAMdiagnostic diag[], int *ndiag, struct FTAMindication *fti);

struct type_FTAM_FADU__Identity *faduid2fpm (const struct ftamblk *fsb, struct FADUidentity *fa, struct FTAMindication *fti);
int	fpm2faduid (const struct ftamblk *fsb, const struct type_FTAM_FADU__Identity *fpm, struct FADUidentity *fa, struct FTAMindication *fti);

struct type_FTAM_Access__Passwords *pass2fpm (const struct ftamblk *fsb, struct FTAMpasswords *fp, struct FTAMindication *fti);
int	fpm2pass (const struct ftamblk *fsb, const struct type_FTAM_Access__Passwords *fpm, struct FTAMpasswords *fp, struct FTAMindication *fti);
#define	passes_present(fp) \
    ((fp) -> fp_read || (fp) -> fp_insert || (fp) -> fp_replace \
	|| (fp) -> fp_extend || (fp) -> fp_erase || (fp) -> fp_readattr \
	|| (fp) -> fp_chngattr || (fp) -> fp_delete) \
 
struct type_FTAM_Shared__ASE__Information *shared2fpm (const struct ftamblk *fsb, PE sharedASE, struct FTAMindication *fti);
int	fpm2shared (const struct ftamblk *fsb, const struct type_FTAM_Shared__ASE__Information *fpm, PE *sharedASE, struct FTAMindication *fti);

int	acs2ftamlose (const struct ftamblk *fsb, struct FTAMindication *fti, char *event, struct AcSAPabort *aca), acs2ftamabort (struct ftamblk *fsb, struct AcSAPabort *aca, struct FTAMindication *fti);
int	ps2ftamlose (const struct ftamblk *fsb, struct FTAMindication *fti, char *event, struct PSAPabort *pa);

int FWaitRequestAux (struct ftamblk *fsb, const int secs, struct FTAMindication *fti);
int FCancelResponseAux (struct ftamblk *fsb, const int action, PE sharedASE, struct FTAMdiagnostic diag[], int ndiag, struct FTAMindication *fti);
int FAbortRequestAux (struct ftamblk *fsb, const int id, const int action, struct FTAMdiagnostic diag[], int ndiag, struct FTAMindication *fti);
