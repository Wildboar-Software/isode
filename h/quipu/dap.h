/* dap.h - */

#ifndef QUIPUDAP
#define QUIPUDAP

extern	int	  dsap_ad;	/* Association descriptor in simple DUA mode */
extern	int	  dsap_id;	/* Invocation identifier in simple DUA mode */

#define ds_read(arg,err,res) dap_read(dsap_ad,&(dsap_id),arg,err,res)
#define ds_compare(arg,err,res) dap_compare(dsap_ad,&(dsap_id),arg,err,res)
#define ds_abandon(arg,err) dap_abandon(dsap_ad,&(dsap_id),arg,err)
#define ds_list(arg,err,res) dap_list(dsap_ad,&(dsap_id),arg,err,res)
#define ds_search(arg,err,res) dap_search(dsap_ad,&(dsap_id),arg,err,res)
#define ds_addentry(arg,err) dap_addentry(dsap_ad,&(dsap_id),arg,err)
#define ds_removeentry(arg,err) dap_removeentry(dsap_ad,&(dsap_id),arg,err)
#define ds_modifyentry(arg,err) dap_modifyentry(dsap_ad,&(dsap_id),arg,err)
#define ds_modifyrdn(arg,err) dap_modifyrdn(dsap_ad,&(dsap_id),arg,err)

int ds_unbind (void);

#endif
