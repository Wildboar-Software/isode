/* tcp.c - MIB realization of the TCP group */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/snmp/RCS/tcp.c,v 9.0 1992/06/16 12:38:11 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/snmp/RCS/tcp.c,v 9.0 1992/06/16 12:38:11 isode Rel $
 *
 * Contributed by NYSERNet Inc.  This work was partially supported by the
 * U.S. Defense Advanced Research Projects Agency and the Rome Air Development
 * Center of the U.S. Air Force Systems Command under contract number
 * F30602-88-C-0016.
 *
 *
 * $Log: tcp.c,v $
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


#include <stdio.h>
#include <string.h>
#include "mib.h"

#include "internet.h"
#ifdef	BSD43
#include <sys/param.h>
#endif
#ifndef LINUX
#include <sys/protosw.h>
#endif
#include <sys/socketvar.h>
#include <net/route.h>
#include <netinet/in_systm.h>
#ifdef	BSD44
#include <netinet/ip.h>
#endif
#ifndef LINUX
#include <netinet/in_pcb.h>
#endif
#include <netinet/tcp.h>
#ifndef LINUX
#include <netinet/tcp_fsm.h>
#include <netinet/tcp_timer.h>
#include <netinet/tcp_var.h>
#else
struct	tcpstat {
	u_long	tcps_rtoalgorithm;	/*  */
	u_long	tcps_rtomin;	/*  */
	u_long	tcps_rtomax;	/*  */
	u_long	tcps_maxconn;	/*  */
	u_long	tcps_outsegs;	/*  */
	u_long	tcps_ierrors;	/*  */
	u_long	tcps_orsts;  	/*  */
	u_long	tcps_connattempt;	/* connections initiated */
	u_long	tcps_accepts;		/* connections accepted */
	u_long	tcps_drops;		/* connections dropped */
	u_long	tcps_conndrops;		/* embryonic connections dropped */
	u_long	tcps_sndrexmitpack;	/* data packets retransmitted */
	u_long	tcps_rcvtotal;		/* total packets received */
};
struct inpcb {
	struct	inpcb *inp_next;
	struct	in_addr inp_faddr;	/* foreign host table entry */
	u_short	inp_fport;		/* foreign port */
	struct	in_addr inp_laddr;	/* local host table entry */
	u_short	inp_lport;		/* local port */
};
struct socket {
	struct	sockbuf {
		u_short	sb_cc;		/* actual chars in buffer */
	} so_rcv, so_snd;
};
struct tcpcb {
	short	t_state;		/* state of this connection */
};
#define	TCPS_ESTABLISHED	TCP_ESTABLISHED	/* established */
#define	TCPS_SYN_SENT		TCP_SYN_SENT	/* active, have sent syn */
#define	TCPS_SYN_RECEIVED	TCP_SYN_RECV	/* have send and received syn */
#define	TCPS_FIN_WAIT_1		TCP_FIN_WAIT1	/* have closed, sent fin */
#define	TCPS_FIN_WAIT_2		TCP_FIN_WAIT2	/* have closed, fin is acked */
#define	TCPS_TIME_WAIT		TCP_TIME_WAIT	/* in 2*msl quiet wait after close */
#define	TCPS_CLOSED		    TCP_CLOSE	/* closed */
#define	TCPS_CLOSE_WAIT		TCP_CLOSE_WAIT	/* rcvd fin, waiting for close */
#define	TCPS_LAST_ACK		TCP_LAST_ACK	/* had fin and close; await FIN ACK */
#define	TCPS_LISTEN		    TCP_LISTEN	/* listening for connection */
#define	TCPS_CLOSING		TCP_CLOSING	/* closed xchd FIN; await FIN ACK */
#define	TCP_NSTATES	11
#endif

/*  */

#define	RTOA_OTHER	1		/* tcpRtoAlgorithm */
#define	RTOA_VANJ	4		/*   ..  */

#define	MXCN_NONE	(-1)		/* tcpMaxConn */


static struct tcpstat tcpstat;

static int tcpConnections;

static int  get_connections ();

/*  */

