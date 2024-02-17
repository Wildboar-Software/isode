/* internet.c - TCP/IP abstractions */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/internet.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/compat/RCS/internet.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 *
 * $Log: internet.c,v $
 * Revision 9.0  1992/06/16  12:07:00  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


/* LINTLIBRARY */

#include <unistd.h>
#define getdtablesize() (sysconf (_SC_OPEN_MAX))
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include "general.h"
#include "manifest.h"
#include "tailor.h"

#ifdef ULTRIX_X25_DEMSA
#include "/usr/include/x25.h"
#include <demsa.h>
#include <demsb.h>
#endif

/*  */

#ifdef	TCP
#include "internet.h"




/*    Berkeley UNIX: 4.2 */

#if 	defined(SOCKETS) && !defined(TLI_TCP)

/* For real networking, nothing is better than 4BSD! */


int
start_tcp_client (struct sockaddr_in *sock, int priv) {
	int    port;
	int     eindex,
			sd;
#if	defined(BSD43) || defined(SVR4)
	int	    onoff;
#endif

	if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == NOTOK) {
		SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("socket"));
		return NOTOK;
	}

	if (sock == NULL)
		goto got_socket;

	for (port = IPPORT_RESERVED - priv;; priv ? port-- : port++) {
		sock -> sin_port = htons ((u_short) port);

		if (bind (sd, (struct sockaddr *) sock, sizeof *sock) != NOTOK)
			break;

		switch (errno) {
		case EADDRINUSE:
			if (!priv || (port >= IPPORT_RESERVED / 2))
				continue;	/* else fall */

		case EADDRNOTAVAIL:
		default:
			eindex = errno;
			SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("bind"));
			close_tcp_socket (sd);
			errno = eindex;
			return NOTOK;
		}
	}

got_socket:
	;
#if	!defined(BSD43) && !defined(SVR4)
	if (setsockopt (sd, SOL_SOCKET, SO_KEEPALIVE, NULLCP, 0) == NOTOK)
		SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("set SO_KEEPALIVE"));
#else
	onoff = 1;
	if (setsockopt (sd, SOL_SOCKET, SO_KEEPALIVE, (char *) &onoff,
					sizeof onoff) == NOTOK)
		SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("set SO_KEEPALIVE"));
#endif

	return sd;
}

/*  */

int
start_tcp_server (struct sockaddr_in *sock, int backlog, int opt1, int opt2) {
	int    port;
	int     eindex,
			sd;
#ifdef ULTRIX_X25_DEMSA
	int our_count;
#endif
#if defined(BSD43) || defined(SVR4)
	int	    onoff;
#endif

	if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == NOTOK) {
		SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("socket"));
		return NOTOK;
	}

#ifdef ULTRIX_X25_DEMSA
	if ( X25RegisterFD(sd,rhandler,whandler,xhandler,sd) < NULL ) {
		SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("X25RegisterFD"));
		return NOTOK;
	}
	if(sd >= our_TABLE_SIZE || (our_count=getdtablesize()) >=  our_TABLE_SIZE) {
		SLOG (compat_log, LLOG_EXCEPTIONS, "invalid",
			  ("getdtablesize:increase our_TABLE_SIZE in demsb.h"));
		return NOTOK;
	}
	list_of_conn_desc[sd].TCP_IP_OR_X25_SOCKET = our_TCP_IP_SOCKET;
	list_of_conn_desc[sd].descriptor           = sd;
#endif

	if (sock -> sin_port != 0) {
		if (bind (sd, (struct sockaddr *) sock, sizeof *sock) != NOTOK)
			goto got_socket;

		eindex = errno;
		SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("bind"));
		close_tcp_socket (sd);
		errno = eindex;
		return NOTOK;
	}

	for (port = IPPORT_RESERVED;; port++) {
		sock -> sin_port = htons ((u_short) port);

		if (bind (sd, (struct sockaddr *) sock, sizeof *sock) != NOTOK)
			break;

		switch (errno) {
		case EADDRINUSE:
			continue;

		case EADDRNOTAVAIL:
		default:
			eindex = errno;
			SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("bind"));
			close_tcp_socket (sd);
			errno = eindex;
			return NOTOK;
		}
	}

got_socket:
	;
