/* manifest.h - manifest constants */
#ifndef	_MANIFEST_
#define	_MANIFEST_

#include <stdlib.h>

// Trick to avoid conflicting "entry" symbol
#define entry search_h_entry
#define ACTION search_h_action
#include <search.h>
#undef entry
#undef ACTION

/* current ISODE distribution: major*10+minor */
#define	ISODE	80

#ifndef	_CONFIG_
#include "config.h"		/* system-specific configuration */
#endif

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * glibc's ISO C dialect hides BSD types.  C11 allows these typedefs to
 * repeat when the libc already provided the same types.
 */
typedef unsigned char	u_char;
typedef unsigned short	u_short;
typedef unsigned int	u_int;
typedef unsigned long	u_long;
typedef char		*caddr_t;

/* POSIX signal handler; glibc names this __sighandler_t. */
typedef void (*__sighandler_t)(int);

#ifndef getdtablesize
#define getdtablesize() (sysconf (_SC_OPEN_MAX))
#endif

#ifndef getpagesize
#define getpagesize() ((int) sysconf (_SC_PAGESIZE))
#endif

#ifndef NSIG
#ifdef _NSIG
#define NSIG _NSIG
#else
#define NSIG 65
#endif
#endif

#include <limits.h>
#ifndef MAXNAMLEN
#ifdef NAME_MAX
#define MAXNAMLEN NAME_MAX
#else
#define MAXNAMLEN 255
#endif
#endif

#include <sys/stat.h>
#ifndef S_IWRITE
#define S_IREAD		S_IRUSR
#define S_IWRITE	S_IWUSR
#define S_IEXEC		S_IXUSR
#endif

#include <sys/wait.h>
#ifndef wait3
#define wait3(status, options, rusage) waitpid (-1, (status), (options))
#endif

/*
 * BSD/GNU APIs that glibc hides from ISO C.  chroot(2) and setgroups(2)
 * still exist in libc; initgroups(3) and flock(2) are reimplemented in
 * compat/bsd.c using POSIX getgrent/setgroups and fcntl(F_SETLK).
 */
#ifndef LOCK_SH
#define LOCK_SH	1
#define LOCK_EX	2
#define LOCK_NB	4
#define LOCK_UN	8
#endif

int	chroot (const char *path);
int	setgroups (size_t size, const gid_t *list);
int	initgroups (const char *user, gid_t group);
int	flock (int fd, int operation);
int	vhangup (void);

/* target-dependent defines:

	SYS5NLY -	target has SYS5 types only, no BSD types

	BSDSIGS	-	target supports BSD signals
 */

#ifdef	BSD42
#undef	SYS5NLY
#define	BSDSIGS
#endif 

#ifdef	ROS
#undef	SYS5NLY
#define	BSDSIGS

#ifndef	BSD42
#define	BSD42
#endif

#endif

#ifdef	SYS5
#define	SYS5NLY
#undef	BSDSIGS

#ifdef	masscomp
#undef	SYS5NLY
#endif

#ifdef	OSX
#undef	SYS5NLY
#endif

#ifdef	XOS_2
#undef	SYS5NLY
#define	SIGEMT	SIGSEGV
#endif

#if	defined(WIN) || defined (WINTLI)
#undef	SYS5NLY
#endif

#ifdef	sun
#undef	SYS5NLY
#endif

#ifdef	sgi
#undef	SYS5NLY
#endif

#ifdef SVR4
#undef  SYS5NLY
#endif

#ifdef	HPUX
#undef	SYS5NLY
#define	BSDSIGS
#undef	SIGPOLL
#endif

#ifdef	AIX
#undef	SYS5NLY
#define	BSDSIGS
#define	SIGEMT	SIGUSR1

int   (*_signal (int sig, int (*func)())) ();

#define	signal	_signal
#endif

#ifdef AUX
#undef	SYS5NLY
#define BSDSIGS
#endif
#endif

#ifdef  SVR4
#ifndef SVR3
#define SVR3
#endif
#endif

#ifndef	sigmask
#define	sigmask(s)	(1 << ((s) - 1))
#endif

#ifndef _SIGIO
#ifdef XOS_2
#define _SIGIO SIGEMT
#else
#ifdef	SIGPOLL
#define	_SIGIO	SIGPOLL
#else
#ifdef	SIGUSR1
#define	_SIGIO	SIGUSR1
#else
#define	_SIGIO	SIGEMT
#endif
#endif
#endif
#endif

typedef	sigset_t	SBV;
extern int _iosignals_set;

static inline SBV
sigioblock (void) {
	sigset_t nset, oset;

	(void) sigemptyset (&oset);
	if (_iosignals_set) {
		(void) sigemptyset (&nset);
		(void) sigaddset (&nset, _SIGIO);
		(void) sigprocmask (SIG_BLOCK, &nset, &oset);
	}
	return oset;
}