#define	tcpRtoAlgorithm	0
#define	tcpRtoMin	1
#define	tcpRtoMax	2
#define	tcpMaxConn	3
#define	tcpActiveOpens	4
#define	tcpPassiveOpens	5
#define	tcpAttemptFails	6
#define	tcpEstabResets	7
#define	tcpCurrEstab	8
#define	tcpInSegs	9
#define	tcpOutSegs	10
#define	tcpRetransSegs	11
#if defined(LINUX) || !defined(SUNOS4) || defined(SUNOS41)
#define	tcpInErrs	12
#else
#undef	tcpInErrs	/* 12		/* NOT IMPLEMENTED */
#endif
#ifdef LINUX
#define	tcpOutRsts	13
#else
#undef	tcpOutRsts	/* 13		/* NOT IMPLEMENTED */
#endif

#ifdef LINUX
int _read_snmp_stats ();

static int _read_tcp_stats ()
{
	char *labels, *label;
	long *values, value;
	size_t len;
	int i;

	if (_read_snmp_stats ("tcp", &labels, &values, &len) != OK)
		return NOTOK;

	for (i = 0; i < len; i++) {
		label = i == 0 ? strtok (labels, " \n") : strtok (NULL, " ");
		value = values[i];
		if (!strcmp ("RtoAlgorithm", label))
			tcpstat.tcps_rtoalgorithm = value;
		else if (!strcmp ("RtoMin", label))
			tcpstat.tcps_rtomin = value;
		else if (!strcmp ("RtoMax", label))
			tcpstat.tcps_rtomax = value;
		else if (!strcmp ("MaxConn", label))
			tcpstat.tcps_maxconn = value;
		else if (!strcmp ("ActiveOpens", label))
			tcpstat.tcps_connattempt = value;
		else if (!strcmp ("PassiveOpens", label))
			tcpstat.tcps_accepts = value;
		else if (!strcmp ("AttemptFails", label))
			tcpstat.tcps_conndrops = value;
		else if (!strcmp ("EstabResets", label))
			tcpstat.tcps_drops = value;
		else if (!strcmp ("CurrEstab", label))
			tcpConnections = value;
		else if (!strcmp ("InSegs", label))
			tcpstat.tcps_rcvtotal = value;
		else if (!strcmp ("OutSegs", label))
			tcpstat.tcps_outsegs = value;
		else if (!strcmp ("RetransSegs", label))
			tcpstat.tcps_sndrexmitpack = value;
		else if (!strcmp ("InErrs", label))
			tcpstat.tcps_ierrors = value;
		else if (!strcmp ("OutRsts", label))
			tcpstat.tcps_orsts = value;
	}

	return OK;
}
#endif


