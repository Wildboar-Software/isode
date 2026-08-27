/* oper_ureject.c - deal with user rejection of an operation */

#include "rosap.h"
#include "quipu/util.h"
#include "quipu/connection.h"
static void oper_ureject (const struct connection *conn, const struct RoSAPureject *rou);


extern LLog * log_dsap;

void oper_ureject (const struct connection *conn, const struct RoSAPureject *rou) {
	DLOG(log_dsap,LLOG_TRACE,( "oper_ureject"));
	if(rou->rou_noid) {
		LLOG(log_dsap,LLOG_EXCEPTIONS,( "Non-specific U-REJECT.INDICATION : %d (%d)",
										rou->rou_reason,conn->cn_ad));
	} else {
		struct oper_act *       on;
		for(on=conn->cn_operlist; on!=NULLOPER; on=on->on_next_conn)
			if(on->on_id == rou->rou_id)
				break;
		if(on == NULLOPER) {
			LLOG(log_dsap,LLOG_EXCEPTIONS,( "Unlocatable U-REJECT.INDICATION : %d (%d)",
											rou->rou_reason,conn->cn_ad));
			return;
		}
		if(on->on_state == ON_ABANDONED) {
			LLOG(log_dsap, LLOG_TRACE, ("oper_result - operation had been abandoned"));
			oper_extract(on);
			return;
		}
		oper_fail_wakeup(on);
	}
}
