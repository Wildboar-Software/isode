/* bsd44.h - site configuration file for 4.4BSD UNIX */

#ifndef	_CONFIG_
#define	_CONFIG_

#define	BSD42			/* Berkeley UNIX */
#define	WRITEV			/*   real Berkeley UNIX */
#define	BSD43			/*   4.3BSD or later */
#define	BSD44			/*   4.4BSD to be precise! */

#define X25
#define UBC_X25
#define UBC_X25_WRITEV

#define	VSPRINTF		/* has vprintf(3s) routines */

#define	TCP			/* has TCP/IP (of course) */
#define	SOCKETS			/*   provided by sockets */

#define	TP4			/* has TP4 */
#define	BSD_TP4			/*   provided by UCB/UWisc */

#define	GETDENTS		/* has getdirent(2) call */
#define	NFS			/* network file system -- has getdirentries */

#endif
