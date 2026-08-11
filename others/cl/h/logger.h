/* logger.h - logging routines */




#ifndef	_LOGGER_
#define	_LOGGER_

#ifndef	BSD42

#define	LOG_PID		0x01	/* log process-id */
#define	LOG_TIME	0x02	/* log time-of-day */

#define	LOG_ALERT	1	/* alert */
#define	LOG_SALERT	2	/* subalert */
#define	LOG_ERR		4	/* error */
#define	LOG_WARNING	6	/* warning */
#define	LOG_INFO	8	/* information */
#define	LOG_DEBUG	9	/* debugging */

#else
#include <syslog.h>
#endif

void	setlog (), openlog (), closelog (), syslog (), _syslog ();

#endif
