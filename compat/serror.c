/* serror.c - get system error */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/serror.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/compat/RCS/serror.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 *
 * $Log: serror.c,v $
 * Revision 9.0  1992/06/16  12:07:00  isode
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


/* LINTLIBRARY */

#include <stdio.h>
#include <string.h>
#include "general.h"
#include "manifest.h"

/*    DATA */


/*  */

char *
sys_errname (int i)
{
	static char buffer[128];
	char* se;

#ifdef	LINUX
	if (strerror_r(i, buffer, 128) != 0)
		(void) sprintf (buffer, "Error %d", i);
#else
	se = strerror(i);
	strncpy(buffer, se, 128);
	buffer[127] = '\0';		/* to be sure... */
#endif

	return buffer;
}
