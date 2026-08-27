/* util.h - various useful utility definitions */

#ifndef _DIDUTIL_

#define _DIDUTIL_

#ifndef _H_UTIL			/* PP interwork */

#include <errno.h>
#include <stdio.h>              /* minus the ctype stuff */
#include <ctype.h>
#include <setjmp.h>
#include <sys/types.h>
#include "manifest.h"
#include "logger.h"
#include "psap.h"

#endif

#ifndef       _GENERAL_
#include "general.h"
#endif

#include "quipu/config.h"

#ifndef _H_UTIL			/* PP interwork */

/* some common logical values */

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif
#ifndef MAYBE
#define MAYBE   1
#endif

/* stdio extensions */

#ifndef lowtoup
#define lowtoup(chr) (islower(chr)?toupper(chr):chr)
#endif
#ifndef uptolow
#define uptolow(chr) (isupper(chr)?tolower(chr):chr)
#endif
#ifndef MIN
#define MIN(a,b) (( (b) < (a) ) ? (b) : (a) )
#endif
#ifndef MAX
#define MAX(a,b) (( (b) > (a) ) ? (b) : (a) )
#endif
#if	!defined(MAXINT) && !defined(__linux__)
#define MAXINT (~(1 << ((sizeof(int) * 8) - 1)))
#endif

#define isstr(ptr) ((ptr) != 0 && *(ptr) != '\0')
#define isnull(chr) ((chr) == '\0')
#define isnumber(c) ((c) >= '0' && (c) <= '9')

/*
 * provide a timeout facility
 */

extern  jmp_buf _timeobuf;

#define quipu_timeout(val)    (setjmp(_timeobuf) ? 1 : (_timeout(val), 0))

/*
 * some common extensions
 */
#define LINESIZE 1024    /* what we are prepared to consider a line length */
#define FILNSIZE 256    /* max filename length */
#define LOTS    1024    /* a max sort of thing */
#define MAXFILENAMELEN 15	/* size of largest fine name allowed */

# define        MAXFORK 10      /* no. of times to try a fork() */

void parse_error (char *a, const char *b);
void pslog (LLog *lp, const int event, const char *str, void (*func) (PS ps, caddr_t ptr, int format), caddr_t ptr);
int test_prim_pe (PE pe, const PElementClass class, const PElementID id);
int add_entry_aux (char *a, caddr_t b, const int c, char *d);
int stop_listeners (void);
int check_print_string (const char *str);
int telcmp (const char *a, const char *b);
int telncmp (const char *a, const char *b, int n);
int telstrlen (const char *s);
void octprint (PS ps, char *str, int format);
char *SkipSpace (char *ptr);
void StripSpace (char *b);
void StripSpace2 (const char *b);
void fatal (const int code, char *fmt);
void utcprint (PS ps, char *xtime, int format);
int quipu_pe_cmp (PE a, PE b);
int test_arg (const char *x, const char *y, const int c);
void pdu_dump (PE pe, char *type, const int op);
void pdu_dump_init (char *dir);
int dsa_wait (const int secs);
int dsap_init (const int *acptr, char ***avptr);
void quipu_syntaxes (void);
void standard_syntaxes (void);
void hide_picture (void);
void iso8859print(PS ps, const char *sstr);
int reset_arg (void);
int print_arg_error (PS opt);

#define _H_UTIL

#endif
#endif
