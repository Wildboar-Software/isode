/* sys52-exos.h - site configuration file for SYS5 release 2 with EXOS 8044 */




#ifndef	_CONFIG_
#define	_CONFIG_

#define	SYS5			/* AT&T UNIX */

#define	TCP			/* has TCP/IP */
#define	EXOS			/* EXOS card */

#ifdef	sgi			/* screwy compiler! */
#define	register
#endif

#define	VSPRINTF		/* has vprintf(3s) routines */

#endif
