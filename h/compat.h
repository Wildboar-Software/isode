#ifndef COMPAT_H
#define COMPAT_H
#include <sys/select.h>

int	xselect (int nfds, fd_set* rfds, fd_set* wfds, fd_set* efds, int secs);

#endif /* COMPAT_H */
