/* modify.c - */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/dish/RCS/modify.c,v 9.0 1992/06/16 12:35:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/dish/RCS/modify.c,v 9.0 1992/06/16 12:35:39 isode Rel $
 *
 *
 * $Log: modify.c,v $
 * Revision 9.0  1992/06/16  12:35:39  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#include "quipu/util.h"
#include "quipu/modify.h"
#include "quipu/read.h"
#include "quipu/entry.h"
#include "quipu/dua.h"
#include "pepsy.h"
#include "quipu/DAS-types.h"

#include "osisec-stub.h"

extern DN       dn;

#define	OPT	(!frompipe || rps -> ps_byteno == 0 ? opt : rps)
#define	RPS	(!frompipe || opt -> ps_byteno == 0 ? rps : opt)
extern	char	frompipe;
extern	PS	opt, rps;
extern	char	dad_flag;
extern char     fname[];

int mod_template (char *, char);

struct	list_element {
	char   *mod;
	char	add;			/* 1=add, 0=remove */
	struct  list_element*	next ;
} ;

extern Entry    current_entry;
static char     new_draft;

int
call_modify (int argc, char **argv) {
	struct ds_modifyentry_arg mod_arg;

	struct DSError  error;
	struct entrymod *emnew, *ems_append(), *modify_avs();
	Attr_Sequence   as,
#ifdef TURBO_DISK
					fget_attributes (),
#else
					get_attributes (),
#endif
					temp,
					trail = NULLATTR;
	AV_Sequence     avst = NULLAV;
	extern AttributeType at_objectclass;
	extern int	parse_status;
	Entry           entry_ptr;
	FILE           *fd;
	char            draft_flag = FALSE;
	char		noedit_flag = FALSE;
	int 		x;
	DN		moddn;
	char	       *home;
	RDN		new_rdn;

	struct  list_element   *start = 0 ;
	struct  list_element   *last ;
	struct  list_element   *l_temp ;

	/*	char	add = FALSE ;
		char	rem = FALSE ;
	 */
	if ((argc = service_control (OPT, argc, argv, &mod_arg.mea_common)) == -1)
		return;

	mod_arg.mea_changes = NULLMOD;
	new_draft = FALSE;

	if (home = getenv ("DISHDRAFT"))
		strcpy (fname, home);
	else if (dad_flag) {
		strcpy (fname, "/tmp/dishXXXXXX");
		close (mkstemp (fname));
	} else if (home = getenv ("HOME"))
		sprintf (fname, "%s/.dishdraft", home);
	else
		strcpy (fname, "./.dishdraft");

	for (x=1; x<argc; x++) {
		if (test_arg (argv[x], "-draft",1)) {
			draft_flag = 1;
			shuffle_up (argc--,argv,x);
			if (x == argc) {
				ps_printf (OPT, "Draft file name missing\n");
				Usage (argv[0]);
				return;
			}
			strcpy (fname, argv[x]);
			shuffle_up (argc--,argv,x--);
		} else if (test_arg (argv[x], "-newdraft",2)) {
			new_draft = TRUE;
			shuffle_up (argc--,argv,x--);
		} else if (test_arg (argv[x], "-noedit",3)) {
			noedit_flag = TRUE;
			shuffle_up (argc--,argv,x--);
		} else if (move (argv[x]) == OK)
			shuffle_up (argc--,argv,x--);
		else if (test_arg(argv[x], "-remove", 3)) {
			shuffle_up (argc--, argv, x);
			if (x == argc) {
				ps_printf(OPT, "Attribute to remove missing\n") ;
				Usage(argv[0]) ;
				return ;
			}
			l_temp = (struct list_element *) malloc (sizeof(struct list_element)) ;
			l_temp->mod = (char *) malloc ((unsigned)(strlen(argv[x]) + 1));
			strcpy (l_temp->mod, argv[x]) ;
			l_temp->add = 0 ;
			l_temp->next = 0 ;
			if (start == 0) {
				start = last = l_temp ;
			} else {
				last->next = l_temp ;
				last = l_temp ;
			}
			shuffle_up (argc--,argv,x--);
		} else if (test_arg(argv[x], "-add", 2)) {
			shuffle_up (argc--, argv, x);
			if (x == argc) {
				ps_printf(OPT, "Attribute to insert missing\n") ;
				Usage(argv[0]) ;
				return ;
			}
			l_temp = (struct list_element *) malloc (sizeof(struct list_element)) ;
			l_temp->mod = (char *) malloc ((unsigned)(strlen(argv[x]) + 1));
			strcpy (l_temp->mod, argv[x]) ;
			l_temp->add = 1 ;
			l_temp->next = 0 ;
			if (start == 0) {
				start = last = l_temp ;
			} else {
				last->next = l_temp ;
				last = l_temp ;
			}
			shuffle_up (argc--,argv,x--);
		}
	}

	if (dad_flag && (draft_flag || noedit_flag)) {
		ps_printf (OPT,
				   "operation not allowed when using directory assistance server!\n");
		return;
	}

	/* read attributes we want to modify */
	if ((argc = read_cache_aux (argc, argv, FALSE, &mod_arg.mea_common)) <0 )
		return;

	if (argc != 1) {
		ps_printf (OPT,"Unknown option %s\n",argv[1]);
		Usage (argv[0]);
		return;
	}

	if (start != 0) {
		if (build_modify(start, &mod_arg) == NOTOK) {
			return ;
		}

		while (ds_modifyentry (&mod_arg, &error) != DS_OK) {
			if (dish_error (OPT, &error) == 0) {
				return ;
			}
			mod_arg.mea_object = error.ERR_REFERRAL.DSE_ref_candidates->cr_name ;
		}
		ps_print (RPS, "Modified ");
		dn_print (RPS, dn, EDBOUT);
		ps_print (RPS, "\n");
		delete_cache (dn);  /* re-cache when next read */
		return ;
	}

	if (!draft_flag) {
		if (mod_template (fname,noedit_flag) != OK)
			return;
		noedit_flag = FALSE;
	} else {
		new_draft = TRUE;	/* Ugh ! */
		mod_template ("/dev/null",TRUE);
	}

	if (! noedit_flag)
		if (editentry (1, argv) != OK) {
			make_old (fname,draft_flag);
			return;
		}

	/* now parse the files */

	if ((fd = fopen (fname, "r")) == (FILE *) NULL) {
		ps_printf (OPT, "Can't open draft entry %s\n", fname);
		return;
	}

	entry_ptr = get_default_entry (NULLENTRY);
#ifdef TURBO_DISK
	entry_ptr->e_attributes = fget_attributes (fd);
#else
	entry_ptr->e_attributes = get_attributes (fd);
#endif

	fclose (fd);
	if (parse_status != 0)
		return;

	mod_arg.mea_object = dn;
	for (moddn = dn ; moddn->dn_parent != NULLDN; moddn=moddn->dn_parent)
		;
	entry_ptr->e_name = rdn_cpy (moddn->dn_rdn);

	/* add rdn as attribute */
	for (new_rdn = entry_ptr->e_name; new_rdn != NULLRDN; new_rdn = new_rdn->rdn_next) {
		avst = avs_comp_new (AttrV_cpy (&new_rdn->rdn_av));
		temp = as_comp_new (AttrT_cpy (new_rdn->rdn_at), avst, NULLACL_INFO);
		entry_ptr->e_attributes = as_merge (entry_ptr->e_attributes, temp);
	}

	for (as = entry_ptr->e_attributes; as != NULLATTR; as = as->attr_link) {
		emnew = NULLMOD;
		trail = as->attr_link;
		as->attr_link = NULLATTR;

		temp = current_entry->e_attributes;
		for (; temp != NULLATTR; temp = temp->attr_link)
			if (AttrT_cmp (as->attr_type, temp->attr_type) == 0) {
				/* found it - does it need changing ? */
				if (avs_cmp (as->attr_value, temp->attr_value) != 0)
					emnew = modify_avs (as->attr_value, temp->attr_value,as->attr_type);
				break;
			}

		if (temp == NULLATTR) {
			emnew = em_alloc ();
			emnew->em_type = EM_ADDATTRIBUTE;
			emnew->em_what = as_cpy(as);
			emnew->em_next = NULLMOD;
		}

		if (emnew != NULLMOD)
			mod_arg.mea_changes = ems_append (mod_arg.mea_changes,emnew);

		as->attr_link = trail;
	}

	/* remove attribute missing in new entry */
	for (as = current_entry->e_attributes; as != NULLATTR; as = as->attr_link) {
		emnew = NULLMOD;

		temp = entry_ptr->e_attributes;
		for (; temp != NULLATTR; temp = temp->attr_link)
			if (AttrT_cmp (as->attr_type, temp->attr_type) == 0)
				break;

		if (temp == NULLATTR) {
			emnew = em_alloc ();
			emnew->em_type = EM_REMOVEATTRIBUTE;
			emnew->em_what = as_comp_new(as->attr_type,NULLAV,NULLACL_INFO);
			emnew->em_next = NULLMOD;
		}

		if (emnew != NULLMOD)
			mod_arg.mea_changes = ems_append (mod_arg.mea_changes,emnew);
	}


	if (mod_arg.mea_changes == NULLMOD) {
		ps_print (RPS, "The draft entry and the entry for ");
		dn_print (RPS, dn, EDBOUT);
		ps_print (RPS, "\nare exactly the same - no change made!!!\n");
		entry_free (entry_ptr);
		make_old (fname,draft_flag);
		return;
	}

	if (rebind () != OK) {
		entry_free (entry_ptr);
		return;
	}
	/*
	 * If this operation is time-stamped, it may have expired while the user
	 * was editing the entry. Re-calculate the time-stamp. Modify is the only
	 * dish command where this needs to be done.
	 */

	if ((mod_arg.mea_common.ca_security != (struct security_parms *) 0)
			&& (mod_arg.mea_common.ca_security->sp_time != NULLCP)) {
		char *new_version();

		free(mod_arg.mea_common.ca_security->sp_time);
		mod_arg.mea_common.ca_security->sp_time = new_version();
	}

	/* If security parameters are present, take this to mean that strong
	 * authentication is required. This disallows 'parms + no signature'
	 * (pointless) and 'signature + no parms' (security risk).
	 */
	if (mod_arg.mea_common.ca_security != (struct security_parms *) 0) {
		extern struct SecurityServices *dsap_security;

		mod_arg.mea_common.ca_sig =
			(dsap_security->serv_sign)((caddr_t)&mod_arg,
									   _ZModifyEntryArgumentDataDAS, &_ZDAS_mod);
	}

	while (ds_modifyentry (&mod_arg, &error) != DS_OK) {
		if (dish_error (OPT, &error) == 0) {
			entry_free (entry_ptr);
			return;
		}
		mod_arg.mea_object = error.ERR_REFERRAL.DSE_ref_candidates->cr_name;
	}
	ps_print (RPS, "Modified ");
	dn_print (RPS, dn, EDBOUT);
	ps_print (RPS, "\n");
	delete_cache (dn);	/* re-cache when next read */

	entry_free (entry_ptr);
	ems_part_free (mod_arg.mea_changes);

	make_old (fname,draft_flag);
}


