/* ftamshare.c - FPM: encode/decode shared ASE information */

#include <stdio.h>
#include <stdlib.h>
#include "fpkt.h"

struct type_FTAM_Shared__ASE__Information *shared2fpm (const struct ftamblk *fsb, PE sharedASE, struct FTAMindication *fti) {
	struct type_FTAM_Shared__ASE__Information *fpm;

	if ((fpm = (struct type_FTAM_Shared__ASE__Information *)
			   calloc (1, sizeof *fpm)) == NULL) {
		ftamlose (fti, FS_GEN (fsb), 1, NULLCP, "out of memory");
		if (fpm)
			free_FTAM_Shared__ASE__Information (fpm);
		return NULL;
	}
	fpm -> indirect__reference = sharedASE -> pe_context;
	fpm -> encoding -> offset = choice_UNIV_0_single__ASN1__type;
	(fpm -> encoding -> un.single__ASN1__type = sharedASE) -> pe_refcnt++;
	return fpm;
}

int	fpm2shared (const struct ftamblk *fsb, const struct type_FTAM_Shared__ASE__Information *fpm, PE *sharedASE, struct FTAMindication *fti) {
	PE	    pe;

	if (fpm -> encoding -> offset != choice_UNIV_0_single__ASN1__type)
		return ftamlose (fti, FS_GEN (fsb), 1, NULLCP,
						 "shared ASE information not single-ASN1-type");
	if ((pe = pe_cpy (fpm -> encoding -> un.single__ASN1__type)) == NULLPE)
		ftamlose (fti, FS_GEN (fsb), 1, NULLCP, "out of memory");
	(*sharedASE = pe) -> pe_context = fpm -> indirect__reference;
	return OK;
}
