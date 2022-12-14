/* unbind.c - dish shell unbind and squid commands */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/dish/RCS/unbind.c,v 9.0 1992/06/16 12:44:21 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/others/quipu/uips/dish/RCS/unbind.c,v 9.0 1992/06/16 12:44:21 isode Rel $
 *
 *
 * $Log: unbind.c,v $
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
#include <errno.h>
#include "quipu/util.h"
#include "tailor.h"
#include "general.h"




#ifdef SOCKETS   	/* USE INTERNET SOCKETS */

#include "internet.h"

int
main (int argc, char *argv[]) {
	int 			sd,res,status;
	struct sockaddr_in	sin_buf;
	struct sockaddr_in	* sin = &sin_buf;
	char			buffer [BUFSIZ];
	char 			* ptr;

	if ((sd = start_tcp_client ((struct sockaddr_in *) 0, 0)) == NOTOK) {
		perror("start_tcp_client");
		exit(-20);
	}

	if (get_dish_sock (sin, 0, 1) != 0)
		exit (-21);

	if (join_tcp_server (sd, sin) == NOTOK) {
		fprintf (stderr,"No connection and no cache !!!\n");
		close_tcp_socket (sd);
		exit (0);
	}

	if ((ptr = rindex (argv[0], '/')) == NULLCP)
		strcpy (buffer,argv[0]);
	else
		strcpy (buffer,++ptr);

	argc--,argv++;

	while (argc--) {
		strcat (buffer, " \"");
		strcat (buffer, *argv++);
		strcat (buffer, "\"");
	}
	strcat (buffer, "\n");

	if (send(sd, buffer, strlen(buffer), 0) == -1) {
		perror("send");
		close_tcp_socket (sd);
		exit (-25);
	}

	if ((res = recv(sd, buffer, BUFSIZ-1, 0)) == -1) {
		perror ("recv");
		close_tcp_socket (sd);
		exit (-26);
	}
	*(buffer + res) = 0;

	if (*buffer == '2') {
		status = 1;
		if (res > 1)
			write (2,&buffer[1],--res);
		while ( (res = recv(sd, buffer, BUFSIZ, 0)) > 0)
			write (2,buffer,res);
	} else if (*buffer == '1') {
		status = 0;
		if (res > 1)
			write (1,&buffer[1],--res);
		while ( (res = recv(sd, buffer, BUFSIZ, 0)) > 0)
			write (1,buffer,res);
	}

	close_tcp_socket (sd);

	exit (status);

	return status;
}


#else	/* USE UNIX NAMED PIPES */

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

char retfile [LINESIZE];
int fd;

int
main (int argc, char **argv) {
	int res;
	char buffer [BUFSIZ];
	char sendfile [LINESIZE];
	int i;
	char * ptr;
	void pipe_quit ();
	char * getenv(), *sprintf();

	umask(0);
	sprintf (retfile,"/tmp/dish%d",getpid());
	if ( (ptr = getenv ("DISHPROC")) == NULLCP ) {
		sprintf (sendfile, "/tmp/dish-%d", getppid ());
		setenv ("DISHPROC", sendfile);
	} else
		strcpy (sendfile, ptr);

	setbuf (stdout,NULLCP);
	setbuf (stderr,NULLCP);

	if (mknod (retfile,S_IFIFO|0660,0) == -1) {
		fprintf (stderr,"Can't create result file %s\n",retfile);
		exit (-5);
	}

	for (i=1; i<=15; i++)
		signal(i,pipe_quit);

	if ((fd = open (sendfile,O_WRONLY|O_NDELAY)) == -1) {
		fprintf (stderr,"No connection and no cache !!!\n");
		unlink (retfile);
		exit (0);
	}

	argc--;
	if ((ptr = rindex (argv[0],'/')) == NULLCP)
		sprintf (buffer,"%s:%s",retfile,argv[0]);
	else
		sprintf (buffer,"%s:%s",retfile,++ptr);
	*argv++;

	while (argc--) {
		strcat (buffer," ");
		strcat (buffer,*argv++);
	}

	if (( res =write (fd, buffer,strlen (buffer))) == -1) {
		fprintf (stderr,"Write failed\n");
		close (fd);
		unlink (retfile);
		exit (-2);
	}
	close (fd);


	/* get results */
	if (( fd = open (retfile,O_RDONLY)) < 0) {
		fprintf (stderr,"Can't read results\n");
		unlink (retfile);
		exit (-3);
	}

	if (( res = read (fd,buffer,BUFSIZ)) == -1) {
		fprintf (stderr,"Read failed (%d)\n",errno);
		unlink (retfile);
		close (fd);
		exit (-4);
	}

	*(buffer+res) = 0;

	if (*buffer == '2')
		fputs (&buffer[1], stderr);
	else if (*buffer == '1')
		fputs (&buffer[1], stdout);

	close (fd);
	unlink (retfile);

	if (*buffer == '2')
		exit (-1);
}

void
pipe_quit (int sig) {
	unlink (retfile);
	fprintf (stderr,"(signal %d) exiting...\n",sig);
	exit (0);
}

#endif