static int  o_tcp (oi, v, offset)
OI	oi;
struct type_SNMP_VarBind *v;
int	offset;
{
	int	    ifvar;
	struct tcpstat *tcps = &tcpstat;
	OID    oid = oi -> oi_name;
	OT	    ot = oi -> oi_type;
	static   int lastq = -1;

	ifvar = (int) ot -> ot_info;
	switch (offset) {
	case type_SNMP_PDUs_get__request:
		if (oid -> oid_nelem != ot -> ot_name -> oid_nelem + 1
				|| oid -> oid_elements[oid -> oid_nelem - 1] != 0)
			return int_SNMP_error__status_noSuchName;
		break;

	case type_SNMP_PDUs_get__next__request:
		if (oid -> oid_nelem == ot -> ot_name -> oid_nelem) {
			OID	new;

			if ((new = oid_extend (oid, 1)) == NULLOID)
				return NOTOK;
			new -> oid_elements[new -> oid_nelem - 1] = 0;

			if (v -> name)
				free_SNMP_ObjectName (v -> name);
			v -> name = new;
		} else
			return NOTOK;
		break;

	default:
		return int_SNMP_error__status_genErr;
	}

	switch (ifvar) {
#ifndef LINUX
	case tcpCurrEstab:
		if (get_connections (type_SNMP_PDUs_get__request) == NOTOK)
			return generr (offset);
		break;
#endif

	default:
		if (quantum != lastq) {
			lastq = quantum;

#ifndef LINUX
			if (getkmem (nl + N_TCPSTAT, (caddr_t) tcps, sizeof *tcps)
					== NOTOK)
				return generr (offset);
#else
			if (_read_tcp_stats () != OK)
				advise (LLOG_EXCEPTIONS, "failed", "read tcp stats");
#endif
		}
		break;
	}

	switch (ifvar) {
	case tcpRtoAlgorithm:
#ifdef LINUX
	    return o_integer (oi, v, tcps -> tcps_rtoalgorithm);
#else
#ifdef	TCPTV_REXMTMAX
		return o_integer (oi, v, RTOA_VANJ);
#else
		return o_integer (oi, v, RTOA_OTHER);
#endif
#endif

	case tcpRtoMin:
#ifdef LINUX
		return o_integer (oi, v, tcps -> tcps_rtomin);
#else
		return o_integer (oi, v, TCPTV_MIN * 100);	/* milliseconds */
#endif

#ifdef LINUX
		return o_integer (oi, v, tcps -> tcps_rtomax);
#else
#ifdef	TCPTV_REXMTMAX
	case tcpRtoMax:
		return o_integer (oi, v, TCPTV_REXMTMAX * 100); /*   .. */
#endif
#endif

	case tcpMaxConn:
#ifdef LINUX
		return o_integer (oi, v, tcps -> tcps_maxconn);
#else
		return o_integer (oi, v, MXCN_NONE);
#endif

#if defined(LINUX) || defined(TCPTV_REXMTMAX)
	case tcpActiveOpens:
		return o_integer (oi, v, tcps -> tcps_connattempt);

	case tcpPassiveOpens:
		return o_integer (oi, v, tcps -> tcps_accepts);

	case tcpAttemptFails:
		return o_integer (oi, v, tcps -> tcps_conndrops);

	case tcpEstabResets:
		return o_integer (oi, v, tcps -> tcps_drops);
#endif

	case tcpCurrEstab:
		return o_integer (oi, v, tcpConnections);

#if defined(LINUX) || defined(TCPTV_REXMTMAX)
	case tcpInSegs:
		return o_integer (oi, v, tcps -> tcps_rcvtotal);

	case tcpOutSegs:
#ifdef LINUX
		return o_integer (oi, v, tcps -> tcps_outsegs);
#else
		return o_integer (oi, v, tcps -> tcps_sndtotal
						  - tcps -> tcps_sndrexmitpack);
#endif

	case tcpRetransSegs:
		return o_integer (oi, v, tcps -> tcps_sndrexmitpack);
#endif

#ifdef	tcpInErrs
	case tcpInErrs:
#ifdef LINUX
		return o_integer (oi, v, tcps -> tcps_ierrors);
#elif	!defined(BSD44) && !(defined(BSD43) && defined(ultrix)) && !defined(SVR4) && !defined(__NeXT__) && !defined(SUNOS41)
		return o_integer (oi, v, tcps -> tcps_badsegs);
#else
		return o_integer (oi, v, tcps -> tcps_rcvbadsum
						  + tcps -> tcps_rcvbadoff
						  + tcps -> tcps_rcvshort);
#endif
#endif

#ifdef LINUX
#ifdef	tcpOutRsts
	case tcpOutRsts:
	    return o_integer (oi, v, tcps -> tcps_orsts);
#endif
#endif

	default:
		return int_SNMP_error__status_noSuchName;
	}
}

/*  */

static	int	tcp_states[TCP_NSTATES];


struct tcptab {
#define	TT_SIZE	10			/* object instance */
	unsigned int   tt_instance[TT_SIZE];

	struct inpcb   tt_pcb;		/* protocol control block */

	struct socket  tt_socb;		/* socket info */

	struct tcpcb   tt_tcpb;		/* TCP info */

	struct tcptab *tt_next;
};

static struct tcptab *tts = NULL;

static	int	flush_tcp_cache = 0;


static struct tcptab *get_tcpent ();

/*  */

#define	tcpConnState	0
#define	tcpConnLocalAddress 1
#define	tcpConnLocalPort 2
#define	tcpConnRemAddress 3
#define	tcpConnRemPort	4
#define	unixTcpConnSendQ 5
#define	unixTcpConnRecvQ 6

#ifdef LINUX
static struct tcptab *_read_tcp_connections(int *len)
{
	FILE *f;
	char line[256];
	int i;
	struct tcptab *tt, *t, **tp;
	unsigned char *cp;

	*len = 0;

	f = fopen ("/proc/net/tcp", "r");
	if (!f) {
		advise (LLOG_EXCEPTIONS, "failed", "open /proc/net/tcp");
		return NULL;
	}

	fgets(line, sizeof(line), f); /* header */

	tp = &tt;

