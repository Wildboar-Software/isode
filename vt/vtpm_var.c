#include "acsap.h"		/* definitions for AcS-USERs */

PE	pre, pwe;
PE	mkdeliver(); 
int	fd,
	readfds,
	writefds,
	exfds,
	sd,
	netfile;

char	*dprofile, *cprofile;
#ifndef SVR4
char	*ttyname();
#endif
char	*myname;

int		vns,
		allpmde, /* all draft VTE parameters defined */
		allpmdu, /* all draft VTE parameters defined or undefined */
		cnw,	 /* collision winner right assigned to this VTPM  */
		dcno,	 /* no delivery control */	
		dcqua,	 /* quarantine delivery control */
		dcsim,	 /* simple delivery control     */
		pmacc,	 /* parameter values acceptable */
		dr_pm_st,/* draft parameter status:
						DEFINED,
						UNDEFINED,
						OFI,  (offered, initiator)
						OFR,  (offered, responder)
						COFI, (counter-offered, initiator)
						COFR, (counter-offered, responder)
						INVI, (invited, initiator)
						INVR  (invited, responder)
				*/
		pracc,
		vtpma,
		vcwa,	 /* whether the collision winner right is owned */
		vena,	 /* agreement on current VTE */
		vnt,	 /* number of VT service data units held for local delivery */
		vns,	 /* number of VT service data units held for transmission */
		vacc,	/*action choice: switch, restore, or not specified*/
		vacp,	/*action proposal: switch, restore, or responder
			  choice*/
		vrea,	/*failure reason: collision detected or profile not
			  supplied*/
		vrjo,	/*rejection option: retain, discard, or responder
			  choice*/
		vrsl,	/*result: succes, failure, or success with warning*/
		vsmd,	/*1 if S-Mode, 0 if A-Mode*/
		vtok,	/*1 if tokens held, 0 otherwise*/
		vtkp,	/*token parameters*/
		waca,	/*WACA right*/
		vra,	/*Boolean recording of acknowledgement request*/
		G_Func_Units,	/*Bit map of Functional Units requested*/
		wavar,		/* boolean, this VTPM has the token */
		waci,		/* boolean, this VTPM is assigned the waci right */
		del_cont;	/* type of delivery control		*/
