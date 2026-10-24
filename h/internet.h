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

#ifndef	h_addr
#define	h_addr	h_addr_list[0]
#endif

#define	inaddr_copy(hp,sin) \
    do { \
	size_t _hl; \
	if (int2sizet ((hp) -> h_length, &_hl) == 0) \
	    memmove ((char *) &((sin) -> sin_addr), (hp) -> h_addr, _hl); \
    } while (0)

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
in_addr_t inet_network (const char *cp);
#else
in_addr_t inet_network (const char *cp);
#endif
#else
struct in_addr inet_addr (const char *cp), inet_network (const char *cp);
#endif
#endif

/*
 * BSD/GNU networking names that glibc hides from ISO C.  Values match
 * Linux uapi / historic 4.3BSD so SNMP MIB code keeps working.
 */
#ifndef IFNAMSIZ
#ifdef IF_NAMESIZE
#define	IFNAMSIZ	IF_NAMESIZE
#else
#define	IFNAMSIZ	16
#endif
#endif

#ifndef IFF_UP
#define	IFF_UP		0x1
#define	IFF_BROADCAST	0x2
#define	IFF_DEBUG	0x4
#define	IFF_LOOPBACK	0x8
#define	IFF_POINTOPOINT	0x10
#define	IFF_NOTRAILERS	0x20
#define	IFF_RUNNING	0x40
#define	IFF_NOARP	0x80
#define	IFF_PROMISC	0x100
#define	IFF_ALLMULTI	0x200
#define	IFF_MULTICAST	0x1000
#endif

#ifndef ifr_name
struct ifmap {
	unsigned long	mem_start;
	unsigned long	mem_end;
	unsigned short	base_addr;
	unsigned char	irq;
	unsigned char	dma;
	unsigned char	port;
};

struct ifreq {
	union {
		char	ifrn_name[IFNAMSIZ];
	} ifr_ifrn;
	union {
		struct sockaddr	ifru_addr;
		struct sockaddr	ifru_dstaddr;
		struct sockaddr	ifru_broadaddr;
		struct sockaddr	ifru_netmask;
		struct sockaddr	ifru_hwaddr;
		short	ifru_flags;
		int	ifru_ivalue;
		int	ifru_mtu;
		struct ifmap	ifru_map;
		char	ifru_slave[IFNAMSIZ];
		char	ifru_newname[IFNAMSIZ];
		char	*ifru_data;
	} ifr_ifru;
};
#define	ifr_name	ifr_ifrn.ifrn_name
#define	ifr_hwaddr	ifr_ifru.ifru_hwaddr
#define	ifr_addr	ifr_ifru.ifru_addr
#define	ifr_dstaddr	ifr_ifru.ifru_dstaddr
#define	ifr_broadaddr	ifr_ifru.ifru_broadaddr
#define	ifr_netmask	ifr_ifru.ifru_netmask
#define	ifr_flags	ifr_ifru.ifru_flags
#define	ifr_metric	ifr_ifru.ifru_ivalue
#define	ifr_mtu		ifr_ifru.ifru_mtu
#define	ifr_map		ifr_ifru.ifru_map
#define	ifr_slave	ifr_ifru.ifru_slave
#define	ifr_data	ifr_ifru.ifru_data
#define	ifr_ifindex	ifr_ifru.ifru_ivalue

struct ifconf {
	int	ifc_len;
	union {
		char	*ifcu_buf;
		struct ifreq	*ifcu_req;
	} ifc_ifcu;
};
#define	ifc_buf	ifc_ifcu.ifcu_buf
#define	ifc_req	ifc_ifcu.ifcu_req

struct ifaddr {
	struct sockaddr	ifa_addr;
	union {
		struct sockaddr	ifu_broadaddr;
		struct sockaddr	ifu_dstaddr;
	} ifa_ifu;
	struct ifaddr	*ifa_next;
};
#ifndef ifa_broadaddr
#define	ifa_broadaddr	ifa_ifu.ifu_broadaddr
#define	ifa_dstaddr	ifa_ifu.ifu_dstaddr
#endif
#endif

#ifndef ATF_COM
#define	ATF_COM		0x02
#define	ATF_PERM	0x04
#define	ATF_PUBL	0x08
struct arpreq {
	struct sockaddr	arp_pa;
	struct sockaddr	arp_ha;
	int	arp_flags;
	struct sockaddr	arp_netmask;
	char	arp_dev[16];
};
#endif

#ifndef ICMP_UNREACH
#define	ICMP_UNREACH		3
#define	ICMP_SOURCEQUENCH	4
#define	ICMP_TIMXCEED		11
#define	ICMP_PARAMPROB		12
#define	ICMP_TSTAMP		13
#define	ICMP_TSTAMPREPLY	14
#define	ICMP_MASKREQ		17
#define	ICMP_MASKREPLY		18
#define	ICMP_MAXTYPE		18
#endif

#ifndef TCP_ESTABLISHED
enum {
	TCP_ESTABLISHED = 1,
	TCP_SYN_SENT,
	TCP_SYN_RECV,
	TCP_FIN_WAIT1,
	TCP_FIN_WAIT2,
	TCP_TIME_WAIT,
	TCP_CLOSE,
	TCP_CLOSE_WAIT,
	TCP_LAST_ACK,
	TCP_LISTEN,
	TCP_CLOSING
};
#endif
#endif