struct entrymod *
ems_append (struct entrymod *a, struct entrymod *b) {
	struct entrymod *ptr;

	if ((ptr = a) == NULLMOD)
		return b;

	for ( ; ptr->em_next != NULLMOD; ptr = ptr->em_next)
		;

	ptr->em_next = b;
	return a;
}



struct entrymod * modify_avs (a,b,at)
AV_Sequence a;
AV_Sequence b;
AttributeType at;
{
	AV_Sequence x;
	AV_Sequence y;
	struct entrymod *em = NULLMOD, *emnew;
	int removed_all = TRUE;
	extern short oc_sntx;
	static OID top = NULLOID;

	for (x=b; x != NULLAV; x=x->avseq_next) {
		emnew = NULLMOD;
		for (y=a; y != NULLAV; y=y->avseq_next)
			if (AttrV_cmp (&x->avseq_av,&y->avseq_av) == 0)
				break;
		if (y == NULLAV) {
			emnew = em_alloc ();
			emnew->em_type = EM_REMOVEVALUES;
			emnew->em_what = as_comp_new (at,avs_comp_new(AttrV_cpy(&x->avseq_av)),NULLACL_INFO);
			emnew->em_next = NULLMOD;
		} else
			removed_all = FALSE;
		if (emnew != NULLMOD)
			em = ems_append (em,emnew);
	}

	if (removed_all) {
		ems_part_free (em);
		emnew = em_alloc ();
		emnew->em_type = EM_REMOVEATTRIBUTE;
		emnew->em_what = as_comp_new (at,b,NULLACL_INFO);
		emnew->em_next = em_alloc();
		emnew->em_next->em_type = EM_ADDATTRIBUTE;
		emnew->em_next->em_what = as_comp_new (at,avs_cpy(a),NULLACL_INFO);
		emnew->em_next->em_next = NULLMOD;
		return (emnew);
	}

	for (x=a; x != NULLAV; x=x->avseq_next) {
		emnew = NULLMOD;
		for (y=b; y != NULLAV; y=y->avseq_next)
			if (AttrV_cmp (&x->avseq_av,&y->avseq_av) == 0)
				break;
		if (y == NULLAV) {
			if (at->oa_syntax == oc_sntx) {
				/* Don't add 'top' if missing */
				objectclass * oc;
				if (!top)
					top = oid_cpy(str2oid (TOP_OC));

				if (oc = (objectclass *) x->avseq_av.av_struct) 	/* assign */
					if (oid_cmp(oc->oc_ot.ot_oid, top) == 0)
						continue;
			}
			emnew = em_alloc ();
			emnew->em_type = EM_ADDVALUES;
			emnew->em_what = as_comp_new (at,avs_comp_new(AttrV_cpy(&x->avseq_av)),NULLACL_INFO);
			emnew->em_next = NULLMOD;
		}
		if (emnew != NULLMOD)
			em = ems_append (em,emnew);
	}


	return (em);
}

