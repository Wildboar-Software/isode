/* sys52-rt.h - site configuration file for RT PC running AIX */

/*
 * 
 *
 * Contributed by by Jacob Rekhter, T.J. Watson Research Center, IBM Corp.
 *
 *
 * 
 *
 *
 *
 */



#ifndef	_CONFIG_
#define	_CONFIG_

#define	SYS5			/* AT&T UNIX */
#define	AIX			/*   with IBMs running AIX */
#define	RT			/*   on the RT/PC */

#define	VSPRINTF		/* has vprintf(3s) routines */

#define	TCP			/* has TCP/IP */
#define	SOCKETS			/*   provided by sockets */

#endif
