/* ftamfdf.c - FDF support */

#include <stdio.h>
#include "fpkt.h"

int fdf_p2names (const int fd, PE bits, int *names, struct FTAMindication *fti);
int fdf_names2p (const int fd, const int names, PE *bits, struct FTAMindication *fti);
int fdf_attrs2d (const int fd, struct FTAMattributes *fa, struct type_FTAM_Read__Attributes **attrs, struct FTAMindication *fti);
int fdf_d2attrs (const int fd, struct type_FTAM_Read__Attributes *attrs, struct FTAMattributes *fa, struct FTAMindication *fti);

int fdf_p2names (const int fd, PE bits, int *names, struct FTAMindication *fti) {
	struct ftamblk *fsb;

	if ((fsb = findfsblk (fd)) == NULL)
		return ftamlose (fti, FS_GEN_NOREASON, 0, NULLCP,
						 "invalid ftam descriptor");
	return fpm2bits (fsb, fname_pairs, bits, names, fti);
}

int fdf_names2p (const int fd, const int names, PE *bits, struct FTAMindication *fti) {
	struct ftamblk *fsb;

	if ((fsb = findfsblk (fd)) == NULL)
		return ftamlose (fti, FS_GEN_NOREASON, 0, NULLCP,
						 "invalid ftam descriptor");
	if ((*bits) = bits2fpm (fsb, fname_pairs, names, fti))
		return OK;
	return NOTOK;
}

int fdf_attrs2d (const int fd, struct FTAMattributes *fa, struct type_FTAM_Read__Attributes **attrs, struct FTAMindication *fti) {
	struct ftamblk *fsb;

	if ((fsb = findfsblk (fd)) == NULL)
		return ftamlose (fti, FS_GEN_NOREASON, 0, NULLCP,
						 "invalid ftam descriptor");
	if ((*attrs) = attr2fpm (fsb, fa, fti))
		return OK;
	return NOTOK;
}

int fdf_d2attrs (const int fd, struct type_FTAM_Read__Attributes *attrs, struct FTAMattributes *fa, struct FTAMindication *fti) {
	struct ftamblk *fsb;

	if ((fsb = findfsblk (fd)) == NULL)
		return ftamlose (fti, FS_GEN_NOREASON, 0, NULLCP,
						 "invalid ftam descriptor");
	return fpm2attr (fsb, attrs, fa, fti);
}
