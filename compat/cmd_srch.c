/* cmd_srch.c - search a lookup table: return numeric value */

#include "general.h"
#include "manifest.h"
#include "cmd_srch.h"

/* map a string onto a value */

int cmd_srch (char *str, CMD_TABLE *cmd) {
	extern char chrcnv[];

	for(; cmd->cmd_key != NULLCP; cmd++)
		if(chrcnv[*str] == chrcnv[*cmd->cmd_key] &&
				lexequ(str, cmd->cmd_key) == 0)
			return(cmd->cmd_value);
	return(cmd->cmd_value);
}
