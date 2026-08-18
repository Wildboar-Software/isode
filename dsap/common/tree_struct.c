/* tree_struct.c - Tree Structure utility routines */

#include "quipu/util.h"
#include "quipu/entry.h"
#include "quipu/syntaxes.h"

extern int oidformat;

void tree_struct_free (struct tree_struct *ptr) {
	/* don't free objectclass - in static table */
	free ((char *)ptr);
}

void tree_struct_free_void (void *value)
{
	struct tree_struct *ptr = (struct tree_struct *) value;
	tree_struct_free (ptr);
}

static void *
	tree_struct_cpy (void *value) {
	struct tree_struct *a = (struct tree_struct *) value;
	struct tree_struct * result;
	result = tree_struct_alloc ();
	result->tree_object = a->tree_object;
	return (result);
}

static int tree_struct_cmp (void *value1, void *value2) {
	struct tree_struct *a = (struct tree_struct *) value1;
	struct tree_struct *b = (struct tree_struct *) value2;

	if (a == NULLTREE)
		return (b == NULLTREE ? 0 : -1 );
	if (b == NULLTREE)
		return (1);
	return (objclass_cmp(a->tree_object,b->tree_object));
}

static void tree_struct_print (PS ps, void *value, int format) {
	struct tree_struct *tree = (struct tree_struct *) value;

	ps_printf (ps,"%s",oc2name(tree->tree_object,oidformat));
}

static void *str2schema (char *str) {
	struct tree_struct * ts;
	objectclass * str2oc();

	ts = tree_struct_alloc ();
	if ((ts->tree_object = str2oc(str)) == NULLOBJECTCLASS) {
		parse_error ("invalid oid in schema '%s'",str);
		free ((char *) ts);
		return (NULLTREE);
	}
	return (ts);
}

static PE ts_enc (void *value) {
	struct tree_struct *ts = (struct tree_struct *) value;
	PE ret_pe;
	encode_Quipu_TreeStructureSyntax(&ret_pe,0,0,NULLCP,ts);
	return (ret_pe);
}

static void * ts_dec (PE pe) {
	struct tree_struct * ts;
	if (decode_Quipu_TreeStructureSyntax(pe,1,NULL,NULLVP,&ts) == NOTOK)
		return (struct tree_struct *)NULL;
	return (ts);
}

void schema_syntax (void) {
	add_attribute_syntax ("schema",
						  ts_enc,		ts_dec,
						  str2schema,	tree_struct_print,
						  tree_struct_cpy,	tree_struct_cmp,
						  tree_struct_free_void,	NULLCP,
						  NULL,			FALSE );
}