int
ems_part_free (struct entrymod *emp) {
	if(emp == NULLMOD)
		return;
	ems_part_free(emp->em_next);
	free((char *)emp);
}

static	int	raboof = 0;

static char *
foobar (char *string) {
	DN	    fb;
	PS	    ps;
	static char    buffer[BUFSIZ];
	DN	    sequence_dn ();

	if (!isdigit (*string))
		return string;
	if ((fb = sequence_dn (atoi (string))) == NULLDN) {
		ps_printf (OPT, "Invalid sequence in directive %s\n", string);
you_lose:
		;
		raboof = 1;
		return string;
	}

	if ((ps = ps_alloc (str_open)) == NULLPS) {
		ps_printf (OPT, "ps_alloc: failed");
		goto you_lose;
	}
	if (str_setup (ps, buffer, sizeof buffer - 2, 1) == NOTOK) {
		ps_printf (OPT, "str_setup: %s", ps_error (ps -> ps_errno));
		ps_free (ps);
		goto you_lose;
	}

	dn_print (ps, fb, EDBOUT);
	ps_print (ps, " ");
	*--ps -> ps_ptr = NULL, ps -> ps_cnt++;

	ps_free (ps);

	return buffer;
}

int
dsa_control (int argc, char **argv) {
	static struct entrymod mod = {
		EM_ADDATTRIBUTE,
		NULLATTR,
		NULLMOD
	};

	static struct ds_modifyentry_arg mod_arg = {
		default_common_args,
		NULLDN,
		&mod
	};

	AttributeType at;
	struct DSError  error;
	char            buffer[100];
	char * 		msg = "Done\n";
	char 		do_unbind = FALSE;

	if (argc < 2) {
		Usage(argv[0]);
		return;
	}

	if (test_arg (argv[1], "-dump",1))
		if (argc != 3) goto out;
		else
			sprintf (buffer, "d %s", argv[2]);
	else if (test_arg (argv[1], "-tailor",1))
		if (argc != 3) goto out;
		else
			sprintf (buffer, "t %s", argv[2]);
	else if (test_arg (argv[1], "-abort",1)) {
		strcpy (buffer,"a");
		argc++;		/* to get through if (argc != 3) */
		do_unbind = TRUE;
	} else if (test_arg (argv[1], "-restart",1)) {
		strcpy (buffer,"b");
		argc++;		/* to get through if (argc != 3) */
		do_unbind = TRUE;
	} else if (test_arg (argv[1], "-refresh",3))
		if (argc != 3) goto out;
		else
			sprintf (buffer, "r %s", foobar (argv[2]));
	else if (test_arg (argv[1], "-resync",2))
		if (argc != 3) goto out;
		else
			sprintf (buffer, "f %s", foobar (argv[2]));
	else if (test_arg (argv[1], "-lock",1))
		if (argc != 3) goto out;
		else
			sprintf (buffer, "l %s", foobar (argv[2]));
	else if (test_arg (argv[1], "-unlock",1))
		if (argc != 3) goto out;
		else
			sprintf (buffer, "u %s", foobar (argv[2]));
	else if (test_arg (argv[1], "-info",1)) {
		dsa_control_info();
		return;
	} else if (test_arg (argv[1], "-slave",1)) {
		msg = "Scheduled\n";
		if (argc == 2) {
			strcpy (buffer,"s");
			argc++;		/* to get through if (argc != 3) */
		} else if (argc != 3) goto out;
		else
			sprintf (buffer, "s %s", foobar (argv[2]));
	} else
		argc = 1;	/* to force error */

	if (raboof) {
		raboof = 0;
		return;
	}

	if (argc != 3) {
out:
		;
		Usage (argv[0]);
		return;
	}
	mod_arg.mea_object = dn;
	at = AttrT_new (CONTROL_OID);
	mod_arg.mea_changes->em_what = as_comp_new (at, avs_comp_new (str_at2AttrV (buffer, at)), NULLACL_INFO);

	if (rebind () != OK)
		return;

	if (ds_modifyentry (&mod_arg, &error) != DS_OK) {
		/* deal with error */
		dish_error (OPT, &error);
	} else {
		ps_print (RPS, msg);
	}

	if (do_unbind)
		unbind_from_dsa();


	/* as_free (mod_arg.mea_changes->em_what); */
}

