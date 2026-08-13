/* acl_info.c - ? */

#include "quipu/util.h"
#include "quipu/entry.h"

extern AV_Sequence super_user;
extern LLog * log_dsap;

int check_acl (DN who, int mode, struct acl_info *acl, DN node) {
	struct acl_info *ptr;
	AV_Sequence avs;

	for (ptr=acl; ptr!= NULLACL_INFO; ptr=ptr->acl_next) {
		switch (ptr->acl_selector_type) {
		case ACL_ENTRY:
			if ( mode <= ptr->acl_categories ) {
				if (who == NULLDN) {
					break;
				}
				if (dn_cmp (who,node) == OK)
					return (OK);
			}
			break;
		case ACL_OTHER:
			if ( mode <= ptr->acl_categories )
				return (OK);
			break;
		case ACL_PREFIX:
			if ( mode <= ptr->acl_categories ) {
				if ( who == NULLDN)
					break;
				if (check_dnseq_prefix (ptr->acl_name,who) == OK)
					return (OK);
			}
			break;
		case ACL_GROUP:
			if ( mode <= ptr->acl_categories ) {
				if ( who == NULLDN) {
					break;
				}
				if (check_dnseq (ptr->acl_name,who) == OK)
					return (OK);
			}
			break;
		}
	}
	/* one last try for access */
	for (avs=super_user; avs != NULLAV;  avs=avs->avseq_next)
		if ( dn_cmp (who,(DN) avs->avseq_av.av_struct) == OK)
			return (OK);
#ifdef DEBUG
	if (log_dsap -> ll_events & LLOG_TRACE) {
		pslog (log_dsap,LLOG_TRACE,"access denied for user ",
			   (IFP)dn_print,(caddr_t)who);
		LLOG (log_dsap,LLOG_TRACE,("  attempting mode=%d", mode));
		pslog (log_dsap,LLOG_TRACE,"  on entry ",
			   (IFP)dn_print,(caddr_t)node);
	}
#endif
	return (NOTOK);
}

int manager (DN dn) {
	AV_Sequence avs;

	for (avs=super_user; avs != NULLAV;  avs=avs->avseq_next)
		if ( dn_cmp (dn,(DN) avs->avseq_av.av_struct) == OK)
			return (TRUE);
	return (FALSE);
}
