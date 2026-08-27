#ifndef QUIPU_DISH_H
#define QUIPU_DISH_H

#include <sys/stat.h>
#include <string.h>

#include "psap.h"
#include "quipu/DAS-types.h"
#include "quipu/ds_error.h"
#include "quipu/commonarg.h"

/*
 * Shared prototypes for DISH command sources in this directory.
 * This is an application (not a library), so centralizing these
 * prototypes keeps compilation free of implicit-function-declaration errors.
 */

void Usage(const char *rtn);
int move(char *arg);

int rebind(void);
int dish_error(PS ps, struct DSError *error);
void show_sequence (PS ps, char *str, const char ufn);
int referral_bind (const struct PSAPaddr *addr);

struct ds_bind_error;
void ds_bind_error(PS ps, const struct ds_bind_error *err);

void pdu_dump_init(char *dir);
void get_password(char *str, char *buffer);
void set_sequence(char *str);
int set_cmd_default(const char *cmd, const char *dflt);
void new_service(const char *ptr);
void show_unknown(void);
void new_ignore(char *ptr);
int add_sequence(DN adn);
void hide_picture(void);

/* dish main/init */
void quipu_syntaxes (void);
int osisecinit (int *argc, char ***argv, const int fn);
int dish_init (int argc, char **argv);
void add_dish_help (const char *command, const char *args, const char serv, const char other, const char *use);
int do_dish (void);
void add_dish_command (char *name, void (*func)(int, char **), const int len);
int dish_cmd_init (void);

void call_add (int argc, char **argv);
void call_compare (int argc, char **argv);
void call_delete (int argc, char **argv);
void call_list (int argc, char **argv);
void call_fred (int argc, char **argv);
void call_search (int argc, char **argv);
void call_modify (int argc, char **argv);
void call_modifyrdn (int argc, char **argv);
void call_showentry (int argc, char **argv);
void call_unbind (int argc, char **argv);
void call_moveto (int argc, char **argv);
void call_ds (int argc, char **argv);
void call_help (int argc, char **argv);
void call_showattribute (int argc, char **argv);
void dsa_control (int argc, char **argv);

int editentry(const int argc, char **argv);
int yesno(char *str);

void delete_list_cache(DN adn);
void delete_cache(DN adn);
int exit_pipe(void);

int get_ava (AVA *avasert, char *type, char *value);

/* dishlib/dish.c shared helpers */
void dish_help_init (void);
void shuffle_up (const int argc, char **argv, const int start);
void help_arg (const char *rtn);
int init_pipe (void);
void want_oc_hierarchy (void);
int dsap_init (const int *acptr, char ***avptr);
int user_tailor (void);
int print_arg_error (PS opt);
int reset_arg (void);
void set_current_pos (void);
void set_alarm (void);
int read_pipe (char *buf, const int len);
void test_rc_file (PS ps);
void consolidate_move (void);

/* pipe helpers used by edit.c and elsewhere */
void send_pipe_aux (char *buf);
void send_pipe_aux2 (const char *buf, const int i);
int read_pipe_aux (char *buf, const int len);
int read_pipe_aux2 (char **buf, int *len);

/* dsap/cache and fred helpers */
struct subordinate;
void cache_list (struct subordinate *ptr, const int prob, DN dn, const int sizelimit);
int fdx_reset (PS ps);
void showfredDNs (DN dn, const char islong);
int showfred (DN mydn, const char islong, const char subdisplay);
void call_showname (int argc, char **argv);
void showattribute (AttributeType at);
void as_comp_free (Attr_Sequence as);

/* read/modify shared helpers */
int read_cache (const int argc, char **argv);
int read_cache_aux (int argc, char **argv, const char ali, const CommonArgs *ca);
int set_read_flags (int argc, char **argv);
void make_old (char *file, const char commit);
void unbind_from_dsa (void);
int avs_cmp (AV_Sequence a, AV_Sequence b);

#endif /* QUIPU_DISH_H */