	for (i = 0; fgets(line, sizeof(line), f); i++) {

		if ((t = calloc (1, sizeof (struct tcptab))) == NULL)
			adios (NULLCP, "out of memory");

		sscanf(line, "%*d: %x:%hx %x:%hx %hx %hx:%hx",
				&t -> tt_pcb.inp_laddr.s_addr, &t -> tt_pcb.inp_lport,
				&t -> tt_pcb.inp_faddr.s_addr, &t -> tt_pcb.inp_fport,
				&t -> tt_tcpb.t_state,
				&t -> tt_socb.so_snd.sb_cc, &t -> tt_socb.so_rcv.sb_cc);

		cp = t -> tt_instance;
		cp += ipaddr2oid (cp, &t -> tt_pcb.inp_laddr);
		*cp++ = ntohs (t -> tt_pcb.inp_lport);
		cp += ipaddr2oid (cp, &t -> tt_pcb.inp_faddr);
		*cp++ = ntohs (t -> tt_pcb.inp_fport);

		*tp = t; tp = &t -> tt_next;
	}

	*len = i;

	fclose (f);

	return tt;
}
#endif


static int  o_tcp_conn (oi, v, offset)
OI	oi;
struct type_SNMP_VarBind *v;
int	offset;
{
	int	    ifvar;
	int    i;
	unsigned int *ip,
			 *jp;
	struct tcptab *tt;
	struct sockaddr_in netaddr;
	OID    oid = oi -> oi_name;
	OID	    new;
	OT	    ot = oi -> oi_type;

	if (get_connections (offset) == NOTOK)
		return generr (offset);

	ifvar = (int) ot -> ot_info;
try_again:
	;
	switch (offset) {
	case type_SNMP_PDUs_get__request:
		if (oid -> oid_nelem != ot -> ot_name -> oid_nelem + TT_SIZE)
			return int_SNMP_error__status_noSuchName;
		if ((tt = get_tcpent (oid -> oid_elements + oid -> oid_nelem
							  - TT_SIZE, 0)) == NULL)
			return int_SNMP_error__status_noSuchName;
		break;

	case type_SNMP_PDUs_get__next__request:
		if ((i = oid -> oid_nelem - ot -> ot_name -> oid_nelem) != 0
				&& i < TT_SIZE) {
			for (jp = (ip = oid -> oid_elements + ot -> ot_name -> oid_nelem - 1) + i;
					jp > ip;
					jp--)
				if (*jp != 0)
					break;
			if (jp == ip)
				oid -> oid_nelem = ot -> ot_name -> oid_nelem;
			else {
				if ((new = oid_normalize (oid, TT_SIZE - i, 65536))
						== NULLOID)
					return NOTOK;
				if (v -> name)
					free_SNMP_ObjectName (v -> name);
				v -> name = oid = new;
			}
		} else if (i > TT_SIZE)
			oid -> oid_nelem = ot -> ot_name -> oid_nelem + TT_SIZE;

		if (oid -> oid_nelem == ot -> ot_name -> oid_nelem) {
			if ((tt = tts) == NULL)
				return NOTOK;

			if ((new = oid_extend (oid, TT_SIZE)) == NULLOID)
				return NOTOK;
			ip = new -> oid_elements + new -> oid_nelem - TT_SIZE;
			jp = tt -> tt_instance;
			for (i = TT_SIZE; i > 0; i--)
				*ip++ = *jp++;

			if (v -> name)
				free_SNMP_ObjectName (v -> name);
			v -> name = new;

			oid = new;	/* for try_again... */
		} else {
			if ((tt = get_tcpent (ip = oid -> oid_elements
									   + oid -> oid_nelem - TT_SIZE, 1))
					== NULL)
				return NOTOK;

			jp = tt -> tt_instance;
			for (i = TT_SIZE; i > 0; i--)
				*ip++ = *jp++;
		}
		break;

	default:
		return int_SNMP_error__status_genErr;
	}

	switch (ifvar) {
	case tcpConnState:
		if ((i = tt -> tt_tcpb.t_state) < 0
				|| i >= sizeof tcp_states / sizeof tcp_states[0]) {
			if (offset == type_SNMP_SMUX__PDUs_get__next__request)
				goto try_again;
			return int_SNMP_error__status_genErr;
		}
		return o_integer (oi, v, tcp_states[i]);

	case tcpConnLocalAddress:
		netaddr.sin_addr = tt -> tt_pcb.inp_laddr;	/* struct copy */
		return o_ipaddr (oi, v, &netaddr);

	case tcpConnLocalPort:
		return o_integer (oi, v, ntohs (tt -> tt_pcb.inp_lport) & 0xffff);

	case tcpConnRemAddress:
		netaddr.sin_addr = tt -> tt_pcb.inp_faddr;	/* struct copy */
		return o_ipaddr (oi, v, &netaddr);

	case tcpConnRemPort:
		return o_integer (oi, v, ntohs (tt -> tt_pcb.inp_fport) & 0xffff);

	case unixTcpConnSendQ:
		return o_integer (oi, v, tt -> tt_socb.so_snd.sb_cc);

	case unixTcpConnRecvQ:
		return o_integer (oi, v, tt -> tt_socb.so_rcv.sb_cc);

	default:
		return int_SNMP_error__status_noSuchName;
	}
}