#if	!defined(BSD43) && !defined(SVR4)
	if (setsockopt (sd, SOL_SOCKET, SO_KEEPALIVE, NULLCP, 0) == NOTOK)
		SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("set SO_KEEPALIVE"));
	if (opt1 && setsockopt (sd, SOL_SOCKET, opt1, NULLCP, 0) == NOTOK)
		SLOG (compat_log, LLOG_EXCEPTIONS, "failed",
			  ("set socket option 0x%x", opt1));
	if (opt2 && setsockopt (sd, SOL_SOCKET, opt2, NULLCP, 0) == NOTOK)
		SLOG (compat_log, LLOG_EXCEPTIONS, "failed",
			  ("set socket option 0x%x", opt2));
#else
	onoff = 1;
	if (setsockopt (sd, SOL_SOCKET, SO_KEEPALIVE, (char *) &onoff,
					sizeof onoff) == NOTOK)
		SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("set SO_KEEPALIVE"));
	if (opt1
			&& setsockopt (sd, SOL_SOCKET, opt1, (char *) &onoff, sizeof onoff)
			== NOTOK)
		SLOG (compat_log, LLOG_EXCEPTIONS, "failed",
			  ("set socket option 0x%x", opt1));
	if (opt2
			&& setsockopt (sd, SOL_SOCKET, opt2, (char *) &onoff, sizeof onoff)
			== NOTOK)
		SLOG (compat_log, LLOG_EXCEPTIONS, "failed",
			  ("set socket option 0x%x", opt2));
#endif

	listen (sd, backlog);

	return sd;
}

/*  */

int	join_tcp_client (fd, sock)
int	fd;
struct sockaddr_in *sock;
{
	int     eindex,
			len = sizeof *sock,
			result;

#ifdef ULTRIX_X25_DEMSA
	int our_count;
#endif


	if ((result = accept (fd, (struct sockaddr *) sock, &len)) == NOTOK) {
		eindex = errno;
		SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("accept"));
		errno = eindex;
	}
#ifdef ULTRIX_X25_DEMSA
	else {
		/* this descriptor will be used for the accepted connection */
		/* so we register the descriptor result                     */
		if ( X25RegisterFD(result,rhandler,whandler,xhandler,result) < NULL) {
			SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("X25RegisterFD"));
			return NOTOK;
		}
		if(result>=our_TABLE_SIZE || (our_count=getdtablesize())>=our_TABLE_SIZE) {
			SLOG (compat_log, LLOG_EXCEPTIONS, "invalid",
				  ("getdtablesize:increase our_TABLE_SIZE in demsb.h"));
			return NOTOK;
		}
		list_of_conn_desc[result].TCP_IP_OR_X25_SOCKET = our_TCP_IP_SOCKET;
		list_of_conn_desc[result].descriptor           = result;
	}
#endif


	return result;
}

/*  */

int	join_tcp_server (fd, sock)
int	fd;
struct sockaddr_in *sock;
{
	int     eindex,
			result;

#ifdef ULTRIX_X25_DEMSA
	int our_count;
#endif

	if ((result = connect (fd, (struct sockaddr *) sock, sizeof *sock))
			== NOTOK) {
		eindex = errno;
#ifdef EINPROGRESS
		if (errno != EINPROGRESS)
#endif
			SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("connect"));
		errno = eindex;
	}
#ifdef ULTRIX_X25_DEMSA
	else {
		/* connect returned success, now we register the descriptor */
		/* fd for the connection, which is going to be established  */
		if ( X25RegisterFD(fd,rhandler,whandler,xhandler,fd) < NULL) {
			SLOG (compat_log, LLOG_EXCEPTIONS, "failed", ("X25RegisterFD"));
			return NOTOK;
		}
		if(fd>=our_TABLE_SIZE || (our_count=getdtablesize())>=our_TABLE_SIZE) {
			SLOG (compat_log, LLOG_EXCEPTIONS, "invalid",
				  ("getdtablesize:increase our_TABLE_SIZE in demsb.h"));
			return NOTOK;
		}
		list_of_conn_desc[fd].TCP_IP_OR_X25_SOCKET = our_TCP_IP_SOCKET;
		list_of_conn_desc[fd].descriptor           = fd;
	}
#endif

	/* result is either OK or NOTOK - not a socket descriptor */

	return result;
}

/*  */

