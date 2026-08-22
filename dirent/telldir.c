/*
	telldir -- report directory stream position
	NOTE:	4.nBSD directory compaction makes seekdir() & telldir()
		practically impossible to do right.  Avoid using them!
*/

#include	<sys/errno.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	"usr.dirent.h"
static int _telldir_stub(void);


#ifndef	GETDENTS
extern off_t	lseek(int fd, off_t offset, int whence);

extern int	errno;

#ifndef SEEK_CUR
#define	SEEK_CUR	1
#endif

/**
 * @param dirp stream from opendir()
 * @returns offset of next entry
 */
off_t telldir( DIR *dirp ) {
	if ( dirp == NULL || dirp->dd_buf == NULL ) {
		errno = EFAULT;
		return -1;		/* invalid pointer */
	}

	if ( dirp->dd_loc < dirp->dd_size )	/* valid index */
		return ((struct dirent *)&dirp->dd_buf[dirp->dd_loc])->d_off;
	else				/* beginning of next directory block */
		return lseek( dirfd(dirp), (off_t)0, SEEK_CUR );
}
#else
static int _telldir_stub(void) {
	;
}
#endif
