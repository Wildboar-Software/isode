/* mips.h - site configuration file for RISC/OS */

#ifndef	_CONFIG_
#define	_CONFIG_

#define	BSD42			/* Berkeley UNIX */
#define	WRITEV			/*   real Berkeley UNIX */
#define	BSD43			/*   4.3BSD or later */

#define	TCP			/* has TCP/IP (of course) */
#define	SOCKETS			/*   provided by sockets */

#define	MIPS			/* RISC/OS */

#define	NFS			/* network file system -- has getdirentries */

#define	BIND

#endif
