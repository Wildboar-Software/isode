/* template.c - your comments here */

struct cnamelist {
	char *shortname;
	char *longname;
	struct cnamelist *next;
};

#define NULLCNLIST (struct cnamelist *)NULL
#define cname_alloc()   (struct cnamelist *)smalloc(sizeof (struct cnamelist))

void addToCoList(char *str);
void addCoNode(char *shortname, char *longname);
char * mapCoName(char *shortname);
