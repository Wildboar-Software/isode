/* isoservent.h - ISO services database access routines */

#ifndef	_ISOSERVENT_
#define	_ISOSERVENT_

struct isoservent {
	char         *is_entity;	/* name of entity */
	char         *is_provider;	/* name of service provider */

#define	ISSIZE	64		/* xSAP selector/ID */
	int		  is_selectlen;
	union {
		char		is_un_selector[ISSIZE];
		unsigned short  is_un_port;
	}		un_is;
#define	is_selector	un_is.is_un_selector
#define	is_port		un_is.is_un_port

	char        **is_vec;	/* exec vector */
	char        **is_tail;	/* next free slot in vector */
};

int	setisoservent (), endisoservent ();

struct isoservent *getisoservent ();

struct isoservent *getisoserventbyname ();
struct isoservent *getisoserventbyselector ();
struct isoservent *getisoserventbyport ();

#endif
