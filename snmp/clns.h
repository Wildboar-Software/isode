/* clns.h - support for MIB realization of the experimental CLNS group */

/*
 * 
 *
 * Contributed by NYSERNet Inc.  This work was partially supported by the
 * U.S. Defense Advanced Research Projects Agency and the Rome Air Development
 * Center of the U.S. Air Force Systems Command under contract number
 * F30602-88-C-0016.
 *
 *
 * 
 *
 *
 *
 */



#ifndef	BSD44
#define	AF_ISO	AF_MAX		/* any value will do */

struct iso_addr {
	u_char	isoa_len;	    /* length in octets */
	char	isoa_genaddr[20];   /* general opaque address */
};

struct sockaddr_iso {
	u_char	siso_len;	/* length */
	u_char	siso_family;	/* address family */
	u_char	siso_plen;	/* psel length */
	u_char	siso_slen;	/* ssel length */
	u_char	siso_tlen;	/* tsel length */

	struct iso_addr siso_addr;	/* network address */

	u_char	siso_pad[6];	/* space for gosip v2 sels */
};
#define	siso_nlen	siso_addr.isoa_len
#define	siso_data	siso_addr.isoa_genaddr

#else
#include <netiso/iso.h>
#endif
