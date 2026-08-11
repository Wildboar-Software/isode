/* osi.h - site configuration file for osi (SunOS) */

#ifndef	_CONFIG_
#define	_CONFIG_

#define	BSD42			/* Berkeley UNIX */
#define	WRITEV			/*   real Berkeley UNIX */

#define VSPRINTF		/* libc has vsprintf */

#define	TCP			/* has TCP/IP (of course) */
#define	SOCKETS			/*   provided by sockets */

#define X25			/* support for X.25 */
#define SUN_X25			/* using SunLink X.25 V5.2 */

#define	NFS			/* network filesystem -- has getdirentries() */

#define	ANON	"anon"		/* guest login for ftam */

#endif
