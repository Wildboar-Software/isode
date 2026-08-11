/* aetdap.c - DAP-based DSE */

#include <string.h>
#include "quipu/util.h"
#include "quipu/read.h"
#include "quipu/dua.h"
#include "quipu/bind.h"
#include "tailor.h"

static char bound = FALSE;
static char unbind = FALSE;

static DN username = NULLDN;
static char password[DBA_MAX_PASSWD_LEN] = "";

static int bind_to_dsa (void) {
	struct ds_bind_arg bindarg;
	struct ds_bind_arg bindresult;
	struct ds_bind_error binderr;

	bindarg.dba_version = DBA_VERSION_V1988;
	bindarg.dba_dn = username;
	if (bindarg.dba_passwd_len = strlen (password))
		strcpy (bindarg.dba_passwd, password);

	if (ds_bind (&bindarg,&binderr,&bindresult) != DS_OK) {
		PY_advise (NULLCP, "unable to bind to directory (%s)",
				   binderr.dbe_type == DBE_TYPE_SECURITY ? "security error"
				   : "DSA unavailable");
		return FALSE;
	}
	return TRUE;
}

static struct mapping {
	char   *m_key;
	char   *m_value;
}	sac2cn[] = {
	"iso ftam", 	"filestore",
	"iso vt",		"terminal",
	"iso cmip",		"mib",
	"isode passwd lookup demo",
	"passwdstore",
	"isode shell",	"shell",
	"IRP Z39.50",	"Z39.50",
	"pp qmgr interface","pp qmgr",

	NULL
};

static PE name2value_dap (name, context, ontty, userdn, passwd, real_name)
char   *name,
	   *context,
	   *userdn,
	   *passwd;
int	ontty;
PE     *real_name;
{
	char buffer[BUFSIZ];
	DN dn;
	AttributeType at;
	extern char * oidtable;
	extern PE grab_pe();
	PE res_pe;
	static struct ds_read_arg read_arg = {
		default_common_args,
		NULLDN,   /* read_arg DN */
		{
			/* entry info selection */
			FALSE,
			NULLATTR,
			EIS_ATTRIBUTESANDVALUES
		}
	};
	struct DSError  error;
	struct ds_read_result result;

	*real_name = NULLPE;

	{
		char *qualifier = NULLCP;
		struct mapping *m;

		for (m = sac2cn; m -> m_key; m++)
			if (strcmp (m -> m_key, context) == 0) {
				qualifier = m -> m_value;
				break;
			}

		if (qualifier == NULLCP)
			qualifier = context ? context: "default";
		sprintf (buffer, "%s@cn=%s", name, qualifier);
	}

	name = buffer;

	if ( (dn=str2dn (name)) == NULLDN) {
		PY_advise (NULLCP, "build of DN failed: %s", name);
out:
		;
		SLOG (addr_log, LLOG_EXCEPTIONS, NULLCP, ("%s", PY_pepy));
		return NULLPE;
	}

	if ( (at = AttrT_new (DSAADDRESS_OID)) == NULLAttrT) {
		dn_free (dn);
		PY_advise (NULLCP, "build of attribute failed: %s",
				   DSAADDRESS_OID);
		goto out;
	}

	if (username)
		dn_free (username), username = NULLDN;
	if (userdn) {
		if ((username = str2dn (userdn)) == NULLDN) {
			PY_advise (NULLCP, "invalid DN for binding: \"%s\"", userdn);
			goto out;
		}
	}
	password[0] = 0;
	if (passwd)
		strcpy (password, passwd);

	if (! bound) {
		if (!bind_to_dsa ()) {
			dn_free (dn);
			AttrT_free (at);
			goto out;
		}
		bound = TRUE;
	}

	read_arg.rda_common.ca_servicecontrol.svc_prio = SVC_PRIO_HIGH;
	read_arg.rda_object = dn;
	read_arg.rda_eis.eis_select = as_comp_new (AttrT_cpy (at), NULLAV, NULLACL_INFO);

	if (ds_read (&read_arg,&error,&result) != DS_OK) {
		PY_advise (NULLCP, "DAP lookup failed: %s",name);
		log_ds_error (&error);
		ds_error_free (&error);
		if (unbind) {
			bound = FALSE;
			ds_unbind();
		}
		dn_free (dn);
		AttrT_free (at);
		as_free (read_arg.rda_eis.eis_select);
		goto out;
	} else {
		encode_IF_DistinguishedName (real_name,1,0,NULLCP,dn);
		if (result.rdr_entry.ent_attr == NULLATTR) {
			PY_advise (NULLCP, "No '%s' attribute in entry '%s'",
					   DSAADDRESS_OID, name);
			if (unbind) {
				bound = FALSE;
				ds_unbind();
			}
			dn_free (dn);
			AttrT_free (at);
			as_free (read_arg.rda_eis.eis_select);
			goto out;
		}
		if (unbind) {
			bound = FALSE;
			ds_unbind();
		}
		dn_free (dn);
		AttrT_free (at);
		as_free (read_arg.rda_eis.eis_select);
		res_pe = grab_pe(&result.rdr_entry.ent_attr->attr_value->avseq_av);
		as_free (result.rdr_entry.ent_attr);
		return (res_pe);
	}
}

/* flag: if TRUE always unbind */
int set_lookup_dap (int flag) {
	extern char * oidtable;

	if ((unbind = flag) && bound) {
		bound = FALSE;
		ds_unbind ();
	}
	acsap_lookup = name2value_dap;
	string_syntaxes();
	if (dsap_tai_init () != OK || load_oid_table (oidtable) != OK)
		LLOG (addr_log,LLOG_EXCEPTIONS,("DAP initialization failed"));
}
