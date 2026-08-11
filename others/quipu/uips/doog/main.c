/* 

/*
 * 
 *
 *
 *
 */

#include "interact.h"
#include "query.h"
#include "initialize.h"

#include "quipu/util.h"

void exit();

static void read_doog_args();

QBool testing = FALSE;

int main (int argc, char *argv[]) {
	fprintf(stderr, "Binding to directory.....\n");

	read_doog_args(argc, argv);
	init_query_engine(argc, argv);
	initialize();
	if (bind_to_ds(argc, argv, SIMPLE_AUTH) == FALSE) exit(1);

	interact();

	return 0;
}

static void
read_doog_args (int argc, char *argv[]) {
	char *arg;
	int count;

	for (count = 0, arg = argv[count];
			count < argc;
			count++, arg = argv[count])
		if (*arg == '-')
			switch (*++arg) {
			case 't':
			case 'T':
				if (lexequ(arg, "test") == 0) testing = TRUE;
				break;
			}
}
