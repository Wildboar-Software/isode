/* pepdefs.h */





#ifndef PEPDEF_DEFINITIONS
#define PEPDEF_DEFINITIONS
/*
 * Globally known pep definitions
 */

typedef	struct	{
	char	*md_name;	/* Name of this module */
	int	md_nentries;	/* Number of entries */
	tpe	**md_etab;	/* Pointer to encoding tables */
	tpe	**md_dtab;	/* Pointer to decoding tables */
	ptpe **md_ptab;	/* Pointer to Printing tables */
	PE	(*md_eucode)();	/* User code for encoding */
	PE	(*md_ducode)();	/* User code for decoding */
	PE	(*md_pucode)();	/* User code for printing */

}	modtyp;

#ifndef NULL
#define NULL	(char *)0
#endif

#endif
