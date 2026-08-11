/* sys52-sun.h - site configuration file for SUN's SVR2 Compatiblity Package */





#ifndef	_CONFIG_
#define	_CONFIG_

#define	SYS5			/* AT&T UNIX emulation */

#define	VSPRINTF		/* has vprintf(3s) routines */

#define	TCP			/* has TCP/IP */
#define	SOCKETS			/*   provided by sockets */

#define	NFS			/* network filesystem -- has getdirentries() */

#endif
