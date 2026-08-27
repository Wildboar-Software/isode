/* general.h - general compatibility */
#ifndef	_GENERAL_
#define	_GENERAL_

#ifndef	_CONFIG_
#include "config.h"		/* system-specific configuration */
#endif

/* target-dependent defines:

	BSDFORK -	target has BSD vfork
	BSDLIBC -	target has real BSD libc
	BSDSTRS -	target has BSD strings
	SWABLIB -	target has byteorder(3n) routines
 */

#ifdef	SOCKETS
#define	SWABLIB
#endif

#ifdef	WINTLI
#define	SWABLIB
#endif

#ifdef	EXOS
#define	SWABLIB
#endif

#ifdef	BSD42
#define	BSDFORK
#define	BSDLIBC
#define	BSDSTRS
#endif

#ifdef  SVR4_UCB
#ifndef SVR4
#define SVR4
#endif
#endif

#ifdef  SVR4
#ifndef SVR3
#define SVR3
#endif
#endif

#ifdef	ROS
#undef	BSDFORK
#undef	BSDLIBC
#define	BSDSTRS
#ifndef	BSD42
#define	BSD42
#endif
#undef	SWABLIB
#endif

#ifdef	SYS5
#undef	BSDFORK
#undef	BSDLIBC
#undef	BSDSTRS
#endif

#ifdef	sgi
#undef	BSDFORK
#undef	BSDLIBC
#undef	BSDSTRS
#endif

#ifdef	HPUX
#define	BSDFORK
#undef	BSDLIBC
#undef	BSDSTRS
#undef	SWABLIB
#endif

#ifdef	pyr
#undef	SWABLIB
#endif

#ifdef	XOS
#undef	SWABLIB
#endif

#ifdef	XOS_2
#undef	SWABLIB
#endif

#ifdef  apollo
#undef  SWABLIB
#endif

#ifdef	AUX
#undef	BSDFORK
#define BSDSTRS
#undef SWABLIB
#define BSDLIBC
#endif 

#if defined(_AIX) && defined(SYS5)
#define BSDSTRS
#endif

#ifndef	BSDFORK
#define	vfork	fork
#endif

/* STRINGS */

#if defined(SVR4) || defined (__NeXT__)
#include <stdio.h>
#include <stdlib.h>

#else /* SVR4 */

#ifndef	LINUX
#if	defined(BSDSTRS) && !defined(BSD44) && (!defined(BSD43) || defined(SUNOS4) || defined(vax) || defined(RT) || (defined(mips) && defined(ultrix))) && !defined(XOS_2)
#if !(defined(__STDC__) && defined(__GNUC__) && defined(mips) && defined(ultrix))
//char   *sprintf ();
#endif
#else
//int     sprintf ();
#endif
#endif

char   *getenv (const char *);
char   *mktemp (char *);

#endif /* SVR4 */

#ifdef __STDC__
#ifndef __NeXT__
#include <malloc.h>
#endif
#else
#if defined(BSD44) || defined(_AIX)
void   *calloc (size_t, size_t), *malloc (size_t), *realloc (void *, size_t);
void free(void *);
#else
char   *calloc (unsigned, unsigned), *malloc (unsigned), *realloc (char *, unsigned);
int free(char *);
#endif /* defined BSD44 */
#endif

#ifndef	BSDSTRS
#define	index	strchr
#define	rindex	strrchr
#include <string.h>
#else
#include <strings.h>
#endif

#if	defined(SYS5) && !defined(_AIX) && !defined(XOS) && !defined(XOS_2)
#include <memory.h>

#define	bcopy(b1,b2,length)	(void) memcpy ((b2), (b1), (length))
#define	bcmp(b1,b2,length)	memcmp ((b1), (b2), (length))
#define	bzero(b,length)		(void) memset ((b), 0, (length))
#endif
/* HEXIFY */

/* SPRINTB */

char   *sprintb (const int v, const char *bits);

/* STR2VEC */

#define	NVEC	100
#define	NSLACK	10

#define	str2vec(s,v)	str2vecX ((s), (v), 0, (int *)0, 0, 1)

int	str2vecX (char *, char **, const int,  int *, const char,  const int);

/* STR2ELEM */

#define	NELEM	100

int	str2elem (const char *s, unsigned int elements[]);

/* STR2SEL */

int	str2sel (char *s, const int quoted, char *sel, int n);
char   *sel2str (char *sel, int len, const int quoted);

/* GETPASS */

char   *getpassword (char *prompt);

/* BADUSER */

int	baduser (char *file, const char *user);

/* UTILITIES */

extern char chrcnv[], nochrcnv[];

int	lexequ (char *str1, char *str2), lexnequ (const char *str1, const char *str2, const int len);

struct ll_struct;
int	log_tai (struct ll_struct *lgptr, char **av, const int ac);

int	sstr2arg (const char *srcptr, const int maxpf, char *argv[], const char *dlmstr);

void	(*set_smalloc_handler (void (*fnx)(void)))(void);
char    *smalloc (const int size);

/* MISC */

char   *sys_errname (const int i);

#ifdef	lint
#define	insque(e,p)	INSQUE ((char *) (e), (char *) (p))
#define	remque(e)	REMQUE ((char *) (e))
#endif

#include <stdarg.h>
#ifndef ASPRINTF
void    asprintf(char*bp, va_list ap);            /* fmt, args, ... */
#endif
void    _asprintf(char*bp, char*what, const char* fmt, va_list ap);   /* fmt, args, ... */

/* time */

#ifdef LINUX
#include <time.h>
#endif
#ifndef makedev
#include <sys/types.h>
#endif
#ifndef	OSX
#ifndef	BSD42
#ifdef _AIX
#include <sys/time.h>		/* Why is AIX always different ! */
#endif
#include <time.h>
#else  /* BSD42 */
#ifndef	timerisset
#include <sys/time.h>
#endif /* timerisset */
#endif /* BSD42 */
#endif /* OSX */

#ifndef __STDC__
extern time_t time (time_t *);
#endif

/*  ntohs etc */
#ifndef SWABLIB
#ifndef	ntohs
unsigned short	ntohs (unsigned short);
#endif
#ifndef	htons
unsigned short	htons (unsigned short);
#endif
#ifndef	ntohl
unsigned long	ntohl (unsigned long);
#endif
#ifndef	htonl
unsigned long	htonl (unsigned long);
#endif
#endif

#include <stdint.h>
int	explode (char *a, const uint8_t *b, const int n), implode (uint8_t *a, const char *b, const int n);
int	char2bcd (const char *s, int n, uint8_t *d);
int	bcd2char (const uint8_t *s, char *d, const int len);

#include "conv.h"

#endif
