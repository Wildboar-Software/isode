/* vt_telnet.c - VT telnet profile */

#include <strings.h>

#include "vtpm.h"
#include "sector1.h"
#ifdef SVR4_UCB
#include <termio.h>
#else
#include <sys/ioctl.h>
#include "vt.h"
#endif

#define VT_BREAK
#undef PEPYPARM
#define PEPYPARM int *

extern struct sgttyb ottyb;
extern int cur_emode;
extern char *my_displayobj;
extern char *my_signal_obj;
extern char *my_echo_obj;
extern char kb_image;
extern char di_image;
extern char ni_image;
extern char na_image;
extern char sync_image;
extern char ga_image;
extern char nego_state;
extern int my_right;
extern int transparent;
extern int do_break;
extern int telnet_profile;
extern int connected;

void adios (char *, char *, ...);
void advise (int, char *, char *, ...);

int send_queue (const TEXT_UPDATE ud);
void vtsend (void);
int tmode (const int f);
void vt_clr_obj (void);
int vbrkreq (void);
int do_event (const int event, PE pe);
void vt_echo (const int echo);
extern int build_UDQPDU_UDQpdu (PE *pe, int explicit, int len, char *buffer, PEPYPARM parm);

void vt_newline(void) {	/*Produce Newline update*/
	TEXT_UPDATE ud;
	bzero ((char *) &ud, sizeof ud);
	ud.echo_sw = cur_emode;
	ud.type_sw = DISPLAY_OBJ;
	ud.updates.do_list.do_name = my_displayobj;
	ud.updates.do_list.do_type = DO_NEXT_X;		/*Next X-Array*/
	send_queue(ud);
}

void vt_char_erase (void) {	/*Pointer Relative (x=x-1) & erase current*/
	TEXT_UPDATE ud;
	bzero ((char *) &ud, sizeof ud);
	ud.echo_sw = cur_emode;
	ud.type_sw = DISPLAY_OBJ;
	ud.updates.do_list.do_name = my_displayobj;
	ud.updates.do_list.do_type = DO_PTR_REL;
	ud.updates.do_list.do_cmd.ptr_rel.x_true = 1;
	ud.updates.do_list.do_cmd.ptr_rel.y_true = 0;
	ud.updates.do_list.do_cmd.ptr_rel.z_true = 0;
	ud.updates.do_list.do_cmd.ptr_rel.x_value = -1;
	send_queue(ud);
	ud.updates.do_list.do_type = DO_ERASE;
	ud.updates.do_list.do_cmd.erase.start_erase.ptr_type = 0; /*Current*/
	ud.updates.do_list.do_cmd.erase.end_erase.ptr_type = 0; /*Current*/
	ud.updates.do_list.do_cmd.erase.erase_attr = 0;
	send_queue(ud);
}

void vt_line_erase (void) {	/*Erase full x-array & pointer to x = 1*/
	TEXT_UPDATE ud;
	bzero ((char *) &ud, sizeof ud);
	ud.echo_sw = cur_emode;
	ud.type_sw = DISPLAY_OBJ;
	ud.updates.do_list.do_name = my_displayobj;
	ud.updates.do_list.do_type = DO_ERASE;
	ud.updates.do_list.do_cmd.erase.start_erase.ptr_type = 3; /*Start X*/
	ud.updates.do_list.do_cmd.erase.end_erase.ptr_type = 6; /*End X*/
	ud.updates.do_list.do_cmd.erase.erase_attr = 0;
	send_queue(ud);
	ud.updates.do_list.do_type = DO_PTR_ABS;
	ud.updates.do_list.do_cmd.ptr_abs.ptr_type = 3; /*Start X*/
	send_queue(ud);
}

