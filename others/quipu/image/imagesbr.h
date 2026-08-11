/* imagesbr.h - include file for image subroutines */





#include "psap.h"
#include "isoaddrs.h"
#include "logger.h"

extern int   debug;
extern int   errsw;

/* GENERAL */

extern int   recording;
extern LLog *pgm_log;

/* AKA */

void	init_aka ();

/* DIRECTORY */

/* IMAGE */

struct type_IMAGE_Image {
	int     width;

	int     height;

	struct qbuf *data;
};

struct type_IMAGE_Image *fetch_image ();

/* ERRORS */

void	adios (char *what, char *fmt, ...);
void	advise (char *what, char *fmt, ...);

/* MISC */

char   *strdup ();
