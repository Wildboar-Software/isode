/* rcmd_srch.c: search a lookup table: return string value */

#include "manifest.h"
#include "cmd_srch.h"

char *rcmd_srch (int val, CMD_TABLE *cmd) {
	for(; cmd->cmd_key != NULLCP; cmd++)
		if(val == cmd->cmd_value)
			return(cmd->cmd_key);
	return(NULLCP);
}
