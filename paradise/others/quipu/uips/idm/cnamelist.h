struct cnamelist {
	char *shortname;
	char *longname;
	struct cnamelist *next;
};

#define NULLCNLIST (struct cnamelist *)NULL
#define cname_alloc()   (struct cnamelist *)smalloc(sizeof (struct cnamelist))

void addToCoList();
void addCoNode();
char * mapCoName();
