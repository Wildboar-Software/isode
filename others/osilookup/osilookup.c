/* osilookup.c - convert entry in /etc/osi.hosts to isoentities format */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/osilookup/RCS/osilookup.c,v 9.0 1992/06/16 12:43:08 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/others/osilookup/RCS/osilookup.c,v 9.0 1992/06/16 12:43:08 isode Rel $
 *
 * Contributed by John A. Scott, the MITRE Corporation
 *
 * N.B.:	I whipped up this code quickly to fill a need I had.  I
 *		do not, it any way, shape, or form, warrant its output.
 *
 *
 * $Log: osilookup.c,v $
 * Revision 9.0  1992/06/16  12:43:08  isode
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


#include "config.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#if defined(SUNLINK_5_2) || defined(SUNLINK_6_0) || defined(SUNLINK_7_0)

/*    SunLink OSI */

#ifndef	SUNLINK_6_0
#include <sys/ieee802.h>
#else
#include <net/if_ieee802.h>
#endif
#include <netosi/osi.h>
#include <netosi/osi_addr.h>
#include <sunosi/mapds_user.h>

/*  */

/* ARGSUSED */

main (int argc, char **argv, char **envp) {
	int	    len,
			paddr_type;
	char   *prefix,
		   *service,
		   buf[BUFSIZ],
		   buf2[BUFSIZ];
	OSI_ADDR p_addr;

	if (argc < 2) {
		fprintf (stderr,"usage: %s host [service]\n", argv[0]);
		exit (0);
	}
	service = (argc > 2) ? argv[2] : "FTAM";

	/* SUNLink OSI directory lookup */
	mds_lookup (argv[1], service, &p_addr);

	/* SUNLink function to slice out SAP bytes from full address */
	paddr_type = 0;
	if ((len = osi_get_sap (&p_addr, buf, sizeof buf, OSI_NSAP, &paddr_type))
			<= 0) {
		fprintf (stderr, "no entry for %s %s\n", argv[1], service);
		exit (1);
	}

	buf2[explode (buf2, (u_char *) buf, len)]= NULL;
	switch (paddr_type) {
	case AF_NBS:
		prefix = "49";
		break;

	case AF_OSINET:
		prefix = "470004";
		break;

	default:
		prefix = "";
		break;
	}
	printf ("\t\t\t\tNS+%s%s\n\n", prefix, buf2);

	exit (0);
}

/* so we don't have to load libisode.a */

static char nib2hex[0x10] = {
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};


static int  explode (a, b, n)
char  *a;
u_char *b;
int    n;
{
	int    i;
	u_char c;

	for (i = 0; i < n; i++) {
		c = *b++;
		*a++ = nib2hex[(c & 0xf0) >> 4];
		*a++ = nib2hex[(c & 0x0f)];
	}
	*a = NULL;

	return (n * 2);
}

#else

/*   Not SunLink OSI */

main (argc, argv, envp)
int	argc;
char  **argv,
      **envp;
{
	fprintf (stderr,"%s only works on SunOS with SunLink OSI\n", argv[0]);
	exit (0);
}

#endif