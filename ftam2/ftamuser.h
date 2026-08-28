/* ftamuser.h - include file for interactive FTAM initiator */

#include "ftamsbr.h"

extern int  ontty;
extern int  interrupted;

void	adios (char *what, const char *fmt, ...);
void	advise (char *what, const char *fmt, ...);
int getftamline(const char* prompt, const char* buffer);
char *default_prompt (void);

#ifndef	BRIDGE
int	ask (const char *fmt, ...);
#endif

extern int  ftamfd;
#ifdef	BRIDGE
extern int  dataconn (const char *modeX);
int f_type (int mode);
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

char   *str2file (char *s);

/* DISPATCH */

struct dispatch {
	const char   *ds_name;
	int (*ds_fnx)(char **vec);

	int	    ds_flags;
#define	DS_NULL		0x00
#define	DS_OPEN		0x01	/* association required */
#define	DS_CLOSE	0x02	/* association avoided */
#define	DS_MODES	0x04	/* class/units meaningful */

	int	    ds_class;
	int	    ds_units;

	const char   *ds_help;
};

struct dispatch *getds (char *name);

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

void	ftam_advise (struct FTAMabort *fta, char *event);
void	ftam_chrg (struct FTAMcharging *charges);
void	ftam_diag (struct FTAMdiagnostic diag[], int ndiag, const int peer, const int action);

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

int	fdffnx (int fd, struct PSAPdata *px, int status);

/* GLOB */

extern int   xglobbed;
extern char *globerr;

int	blkfree (char **av0), blklen (char **av);
char  **blkcpy (char **oav, char **bv);

char   *xglob1val (char *v, const int remote);
char  **xglob (char **v, const int remote);
void rcinit (void);

int getvf (
	char *src,
	char *dst,
	struct FADUidentity *faduid,
	const struct vfsmap *vf,
	int (*wfnx) (int fd, struct PSAPdata *px, int status)
);
int isdir (const char *dir, char *dp, const int silent);
int	ncols (FILE *fp);
int f_cd (char **vec);
int f_close (char **vec);
int f_ls (char **vec);
int f_pwd (char **vec);
int f_lcd (char **vec);
int f_put (char **vec);
int f_chgrp (char **vec);
int f_echo (char **vec);
int f_fls (char **vec);
int f_get (char **vec);
int f_mkdir (char **vec);
int f_mv (char **vec);
int f_open (char **vec);
int f_rm (char **vec);
int f_status (char **vec);
int f_quit (char **vec);

int fdf_p2names (const int fd, PE bits, int *names, struct FTAMindication *fti);
int fdf_names2p (const int fd, const int names, PE *bits, struct FTAMindication *fti);
int fdf_attrs2d (const int fd, struct FTAMattributes *fa, struct type_FTAM_Read__Attributes **attrs, struct FTAMindication *fti);
int fdf_d2attrs (const int fd, struct type_FTAM_Read__Attributes *attrs, struct FTAMattributes *fa, struct FTAMindication *fti);
void timer (int cc, const char *action);
int	de2fadu (PE pe, const int concat);

extern char *isodeversion;
#ifdef	BRIDGE
extern char ftam_error[];
#endif
