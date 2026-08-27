/* ftamsystem.h - include file for FTAM responder */

#include <errno.h>
#include "ftamsbr.h"
#include "logger.h"

#define	SCPYN(a,b)	strncpy ((a), (b), sizeof (a))

/* SERVER */

extern int  ftamfd;

extern int  cflag;
extern int  debug;
extern char *myname;

void	ftam_adios (const struct FTAMabort *fta, char *event);
void	ftam_advise (const struct FTAMabort *fta, char *event);
void	ftam_diag (struct FTAMdiagnostic diag[], const int ndiag);

void	adios (char *, char *, ...);
void	advise (int, char *, char *, ...);

/*    UNIX DATA */

extern int  myuid;

extern int  myhomelen;
extern char myhome[];

extern dev_t null_dev;
extern ino_t null_ino;

/*    VFS DATA */

#define	NMAX	8		/* too painful to get right! */

#ifndef	NGROUPS
#define	NACCT	32
#else
#define	NACCT	(NGROUPS + 20)
#endif

extern struct vfsmap vfs[];	/* ordering affects default action in st2vfs()
				   put preferential entries towards the end */
#define	VFS_UBF	0		/* offset to FTAM-3 */
#define	VFS_UTF	1		/*  ..       FTAM-1 */
#define	VFS_FDF	2		/*  ..       NIST-9 */
#define VFS_FDF_OLD 3		/*  ..       NBS-9 with old OID (prov.) */

/*    REGIME DATA */

extern int level;
extern int ftam_class;
extern int units;
extern int attrs;
extern int fadusize;

/*    ACTIVITY DATA */

extern int  myfd;
extern char *myfile;
extern struct stat  myst;
extern int  statok;

extern struct vfsmap   *myvf;	/* active contents type */
extern caddr_t myparam;		/* .. */

extern int  myaccess;		/* current access request */

extern char *initiator;		/* current initiator identity */

extern struct FADUidentity mylocation;/* current location */

extern int  mymode;		/* current processing mode */
extern int  myoperation;	/* .. */

#ifdef	notdef
extern AEI mycalling;		/* current calling AET */
extern AEI myresponding;	/* current responding AET */
#endif

extern char *account;		/* current account */
extern int  mygid;		/* "inner" account */

extern int  mylock;		/* current concurrency control */
extern struct FTAMconcurrency myconctl;/* .. */

extern int mylockstyle;		/* current locking style */

extern int  mycontext;		/* current access context */
extern int  mylevel;		/* .. */

#ifndef	SYS5
#define	unlock()	if (mylock) (void) flock (myfd, LOCK_UN); else
#else
#define	unlock() \
    if (mylock) { \
	struct flock fs; \
 \
	fs.l_type = F_UNLCK; \
	fs.l_whence = L_SET; \
	fs.l_start = fs.l_len = 0; \
	(void) fcntl (myfd, F_SETLK, &fs); \
    } \
    else
#endif

#ifdef	BRIDGE
/* FTP interface routines and variables */

extern char *ftp_error;

int	ftp_exist (const char *filename), ftp_delete (const char *file), ftp_mkdir (const char *dir), ftp_rename (const char *from, const char *to), ftp_type (const int modeX),
	ftp_write (const char *file), ftp_append (const char *file), ftp_read (const char *file), ftp_ls (const char *dir), ftp_login (char *host, const char *user, const char *passwd, const char *acct),
	ftp_quit (void), ftp_abort (void), ftp_reply (void);
#endif

void ftam_start (struct FTAMstart *fts);
// void ftam_bulkbeginindication (struct FTAMgroup *ftg);
void ftam_bulkbeginindication (const struct FTAMgroup *ftg);
void ftam_readwriteindication (const struct FTAMreadwrite *ftrw);
void ftam_dataindication (const struct PSAPdata *px);
void ftam_dataendindication (const struct FTAMdataend *ftda);
void ftam_cancelindication (const struct FTAMcancel *ftcn);
void ftam_transendindication (const struct FTAMtransend *ftre);
void ftam_bulkendindication (const struct FTAMgroup *ftg);
void ftam_managementindication (const struct FTAMgroup *ftg);
void ftam_selection (const struct FTAMgroup *ftg, struct FTAMgroup *ftm);
int ftam_indication (const struct FTAMindication *fti);
int	readattrs (
	const int attrnames,
	struct FTAMattributes *fa,
	OID proposed,
	PE parameter,
	char *file,
	struct stat *st,
	struct FTAMdiagnostic **diags
);
int fdf_p2names (const int fd, PE bits, int *names, struct FTAMindication *fti);
int fdf_names2p (const int fd, const int names, PE *bits, struct FTAMindication *fti);
int fdf_attrs2d (const int fd, const struct FTAMattributes *fa, struct type_FTAM_Read__Attributes **attrs, struct FTAMindication *fti);
int fdf_d2attrs (const int fd, struct type_FTAM_Read__Attributes *attrs, struct FTAMattributes *fa, struct FTAMindication *fti);
void timer (const int cc, const char *action);
int closewtmp (void);
void rcinit (void);
