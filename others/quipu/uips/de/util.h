/* template.c - your comments here */

#ifndef _xdua_util_h_
#define _xdua_util_h_

char *copy_string();
char *lastComponent();
char * removeLastRDN();
char * lastRDN();
char *dn2pstr();

void addToList();
void addToAttList();
void addToCoList();
void addToPhoneList();
void printLastComponent();
void printListCos();
void printListOrgs();
void printListOUs();
void printListPRRs();
void freeCos();
void freeOrgs();
void freeOUs();
void freePRRs();
void initVideo();
void clearProblemFlags();
void setProblemFlags();
void logSearchSuccess();
void logListSuccess();
void initAlarm();
void alarmCleanUp();
void startUnbindTimer();
void stopUnbindTimer();

#define LEADSUBSTR 1
#define TRAILSUBSTR 2
#define ANYSUBSTR 3
#define LEADANDTRAIL 4

#endif _xdua_util_h_
