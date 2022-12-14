/* socket.c - dish -pipe support */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/dish/RCS/socket.c,v 9.0 1992/06/16 12:44:21 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/others/quipu/uips/dish/RCS/socket.c,v 9.0 1992/06/16 12:44:21 isode Rel $
 *
 *
 * $Log: socket.c,v $
 * Revision 9.0  1992/06/16  12:44:21  isode
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
#include "quipu/util.h"
#include "tailor.h"
#include "general.h"

#ifdef SOCKETS   	/* USE INTERNET SOCKETS */

#include "internet.h"

get_dish_sock (isock, pid, islocal)
struct sockaddr_in *isock;
int	pid,
	islocal;
{
	int    myppid;
	char * getenv ();
	char * ptr, * prnt;
	static char buffer [BUFSIZ];
	static char parent [BUFSIZ];
	int     portno;
	char   *dp;
	struct hostent *hp;

	if ((myppid = pid) == 0)
		myppid = getppid ();

	if (pid != 0 || (ptr = getenv ("DISHPROC")) == NULLCP) {
		char	*cp;

		portno = (myppid & 0xffff) | 0x8000;
		if (!islocal) {
			if ((hp = gethostbystring (cp = getlocalhost ()))
					== NULL) {
				fprintf (stderr,"%s: unknown host", cp);
				return (-1);
			}
			sprintf (buffer, "%s %d",
					 inet_ntoa (*(struct in_addr *)
								hp -> h_addr),
					 portno);
		} else
			sprintf (buffer, "127.0.0.1 %d", portno);

		setenv ("DISHPROC", ptr = buffer);
	}

	if (pid !=0 || (prnt = getenv ("DISHPARENT")) == NULLCP) {
		sprintf (parent, "%d", myppid);
		setenv ("DISHPARENT", prnt = parent);
	}


	if (sscanf (prnt, "%d", &pid) != 1) {
		fprintf (stderr,"DISHPARENT malformed");
		return (-1);
	}

	if ((dp = index (ptr, ' ')) == NULLCP || sscanf (dp + 1, "%d", &portno) != 1) {
		fprintf (stderr,"DISHPROC malformed");
		return (-1);
	}
	*dp = NULL;

	if ((hp = gethostbystring (ptr)) == NULL) {
		fprintf (stderr,"%s: unknown host in DISHPROC", ptr);
		return (-1);
	}
	*dp = ' ';

	bzero ((char *) isock, sizeof *isock);
	isock -> sin_family = hp -> h_addrtype;
	isock -> sin_port = htons ((u_short) portno);
	inaddr_copy (hp, isock);

	return (0);

}

#else	/* USE UNIX NAMED PIPES */


void
dummy () {
	;
}

#endif