/*  */

static int  tt_compar (a, b)
struct tcptab **a,
		   **b;
{
	return elem_cmp ((*a) -> tt_instance, TT_SIZE,
					 (*b) -> tt_instance, TT_SIZE);
}


static int  get_connections (offset)
int	offset;
{
	int    i;
	unsigned int  *cp;
	struct tcptab *ts,
			   *tp,
			   **tsp;
	struct inpcb  *ip;
	struct inpcb *head,
			   tcb;
#ifndef LINUX
	struct nlist nzs;
	struct nlist *nz = &nzs;
#endif
	static   int first_time = 1;
	static   int lastq = -1;

	if (quantum == lastq)
		return OK;
	if (!flush_tcp_cache
			&& offset == type_SNMP_PDUs_get__next__request
			&& quantum == lastq + 1) {			/* XXX: caching! */
		lastq = quantum;
		return OK;
	}
	lastq = quantum, flush_tcp_cache = 0;

#ifndef LINUX
	for (ts = tts; ts; ts = tp) {
		tp = ts -> tt_next;

		free ((char *) ts);
	}
	tts = NULL;

	if (getkmem (nl + N_TCB, (char *) &tcb, sizeof tcb) == NOTOK)
		return NOTOK;
	head = (struct inpcb *) nl[N_TCB].n_value;

	tsp = &tts, i = 0;
	ip = &tcb;
	while (ip -> inp_next != head) {
		if ((ts = (struct tcptab *) calloc (1, sizeof *ts)) == NULL)
			adios (NULLCP, "out of memory");
		/* no check needed for duplicate connections... */
		*tsp = ts, tsp = &ts -> tt_next, i++;

		nz -> n_name = "struct inpcb",
			  nz -> n_value = (unsigned long) ip -> inp_next;
		if (getkmem (nz, (caddr_t) &ts -> tt_pcb, sizeof ts -> tt_pcb)
				== NOTOK)
			return NOTOK;
		ip = &ts -> tt_pcb;

		nz ->n_name = "struct socket",
			 nz -> n_value = (unsigned long) ip -> inp_socket;
		if (getkmem (nz, (caddr_t) &ts -> tt_socb, sizeof ts -> tt_socb)
				== NOTOK)
			return NOTOK;

		nz ->n_name = "struct tcb",
			 nz -> n_value = (unsigned long) ip -> inp_ppcb;
		if (getkmem (nz, (caddr_t) &ts -> tt_tcpb, sizeof ts -> tt_tcpb)
				== NOTOK)
			return NOTOK;

		cp = ts -> tt_instance;
		cp += ipaddr2oid (cp, &ip -> inp_laddr);
		*cp++ = ntohs (ip -> inp_lport) & 0xffff;
		cp += ipaddr2oid (cp, &ip -> inp_faddr);
		*cp++ = ntohs (ip -> inp_fport) & 0xffff;

		if (debug && first_time) {
			OIDentifier	oids;

			oids.oid_elements = ts -> tt_instance;
			oids.oid_nelem = TT_SIZE;
			advise (LLOG_DEBUG, NULLCP,
					"add connection: %s", sprintoid (&oids));
		}
	}
	first_time = 0;
#else
	tts = _read_tcp_connections(&i);
#endif

	if ((tcpConnections = i) > 1) {
		struct tcptab **base,
				   **tse;

		if ((base = (struct tcptab **) malloc ((unsigned) (i * sizeof *base)))
				== NULL)
			adios (NULLCP, "out of memory");

		tse = base;
		for (ts = tts; ts; ts = ts -> tt_next)
			*tse++ = ts;

		qsort ((char *) base, i, sizeof *base, (IFP)tt_compar);

		tsp = base;
		ts = tts = *tsp++;

		while (tsp < tse) {
			ts -> tt_next = *tsp;
			ts = *tsp++;
		}
		ts -> tt_next = NULL;

		free ((char *) base);
	}

	return OK;
}

