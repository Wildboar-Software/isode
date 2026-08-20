/* clns.h - support for MIB realization of the experimental CLNS group */
/*
 * Contributed by NYSERNet Inc.  This work was partially supported by the
 * U.S. Defense Advanced Research Projects Agency and the Rome Air Development
 * Center of the U.S. Air Force Systems Command under contract number
 * F30602-88-C-0016.
 */
#include <stdint.h>
#include <sys/socket.h>

#ifndef	BSD44
#define	AF_ISO	AF_MAX		/* any value will do */

struct iso_addr {
	uint8_t	isoa_len;	    /* length in octets */
	char	isoa_genaddr[20];   /* general opaque address */
};

struct sockaddr_iso {
	uint8_t	siso_len;	/* length */
	uint8_t	siso_family;	/* address family */
	uint8_t	siso_plen;	/* psel length */
	uint8_t	siso_slen;	/* ssel length */
	uint8_t	siso_tlen;	/* tsel length */

	struct iso_addr siso_addr;	/* network address */

	uint8_t	siso_pad[6];	/* space for gosip v2 sels */
};
#define	siso_nlen	siso_addr.isoa_len
#define	siso_data	siso_addr.isoa_genaddr

#else
#include <netiso/iso.h>
#endif