int
dsa_control_info (void) {
	struct ds_read_arg read_arg;
	struct DSError  error;
	struct ds_read_result result;
	static CommonArgs      ca = default_common_args;

	read_arg.rda_eis.eis_infotypes = EIS_ATTRIBUTESANDVALUES;
	read_arg.rda_eis.eis_allattributes = FALSE;
	read_arg.rda_eis.eis_select = as_comp_new (AttrT_new (CONTROL_OID), NULLAV, NULLACL_INFO);
	read_arg.rda_common = ca;	/* struct copy */
	read_arg.rda_object = NULLDN;

	if (rebind () != OK)
		return;

	if (ds_read (&read_arg, &error, &result) != DS_OK) {
		dish_error (OPT, &error);
		return;
	}

	if (result.rdr_entry.ent_attr) {
		avs_print (RPS,result.rdr_entry.ent_attr->attr_value,READOUT);
	} else
		ps_printf (OPT, "No information !!!\n");
}

int
mod_template (char *name, char noedit) {
	FILE           *fptr;
	PS              ps;
	extern AttributeType at_objectclass;
	Attr_Sequence   as;
	Attr_Sequence   nas, tas, make_template_as ();
	int		um;

	if (! new_draft)
		if ((fptr = fopen (name, "r")) != NULL) {
			fclose (fptr);
			if (!noedit) {
				if (!yesno ("Use existing draft file ? "))
					return OK;
				else
					make_old (fname,FALSE);
			} else
				return (OK);	/* template already exists ! */

		}

	um = umask (0177);
	if ((fptr = fopen (name, "w")) == NULL) {
		ps_printf (OPT, "Can't open template entry %s\n", name);
		return (-1);
	}
	umask (um);

	if ((ps = ps_alloc (std_open)) == NULLPS) {
		return (-1);
	}
	if (std_setup (ps, fptr) == NOTOK) {
		return (-1);
	}
	for (as = current_entry->e_attributes; as != NULLATTR; as = as->attr_link)
		if (as->attr_type == at_objectclass)
			break;

	tas = make_template_as (as->attr_value);
	nas = as_cpy(current_entry->e_attributes);

	tas = as_merge (tas,nas);

	as_print (ps,tas,EDBOUT);

	as_free (tas);
	ps_free (ps);
	fclose (fptr);

	return (OK);
}

