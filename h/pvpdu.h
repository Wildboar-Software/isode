#include "logger.h"
#include "pepsy.h"
#include "psap.h"

void pvpdu (LLog *lp, int ind, modtyp *mod, PE pe, char *text, int rw);

#ifdef __STDC__

#define	vpdu(lp,fnx,pe,text,rw) \
{ \
    pvpdu ((lp), fnx##_P, (pe), (text), (rw)); \
}

#define	PLOGP(lp,args,pe,text,rw) \
    if ((lp) -> ll_events & LLOG_PDUS) { \
	pvpdu (lp, print_##args##_P, pe, text, rw); \
    } \
    else

#define	PLOG(lp,fnx,pe,text,rw)	\
    if ((lp) -> ll_events & LLOG_PDUS) { \
	pvpdu (lp, fnx##_P, pe, text, rw); \
    } \
    else

#else   /* __STDC__ */

#define	vpdu(lp,fnx,pe,text,rw) \
{ \
    pvpdu ((lp), fnx/**/_P, (pe), (text), (rw)); \
}

#define	PLOGP(lp,args,pe,text,rw) \
    if ((lp) -> ll_events & LLOG_PDUS) { \
	pvpdu (lp, print_/**/args/**/_P, pe, text, rw); \
    } \
    else

#define	PLOG(lp,fnx,pe,text,rw)	\
    if ((lp) -> ll_events & LLOG_PDUS) { \
	pvpdu (lp, fnx/**/_P, pe, text, rw); \
    } \
    else

#endif /* __STDC__ */

#ifdef	DEBUG

#ifndef PLOGP
#define	PLOGP(lp,args,pe,text,rw) \
    if ((lp) -> ll_events & LLOG_PDUS) { \
	pvpdu (lp, 0, NULL, pe, text, rw); \
    } \
    else
#endif

#else	/* !DEBUG */
#define	PLOG(lp,fnx,pe,text,rw)
#define	PLOGP(lp,args,pe,text,rw)
#endif

typedef int (*pepy_printfn)(PE pe, int explicit, int *len, char **buffer, char *parm);

void _vpdu (LLog *lp, pepy_printfn fnx, PE pe, char *text, int rw);

/**
 * For projects still built using pepy.
 * Deprecated. To be removed once all projects don't use pepy.
 * Casts fnx: pepy printers take PEPYPARM as the last argument, which
 * some compilation units redefine (e.g. VT uses int *).
 */
#define	OLDPLOG(lp,fnx,pe,text,rw)	\
    if ((lp) -> ll_events & LLOG_PDUS) { \
        _vpdu (lp, (pepy_printfn)(fnx), pe, text, rw); \
    } \
    else
