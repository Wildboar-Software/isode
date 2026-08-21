/*
 * 
 */

#include "widget.h"

/* REMEMBER TO NULL-TERMINATE ALL OF THESE STRUCTURE ARRAYS     */
/*--------------------------------------------------------------*/

void	main_help(void);
void	list_start(void), back_start(void);
void	srch_start(void), widen(void);
void	rd_start(void);
void	sd_quit(void), cache_quit();
void    get_listed_object(char number, WIDGET *wdgt), goto_addr(void), scrollbar(int command);

WIDGET mainwdgts[] = {
	{LABEL," QUIPU X.500 Screen Directory. ",CENTRE,NULLFN,0,0,EXPAND ,0},
	{COMMAND, "q Quit ",	 	'q',sd_quit,   0,CRNL,0,0},
	{COMMAND, "h Help ",	 	'h',main_help,   0,0,0,0},
	{COMMAND, "l List ",	 	'l',list_start,   0,0,0,0},
	{COMMAND, "w Widen Search Area",     'w', widen, 0, 0, 0, 0},
	{COMMAND, "b History ", 	'b', back_start, 0,0,0,0},
	{DIALOG,  "Go To Number:",	      	'*',get_listed_object,0,0,EXPAND,0},
	{DIALOG , "Search Area: ",	'\0', goto_addr,     0,CRNL,EXPAND,0},
	{TOGGLE,  "t Type: ",     	't',TOGGLEFN, 	0,0,0,0},
	{DIALOG,  "s Search for: ",    's',srch_start, 0,0,EXPAND,0},
	{DUMMY, "Search",		'\n', srch_start, 0,0,0,0},
	/* Scrollbar always last */
	{SCROLLBAR, "",                 '%', scrollbar, 0,0,3,0},
	{FINISH,  "", 0,NULLFN}
} ;

/*--------------------------------------------------------------*/

void help_read(), help_list(void), help_srch(void), help_back(void),
	 help_number(void), help_addr(), help_up(void), returnmain(void);

WIDGET dethelpwdgts[] = {
	{LABEL,"QUIPU X.500 Screen Directory: Help",CENTRE,NULLFN,0,0,EXPAND,0},
	{COMMAND, "q QUIT Help",'q',returnmain,   0,CRNL,0,0},
	{COMMAND, "s Search for",'s',help_srch, 0,0,0,0},
	{COMMAND, "b History ",'b', help_back, 0,0,0,0},
	{COMMAND, "w Widen Search Area",'w', help_up, 0,0,0,0},
	{COMMAND,  "l List ",'l',help_list,0,0,0,0},
	{COMMAND,  "n Number ",'n',help_number,0,0,0,0},
	/*scrollbar always last*/
	{SCROLLBAR, "",                 '%',scrollbar, 0,0,3,0},
	{FINISH,  "", 0,NULLFN}
} ;
