#ifndef	lint
static char *RCSid = "$Header: /xtel/isode/isode/others/ntp/RCS/ntpsubs.c,v 9.0 1992/06/16 12:42:48 isode Rel $";
#endif	lint

/*
 * $Header: /xtel/isode/isode/others/ntp/RCS/ntpsubs.c,v 9.0 1992/06/16 12:42:48 isode Rel $
 * subroutines for ntp - based on 3.4 ntp code.
 * $Log: ntpsubs.c,v $
 * Revision 9.0  1992/06/16  12:42:48  isode
 * Release 8.0
 *
 */

#include <errno.h>
#include "ntp.h"



#define	TRUE	1
#define	FALSE	0

/*
 *  The nice thing here is that the quantity is NEVER signed.
 */
double
ul_fixed_to_double (struct l_fixedpt *t) {
	double a, b;
#ifdef	GENERIC_UNS_BUG
	int i;

	i = ntohl(t->fraction);
	a = (long)((i >> 1) & 0x7fffffff);
	a *= 2.0;
	if (i & 1)
		a += 1.0;
	a = a / (4.294967296e9);	/* shift dec point over by 32 bits */
	i = ntohl(t->int_part);
	b = (long)((i >> 1) & 0x7fffffff);
	b *= 2.0;
	if (i & 1)
		b += 1.0;
#else	/* GENERIC_UNS_BUG */
	a = (unsigned long) ntohl(t->fraction);
#ifdef	VAX_COMPILER_FLT_BUG
	if (a < 0.0) a += 4.294967296e9;
#endif
	a = a / (4.294967296e9);/* shift dec point over by 32 bits */
	b = (unsigned long) ntohl(t->int_part);
#ifdef	VAX_COMPILER_FLT_BUG
	if (b < 0.0) b += 4.294967296e9;
#endif
#endif	/* GENERIC_UNS_BUG */
	return (a + b);
}

/*
 *  Here we have to worry about the high order bit being signed
 */

#if	0
double
l_fixed_to_double (struct l_fixedpt *t) {
	double a,b;

	if (ntohl(t->int_part) & 0x80000000) {
		a = ntohl(~t->fraction);
#ifdef	VAX_COMPILER_FLT_BUG
		if (a < 0.0) a += 4.294967296e9;
#endif
		a = a / (4.294967296e9);
		b = ntohl(~t->int_part);
#ifdef	VAX_COMPILER_FLT_BUG
		if (b < 0.0) b += 4.294967296e9;
#endif
		a += b;
		a = -a;
	} else {
		a = ntohl(t->fraction);
#ifdef	VAX_COMPILER_FLT_BUG
		if (a < 0.0) a += 4.294967296e9;
#endif
		a = a / (4.294967296e9);
		b = ntohl(t->int_part);
#ifdef	VAX_COMPILER_FLT_BUG
		if (b < 0.0) b += 4.294967296e9;
#endif
		a += b;
	}
	return (a);
}
#endif

/*
 *  Here we have to worry about the high order bit being signed
 */
double
s_fixed_to_double (struct s_fixedpt *t) {
	double a;

	if (ntohs(t->int_part) & 0x8000) {
		a = (int)ntohs(~t->fraction & 0xFFFF);
		a = a / 65536.0;	/* shift dec point over by 16 bits */
		a +=  (int)ntohs(~t->int_part & 0xFFFF);
		a = -a;
	} else {
		a = ntohs(t->fraction);
		a = a / 65536.0;	/* shift dec point over by 16 bits */
		a += ntohs(t->int_part);
	}
	return (a);
}

void
double_to_l_fixed (struct l_fixedpt *t, double value) {
	double temp;

	if (value >= (double) 0.0) {
		t->int_part = value;
		temp = value - t->int_part;
		temp *= 4.294967296e9;
		t->fraction = temp;
		t->int_part = htonl(t->int_part);
		t->fraction = htonl(t->fraction);
	} else {
		value = -value;
		t->int_part = value;
		temp = value - t->int_part;
		temp *= 4.294967296e9;
		t->fraction = temp;
		t->int_part = htonl(~t->int_part);
		t->fraction = htonl(~t->fraction);
	}
}

void
double_to_s_fixed (struct s_fixedpt *t, double value) {
	double temp;

	if (value >= (double) 0.0) {
		t->int_part = value;
		temp = value - t->int_part;
		temp *= 65536.0;
		t->fraction = temp;
		t->int_part = htons(t->int_part);
		t->fraction = htons(t->fraction);
	} else {
		value = -value;
		t->int_part = value;
		temp = value - t->int_part;
		temp *= 65536.0;
		t->fraction = temp;
		t->int_part = htons(~t->int_part);
		t->fraction = htons(~t->fraction);
	}
}
/*
	in the sun, trying to assign a float between 2^31 and 2^32
	results in the value 2^31.  Neither 4.2bsd nor VMS have this
	problem.  Reported it to Bob O'Brien of SMI
*/
#ifdef	SUN_FLT_BUG
void
tstamp (struct l_fixedpt *stampp, struct timeval *tvp) {
	int tt;
	double dd;

	stampp->int_part = ntohl(JAN_1970 + tvp->tv_sec);
	dd = (float) tvp->tv_usec / 1000000.0;
	tt = dd * 2147483648.0;
	stampp->fraction = ntohl((tt << 1));
}
#else
void
tstamp (struct l_fixedpt *stampp, struct timeval *tvp) {
	stampp->int_part = ntohl((u_long) (JAN_1970 + tvp->tv_sec));
	stampp->fraction = ntohl((u_long) ((float) tvp->tv_usec * 4294.967295));
}
#endif

/*
 * ntoa is similar to inet_ntoa, but cycles through a set of 8 buffers
 * so it can be invoked several times in a function parameter list.
 */

char *
ntoa (struct sockaddr_in *nsin) {
	static int i = 0;
	static char bufs[8][64];

	sprintf (bufs[i], ntohs (nsin->sin_port) ?
			 "INET %s/%d" : "INET %s", inet_ntoa (nsin->sin_addr),
			 ntohs(nsin->sin_port));
	return bufs[i];
}

