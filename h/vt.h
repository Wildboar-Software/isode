/* vt.h - include file for Virtual Terminal functions */

#ifndef	_VT_
#define	_VT_

#ifndef PEPYPARM
#define PEPYPARM char *
#endif

#include "psap.h"

/* actions1.c */
int a1_0 (PE pe);
int a1_1 (PE pe);
int a1_2 (PE pe);
int a1_3 (PE pe);
int a1_4 (PE pe);
int a1_5 (PE pe);
int a1_6 (PE pe);
int a1_7 (PE pe);
int a1_8 (PE pe);
int a1_9 (PE pe);
int a1_10 (PE pe);
int a1_11 (PE pe);
int a1_12 (PE pe);
int a1_13 (PE pe);
int a1_14 (PE pe);
int a1_15 (PE pe);
int a1_16 (PE pe);
int a1_17 (PE pe);
int a1_18 (PE pe);
int a1_19 (PE pe);
int a1_20 (PE pe);
int a1_21 (PE pe);
int a1_22 (PE pe);
int a1_23 (PE pe);
int a1_24 (PE pe);
int a1_25 (PE pe);
int a1_26 (PE pe);
int a1_27 (PE pe);
int a1_28 (PE pe);
int a1_29 (PE pe);
int a1_30 (PE pe);
int a1_100 (PE pe);
int a1_101 (PE pe);
int a1_102 (PE pe);
int a1_103 (PE pe);
int a1_107 (PE pe);

/* actions5.c */
int ce_104 (PE pe);
int ce_105 (void);
int a5_0 (PE pe);
int a5_1 (PE pe);
int a5_2 (PE pe);
int a5_3 (PE pe);
int a5_5 (PE pe);
int a5_6 (PE pe);
int a5_9 (PE pe);
int a5_11 (PE pe);
int a5_17 (PE pe);
int a5_28 (PE pe);
int a5_31 (PE pe);
int a5_32 (PE pe);
int a5_34 (PE pe);
int a5_35 (PE pe);
int a5_38 (PE pe);
int a5_106 (PE pe);

/* map.c */
void map (PE ndq);
// void display_ud (DO_UPDATE *doptr);
// void control_ud (CO_UPDATE *coptr);
// void attrib_hdlr (DO_UPDATE *doptr);
int tmode (int f);
void kill_proc (void);
// void def_echo (CO_UPDATE *coptr);

/* states1.c */
int s1_01 (int event, PE pe);
int s1_02B (int event, PE pe);
int s1_02S (int event, PE pe);
int s1_03B (int event, PE pe);
int s1_03S (int event, PE pe);
int s1_10B (int event, PE pe);
int s1_10N (int event, PE pe);
int s1_10T (int event, PE pe);
int s1_50B (int event, PE pe);
int s1_51Q (int event, PE pe);
int s1_51R (int event, PE pe);
int s1_51N (int event, PE pe);
int s1_51T (int event, PE pe);

/* states5.c */
int s5_400B (int event, PE pe);
int s5_402B (int event, PE pe);
int s5_420B (int event, PE pe);
int s5_422B (int event, PE pe);
int s5_40N (int event, PE pe);
int s5_40T (int event, PE pe);
int s5_42T (int event, PE pe);
int s5_42N (int event, PE pe);
int s5_61 (int event, PE pe);
int s5_62 (int event, PE pe);

/* vt.c */
void do_vt (void);
struct dispatch;
struct dispatch *getds (char *name);
char *control (int c);
void deadpeer (void);
void intr (void);
void ttyflush (int dd);
void flushbufs (void);
void finalbye (void);
void adios (char *what, char *fmt, ...);
void advise (int code, char *what, char *fmt, ...);
void ptyecho (int on);
void setmode (int on, int off);

/* vt_telnet.c */
void vt_newline (void);
void vt_char_erase (void);
void vt_line_erase (void);
void vt_interrupt (void);
void vt_set_nego (char image, char mask);
void vt_echo (int echo);
void vt_rem_echo (char *img_addr);
void vt_sup_ga (char *img_addr);
int vt_break (char **vec);
int vt_ayt (char **vec);
void switch_rep (int rep_num);
void vt_repertoire (int repertoire);
void vt_clr_obj (void);
int vt_sync (char **vec);

/* vtd.c */
void interrupt (void);
void cleanup (void);
void rmut (void);
void bye (void);

/* vtpm.c */
int get_event (int dd, PE *pe);
int do_event (int event, PE pe);
int pn_ind (int dd, struct PSAPsync *psync);
int p_data (PE pdu);
int p_maj_sync_req (PE pdu);
int p_maj_sync_resp (PE pdu);
int p_typed_data (PE pdu);
int p_resync_req (PE pdu, int type);
int p_resync_resp (PE pdu);
int asr (PE pe, int status);
int send_bad_asr (int reason);
int send_rlr (PE pe);
int clear_vte (void);
int vgvt_ind (void);
int vrtq_ind (void);
int give_token (void);
int request_token (void);
int send_all (void);
void acs_adios (struct AcSAPabort *aa, char *event);