int
build_modify (struct list_element *start, struct ds_modifyentry_arg *mod_arg) {
	struct	list_element	*temp_elem ;
	struct	entrymod	*emnew ;
	AttributeType	a_t ;
	AV_Sequence	tmp_avs ;
	Attr_Sequence	eptr ;

	char   *ptr ;
	int    num_attr_vals = 0 ;
	int    attr_val_match = 0 ;

	while (start) {
		emnew = em_alloc() ;

		ptr = start->mod;

		while (*ptr != 0)
			if (*ptr == '=')
				break;
			else
				ptr++;

		if (*ptr == 0) {
			if ((a_t = AttrT_new (start->mod)) == NULLAttrT) {
				ps_printf(OPT, "invalid attribute type '%s'",
						  start->mod) ;
				return (NOTOK) ;
			}
			emnew->em_what = as_comp_new (a_t, NULLAV,
										  NULLACL_INFO);

		} else 	if ((emnew->em_what = str2as (start->mod)) ==
					NULLATTR) {
			ps_printf(OPT, "invalid attribute '%s' ",
					  start->mod) ;
			return (NOTOK) ;
		}

		emnew->em_type = -1 ;
		a_t = emnew->em_what->attr_type;

		for (eptr = current_entry->e_attributes; eptr != NULLATTR;
				eptr = eptr->attr_link) {
			if ( AttrT_cmp (eptr->attr_type, a_t) == 0 ) {
				if (emnew->em_type == -1) {
					if (start->add == 0) { /* Removing... */
						if (emnew->em_what->attr_value == NULLAV) {
							emnew->em_type = EM_REMOVEATTRIBUTE ;
						} else {
							num_attr_vals = 0 ;
							attr_val_match = 0 ;

							for (tmp_avs = eptr->attr_value;
									tmp_avs != NULLAV;
									tmp_avs = tmp_avs->avseq_next) {
								if (AttrV_cmp(&(tmp_avs->avseq_av),
											  &emnew->em_what->attr_value->avseq_av)
										== OK) {
									attr_val_match = 1 ;
								}
								num_attr_vals++ ;
							}
							if (num_attr_vals == 1)
								emnew->em_type = EM_REMOVEATTRIBUTE ;
							else
								emnew->em_type = EM_REMOVEVALUES ;
							if (attr_val_match == 0) {
								ps_printf(OPT,"Can't remove value that is not present.\n") ;
								return (NOTOK) ;
							}
						}
					} else
						emnew->em_type = EM_ADDVALUES ;
				}
			}
		}

		if (emnew->em_type == -1) { /* No matches, so attrType is a new one */
			if (start->add == 0) { /* Remove */
				ps_print(OPT, "Removing attribute that is not present.\n") ;
				return (NOTOK) ;
			}
			emnew->em_type = EM_ADDATTRIBUTE ;
		}
		temp_elem = start ;
		start = start->next ;
		free (temp_elem->mod) ;
		free ((char *)temp_elem) ;
		emnew->em_next = NULLMOD ;
		mod_arg->mea_changes = ems_append (mod_arg->mea_changes, emnew) ;
	}
	mod_arg->mea_object = dn;

	return (OK) ;
}