close_tcp_socket (fd)
int	fd;
{

#ifdef ULTRIX_X25_DEMSA
	if( fd >= our_TABLE_SIZE ) {
		SLOG (compat_log, LLOG_EXCEPTIONS, "close_tcp_socket",
			  ("fd >= our_TABLE_SIZE"));
		return NOTOK;
	}
	if (list_of_conn_desc[fd].TCP_IP_OR_X25_SOCKET!=our_TCP_IP_SOCKET) {
		SLOG (compat_log, LLOG_EXCEPTIONS, "close_tcp_socket",
			  ("list_of_conn_desc[fd].TCP_IP_OR_X25_SOCKET=!our_TCP_IP_SOCKET"));
		return NOTOK;
	}

	list_of_conn_desc[fd].TCP_IP_OR_X25_SOCKET = NOTOK;
	list_of_conn_desc[fd].descriptor           = NOTOK;

	X25RegisterFD(fd,NULL,NULL,NULL,NULL);
#endif

#ifdef	never_do_this_if_from_join_tcp_client
	shutdown (fd, 2);
#endif

	return (close (fd));
}

#endif

/*    AT&T UNIX: 5r3 using TLI */


/*    AT&T UNIX: 5 with EXOS 8044 TCP/IP card */

#ifdef	EXOS

/* If we had a getsockname() for the EXOS card, then we could postpone some
   of the binding until connect time.  But since we don't, our hand is forced
   and we must prematurely bind the sockets to IP addresses. */


int
start_tcp_client (struct sockaddr_in *sock, int priv) {
	int    port;
	int     sd;
	struct hostent *hp;

	if (sock == NULL)
		return socket (SOCK_STREAM, 0, (struct sockaddr *) 0, SO_KEEPALIVE);

	if (sock -> sin_addr.s_addr == 0) {
		if ((hp = gethostbyname ("localhost")) == NULL) {
			errno = EADDRNOTAVAIL;
			return NOTOK;
		}
		sock -> sin_family = hp -> h_addrtype;
		inaddr_copy (hp, sock);
	}

	for (port = IPPORT_RESERVED - priv;; priv ? port-- : port++) {
		sock -> sin_port = htons ((u_short) port);

		if ((sd = socket (SOCK_STREAM, 0, (struct sockaddr *) sock,
						  SO_KEEPALIVE)) != NOTOK)
			return sd;

		switch (errno) {
		case EADDRINUSE:
			if (!priv || (port >= IPPORT_RESERVED / 2))
				continue;	/* else fall */

		case EADDRNOTAVAIL:
		default:
			return NOTOK;
		}
	}
}

/*  */

int
start_tcp_server (struct sockaddr_in *sock, int backlog, int opt1, int opt2) {
	int    port;
	int     sd;
	struct hostent *hp;

	if (backlog != 1)
		return socket (SOCK_STREAM, 0, (struct sockaddr *) sock,
					   SO_ACCEPTCONN | SO_KEEPALIVE | opt1 | opt2);

	if (sock -> sin_addr.s_addr == 0) {
		if ((hp = gethostbyname ("localhost")) == NULL) {
			errno = EADDRNOTAVAIL;
			return NOTOK;
		}
		sock -> sin_family = hp -> h_addrtype;
		inaddr_copy (hp, sock);
	}

	for (port = IPPORT_RESERVED;; port++) {
		sock -> sin_port = htons ((u_short) port);

		if ((sd = socket (SOCK_STREAM, 0, (struct sockaddr *) sock,
						  SO_ACCEPTCONN | SO_KEEPALIVE | opt1 | opt2)) != NOTOK)
			return sd;

		switch (errno) {
		case EADDRINUSE:
			continue;

		case EADDRNOTAVAIL:
		default:
			return NOTOK;
		}
	}
}

#endif

/*    GETHOSTENT PLUS */

static char *empty = NULL;
#ifdef	h_addr
static char *addrs[2] = { NULL, NULL };
#endif

struct hostent *
gethostbystring (char *s) {
	struct hostent *h;
#ifndef	DG
	int result;
	static struct in_addr iaddr;
	result =  inet_aton(s, &iaddr);
	if (result == 0)
#ifdef LINUX
		return gethostbyname2 (s, AF_INET);
#else
		return gethostbyname (s);
#endif
#else
	static struct in_addr iaddr;
	iaddr = inet_addr (s);
	if (iaddr.s_addr == NOTOK && strcmp (s, "255.255.255.255"))
#ifdef LINUX
		return gethostbyname2 (s, AF_INET);
#else
		return gethostbyname (s);
#endif
#endif
	static struct hostent   hs;

	h = &hs;
	h -> h_name = s;
	h -> h_aliases = &empty;
	h -> h_addrtype = AF_INET;
	h -> h_length = sizeof (iaddr);
#ifdef	h_addr
	h -> h_addr_list = addrs;
	bzero ((char *) addrs, sizeof addrs);
#endif
	h -> h_addr = (char *) &iaddr;

	return h;
}

