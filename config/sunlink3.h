/* sunlink3.h - site configuration file for SunLink OSI 5.2 on SunOS 3 */





#ifndef	_CONFIG_
#define	_CONFIG_

#define	BSD42			/* Berkeley UNIX */
#define	WRITEV			/*   real Berkeley UNIX */

#define	TCP			/* has TCP/IP (of course) */
#define	SOCKETS			/*   provided by sockets */

#define	X25			/* has X.25 */
#define	SUN_X25			/*   provided by SunLink X.25 */

#define TP4			/* has TP4 */
#define SUN_TP4			/*   provided by SunLink OSI */
#define	SUNLINK_5_2		/*     define if using SunLink OSI release 5.2
				       or greater */

#define	NFS			/* network filesystem -- has getdirentries() */

#endif