void vt_interrupt (void) {	/*Toggle Bit 1 of DI/KB control object*/
	TEXT_UPDATE ud;
	char int_mask;
	char image;

	int_mask = 0;
	if (int2octet (IP_OBJ, &int_mask) != 0)
		return;
	if(my_right == INITIATOR) {
		if (char_bxor (&kb_image, IP_OBJ) != 0)
			return;
		image = kb_image;
	} else {
		if (char_bxor (&di_image, IP_OBJ) != 0)
			return;	/*Toggle the Interrupt Process bit*/
		image = di_image;
	}
	bzero ((char *) &ud, sizeof ud);
	ud.echo_sw = cur_emode;
	ud.type_sw = CTRL_OBJ;
	ud.updates.co_list.co_name = my_signal_obj;
	ud.updates.co_list.co_type = 1;	/*Boolean Update*/
	ud.updates.co_list.co_cmd.bool_update.value = &image;
	ud.updates.co_list.co_cmd.bool_update.val_count = KB_SIZE;
	ud.updates.co_list.co_cmd.bool_update.mask = &int_mask;
	ud.updates.co_list.co_cmd.bool_update.mask_count = KB_SIZE;
	send_queue(ud);
}

/* Update NA/NI control object as in image */
void vt_set_nego (const char image, const int maskbits) {
	TEXT_UPDATE ud;
	char e_image;
	char mask;

	if (int2octet (maskbits, &mask) != 0)
		return;
	bzero ((char *) &ud, sizeof ud);
	ud.echo_sw = cur_emode;
	ud.type_sw = CTRL_OBJ;
	ud.updates.co_list.co_name = my_echo_obj;
	ud.updates.co_list.co_type = 1;			/*Boolean*/
	e_image = image;
	ud.updates.co_list.co_cmd.bool_update.value = &e_image;
	ud.updates.co_list.co_cmd.bool_update.val_count = NA_SIZE;
	ud.updates.co_list.co_cmd.bool_update.mask = &mask;
	ud.updates.co_list.co_cmd.bool_update.mask_count = NA_SIZE;
	send_queue(ud);
	vtsend();	/*Since we're bypassing normal keyboard entry*/
}

void vt_echo (const int echo) {
	if (!telnet_profile) {
		advise (LLOG_NOTICE,NULLCP, "not using TELNET profile");
		return;
	}
	if ((ni_image & ECHO_OBJ) != (nego_state & ECHO_OBJ)) {
		advise (LLOG_NOTICE, NULLCP, "negotiation in progress, try again later...");
		return;
	}
	if (echo != ((nego_state & ECHO_OBJ) ? 1 : 0)) {
		if (echo) {
			if (char_bis (&ni_image, ECHO_OBJ) != 0)
				return;
		} else {
			if (char_bic (&ni_image, ECHO_OBJ) != 0)
				return;
		}
		vt_set_nego(ni_image, ECHO_OBJ);/*Set proper UNIX echo state when reponse
				  is received. */
	} else
		advise (LLOG_NOTICE,NULLCP,  "already using %s echoing",
				echo ? "remote" : "local");
}

/* Request Remote Echo Mode.  Parameter is pointer to image byte. */
void vt_rem_echo (char *img_addr) {
	if (char_bis (img_addr, ECHO_OBJ) != 0)
		return;
	vt_set_nego(*img_addr, ECHO_OBJ);
}

/* Request Suppress Go Ahead */
void vt_sup_ga (char *img_addr) {
	if (char_bis (img_addr, SUP_GA) != 0)
		return;
	vt_set_nego(*img_addr, SUP_GA);
}

int vt_break (char **vec) {
#ifdef VT_BREAK
	if(!do_break) {
		advise(LLOG_NOTICE, NULLCP, "VT-BREAK Functional Unit Not Chosen");
		return OK;
	}
	tmode(2);
	vt_clr_obj();	/*Initialize all control objects*/
	vbrkreq();
#else
	TEXT_UPDATE ud;
	if (int2octet (BRK_OBJ, &mask) != 0)
		return NOTOK;
	if (char_bxor (&kb_image, BRK_OBJ) != 0)
		return NOTOK;	/*Can Only be called by User side*/
	image = kb_image;
	bzero ((char *) ud, sizeof *ud);
	ud.echo_sw = cur_emode;
	ud.type_sw = CTRL_OBJ;
	ud.updates.co_list.co_name = my_signal_obj;
	ud.updates.co_list.co_type = 1;	/*Boolean Update*/
	ud.updates.co_list.co_cmd.bool_update.value = &image;
	ud.updates.co_list.co_cmd.bool_update.val_count = KB_SIZE;
	ud.updates.co_list.co_cmd.bool_update.mask = &mask;
	ud.updates.co_list.co_cmd.bool_update.mask_count = KB_SIZE;
	send_queue(ud);
	vtsend();
#endif
	return OK;
}

