struct mapphonelist {
	char *mapfrom;
	char *mapto;
	struct mapphonelist *next;
};

#define NULLPHLIST (struct mapphonelist *)NULL
#define mapphone_alloc()   (struct mapphonelist *)smalloc(sizeof (struct mapphonelist))

void addToPhoneList(char *str);
void addPhoneNode(char *from, char *to);
char * mapPhone(char *from);
