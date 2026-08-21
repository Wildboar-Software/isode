/* tusap.h - include file for connectionless transport users (TS-USER) */

/*
 *
 * Kurt Dobbins		3/89
 *
 * Added UNITDATA for HULA connectionless service.
 *
 */

#ifndef	_TuSAP_
#define	_TuSAP_

#ifndef	_MANIFEST_
#include "manifest.h"
#endif
#ifndef	_GENERAL_
#include "general.h"
#endif
#ifndef	_ISOADDRS_
#include "isoaddrs.h"
#endif

#ifdef HULA

struct TSAPunitdata {			/* T-UNITDATA.INDICATION */
	int     tud_sd;			/* TRANSPORT descriptor */

	struct TSAPaddr tud_calling;	/* address of peer calling */
	struct TSAPaddr tud_called;		/* address of peer called */
	struct QOStype  tud_qos;		/* quality of service */
	int	    	    tud_cc;		/*   total length */
	struct qbuf     tud_qbuf;		/*   chained data */
	char 	   *tud_base;		/* base ptr for free */
};
#define	TUDFREE(tud)	QBFREE (&((tud) -> tud_qbuf))

int	TUnitDataListen(struct TSAPaddr *listen, struct QOStype *qos, struct TSAPdisconnect *td);	/* Listen on a datagram socket */
int	TUnitDataBind(int sd, struct TSAPaddr *calling, struct TSAPaddr *called, struct QOStype *qos, struct TSAPdisconnect *td); 	/* Bind socket to a remote address */
int	TUnitDataUnbind(int sd, struct TSAPdisconnect *td); 	/* UnBind socket to a remote address */
int	TUnitDataRequest(struct TSAPaddr *calling, struct TSAPaddr *called, struct QOStype *qos, struct udvec *uv, struct TSAPdisconnect *td);	/* Unit Data write on unbound socket */
int 	TUnitDataWrite(int sd, struct udvec *uv, struct TSAPdisconnect td);	/* Write unit data on a bound socket */
int 	TUnitDataRead(int sd, struct TSAPunitdata *tud, int secs, struct TSAPdisconnect *td);	/* Read unit data on a bound socket */
int	TUnitDataWakeUp();	/* Sync wakeup routine on kill */
int     TuSave(int sd, int vecp, char **vec, struct TSAPdisconnect *td);		/* Save a unitdata from buffer */

int     T_UnitDataWrite(struct tsapblk *tb, struct udvec *uv, char **hptr, struct TSAPdisconnect *td);	/* ISO T_UNITDATA.write */
int     T_UnitDataRead(struct tsapblk *tb, struct TSAPunitdata *tud, struct TSAPdisconnect *td);	/* ISO T_UNITDATA.read */

#endif

#endif
