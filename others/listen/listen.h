/* listen.h - listen demo definitions */





#include "psap2.h"
#include "ssap.h"
#include "tsap.h"
#include "logger.h"

#define	RMASK \
    "\020\01HALFDUPLEX\02DUPLEX\03EXPEDITED\04MINORSYNC\05MAJORSYNC\06RESYNC\
\07ACTIVITY\010NEGOTIATED\011CAPABILITY\012EXCEPTIONS\013TYPEDATA"

typedef struct sblk {
	int	    sb_sd;		/* session-descriptor */

	struct SSAPref sb_connect;	/* session connection reference */

	int	    sb_requirements;	/* session requirements */
	int	    sb_settings;	/* initial settings */
	int	    sb_owned;		/* session tokens we own */

	long    sb_ssn;		/* session serial number */
	long    sb_isn;		/* initial serial number */
}		*SB;

void	adios (char *, char *, ...);
void	advise (int, char *, char *, ...);
