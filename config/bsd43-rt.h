/* bsd43-rt.h - site configuration file for RT PC running 4.3BSD UNIX */
/*
 * Contributed by Jacob Rekhter, T.J. Watson Research Center, IBM Corp.
 */

#ifndef	_CONFIG_
#define	_CONFIG_

#define	BSD42			/* Berkeley UNIX */
#define	WRITEV			/*   real Berkeley UNIX */
#define	BSD43			/*   4.3BSD or later */

#define	TCP			/* has TCP/IP (of course) */
#define	SOCKETS			/*   provided by sockets */

#define	RT			/* RT/PC */

#if   defined(__STDC__) && defined(__HIGHC__)
/* hc thinks it's ANSI C, but it isn't! */
#undef	__STDC__
#endif

#endif
