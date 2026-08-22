/* acserver1.c - generic server dispatch */

#include <signal.h>
#include "psap.h"
#include "tsap.h"
#include "sys.file.h"
#include "tailor.h"

int iserver_init (
	int argc,
	char **argv,
	AEI aei,
	int (*initfnx)(int vecp, char **vec),
	struct TSAPdisconnect *td
);

int isodeserver (int argc, char **argv, AEI aei, int (*initfnx)(int vecp, char **vec), int (*workfnx)(int fd), void (*losefnx)(struct TSAPdisconnect *td), struct TSAPdisconnect *td) {
	if (iserver_init (argc, argv, aei, initfnx, td) == NOTOK)
		return NOTOK;

	for (;;) {
		int     result;

		if ((result = iserver_wait (initfnx, workfnx, losefnx, 0, NULLFD,
									NULLFD, NULLFD, NOTOK, td)) != OK)
			return (result == DONE ? OK : result);
	}
}
