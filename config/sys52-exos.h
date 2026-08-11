/* sys52-exos.h - site configuration file for SVR2 with EXOS 8044 card */

#ifndef	_CONFIG_
#define	_CONFIG_

#define	SYS5			/* AT&T UNIX */

#define	VSPRINTF		/* has vprintf(3s) routines */

#define	TCP			/* has TCP/IP */
#define	EXOS			/*   provided by EXOS 8044 card */

#ifdef	sgi			/* screwy compiler! */
#define	register
#endif

#endif
