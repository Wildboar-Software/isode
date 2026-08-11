/* sys54.h - site configuration file for generic SVR4 */

#ifndef	_CONFIG_
#define	_CONFIG_

#define	SYS5			/* AT&T UNIX */
#define SVR4			/*   Systems V

/* If you have the UCB extensions ...
#define SVR4_UCB
*/

#define	VSPRINTF		/* has vprintf(3s) routines */

#define GETDENTS

#define	TCP			/* has TCP/IP */
#define	SOCKETS			/*   provided by sockets */

#endif
