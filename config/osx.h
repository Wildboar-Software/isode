/* osx.h - site configuration file for Olivetti LSX 30xx */

#ifndef _CONFIG_
#define _CONFIG_

#define BSD42                   /* Berkeley UNIX */
#define WRITEV                  /* real Berkeley UNIX (sort of) */
#define XOS_2			/*  Olivetti's version */

#define VSPRINTF

#define TCP                     /* has TCP/IP (of course) */
#define SOCKETS                 /* 4.2BSD sockets */
#define NOGOSIP
#define TSBRIDGE

#define NFS
#endif