/*  */

static struct tcptab *get_tcpent (ip, isnext)
unsigned int *ip;
int	isnext;
{
	struct tcptab *tt;

	for (tt = tts; tt; tt = tt -> tt_next)
		switch (elem_cmp (tt -> tt_instance, TT_SIZE, ip, TT_SIZE)) {
		case 0:
			if (!isnext)
				return tt;
			if ((tt = tt -> tt_next) == NULL)
				goto out;
		/* else fall... */

		case 1:
			return (isnext ? tt : NULL);
		}

out:
	;
	flush_tcp_cache = 1;

	return NULL;
}

/*    UNIX */


static	int	unixNetstat = 1;

#ifndef LINUX
static	struct mbstat mbstat;

/*  */

#define	mbufs		0
#define	mbufClusters	1
#define	mbufFreeClusters 2
#define	mbufDrops	3
#if	defined(BSD44) || defined(SUNOS41)
#define	mbufWaits	4
#define	mbufDrains	5
#endif
#ifndef	BSD43
#define	mbufFrees	6
#endif
#ifdef	SUNOS41
#define	mbufSpaces	7
#endif


static int  o_mbuf (oi, v, offset)
OI	oi;
struct type_SNMP_VarBind *v;
int	offset;
{
	int	    ifvar;
	struct mbstat *m = &mbstat;
	OID    oid = oi -> oi_name;
	OT	    ot = oi -> oi_type;
	static   int lastq = -1;

	ifvar = (int) ot -> ot_info;
	switch (offset) {
	case type_SNMP_PDUs_get__request:
		if (oid -> oid_nelem != ot -> ot_name -> oid_nelem + 1
				|| oid -> oid_elements[oid -> oid_nelem - 1] != 0)
			return int_SNMP_error__status_noSuchName;
		break;

	case type_SNMP_PDUs_get__next__request:
		if (oid -> oid_nelem == ot -> ot_name -> oid_nelem) {
			OID	new;

			if ((new = oid_extend (oid, 1)) == NULLOID)
				return NOTOK;
			new -> oid_elements[new -> oid_nelem - 1] = 0;

			if (v -> name)
				free_SNMP_ObjectName (v -> name);
			v -> name = new;
		} else
			return NOTOK;
		break;

	default:
		return int_SNMP_error__status_genErr;
	}

	if (quantum != lastq) {
		lastq = quantum;

		if (getkmem (nl + N_MBSTAT, (caddr_t) m, sizeof *m) == NOTOK)
			return generr (offset);
	}

	switch (ifvar) {
	case mbufs:
		return o_integer (oi, v, m -> m_mbufs);

	case mbufClusters:
		return o_integer (oi, v, m -> m_clusters);

	case mbufFreeClusters:
		return o_integer (oi, v, m -> m_clfree);

	case mbufDrops:
		return o_integer (oi, v, m -> m_drops);

#ifdef	mbufWaits
	case mbufWaits:
		return o_integer (oi, v, m -> m_wait);
#endif

#ifdef	mbufDrains
	case mbufDrains:
		return o_integer (oi, v, m -> m_drain);
#endif

#ifdef	mbufFrees
	case mbufFrees:
		return o_integer (oi, v, m -> m_mbfree);
#endif

#ifdef	mbufSpaces
	case mbufSpaces:
		return o_integer (oi, v, m -> m_space);
#endif

	default:
		return int_SNMP_error__status_noSuchName;
	}
}

/*  */

#define	mbufType	0
#define	mbufAllocates	1


