/* udptest.c - test out -ltsap for unit data service over UDP */

#include <stdio.h>
#include <ctype.h>
#include "tpkt.h"
#include "tsap.h"
#include "tusap.h"
#include "isoservent.h"
#include "isoaddrs.h"
#include "internet.h"

int main (void) {

	TUnitDataListen();	/* Listen on a datagram socket */
	TUnitDataBind(); 	/* Bind socket to a remote address */
	TUnitDataUnbind(); 	/* UnBind socket to a remote address */
	TUnitDataRequest();	/* Unit Data write on unbound socket */
	TUnitDataWrite();	/* Write unit data on a bound socket */
	TUnitDataRead();	/* Read unit data on a bound socket */
	TUnitDataWakeUp();	/* Sync wakeup routine on kill */

}
