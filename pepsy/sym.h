/* sym.h */

typedef struct symlist {
	char   *sy_encpref;
	char   *sy_decpref;
	char   *sy_prfpref;
	char   *sy_module;
	char   *sy_name;

	YP	    sy_type;

	struct symlist *sy_next;
}		symlist, *SY;
#define	NULLSY	((SY) 0)

