/* widget.h - definition of the widget structure and assorted constants */

/*
 * 
 */

/*****************************************************************************/
/*This file has been modified;
/*Modifier:     Damanjit Mahl @ Brunel University, Uxbridge
/*****************************************************************************/

/*****************************************************************************/
/* File:	widget.h
/* Author:	Paul Sharpe @ GEC Research, Hirst Research Centre.
/* Date:	August 12, 1988.
/* Function:	Definition of the widget structure and assorted constants.
/*
/* DISCLAIMER:
/* This source file is deemed to be public domain: any person may use the
/* code in any way, on two simple provisos -
/*   1 - That this header remains in this file,
/*   2 - It is accepted that neither the author, nor the authors employees
/*       accept any responsibility for the use, abuse or misuse of the
/*       contained code.
/* No guarantee is made by the author to provide maintainance or up-grading
/* of this source. However, any adaptations made to this file will be viewed
/* at least with interest.
/*****************************************************************************/

#ifndef WIDGETDEFS
#define WIDGETDEFS

#include <curses.h>

/* These define the type of the widget involved */
#define FINISH		0
#define LABEL		1
#define COMMAND		2
#define TOGGLE		3
#define DIALOG		4
#define SCROLLBAR       5
#define DUMMY		6

/* These define functions that aren't really needed */
#define NULLFN		nullfn
#define TOGGLEFN	NULLFN
#define QUITFN		quitfn

/* These define the LABEL wdgt text justification */
#define CENTER		CENTRE
#define CENTRE		1
#define LEFT		2
#define RIGHT		4

/* Thisdefines an expanded widget box */
#define EXPAND		-1

/* This defines a widget position starting on a new line */
#define CRNL		-1

/* This is the default shown length of the dialog string */
#define DIALOGLEN	4

/* This defines the maximum number of levels of active widgets */
#define MAXACTIVE	10
#ifndef BUFLEN
#define BUFLEN		1024
#endif

extern int lowy;

/* This is the height of a widget box, in number of lines */
#define WDGTHGHT	3

typedef struct widget {
	char	type;		/* Type of widget: see the above definitions */
	char	*label;		/* text string label for the widget          */

	/* The former is used by LABEL and COMMAND widgets: the latter by COMMAND */
	char	callch;		/* Character to activate the COMMAND widget  */
	void	(*callfn)();	/* Function called by an activated COMMAND  */

	/* ALL widgets need these fields to be set */
	int		x,y;		/* Position of the top right of the window  */
	int		wdth, hght;	/* width and height of the widget window    */

	/* These are only used by the DIALOG type widgets                  */
	int		dstrlen;	/* Maximum length of the DIALOG widget str  */
	char	*dstr;		/* Pointer to the DIALOG string to fill in   */

	/* These are only used by the TOGGLE type widgets                  */
	char	tindx;		/* Index into the toggle values              */
	char	**tvalues;	/* NULL-terminated array of TOGGLE strings   */

	WINDOW	*wndw;		/* The curses-widget window structure */
} WIDGET;

extern WIDGET *currwidgets;
extern char typetoggled;

void initwidgets(void), textfresh(void), makewidgets(WIDGET wdgts[]), setwdgtwdth(WIDGET *wdgt, int currx),
	 killwidgets(WIDGET *thesewdgts), activewidget(WIDGET wdgts[], WINDOW *text), deleteactive(void), activeindex(int indx),
	 redraw(void), rfrshwidgets(WIDGET *thesewdgts), boxwdgt(WIDGET *wdgt, char xch, char ych), printwdgt(WIDGET *wdgt), printbar(int list_size, int first, int display_num),
	 printlabel(WIDGET *wdgt), printdialog(WIDGET *wdgt), printtoggle(WIDGET *wdgt), printcommand(WIDGET *wdgt),
	 interact(void), docallback(int indx), dialog(WIDGET *wdgt), setdialogstr(WIDGET *wdgt, char *dstr, int maxlen),
	 toggle(WIDGET *wdgt), settogglstrs(WIDGET *wdgt, char **togglstrs, int togglindx), setlabel(WIDGET *wdgt, char *label), getlabel(WIDGET *wdgt, char label[]), wprint(WINDOW *here, const char *fmt, char *a,char *b,char *c,char *d,char *e,char *f,char *g,char *h,char *i,char *j), tprint(const char *fmt, char *a, char *b, char *c, char *d, char *e, char *f, char *g, char *h, char *i, char *j),
	 xprint(const char *fmt), xprintint(const char *fmt, int a), cleartext(void), jumpback(void), nullfn(void), quitfn(void),
	 endwidgets(void);

int linec(void), gety(void), posnwidgets(WIDGET thesewdgts[], int starty), getwidgetindex(WIDGET wdgts[], int callch), getdialogstr(WIDGET *wdgt, char str[]),
	settogglindx(WIDGET *wdgt, int indx), gettogglindx(WIDGET *wdgt), gettogglstr(WIDGET *wdgt, char str[]), lowesty(void),
	findactiveinput(int ch);

WIDGET *getwidget(WIDGET wdgts[], int callch);

#endif
