/* internet.h - TCP/IP abstractions */

#ifndef	_INTERNET_
#define	_INTERNET_

#include <stdint.h>

#ifndef	_MANIFEST_
#include "manifest.h"
#endif

/* SOCKETS */

#include "sys/socket.h"
#include "asm/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"

#ifndef	SOMAXCONN
#define	SOMAXCONN	5
#endif

#if	defined(WIN) || defined(WINTLI)
#include "sys/in.h"
#else

#ifdef SVR4	/* Has a different defn for NULLVP */
#ifdef NULLVP
#undef NULLVP
#endif
#endif /* SVR4 */
#include "netinet/in.h"
#ifdef SVR4	/* Put back the ISODE defn */
#ifdef NULLVP
#undef NULLVP
#endif
#define NULLVP ((char **) 0)
#endif /* SVR4 */

#endif /* defined(WIN) || defined(WINTLI) */

int	start_tcp_client (struct sockaddr_in *sock, int priv);
int	start_tcp_server (struct sockaddr_in *sock, int backlog, int opt1, int opt2);

#if defined(SOCKETS) && !defined(TLI_TCP)
int	join_tcp_client (int fd, struct sockaddr_in *sock);
int	join_tcp_server (int fd, struct sockaddr_in *sock);
#include <unistd.h>
#define	read_tcp_socket		read
#define	write_tcp_socket	write
#define	select_tcp_socket	selsocket
#endif

#ifdef TLI_TCP
int	join_tcp_client (int fd, struct sockaddr_in *sock);
int	join_tcp_server (int fd, struct sockaddr_in *sock);
ssize_t	read_tcp_socket (int fd, char *buffer, int len);
ssize_t	write_tcp_socket (int fd, char *buffer, int len);
int	close_tcp_socket (int fd);
#define	select_tcp_socket	selsocket
#endif

#ifdef	WINTLI
int	join_tcp_client (int fd, struct sockaddr_in *sock);
int	join_tcp_server (int fd, struct sockaddr_in *sock);

#define	select_tcp_socket	selsocket
#endif

#ifdef	EXOS

#ifdef	SYS5
#define	join_tcp_client(s,f) \
	(accept ((s), (struct sockaddr *) (f)) != NOTOK ? (s) : NOTOK)
#define	join_tcp_server(s,t)	connect ((s), (struct sockaddr *) (t))

#define	read_tcp_socket		read
#define	write_tcp_socket	write
#define	close_tcp_socket	close
#define	select_tcp_socket	selsocket
#endif
#endif

ssize_t	read_tcp_socket (int fd, void *buf, size_t n);
ssize_t	write_tcp_socket (int fd, const void *buf, size_t n);
int	close_tcp_socket (int fd);

int	select_tcp_socket (int nfds, fd_set *rfds, fd_set *wfds, fd_set *efds, int secs);

/* UDP */

#ifdef	SOCKETS
#ifndef	_DGRAM_
#include "dgram.h"
#endif

int	start_udp_server (struct sockaddr_in *sock, int backlog, int opt1, int opt2);
#define	start_udp_client	start_udp_server

#define	join_udp_server(fd,sock) \
		join_dgram_aux ((fd), (struct sockaddr *) (sock), 0)
#define	join_udp_client(fd,sock) \
		join_dgram_aux ((fd), (struct sockaddr *) (sock), 1)

#define	read_udp_socket		read_dgram_socket
#define	write_udp_socket	write_dgram_socket
#define	close_udp_socket	close_dgram_socket

#define	select_udp_socket	select_dgram_socket
#define	check_udp_socket	check_dgram_socket
#endif

/* NETDB */

#if	defined(SOCKETS) || defined (WINTLI) || defined (WIN)
#if     defined(_AIX) && defined(n_name)	/* different def in nlist.h clashes with netdb.h */
#undef n_name
#endif
#include <netdb.h>
#if   defined(_AIX) && defined(SYS5)
#define n_name          _n._n_name
#endif
#endif

#if	defined(BIND) && !defined(h_addr)
#define	h_addr	h_addr_list[0]
#endif

#define	inaddr_copy(hp,sin) \
    bcopy ((hp) -> h_addr, (char *) &((sin) -> sin_addr), (hp) -> h_length)

#ifdef	EXOS
struct hostent {
	char   *h_name;		/* official name */
	char  **h_aliases;		/* alias list */
	int     h_addrtype;		/* address type: AF_INET */
	int     h_length;		/* address length: sizeof (uint32_t) == 4 */
	char   *h_addr;		/* address value: (struct in_addr *) */
};

struct servent {
	char   *s_name;		/* official name */
	char  **s_aliases;		/* alias list */
	int     s_port;		/* port number */
	char   *s_proto;		/* protocol beneath service */
};

struct hostent *gethostbyaddr (const char *, int, int), *gethostbyname (const char *);
struct servent *getservbyname (const char *, const char *);
#endif

struct hostent *gethostbystring (char *s);

/* INET */

/* under BSD42, we could simply include <arpa/inet.h> instead.  However,
   the definition of inet_addr contained therein causes problems with some
   compilers. */

char   *inet_ntoa (struct in_addr in);
#ifdef LINUX
extern in_addr_t inet_addr (const char *cp), inet_network (const char *cp);
#else
#ifndef	DG
#ifndef	HPUX
in_addr_t inet_network ();
#else
in_addr_t inet_network ();
#endif
#else
struct in_addr inet_addr (), inet_network ();
#endif
#endif
#endif
