/* ftp_var.h - FTP global variables */

/*
 *	Shamelessly taken from UCB
 */

#include "general.h"

/*
 * Options and other state info.
 */
extern int	trace;			/* trace packets exchanged */
extern int	hash;			/* print # for each buffer transferred */
extern int	verbose;		/* print messages to/from server */
extern int	connected;		/* connected to server */
extern int	fromatty;		/* input is from a terminal */
extern int	interactive;		/* interactively prompt on m* cmds */
extern int	bell;			/* ring bell on cmd completion */
extern int	doglob;			/* glob local file names */
extern int	autologin;		/* establish user account on connection */

extern char	typename[32];		/* name of file transfer type */
extern int	type;			/* file transfer type */
extern char	structname[32];		/* name of file transfer structure */
extern int	stru;			/* file transfer structure */
extern char	formname[32];		/* name of file transfer format */
extern int	form;			/* file transfer format */
extern char	modename[32];		/* name of file transfer mode */
extern int	mode;			/* file transfer mode */
extern char	bytename[32];		/* local byte size in ascii */
extern int	bytesize;		/* local byte size in binary */

extern char	*hostname;		/* name of host connected to */

extern struct	servent *sp;		/* service spec for tcp/ftp */

#include <setjmp.h>
extern jmp_buf	toplevel;		/* non-local goto stuff for cmd scanner */

extern char	line[200];		/* input line buffer */
extern char	*stringbase;		/* current scan point in line buffer */
extern char	argbuf[200];		/* argument storage buffer */
extern char	*argbase;		/* current storage point in arg buffer */
extern int	margc;			/* count of arguments on input line */
extern char	*margv[20];		/* args parsed from input line */

extern int	options;		/* used during socket creation */

/*
 * Format of command table.
 */
struct cmd {
	char	*c_name;	/* name of command */
	char	*c_help;	/* help string */
	char	c_bell;		/* give bell when command completes */
	char	c_conn;		/* must be connected to use command */
	int	(*c_handler)(void);	/* function to call */
};

extern	char *tail(void);
extern	char *remglob(void);

/* global interface variables */
extern int ftp_directory;/* TRUE if last ftp_exist was a multiple listing */
extern char ftp_error_buffer[BUFSIZ];
extern char *ftp_error; /* points to FTP diagnostic string */

/* ftp.c client routines */
int hookup(char *host, const int port);
int login(const char *user, const char *pass, const char *acct);
int command(char *fmt, ...);
int getreply(const int expecteof);
int sendrequest(const char *cmd, const char *remote);
int recvrequest(const char *cmd, const char *remote);

/* ftp_lib.c bridge routine not declared in ftamsystem.h */
int ftp_create(const char *filename);
