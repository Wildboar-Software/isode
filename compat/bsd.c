/* bsd.c - BSD/GNU APIs hidden by ISO C23 + glibc */

#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "general.h"
#include "manifest.h"

/*
 * ISO C23 / glibc hides initgroups(3).  Walk the group database with
 * POSIX getgrent() and install the resulting list via setgroups(2).
 */
int
initgroups (const char *user, gid_t group)
{
	gid_t	*groups;
	int	ng = 0;
	int	maxg;
	int	i;
	long	nmax;
	struct group *gr;
	char	**mem;

	if (user == NULL) {
		errno = EINVAL;
		return -1;
	}

	nmax = sysconf (_SC_NGROUPS_MAX);
	if (nmax < 1)
		nmax = 32;
	if (nmax > 1024)
		nmax = 1024;

	groups = malloc ((size_t) nmax * sizeof *groups);
	if (groups == NULL)
		return -1;

	maxg = (int) nmax;
	groups[ng++] = group;

	setgrent ();
	while ((gr = getgrent ()) != NULL && ng < maxg) {
		if (gr -> gr_gid == group || gr -> gr_mem == NULL)
			continue;
		for (mem = gr -> gr_mem; *mem != NULL; mem++) {
			if (strcmp (*mem, user) != 0)
				continue;
			for (i = 0; i < ng; i++) {
				if (groups[i] == gr -> gr_gid)
					break;
			}
			if (i == ng)
				groups[ng++] = gr -> gr_gid;
			break;
		}
	}
	endgrent ();

	i = setgroups ((size_t) ng, groups);
	free (groups);
	return i;
}

/*
 * ISO C23 / glibc hides flock(2).  Approximate BSD flock with POSIX
 * fcntl record locks on the whole file.
 */
int
flock (int fd, int operation)
{
	struct flock fl;
	int	cmd;

	memset (&fl, 0, sizeof fl);
	switch (operation & (LOCK_SH | LOCK_EX | LOCK_UN)) {
	case LOCK_SH:
		fl.l_type = F_RDLCK;
		break;
	case LOCK_EX:
		fl.l_type = F_WRLCK;
		break;
	case LOCK_UN:
		fl.l_type = F_UNLCK;
		break;
	default:
		errno = EINVAL;
		return -1;
	}
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;
	cmd = (operation & LOCK_NB) ? F_SETLK : F_SETLKW;
	return fcntl (fd, cmd, &fl);
}
