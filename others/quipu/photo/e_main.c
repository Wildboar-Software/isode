/* e_main.c - make the encoding routines into a stand alone program */

#include <stdio.h>
#include "quipu/photo.h"
#include <pixrect/pixrect_hs.h>

struct pixrect *pr_load();

extern int PIC_LINESIZE;
extern int STOP;
extern int NUMLINES;
extern int optlen;

char * encode_t4 ();

/**
 * @brief Encode a pixrect image from stdin to stdout as T.4.
 *
 * Interprets the command-line parameters then calls the encoding
 * routine. The format of the command line is:
 *
 *     encode [-K] infile outfile
 *
 * `-K` is the K parameter (default 1). If infile or outfile is omitted
 * then stdin or stdout is used.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 */
int main (int argc, char **argv)

{
	int     k_param = 1;
	int length;
	char *  inbuf;
	char * outbuf;
	FILE *  fptr;
	struct pixrect *pix;
	struct mpr_data *src_mpr;
	int skip;

	argv++;
	if ( (argc > 1) && (**argv == '-')) {
		switch (*++*argv) {
		case '1':
			k_param = 1;
			break;
		case '2':
			k_param = 2;
			break;
		case '4':
			k_param = 4;
			break;
		case 'n':
			k_param = 1;
			break;
		case 'l':
			k_param = 2;
			break;
		case 'h':
			k_param = 4;
			break;
		case 'v':
			k_param = 32767;
			break;
		default :
			fprintf (stderr,"Usage: %s -[124nlhv] \n",argv[0]);
			exit (-1);
		}
		argv++;
		argc--;
	}
	if ((pix = pr_load (stdin, NULL)) == (struct pixrect *)NULL)
		fprintf (stderr,"Not a pixrect.\n");
	PIC_LINESIZE = pix->pr_size.x;
	STOP  = PIC_LINESIZE + 1;
	NUMLINES = pix->pr_size.y;
	src_mpr = (struct mpr_data *)(pix->pr_data);
	inbuf = (char *) src_mpr->md_image;
	skip = 16 - (PIC_LINESIZE % 16);
	if  (skip == 16) skip = 0;
	outbuf = encode_t4 (k_param,inbuf,skip);
	fwrite (outbuf, optlen, 1, stdout);
}