static int  o_mbufType (oi, v, offset)
OI	oi;
struct type_SNMP_VarBind *v;
int	offset;
{
	int	    ifnum,
			ifvar;
	struct mbstat *m = &mbstat;
	OID    oid = oi -> oi_name;
	OT	    ot = oi -> oi_type;
	static   int lastq = -1;

	ifvar = (int) ot -> ot_info;
	switch (offset) {
	case type_SNMP_PDUs_get__request:
		if (oid -> oid_nelem != ot -> ot_name -> oid_nelem + 1)
			return int_SNMP_error__status_noSuchName;
		if ((ifnum = oid -> oid_elements[oid -> oid_nelem - 1])
				>= sizeof m -> m_mtypes / sizeof m -> m_mtypes[0])
			return int_SNMP_error__status_noSuchName;
		break;

	case type_SNMP_PDUs_get__next__request:
again:
		;
		if (oid -> oid_nelem == ot -> ot_name -> oid_nelem) {
			OID	new;

			ifnum = 0;

			if ((new = oid_extend (oid, 1)) == NULLOID)
				return NOTOK;
			new -> oid_elements[new -> oid_nelem - 1] = ifnum;

			if (v -> name)
				free_SNMP_ObjectName (v -> name);
			v -> name = new;

			oid = new;	/* for hack... */
		} else {
			int	i = ot -> ot_name -> oid_nelem;

			if ((ifnum = oid -> oid_elements[i] + 1)
					>= sizeof m -> m_mtypes / sizeof m -> m_mtypes[0])
				return NOTOK;

			oid -> oid_elements[i] = ifnum;
			oid -> oid_nelem = i + 1;
		}
		break;

	default:
		return int_SNMP_error__status_genErr;
	}

	if (quantum != lastq) {
		lastq = quantum;

		if (getkmem (nl + N_MBSTAT, (caddr_t) m, sizeof *m) == NOTOK)
			return generr (offset);
	}

	/* hack to compress table size... */
	if (offset == type_SNMP_PDUs_get__next__request
			&& m -> m_mtypes[ifnum] == 0)
		goto again;

	switch (ifvar) {
	case mbufType:
		return o_integer (oi, v, ifnum);

	case mbufAllocates:
		return o_integer (oi, v, m -> m_mtypes[ifnum]);

	default:
		return int_SNMP_error__status_noSuchName;
	}
}
#endif

/*  */