/*    AT&T UNIX: 5 with EXOS 8044 TCP/IP card */

#ifdef	EXOS

long	rhost ();
char   *raddr ();


struct hostent *
gethostbyaddr (char *addr, int len, int type) {
	long    iaddr;
	char   *name;
	static char buffer[BUFSIZ];
	static struct hostent   hs;
	struct hostent *h = &hs;

	if (len != sizeof (long) || type != AF_INET)
		return NULL;
	bcopy (addr, (char *) &iaddr, len);
	if ((name = raddr (iaddr)) == NULL)
		return NULL;

	strcpy (buffer, name);
	free (name);

	h -> h_name = buffer;
	h -> h_aliases = &empty;
	h -> h_addrtype = type;
	h -> h_length = len;
	h -> h_addr = addr;

	return h;
}


struct hostent *
gethostbyname (char *name) {
	static long iaddr;
	static char buffer[BUFSIZ];
	static struct hostent   hs;
	struct hostent *h = &hs;

	if ((iaddr = rhost (&name)) == NOTOK)
		return NULL;

	strcpy (buffer, name);
	free (name);

	h -> h_name = buffer;
	h -> h_aliases = &empty;
	h -> h_addrtype = AF_INET;
	h -> h_length = sizeof (iaddr);
	h -> h_addr = (char *) &iaddr;

	return h;

}

/*  */

/* really only need the "tsap" entry in this table... but why not? */

static struct servent   services[] = {
	"tsap", NULL, 102, "tcp",
	"miscellany", NULL, 17000, "lpp",

	"echo", NULL, 7, "tcp",		/* Network standard functions */
	"echo", NULL, 7, "udp",
	"sink", NULL, 9, "tcp",
	"sink", NULL, 9, "udp",
	"users", NULL, 11, "tcp",
	"users", NULL, 11, "udp",
	"daytime", NULL, 13, "tcp",
	"daytime", NULL, 13, "udp",
	"netstat", NULL, 15, "tcp",
	"netstat", NULL, 15, "udp",
	"qotd", NULL, 17, "tcp",
	"qotd", NULL, 17, "udp",
	"chargen", NULL, 19, "tcp",
	"chargen", NULL, 19, "udp",
	"ftp", NULL, 21, "tcp",
	"telnet", NULL, 23, "tcp",
	"smtp", NULL, 25, "tcp",
	"imagen", NULL, 35, "udp",
	"time", NULL, 37, "tcp",
	"time", NULL, 37, "udp",
	"name", NULL, 42, "tcp",
	"name", NULL, 42, "udp",
	"whois", NULL, 43, "tcp",
	"whois", NULL, 43, "udp",
	"nameserver", NULL, 53, "tcp",
	"nameserver", NULL, 53, "udp",
	"mtp", NULL, 57, "tcp",
	"hostnames", NULL, 101, "tcp",
	"pop", NULL, 109, "tcp",
	"pwdgen", NULL, 129, "tcp",
	"pwdgen", NULL, 129, "udp",
	"x25bridge", NULL, 146, "tcp",
	"iso-ip", NULL, 147, "udp",

	"tftp", NULL, 69, "udp",		/* Host specific functions */
	"rje", NULL, 77, "tcp",
	"nmui", NULL, 77, "udp",
	"finger", NULL, 79, "tcp",
	"finger", NULL, 79, "udp",
	"link", NULL, 87, "tcp",
	"supdup", NULL, 95, "tcp",
	"path", NULL, 117, "tcp",

	"exec", NULL, 512, "tcp",		/* UNIX TCP sockets */
	"login", NULL, 513, "tcp",
	"shell", NULL, 514, "tcp",
	"printer", NULL, 515, "tcp",
	"rfile", NULL, 522, "tcp",
	"ingreslock", NULL, 1524, "tcp",

