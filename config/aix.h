/* aix.h - site configuration file for IBM AIX 3.2 on RS6000 Workstation */

#ifndef	_CONFIG_
#define	_CONFIG_

#define	SYS5			/* AT&T UNIX */
#define SVR4			/*   Systems V

#define	VSPRINTF		/* has vprintf(3s) routines */

#define GETDENTS

#define	TCP			/* has TCP/IP */
#define	SOCKETS			/*   provided by sockets */

/*				If you have OSIMF you can do TP4
				Using the XTI package (alpha test).
				Add -lxti to LSOCKET in CONFIG.make
				Contact bug-isode@xtel.co.uk for latest info.
#define TP4
#define XTI_TP
*/

#define aiws			/* AIX RS6000 workstation ONLY */

#endif
