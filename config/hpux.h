/* hpux.h - site configuration file for HP-UX */

#ifndef _CONFIG_
#define _CONFIG_

#define SYS5                    /* AT&T UNIX */
#define HPUX                    /*   with HP's enhancements */
#define VSPRINTF                /* libc includes vsprintf and vfprintf */

#define TCP                     /* has TCP/IP */
#define SOCKETS                 /*   provided by sockets */

#define	GETDENTS		/* has getdirent(2) call */

#endif
