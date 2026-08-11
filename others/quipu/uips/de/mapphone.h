



struct mapphonelist {
	char *mapfrom;
	char *mapto;
	struct mapphonelist *next;
};

#define NULLPHLIST (struct mapphonelist *)NULL
#define mapphone_alloc()   (struct mapphonelist *)smalloc(sizeof (struct mapphonelist))

void addToPhoneList();
void addPhoneNode();
char * mapPhone();
