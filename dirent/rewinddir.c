/*
	rewinddir -- rewind a directory stream

	This is not simply a call to seekdir(), because seekdir()
	will use the current buffer whenever possible and we need
	rewinddir() to forget about buffered data.
*/
#include <sys/errno.h>
#include <sys/types.h>
#include <unistd.h>
#include "usr.dirent.h"
static int _rewinddir_stub(void);


#ifndef	GETDENTS
extern off_t	lseek(int fd, off_t offset, int whence);

extern int	errno;

#ifndef NULL
#define	NULL	0
#endif

#ifndef SEEK_SET
#define	SEEK_SET	0
#endif

/**
 * @param dirp stream from opendir()
 */
void rewinddir(DIR *dirp) {
	if ( dirp == NULL || dirp->dd_buf == NULL ) {
		errno = EFAULT;
		return;			/* invalid pointer */
	}
	dirp->dd_loc = dirp->dd_size = 0;	/* invalidate buffer */
	lseek( dirfd(dirp), (off_t)0, SEEK_SET );	/* may set errno */
}
#else
static int _rewinddir_stub(void) {
	;
}
#endif
