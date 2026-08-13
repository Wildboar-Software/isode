/* objects.h - MIB objects */

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

#include <stdint.h>
#ifndef	PEPYPATH
#include <isode/psap.h>
#else
#include "psap.h"
#endif
#include "SNMP-types.h"

typedef struct object_syntax object_syntax, *OS;

struct object_syntax {
	char   *os_name;			/* syntax name */

	int (*os_encode)(void *x, PE *pe);			/* data -> PE */
	int (*os_decode)(void **x, PE pe);			/* PE -> data */
	void (*os_free)(void *x);			/* free data */

	int (*os_parse)(void **x, char *s);			/* str -> data */
	void (*os_print)(void *x, OS os);			/* data -> tty */

	char  **os_data1;			/* for moresyntax() in snmpi... */
	int	    os_data2;			/* .. */
};
#define	NULLOS	((OS) 0)

typedef	int	(*EncoderFunction) (void *x, PE *pe);
typedef	int	(*DecoderFunction) (void **x, PE pe);
typedef	void (*FreeFunction) (void *x);
typedef	int	(*ParseFunction) (void **x, char *s);
typedef	void (*PrintFunction) (void *x, OS os);

void readsyntax (void);
int	add_syntax (
	char *name,
	EncoderFunction f_encode,
	DecoderFunction f_decode,
	FreeFunction f_free,
	ParseFunction f_parse,
	PrintFunction f_print
);
OS	text2syn ();

typedef struct object_instance object_instance, *OI;

typedef struct object_type {
	char   *ot_text;			/* OBJECT DESCRIPTOR */
	char   *ot_id;			/* OBJECT IDENTIFIER */
	OID	    ot_name;			/* .. */

	OS	    ot_syntax;			/* SYNTAX */

	int	    ot_access;			/* ACCESS */
#define	OT_NONE		0x00
#define	OT_RDONLY	0x01
#define	OT_WRONLY	0x02
#define	OT_RDWRITE	(OT_RDONLY | OT_WRONLY)

	uint32_t  ot_views;			/* for views */

	int	    ot_status;			/* STATUS */
#define	OT_OBSOLETE	0x00
#define	OT_MANDATORY	0x01
#define	OT_OPTIONAL	0x02
#define	OT_DEPRECATED	0x03

	caddr_t ot_info;			/* object information */
	int	(*ot_getfnx)(OI oi, struct type_SNMP_VarBind *v, int offset);			/* get/get-next method */
	int	(*ot_setfnx)(OI oi, struct type_SNMP_VarBind *v, int offset);			/* set method */

#define	type_SNMP_PDUs_commit	(-1)
#define	type_SNMP_PDUs_rollback	(-2)

	caddr_t ot_save;			/* for set method */

	caddr_t ot_smux;			/* for SMUX */

	struct object_type *ot_chain;	/* hash-bucket for text2obj */

	struct object_type *ot_sibling;	/* linked-list for name2obj */
	struct object_type *ot_children;	/* .. */

	struct object_type *ot_next;	/* linked-list for get-next */
}		object_type, *OT;
#define	NULLOT	((OT) 0)

int	readobjects ();
int	add_objects ();
OT	name2obj (), text2obj ();
OID	text2oid ();
char   *oid2ode_aux ();

struct object_instance {
	OID	    oi_name;			/* instance OID */

	OT	    oi_type;			/* prototype */
};
#define	NULLOI	((OI) 0)

OI	name2inst (), next2inst (), text2inst ();

extern	IFP	o_advise;

int	o_generic (OI oi, struct type_SNMP_VarBind *v, int offset);
int s_generic (OI oi, struct type_SNMP_VarBind *v, int offset);

int	o_number ();
int	o_longword ();
#define	o_integer(oi,v,value)	o_longword ((oi), (v), (integer) (value))

int	o_string ();
int	o_qbstring ();

int	o_specific ();
#define	o_ipaddr(oi,v,value)	o_specific ((oi), (v), (caddr_t) (value))
#ifdef	BSD44
#define	o_clnpaddr(oi,v,value)	o_specific ((oi), (v), (caddr_t) (value))
#endif

int	mediaddr2oid ();
#define	ipaddr2oid(ip,addr) \
	mediaddr2oid ((ip), (uint8_t*) (addr), sizeof (struct in_addr), 0)
#ifdef	BSD44
#define	clnpaddr2oid(ip,addr) \
	mediaddr2oid ((ip), \
		      (uint8_t *) (addr) -> isoa_genaddr, \
		      (int) (addr) -> isoa_len, 1)
#endif

OID	oid_extend (), oid_normalize ();

extern	int	debug;
extern	char	PY_pepy[BUFSIZ];

char   *strdup ();
