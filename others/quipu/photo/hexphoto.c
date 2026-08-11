/* hexphoto.c - your comments here */

#include "quipu/util.h"
#include "quipu/photo.h"
#include "psap.h"

int decode_t4 (char *picture, char *persons_name, int len) {
	int i;

	if (len == 0) len = photolen (picture);

	for (i=0; i<len; i++)
		fprintf (stderr,"%02x",*picture++ & 255);

}
static
photolen (char *s1) {
	int length=0,cnt,i;
	char * temp;

	if (*s1 == 0x03) {
		/* we have a coded picture */

		temp = s1;
		temp++;
		cnt = *temp++ & 0x7f;  /*assume len > 127 for now */
		for (i=0; i<cnt; i++)
			length = (length << 8) | (*temp++ & 0xff) ;

		length += 2 + cnt;
		return (length);

	} else
		return (-1);

}
