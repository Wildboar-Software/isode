/* snmp-g.h - SNMP group */

#include "psap.h"

struct snmpstat {
	integer	s_inpkts;
	integer	s_outpkts;
	integer	s_badversions;
	integer	s_badcommunitynames;
	integer	s_badcommunityuses;
	integer	s_asnparseerrs;
	integer	s_totalreqvars;
	integer	s_totalsetvars;
	integer	s_ingetrequests;
	integer	s_ingetnexts;
	integer	s_insetrequests;
	integer	s_ingetresponses;
	integer	s_intraps;
	integer	s_outgetresponses;
	integer	s_outtraps;
	integer	s_toobigs;
	integer	s_nosuchnames;
	integer	s_badvalues;
	integer	s_readonlys;
	integer	s_generrs;
	caddr_t    *s_enableauthentraps;
#define	TRAPS_ENABLED	1			/* snmpEnableAuthenTraps */
#define	TRAPS_DISABLED	2			/* .. */
#define	TRAPS_MAXVALUE	TRAPS_DISABLED
};

extern struct snmpstat snmpstat;

extern caddr_t	*agentAction;
#define	AGENT_OTHER	1
#define	AGENT_COLDSTART	2
#define	AGENT_MAXVALUE	AGENT_COLDSTART