	"biff", NULL, 512, "udp",		/* UNIX UDP sockets */
	"who", NULL, 513, "udp",
	"syslog", NULL, 514, "udp",
	"talk", NULL, 517, "udp",
	"routed", NULL, 520, "udp",
	"router_1", NULL, 521, "udp",

	NULL, &empty, 0, NULL
};



struct servent *
getservbyname (char *name, char *proto) {
	struct servent *s;

	for (s = services; s -> s_name; s++)
		if (strcmp (name, s -> s_name) == 0
				&& strcmp (proto, s -> s_proto) == 0) {
			if (s -> s_aliases == NULL) {
				s -> s_aliases = &empty;
				s -> s_port = htons ((u_short) s -> s_port);
			}

			return s;
		}

	return NULL;
}

/*  */

#define	s2a(b)	(((int) (b)) & 0xff)

char *
inet_ntoa (struct in_addr in) {
	char  *s = (char *) &in;
	static char addr[4 * 3 + 3 + 1];

	sprintf (addr, "%d.%d.%d.%d",
			 s2a (s[0]), s2a (s[1]), s2a (s[2]), s2a (s[3]));

	return addr;
}


u_long
inet_addr (char *cp) {
	int    base;
	char   c;
	u_long val;
	u_long	parts[4];
	u_long *pp = parts;

	for (;;) {
		val = 0, base = 10;
		if (*cp == '0')
			base = 8, cp++;
		if (*cp == 'x' || *cp == 'X')
			base = 16, cp++;

		for (; isxdigit ((u_char) (c = *cp)); cp++)
			if (base == 16)
				val = (val << 4) + (c + 10 - (islower ((u_char) c) ? 'a' : 'A'));
			else if (isdigit ((u_char) c))
				val = (val * base) + (c - '0');
			else
				break;

		switch (*cp) {
		case '.':
			if (pp >= parts + 4)
				return NOTOK;
			*pp++ = val, cp++;
			continue;

		default:
			if (*cp && !isspace ((u_char) *cp))
				return NOTOK;
			*pp++ = val;
			break;
		}

		break;
	}

	switch (pp - parts) {
	case 1:
		val = parts[0];
		break;

	case 2:
		val = ((parts[0] & 0xff) << 24)
			  | (parts[1] & 0xffffff);
		break;

	case 3:
		val = ((parts[0] & 0xff) << 24)
			  | ((parts[1] & 0xff) << 16)
			  | (parts[2] & 0xffff);
		break;

	case 4:
		val = ((parts[0] & 0xff) << 24)
			  | ((parts[1] & 0xff) << 16)
			  | ((parts[2] & 0xff) << 8)
			  | (parts[3] & 0xff);
		break;

	default:
		return NOTOK;
	}

	return htonl (val);
}

u_long
inet_network (char *cp) {
	int    base;
	char   c;
	u_long val;
	u_long	parts[4];
	u_long *pp = parts;

	for (;;) {
		val = 0, base = 10;
		if (*cp == '0')
			base = 8, cp++;
		if (*cp == 'x' || *cp == 'X')
			base = 16, cp++;

		for (; isxdigit ((u_char) (c = *cp)); cp++)
			if (base == 16)
				val = (val << 4) + (c + 10 - (islower ((u_char) c) ? 'a' : 'A'));
			else if (isdigit ((u_char) c))
				val = (val * base) + (c - '0');
			else
				break;

		switch (*cp) {
		case '.':
			if (pp >= parts + 4)
				return NOTOK;
			*pp++ = val, cp++;
			continue;

		default:
			if (*cp && !isspace ((u_char) *cp))
				return NOTOK;
			*pp++ = val;
			break;
		}

		break;
	}

	switch (pp - parts) {
	case 1:
		val = (parts[0] & 0xff) << 24;
		break;

	case 2:
		val = ((parts[0] & 0xff) << 24)
			  | ((parts[1] & 0xff) << 16);
		break;

	case 3:
		val = ((parts[0] & 0xff) << 24)
			  | ((parts[1] & 0xff) << 16)
			  | ((parts[2] & 0xff) << 8)
			  break;

	case 4:
		val = ((parts[0] & 0xff) << 24)
			  | ((parts[1] & 0xff) << 16)
			  | ((parts[2] & 0xff) << 8)
			  | (parts[3] & 0xff);
		break;

	default:
		return NOTOK;
	}

	return htonl (val);
}
#endif
#endif
