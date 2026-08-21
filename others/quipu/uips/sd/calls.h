#include "quipu/util.h"
#include <sys/stat.h>
#include "quipu/common.h"
#include "quipu/entry.h"
#include "usr.dirent.h"
#include "tailor.h"
#include "sequence.h"
#include "filt.h"
#include "y.tab.h"

void user_tailor(void), main_help(void), main_bind(void), cnnct_quit (void), cnnct_bind(void);
void rd_start(void), back_start(void), widen(void), set_default_type(void), list_start(void);
void rdn2str(caddr_t ptr,char *cptr), srch_start(void), dn2buf(caddr_t ptr,char *cptr), read_print(int (*func)(PS, caddr_t *, int),caddr_t ptr), quipu_print(int (*func)(PS, caddr_t *, int),caddr_t ptr);
void quipu_error(struct DSError *err), returnmain(void), get_listed_object(char number, WIDGET *wdgt), scrollbar(int command);
void make_friendly(char *fstr, char *str), goto_addr(void), entry2str(caddr_t ptr, char *cptr, int size), rfc2jnt(char *string);

int isleafnode(char *name), issubstr(char *str, char *substr), indexstring(char *string, char *substring);

struct attrcomp *sort_attrs(struct attrcomp *entry_attrs);

str_seq SortList();
