/* apollo.h - site configuration file for Apollo */
/*
 * Contributed by John Brezak, Apollo Computer, Inc.
 */

#ifndef	_CONFIG_
#define	_CONFIG_

#define	BSD42			/* Berkeley UNIX */
#define	WRITEV			/*   real Berkeley UNIX */

#define VSPRINTF		/* has vprintf(3s) routines */

#define	TCP			/* has TCP/IP (of course) */
#define	SOCKETS			/*   provided by sockets */

#ifdef	notdef			/* Don Preuss at Apollo says no longer needed*/
#ifdef __STDC__                 /* thinks it's ANSI C, but it isn't! */
#undef __STDC__
#endif
#endif

#endif
