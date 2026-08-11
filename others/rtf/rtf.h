/* rtf.h - definitions for RT-file transfer utility */

#include <stdio.h>
#include "rtsap.h"
#include "logger.h"
#include "RTF-types.h"
#ifdef	NULL
#undef	NULL
#endif
#include <sys/param.h>
#ifndef	NULL
#define	NULL	0
#endif
#include "sys.file.h"
#include <sys/stat.h>

extern LLog *pgm_log;

char   *SReportString ();

void	rts_adios (), rts_advise ();
void	adios (char *, char *, ...);
void	advise (int, char *, char *, ...);
void	ryr_advise (char *, char *, ...);