/* Send Are You There */
int vt_ayt (char **vec) {
	TEXT_UPDATE ud;
	char mask;
	char image;

	if(!telnet_profile) {
		advise(LLOG_NOTICE,NULLCP,  "not using TELNET profile");
		return NOTOK;
	}
	mask = 0;
	if (int2octet (AYT_OBJ, &mask) != 0)
		return NOTOK;
	if (char_bxor (&kb_image, AYT_OBJ) != 0)
		return NOTOK;	/*Can only be called by User side*/
	image = kb_image;
	bzero ((char *) &ud, sizeof ud);
	ud.echo_sw = cur_emode;
	ud.type_sw = CTRL_OBJ;
	ud.updates.co_list.co_name = my_signal_obj;
	ud.updates.co_list.co_type = 1;	/*Boolean Update*/
	ud.updates.co_list.co_cmd.bool_update.value = &image;
	ud.updates.co_list.co_cmd.bool_update.val_count = KB_SIZE;
	ud.updates.co_list.co_cmd.bool_update.mask = &mask;
	ud.updates.co_list.co_cmd.bool_update.mask_count = KB_SIZE;
	send_queue(ud);
	vtsend();
	return OK;
}

/* Change to specified repertoire. Switching is done by sending
a Write Attribute NDQ. */
void switch_rep (const int rep_num) {
	TEXT_UPDATE ud;
	if(rep_num == 1) transparent = 0;
	else transparent = 1;
	bzero ((char *) &ud, sizeof ud);
	ud.echo_sw = cur_emode;
	ud.type_sw =  DISPLAY_OBJ;
	ud.updates.do_list.do_name = my_displayobj;
	ud.updates.do_list.do_type = DO_ATTR;
	ud.updates.do_list.do_cmd.wrt_attrib.attr_id = 0;
	ud.updates.do_list.do_cmd.wrt_attrib.attr_val = rep_num; /*Rep Number*/
	ud.updates.do_list.do_cmd.wrt_attrib.attr_ext = 2; /*Modal Extent*/
	send_queue(ud);
	vtsend();
}

void vt_repertoire (const int repertoire) {
	if (!telnet_profile) {
		advise (LLOG_NOTICE,NULLCP, "not using TELNET profile");
		return;
	}
	if (repertoire != transparent) {
		if (repertoire) {
			if (char_bis (&ni_image, DISP_BIN | KBD_BIN) != 0)
				return;
		} else {
			if (char_bic (&ni_image, DISP_BIN | KBD_BIN) != 0)
				return;
		}
		vt_set_nego(ni_image, DISP_BIN|KBD_BIN);
	} else
		advise (LLOG_NOTICE,NULLCP, "already using %s repertoire",
				transparent ? "BINARY" : "ASCII");
}

void vt_clr_obj (void) {	/*Set TELNET Profile Control Objects to 0*/
	kb_image = di_image = 0;
	nego_state = ni_image = na_image = 0;
	sync_image = ga_image = 0;
}

/*Send TELNET SYNC signal (test for UDQ & typed data)*/
int vt_sync (char **vec) {
	PE 	udqp;
	TEXT_UPDATE 	ud;
	char 	mask, image;
	mask = 0;
	if (int2octet (SYNC, &mask) != 0)
		return NOTOK;
	if (char_bxor (&sync_image, SYNC) != 0)
		return NOTOK;
	image = sync_image;
	bzero( (char *) &ud, sizeof ud);
	ud.echo_sw = cur_emode;
	ud.type_sw = CTRL_OBJ;
	ud.updates.co_list.co_name = "SY";
	ud.updates.co_list.co_type = 1;
	ud.updates.co_list.co_cmd.bool_update.value = &image;
	ud.updates.co_list.co_cmd.bool_update.val_count = SYNC_SIZE;
	ud.updates.co_list.co_cmd.bool_update.mask = &mask;
	ud.updates.co_list.co_cmd.bool_update.mask_count = SYNC_SIZE;
	if(build_UDQPDU_UDQpdu(&udqp,1,0,NULLCP,(PEPYPARM) &ud) == NOTOK)
		adios(NULLCP,"UDQ build failure");
	udqp->pe_context = 1;
	do_event(VDATreq_u,udqp);
	pe_free(udqp);
	return OK;
}
