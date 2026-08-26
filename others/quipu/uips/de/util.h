/* template.c - your comments here */

#ifndef _xdua_util_h_
#define _xdua_util_h_

char *copy_string(char *string);
char *lastComponent(char *dnstr, int objectType);
char * removeLastRDN(char *dnstr);
char * lastRDN(char *dnstr);
char *dn2pstr(DN dn);

void addToList(struct namelist **lp, char *str);
void addToAttList(char *str);
void addToCoList(char *str);
void addToPhoneList(char *str);
void printLastComponent(int indent, char *dnstr, int objectType, int printNumber);
void printListCos(char *cstr, struct namelist *clistp);
void printListOrgs(char *org, struct namelist *olistp);
void printListOUs(char *str, struct namelist *listp);
void printListPRRs(char *str, struct namelist *listp, int searchparent, int pdet);
void freeCos(struct namelist **listpp);
void freeOrgs(struct namelist **listpp);
void freeOUs(struct namelist **listpp);
void freePRRs(struct namelist **listpp);
void initVideo(void);
void clearProblemFlags(void);
void setProblemFlags(struct ds_search_result sresult);
void logSearchSuccess(char *outcome, char *objecttype, char *string, int searchNumber, int noMatches);
void logListSuccess(char *outcome, char *objecttype, int noMatches);
void initAlarm(void);
void alarmCleanUp(void);
void startUnbindTimer(void);
void stopUnbindTimer(void);

#define LEADSUBSTR 1
#define TRAILSUBSTR 2
#define ANYSUBSTR 3
#define LEADANDTRAIL 4

#endif /* _xdua_util_h_ */
