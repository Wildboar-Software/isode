/* entry_dump.c - routines to dump the database */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "psap.h"
#include "quipu/util.h"
#include "quipu/entry.h"
#include <errno.h>
#include <unistd.h>
#include "tailor.h"
static void header_print (PS psa, Entry edb);
static void entry_print (PS psa, Entry entryptr);
static void entry_block_print (PS psa, Entry block);


extern LLog * log_dsap;

#ifndef TURBO_DISK

extern RDN parse_rdn;
extern char * new_version(void);

static void header_print (PS psa, Entry edb) {
	switch (edb->e_data) {
	case E_DATA_MASTER:
		ps_print (psa,"MASTER\n");
		break;
	case E_TYPE_SLAVE :
		ps_print (psa,"SLAVE\n");
		break;
	default:
		ps_print (psa,"CACHE\n");
		break;
	}
	if (edb->e_parent != NULLENTRY)
		ps_printf (psa,"%s\n",edb->e_parent->e_edbversion);
	else
		ps_printf (psa,"%s\n",new_version());
}

static void entry_print (PS psa, Entry entryptr) {
	rdn_print (psa,entryptr->e_name,EDBOUT);
	parse_rdn = entryptr->e_name;
	ps_print (psa,"\n");
	as_print (psa,entryptr->e_attributes,EDBOUT);
	parse_rdn = NULLRDN;
}

static void entry_block_print (PS psa, Entry block) {
	Entry ptr;

	header_print (psa,block);
	if (block != NULLENTRY) {
		for ( ptr = (Entry) avl_getfirst(block->e_parent->e_children); ptr != NULLENTRY;
				ptr = (Entry) avl_getnext()) {
			if (ptr->e_data != E_TYPE_CONSTRUCTOR) {
				entry_print (psa,ptr);
				ps_print (psa,"\n");
			}
		}
	}
}

int write_edb (Entry ptr, const char *filename) {
	mode_t um, mask;
	FILE * fptr;
	PS entryps;
	extern char * parse_file;

	if (int2mode (0177, &mask) != 0)
		return NOTOK;
	um = umask (mask);
	if ((fptr = fopen (filename,"w")) == (FILE *) NULL) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("file_open failed: \"%s\" (%d)",filename,errno));
		return NOTOK;
	}
	(void) umask (um);
	if ((entryps = ps_alloc (std_open)) == NULLPS) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("ps_alloc failed"));
		fclose (fptr);
		return NOTOK;
	}
	if (std_setup (entryps,fptr) == NOTOK) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("std_setup failed"));
		fclose (fptr);
		return NOTOK;
	}
	parse_file = filename;
	entry_block_print (entryps,ptr);
	if (entryps->ps_errno != PS_ERR_NONE) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("write_edb ps error: %s",ps_error(entryps->ps_errno)));
		fclose (fptr);
		return NOTOK;
	}
	ps_free (entryps);
	if (fflush (fptr) != 0) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("write_edb flush error: %d",errno));
		fclose (fptr);
		return NOTOK;
	}
#if     defined(SYS5) && !defined(SVR4)
	sync();
#else
	if (fsync (fileno(fptr)) != 0) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("write_edb fsync error: %d",errno));
		fclose (fptr);
		return NOTOK;
	}
#endif
	if (fclose (fptr) != 0) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("write_edb EDB close error: %d",errno));
		return NOTOK;
	}
	LLOG (log_dsap,LLOG_TRACE,("Written %s",filename));
	return (OK);
}

#else

void write_edb (void) {
	LLOG (log_dsap,LLOG_FATAL,("write_edb implementation error"));
}

#endif /* NOT TURBO_DISK */
