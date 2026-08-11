/* cmd_srch.h - command search structure */

#ifndef _CMD_SRCH_
#define _CMD_SRCH_

typedef struct  cmd_table {
	char    *cmd_key;
	int     cmd_value;
} CMD_TABLE;

struct  cm_args {
	char    *cm_key;
	char    *cm_value;
};

int     cmd_srch ();
char   *rcmd_srch ();

#endif