static inline int
sigiomask (SBV s) {
	if (_iosignals_set)
		return sigprocmask (SIG_SETMASK, &s, NULL);
	return 0;
}

static inline SBV
siginblock (void) {
	sigset_t nset, oset;

	(void) sigemptyset (&nset);
	(void) sigaddset (&nset, SIGINT);
	(void) sigprocmask (SIG_BLOCK, &nset, &oset);
	return oset;
}

static inline int
siginmask (SBV s) {
	return sigprocmask (SIG_SETMASK, &s, NULL);
}

static inline SBV
sigdelmask (int sig) {
	sigset_t nset, oset;

	(void) sigemptyset (&nset);
	(void) sigaddset (&nset, sig);
	(void) sigprocmask (SIG_UNBLOCK, &nset, &oset);
	return oset;
}

/* TYPES */

#ifndef	NOTOK
#define	NOTOK		(-1)
#define	OK		0
#define	DONE		1
#define CONNECTING_1	OK
#define CONNECTING_2	2
#endif

#ifdef SVR4
#ifdef NULLVP
#undef NULLVP
#endif
#endif

#ifndef	NULLCP
typedef char *CP;
#define	NULLCP		((char *) 0)
#define	NULLVP		((char **) 0)
#endif

#include <stdint.h>

#ifndef INTDEF
#define	INTDEF	int32_t
#endif

typedef INTDEF integer;
#define NULLINT		((integer) 0)
#define NULLINTP	((integer *) 0)

void *sbrk (intptr_t increment);

#if defined(SVR4) || defined(SYSV) || defined(BSD44) || defined(LINUX)
#else
#define UNIONWAIT
#endif

#ifndef	makedev
#include <sys/types.h>
#if	defined(WIN) || defined(WINTLI)
#include "sys/inet.h"
#ifndef	NFDBITS
typedef struct fd_set { int fds_bits[1]; } fd_set;
#endif
#endif

#if defined (LINUX) || defined (_AIX) && defined (SYS5)
#include <sys/select.h>
#endif

#ifdef	SYS5NLY
typedef unsigned char	uint8_t;
typedef unsigned short	uint16_t;
typedef unsigned int	u_int;
typedef unsigned long	uint32_t;
#endif
#endif

#ifdef CYGWIN
#include <sys/select.h>
#endif

#if	!defined(FD_SET) && !defined(LINUX)
#define	FD_SETSIZE	    (sizeof (fd_set) * 8)

#define FD_SET(f,s)	    ((s)->fds_bits[0] |= (1 << (f)))
#define FD_CLR(f,s)	    ((s)->fds_bits[0] &= ~(1 << (f)))
#define FD_ISSET(f,s)	    ((s)->fds_bits[0] & (1 << (f)))
#define FD_ZERO(s)	    ((s)->fds_bits[0] = 0)
#endif

#define	NULLFD		((fd_set *) 0)

#undef	IP
typedef ssize_t	*IP;
#define	NULLIP		((IP) NULL)

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-prototypes"
#endif
typedef	int	(*IFP) ();
#define	NULLIFP		((IFP) NULL)

typedef void   (*VFP) ();
#define	NULLVFP		((VFP) NULL)

typedef	void * (*PFP) ();
#define	NULLPFP		((PFP) NULL)
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#ifndef	SFD
#if !defined(SVR3) && !defined(SUNOS4) && !defined(BSD44) && !defined(ultrix)
#define	SFD	int
#define	SFP	IFP
#else
#define	SFD	void
#define	SFP	VFP
#endif
#endif

struct udvec {			/* looks like a BSD iovec... */
    char *uv_base;
    int	    uv_len;

    int	    uv_inline;
};

struct qbuf {
    struct qbuf *qb_forw;	/* doubly-linked list */
    struct qbuf *qb_back;	/* .. */

	// TODO: Make this a size_t
    int	    qb_len;		/* length of data */
    char   *qb_data;		/* current pointer into data */
    char    qb_base[1];		/* extensible... */
};

int qbprintf(void);
int qb_pullup (struct qbuf *qb);

#define	QBFREE(qb) \
{ \
    register struct qbuf *QB, \
			 *QP; \
 \
    for (QB = (qb) -> qb_forw; QB != (qb); QB = QP) { \
	QP = QB -> qb_forw; \
	remque (QB); \
	free ((char *) QB); \
    } \
}

#ifndef	min
#define	min(a, b)	((a) < (b) ? (a) : (b))
#define	max(a, b)	((a) > (b) ? (a) : (b))
#endif

#if	defined(RT) || defined (HPUX)
#define	ntohs(x)	(x)
#define	htons(x)	(x)
#define	ntohl(x)	(x)
#define	htonl(x)	(x)
#endif

/* POSIX */

#ifndef	SETSID
#if	defined(SUNOS41) || defined(BSD44) || defined (SVR4) || defined(_AIX)
#define	SETSID
#endif
#endif

#endif
