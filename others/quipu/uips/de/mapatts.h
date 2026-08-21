/* template.c - your comments here */

struct mapnamelist {
	char *tablename;
	char *nicename;
	struct mapnamelist *next;
};

#define NULLMNLIST (struct mapnamelist *)NULL
#define mapname_alloc()   (struct mapnamelist *)smalloc(sizeof (struct mapnamelist))

void addToAttList(char *str);
void addAttNode(char *tablename, char *nicename);
char * mapAttName(char *tablename);
