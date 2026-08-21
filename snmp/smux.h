/* smux.h - SMUX include file */
/*
 * Contributed by NYSERNet Inc.  This work was partially supported by the
 * U.S. Defense Advanced Research Projects Agency and the Rome Air Development
 * Center of the U.S. Air Force Systems Command under contract number
 * F30602-88-C-0016.
 */

#ifndef	PEPYPATH
#include <isode/pepsy/SNMP-types.h>
#else
#include "SNMP-types.h"
#endif

#define	readOnly	int_SNMP_operation_readOnly
#define	readWrite	int_SNMP_operation_readWrite
#define	delete		int_SNMP_operation_delete

#define	goingDown		int_SNMP_ClosePDU_goingDown
#define	unsupportedVersion	int_SNMP_ClosePDU_unsupportedVersion
#define	packetFormat		int_SNMP_ClosePDU_packetFormat
#define	protocolError		int_SNMP_ClosePDU_protocolError
#define	internalError		int_SNMP_ClosePDU_internalError
#define	authenticationFailure	int_SNMP_ClosePDU_authenticationFailure

#define	invalidOperation	(-1)
#define	parameterMissing	(-2)
#define	systemError		(-3)
#define	youLoseBig		(-4)
#define	congestion		(-5)
#define	inProgress		(-6)

extern	integer	smux_errno;
extern	char	smux_info[];

int	smux_init (int debug);				/* INIT */
int	smux_simple_open (OID identity, char *description, char *commname, int commlen);			/* (simple) OPEN */
int	smux_close (int reason);				/* CLOSE */
int	smux_register (OID subtree, int priority, int operation);			/* REGISTER */
int	smux_response (struct type_SNMP_GetResponse__PDU *event);			/* RESPONSE */
int	smux_wait (struct type_SNMP_SMUX__PDUs **event, int secs);				/* WAIT */
int	smux_trap (int generic, int specific, struct type_SNMP_VarBindList *bindings);				/* TRAP */

char   *smux_error (integer i);				/* TEXTUAL ERROR */

struct smuxEntry {
	char   *se_name;

	OIDentifier se_identity;
	char   *se_password;

	int	    se_priority;
};

int	setsmuxEntry (int f), endsmuxEntry (void);

struct smuxEntry *getsmuxEntry (void);

struct smuxEntry *getsmuxEntrybyname (char *name);
struct smuxEntry *getsmuxEntrybyidentity (OID identity);
