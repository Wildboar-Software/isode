/* ftamuser.h - include file for interactive FTAM initiator */

#include "ftamsbr.h"

extern int  ontty;
extern int  interrupted;

void	adios (char *what, char *fmt, ...);
void	advise (char *what, char *fmt, ...);
int getftamline(char* prompt, char* buffer);

#ifndef	BRIDGE
int	ask (char *fmt, ...), _getline ();
#endif

extern int  ftamfd;
#ifdef	BRIDGE
extern int  dataconn ();
#endif

extern char *host;
extern char *user;
extern char *account;
#ifndef	BRIDGE
extern char *userdn;
extern char *storename;
#endif

extern int  bell;
extern int  concurrency;	/* Olivier Dubois */
extern int  debug;
extern int  globbing;
extern int  hash;
extern int  marks;
extern int  omode;
extern int  query;
extern int  runcom;
extern int  tmode;
extern int  trace;
extern int  verbose;
extern int  watch;

extern char *myhome;
extern char *myuser;

extern int  realstore;
#define	RFS_UNKNOWN	0
#define	RFS_UNIX	1

extern char *rs_unknown;
extern char *rs_support;

extern char *rcwd;

extern struct QOStype myqos;

char   *str2file ();

/* DISPATCH */

struct dispatch {
	char   *ds_name;
	int (*ds_fnx)(char **vec);

	int	    ds_flags;
#define	DS_NULL		0x00
#define	DS_OPEN		0x01	/* association required */
#define	DS_CLOSE	0x02	/* association avoided */
#define	DS_MODES	0x04	/* class/units meaningful */

	int	    ds_class;
	int	    ds_units;

	char   *ds_help;
};

struct dispatch *getds ();

/* FTAM */

#define	UMASK	"\020\01READ\02WRITE\03ACCESS\04LIMITED\05ENHANCED\06GROUPING\
\07RECOVERY\08RESTART"

extern OID context;
extern int fqos;
extern int ftam_class;
extern int units;
extern int attrs;
extern int fadusize;

extern struct vfsmap vfs[];	/* ordering depends on char *tmodes[] */
#define	VFS_DEF	0		/* try to default it */
#define	VFS_UBF	1		/* offset to unstructured binary file */
#define	VFS_UTF	2		/*   ..	     unstructured text file */
#define	VFS_FDF	3		/*   ..	     file directory file */

extern struct vfsmap *myvf;

void	ftam_advise (), ftam_chrg (), ftam_diag (), ftam_watch ();

/* FILES */

struct filent {
	char   *fi_name;
	OID	    fi_oid;

	char   *fi_entry;

	struct filent *fi_next;
};

extern int  toomany;

extern int  nfilent;
extern struct filent *filents;

int	fdffnx ();

/* GLOB */

extern int   xglobbed;
extern char *globerr;

int	blkfree (), blklen ();
char  **blkcpy ();

char   *xglob1val ();
char  **xglob ();
void rcinit (void);

int getvf (
	char *src,
	char *dst,
	struct FADUidentity *faduid,
	struct vfsmap *vf,
	int (*wfnx) (int fd, struct PSAPdata *px, int status)
);
int isdir (char *dir, char *dp, int silent);
int	ncols (FILE *fp);
int f_cd (char **vec);
int f_close (char **vec);
int f_ls (char **vec);
int f_pwd (char **vec);
int f_lcd (char **vec);

int fdf_p2names (int fd, PE bits, int *names, struct FTAMindication *fti);
int fdf_names2p (int fd, int names, PE *bits, struct FTAMindication *fti);
int fdf_attrs2d (int fd, struct FTAMattributes *fa, struct type_FTAM_Read__Attributes **attrs, struct FTAMindication *fti);
int fdf_d2attrs (int fd, struct type_FTAM_Read__Attributes *attrs, struct FTAMattributes *fa, struct FTAMindication *fti);
void timer (int cc, char *action);
int	de2fadu (PE pe, int concat);

extern int  errno;
extern char *isodeversion;
#ifdef	BRIDGE
extern char ftam_error[];
#endif
