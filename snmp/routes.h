/* routes.h - support for MIB support of the routing tables */

/*
 * $Header: /xtel/isode/isode/snmp/RCS/routes.h,v 9.0 1992/06/16 12:38:11 isode Rel $
 *
 * Contributed by NYSERNet Inc.  This work was partially supported by the
 * U.S. Defense Advanced Research Projects Agency and the Rome Air Development
 * Center of the U.S. Air Force Systems Command under contract number
 * F30602-88-C-0016.
 *
 *
 * $Log: routes.h,v $
 * Revision 9.0  1992/06/16  12:38:11  isode
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


#ifdef	BSD44
#include <sys/param.h>
#endif
#ifndef __linux__
#include <sys/mbuf.h>
#endif

#ifndef __linux__
#include <net/route.h>
#else
#define	RTF_UP		0x1		/* route useable */
#define	RTF_GATEWAY	0x2		/* destination is a gateway */
#define	RTF_HOST	0x4		/* host entry (net otherwise) */
#define	RTF_DYNAMIC	0x10		/* created dynamically (by redirect) */

struct rtentry {
	struct	sockaddr rt_dst;	/* key */
	struct	sockaddr rt_gateway;	/* value */
	u_short	rt_flags;		/* up/down?, host/net */
	u_short	rt_refcnt;		/* # held references */
	u_long	rt_use;			/* raw # packets forwarded */
	struct	ifnet *rt_ifp;		/* the answer: interface to use */
};
#endif

/*  */

#define	METRIC_NONE	(-1)			/* ipRouteMetric[1234] */

#define	PROTO_OTHER	1			/* ipRouteProto */
#define	PROTO_ICMP	4
#define	PROTO_ESIS	10

/*  */

struct rtetab {
#define	RT_SIZE		20			/* object instance */
	unsigned int   rt_instance[RT_SIZE + 1];
	int	    rt_insize;

	int	    rt_magic;				/* for multiple routes to the
						   same destination */

	struct rtentry rt_rt;			/* from kernel */

	union sockaddr_un rt_dst;			/* key */
	union sockaddr_un rt_gateway;		/* value */

	int	    rt_type;				/* ipRouteType */
#define	TYPE_OTHER	1
#define	TYPE_INVALID	2
#define	TYPE_DIRECT	3
#define	TYPE_REMOTE	4

	int	    rt_touched;				/* set request'd */
	union sockaddr_un rt_oldgwy;		/*   .. ipRouteNextHop */

	struct rtetab *rt_next;
};

extern int	routeNumber;
extern int	flush_rt_cache;

extern struct rtetab *rts;
extern struct rtetab *rts_inet;
#ifdef	BSD44
extern struct rtetab *rts_iso;
#endif


int	get_routes ();
struct rtetab *get_rtent ();
