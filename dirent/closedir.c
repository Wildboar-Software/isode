/* closedir -- close a directory stream */
#include	<sys/errno.h>
#include	<sys/types.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	"usr.dirent.h"

#ifndef	GETDENTS
typedef char	*pointer;		/* (void *) if you have it */

extern void	free(pointer);
extern int close(int fd);

extern int	errno;

#ifndef NULL
#define	NULL	0
#endif

int closedir(DIR *dirp) {
	int	fd;

	if ( dirp == NULL || dirp->dd_buf == NULL ) {
		errno = EFAULT;
		return -1;		/* invalid pointer */
	}

	fd = dirfd(dirp);
	free( (pointer)dirp->dd_buf );
	free( (pointer)dirp );
	return close( fd );
}
#else
int _closedir_stub(void) {
	;
}
#endif