init_tcp () {
	OT	    ot;

	if (ot = text2obj ("tcpRtoAlgorithm"))
		ot -> ot_getfnx = o_tcp,
			  ot -> ot_info = (caddr_t) tcpRtoAlgorithm;
	if (ot = text2obj ("tcpRtoMin"))
		ot -> ot_getfnx = o_tcp,
			  ot -> ot_info = (caddr_t) tcpRtoMin;
#ifdef	TCPTV_REXMTMAX
	if (ot = text2obj ("tcpRtoMax"))
		ot -> ot_getfnx = o_tcp,
			  ot -> ot_info = (caddr_t) tcpRtoMax;
#endif
	if (ot = text2obj ("tcpMaxConn"))
		ot -> ot_getfnx = o_tcp,
			  ot -> ot_info = (caddr_t) tcpMaxConn;
#ifdef	TCPTV_REXMTMAX
	if (ot = text2obj ("tcpActiveOpens"))
		ot -> ot_getfnx = o_tcp,
			  ot -> ot_info = (caddr_t) tcpActiveOpens;
	if (ot = text2obj ("tcpPassiveOpens"))
		ot -> ot_getfnx = o_tcp,
			  ot -> ot_info = (caddr_t) tcpPassiveOpens;
	if (ot = text2obj ("tcpAttemptFails"))
		ot -> ot_getfnx = o_tcp,
			  ot -> ot_info = (caddr_t) tcpAttemptFails;
	if (ot = text2obj ("tcpEstabResets"))
		ot -> ot_getfnx = o_tcp,
			  ot -> ot_info = (caddr_t) tcpEstabResets;
#endif
	if (ot = text2obj ("tcpCurrEstab"))
		ot -> ot_getfnx = o_tcp,
			  ot -> ot_info = (caddr_t) tcpCurrEstab;
#ifdef	TCPTV_REXMTMAX
	if (ot = text2obj ("tcpInSegs"))
		ot -> ot_getfnx = o_tcp,
			  ot -> ot_info = (caddr_t) tcpInSegs;
	if (ot = text2obj ("tcpOutSegs"))
		ot -> ot_getfnx = o_tcp,
			  ot -> ot_info = (caddr_t) tcpOutSegs;
	if (ot = text2obj ("tcpRetransSegs"))
		ot -> ot_getfnx = o_tcp,
			  ot -> ot_info = (caddr_t) tcpRetransSegs;
#endif
#ifdef	tcpInErrs
	if (ot = text2obj ("tcpInErrs"))
		ot -> ot_getfnx = o_tcp,
			  ot -> ot_info = (caddr_t) tcpInErrs;
#endif
#ifdef	tcpOutRsts
	if (ot = text2obj ("tcpOutRsts"))
		ot -> ot_getfnx = o_tcp,
			  ot -> ot_info = (caddr_t) tcpOutRsts;
#endif

	if (ot = text2obj ("tcpConnState"))
		ot -> ot_getfnx = o_tcp_conn,
			  ot -> ot_info = (caddr_t) tcpConnState;
	if (ot = text2obj ("tcpConnLocalAddress"))
		ot -> ot_getfnx = o_tcp_conn,
			  ot -> ot_info = (caddr_t) tcpConnLocalAddress;
	if (ot = text2obj ("tcpConnLocalPort"))
		ot -> ot_getfnx = o_tcp_conn,
			  ot -> ot_info = (caddr_t) tcpConnLocalPort;
	if (ot = text2obj ("tcpConnRemAddress"))
		ot -> ot_getfnx = o_tcp_conn,
			  ot -> ot_info = (caddr_t) tcpConnRemAddress;
	if (ot = text2obj ("tcpConnRemPort"))
		ot -> ot_getfnx = o_tcp_conn,
			  ot -> ot_info = (caddr_t) tcpConnRemPort;

	if (ot = text2obj ("unixTcpConnSendQ"))
		ot -> ot_getfnx = o_tcp_conn,
			  ot -> ot_info = (caddr_t) unixTcpConnSendQ;
	if (ot = text2obj ("unixTcpConnRecvQ"))
		ot -> ot_getfnx = o_tcp_conn,
			  ot -> ot_info = (caddr_t) unixTcpConnRecvQ;

	tcp_states[TCPS_CLOSED] = TCPS_CLOSED;
	tcp_states[TCPS_LISTEN] = TCPS_LISTEN;
	tcp_states[TCPS_SYN_SENT] = TCPS_SYN_SENT;
	tcp_states[TCPS_SYN_RECEIVED] = TCPS_SYN_RECEIVED;
	tcp_states[TCPS_ESTABLISHED] = TCPS_ESTABLISHED;
	tcp_states[TCPS_CLOSE_WAIT] = TCPS_CLOSE_WAIT;
	tcp_states[TCPS_FIN_WAIT_1] = TCPS_FIN_WAIT_1;
	tcp_states[TCPS_CLOSING] = TCPS_CLOSING;
	tcp_states[TCPS_LAST_ACK] = TCPS_LAST_ACK;
	tcp_states[TCPS_FIN_WAIT_2] = TCPS_FIN_WAIT_2;
	tcp_states[TCPS_TIME_WAIT] = TCPS_TIME_WAIT;

	if (ot = text2obj ("unixNetstat"))
		ot -> ot_getfnx = o_generic,
			  ot -> ot_info = (caddr_t) &unixNetstat;

#ifndef LINUX
	if (ot = text2obj ("mbufs"))
		ot -> ot_getfnx = o_mbuf,
			  ot -> ot_info = (caddr_t) mbufs;
	if (ot = text2obj ("mbufClusters"))
		ot -> ot_getfnx = o_mbuf,
			  ot -> ot_info = (caddr_t) mbufClusters;
	if (ot = text2obj ("mbufFreeClusters"))
		ot -> ot_getfnx = o_mbuf,
			  ot -> ot_info = (caddr_t) mbufFreeClusters;
	if (ot = text2obj ("mbufDrops"))
		ot -> ot_getfnx = o_mbuf,
			  ot -> ot_info = (caddr_t) mbufDrops;
#ifdef	mbufWaits
	if (ot = text2obj ("mbufWaits"))
		ot -> ot_getfnx = o_mbuf,
			  ot -> ot_info = (caddr_t) mbufWaits;
#endif
#ifdef	mbufDrains
	if (ot = text2obj ("mbufDrains"))
		ot -> ot_getfnx = o_mbuf,
			  ot -> ot_info = (caddr_t) mbufDrains;
#endif
#ifdef	mbufFrees
	if (ot = text2obj ("mbufFrees"))
		ot -> ot_getfnx = o_mbuf,
			  ot -> ot_info = (caddr_t) mbufFrees;
#endif
#ifdef	mbufSpaces
	if (ot = text2obj ("mbufSpaces"))
		ot -> ot_getfnx = o_mbuf,
			  ot -> ot_info = (caddr_t) mbufSpaces;
#endif
	if (ot = text2obj ("mbufType"))
		ot -> ot_getfnx = o_mbufType,
			  ot -> ot_info = (caddr_t) mbufType;
	if (ot = text2obj ("mbufAllocates"))
		ot -> ot_getfnx = o_mbufType,
			  ot -> ot_info = (caddr_t) mbufAllocates;
#endif
}
