/* usr.dirent.h - ISODE's version of DAG's SVR3 directory access routines */

/*
	<dirent.h> -- definitions for SVR3 directory access routines

	last edit:	25-Apr-1987	D A Gwyn

	Prerequisite:	<sys/types.h>
*/

#ifndef	_CONFIG_
#include "config.h"		/* system-specific configuration */
#endif
#ifndef	GETDENTS
#if	defined(SUNOS4)
#define	GETDENTS
#endif
#endif

#ifdef	GETDENTS
#include <dirent.h>
#else
#include	"sys.dirent.h"

#define	DIRBUF		8192		/* buffer size for fs-indep. dirs */
/* must in general be larger than the filesystem buffer size */

typedef struct {
	int	dd_fd;			/* file descriptor */
	int	dd_loc;			/* offset in block */
	int	dd_size;		/* amount of valid data */
	char	*dd_buf;		/* -> directory block */
}	DIR;			/* stream data from opendir() */

#define	opendir		_opendir
#define	readdir		_readdir
#define	telldir		_telldir
#define	seekdir		_seekdir
#define	rewinddir	_rewinddir
#define	closedir	_closedir
#define	dirfd(dp)	((dp)->dd_fd)

extern DIR		*opendir(const char *);
extern struct dirent	*readdir(DIR *);
extern off_t		telldir(DIR *);
extern void		seekdir(DIR *, off_t);
extern void		rewinddir(DIR *);
extern int		closedir(DIR *);

#ifndef NULL
#define	NULL	0			/* DAG -- added for convenience */
#endif

#define	getcwd		_getcwd

extern char             *getcwd (char *, size_t);

#define	scandir	_scandir
#define	alphasort _alphasort
#endif	/* not GETDENTS */

extern char *direntversion;
