/* sys.file.h - system independent sys/file.h */

#ifndef _ISODE_SYS_FILE_H
#define _ISODE_SYS_FILE_H

#include "general.h"

/* Beware the ordering is important to avoid symbol clashes */

#ifndef SVR4_UCB
#include <sys/ioctl.h>
#endif

#ifdef  BSD42
#include <sys/file.h>
#else
#ifdef  SYS5
#include <fcntl.h>
#else
#include <sys/fcntl.h>
#endif
#endif

#if	defined(SYS5)
#include <termio.h>
#endif
#include <termios.h>

#endif
