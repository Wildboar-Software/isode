/* ultrix.h - site configuration file for Ultrix version greater than 3.1 */





#ifndef	_CONFIG_
#define	_CONFIG_

#define	BSD42			/* Berkeley UNIX */
#define	WRITEV			/*   real Berkeley UNIX */
#define BSD43			/* 4.3BSD networking or later */

#define	VSPRINTF		/* has vprintf(3s) routines */

#define	TCP			/* has TCP/IP (of course) */
#define	SOCKETS			/*   provided by sockets */
#define BIND			/* has h_addr_list in netdb.h */

#define	GETDENTS		/* has getdirent(2) call */

#endif