/* vtuser.c */
// int vass_req (int class, int acc_ri, VT_PROFILE *profile);
int vass_resp (int result);
void vrelreq (void);
void vrelrsp (int result);
void vrelcnf (void);
int vrelind (void);
int vt_text (char *str, int len);
// int send_queue (TEXT_UPDATE ud);
int setemode (int mode);
int getch (void);
int data_pending (void);
int queued (void);
int putch (char c);
void vtsend (void);
void vtdata (PE ndq);
PE mkdeliver (int ack);
void vdelreq (int ack);
void vdelind (PE del_pe, int ack);
void vdatind (int type, PE pe);
void vhdatind (PE pe);
void vudatind (PE pe);
int con_req (void);
int read_asq (PE pe);
int vasscnf (PE pe);
void asq (PE data);
void vt_disconnect (void);
int ass_ind (int argc, char **argv);
int vassind (PE pe);
int vbrkreq (void);
void vbrkrsp (void);
void vbrkind (PE brk_pe);
void vbrkcnf (PE brk_pe);

/* RCV_TEXT.c */
// TEXT_UPDATE *deq (TEXT_UPDATE **qhp);
// size_t enq (TEXT_UPDATE **qhp, TEXT_UPDATE *elem);
// TEXT_UPDATE *fiq (TEXT_UPDATE **qhp);

/* generated pepy builders */
int build_ASQPDU_ASQcontent (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASQPDU_ASQpdu (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASQPDU_CDSOffer (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASQPDU_CompRepOffer (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASQPDU_CSSOffer (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASQPDU_DEVOffer (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASQPDU_DimOffer (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASQPDU_ImplemIdent (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASQPDU_IntOffer (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASQPDU_ObjectOffer (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASQPDU_ParamOfferList (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASQPDU_Profile (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASQPDU_ProfileArgList (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASQPDU_RepFontOffer (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASRPDU_ArgumValueList (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASRPDU_ASRcontent (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASRPDU_ASRpdu (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASRPDU_CDSValues (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASRPDU_CompRepValue (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASRPDU_CSSValues (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASRPDU_DEVValues (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASRPDU_DimValue (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASRPDU_Goobers (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASRPDU_ImplemIdent (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASRPDU_ObjectOffer (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASRPDU_ParamValueList (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASRPDU_RepFontValue (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_ASRPDU_SpecialArgs (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_NDQPDU_AttrExtent (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_NDQPDU_AttrId (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_NDQPDU_COupdate (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_NDQPDU_DOupdate (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_NDQPDU_ExplicitPointer (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_NDQPDU_NDQcontent (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_NDQPDU_NDQpdu (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_NDQPDU_ObjectUpdate (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_NDQPDU_Pointer (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_NDQPDU_VTsdi (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_UDQPDU_COupdate (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_UDQPDU_UDQpdu (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_VT_BKQcontent (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_VT_BKQ__pdu (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_VT_BKRcontent (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_VT_BKR__pdu (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);
int build_VT_ExplicitPointer (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);

/* generated pepy unbuilders */
int unbuild_ASQPDU_ASQcontent (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASQPDU_ASQpdu (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASQPDU_CDSOffer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASQPDU_CompRepOffer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASQPDU_DimOffer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASQPDU_ImplemIdent (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASQPDU_IntOffer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASQPDU_ObjectOffer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASQPDU_ParamOfferList (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASQPDU_Profile (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASQPDU_ProfileArgList (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASQPDU_RepFontOffer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASRPDU_ArgumValueList (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASRPDU_ASRcontent (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASRPDU_ASRpdu (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASRPDU_CDSValues (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASRPDU_CompRepValue (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASRPDU_CSSValues (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASRPDU_DEVValues (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASRPDU_DimValue (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASRPDU_ImplemIdent (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASRPDU_ObjectOffer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASRPDU_ParamValueList (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASRPDU_RepFontValue (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASRPDU_SpecialArgs (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_ASRPDU_Squat (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_NDQPDU_AttrExtent (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_NDQPDU_AttrId (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_NDQPDU_COupdate (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_NDQPDU_DOupdate (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_NDQPDU_ExplicitPointer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_NDQPDU_NDQcontent (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_NDQPDU_NDQpdu (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_NDQPDU_ObjectUpdate (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_NDQPDU_Pointer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_NDQPDU_VTsdi (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_UDQPDU_COupdate (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_UDQPDU_UDQpdu (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_VT_BKQcontent (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_VT_BKQ__pdu (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_VT_BKRcontent (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_VT_BKR__pdu (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int unbuild_VT_ExplicitPointer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);

/* generated pepy printers */
int print_VT_ArgumValueList (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_ASQcontent (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_ASRcontent (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_AttrExtent (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_AttrId (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_BKQcontent (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_BKRcontent (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_CDSOffer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_CDSValues (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_ColorOffer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_ColorValue (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_CompEmpOffer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_CompEmpValue (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_CompRepOffer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_CompRepValue (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_COupdate (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_CSSOffer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_CSSValues (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_DEVOffer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_DEVValues (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_DimOffer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_DimValue (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_DOupdate (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_ExplicitPointer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_ImplemIdent (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_IntOffer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_NDQcontent (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_ObjectOffer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_ObjectUpdate (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_ParamOfferList (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_ParamValueList (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_PDUs (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_Pointer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_Profile (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_ProfileArgList (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_RepFontOffer (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_RepFontValue (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_SpecialArgs (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_Squat (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);
int print_VT_VTsdi (PE pe, int explicit, int *len, char **buffer, PEPYPARM parm);

// Other things I added
void vdatind (int type, PE pe);
void vhdatind (PE pe);
void vudatind (PE pe);
int vt_text (char *str, int len);
#ifdef TERMIOS
void ptyecho (int on);
#endif
#endif
