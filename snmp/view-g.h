/* view-g.h - VIEW group */
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include "isoaddrs.h"
#include "internet.h"
#include "psap.h"

/* VIEWS */

static inline int
inSubtree (OID tree, OID object)
{
	size_t nbytes;

	if (tree == NULLOID || object == NULLOID)
		return 0;
	if (tree -> oid_nelem > object -> oid_nelem)
		return 0;
	if (nmemb_bytes (tree -> oid_nelem, sizeof tree -> oid_elements[0],
			 &nbytes) != 0)
		return 0;
	return memcmp ((char *) tree -> oid_elements,
		       (char *) object -> oid_elements, nbytes) == 0;
}

struct subtree {
	struct subtree *s_forw;	/* doubly-linked list */
	struct subtree *s_back;	/* doubly-linked list */

	OID	    s_subtree;		/* subtree */
};

struct view {
	struct view *v_forw;	/* doubly-linked list */
	struct view *v_back;	/* .. */

	OID	    v_name;		/* view name */
	uint32_t  v_mask;		/* view mask */

	struct subtree v_subtree;	/* list of subtrees */

	struct qbuf *v_community;	/* for proxy, traps... */
	struct sockaddr v_sa;

	unsigned int *v_instance;	/* object instance */
	int	    v_insize;		/* .. */
};

extern struct view *VHead;

/* COMMUNITIES */

struct community {
	struct community *c_forw;	/* doubly-linked list */
	struct community *c_back;	/* .. */

	char   *c_name;		/* community name */
	struct NSAPaddr c_addr;	/* network address */

	int	    c_permission;	/* same as ot_access */
#define	OT_YYY	0x08

	OID	    c_vu;		/* associated view */
	struct view *c_view;	/* .. */

	unsigned int *c_instance;	/* object instance */
	int	    c_insize;		/* .. */
	struct community *c_next;	/* next in lexi-order */
};

extern struct community *CHead;

/* TRAPS */

struct trap {
	struct trap *t_forw;	/* doubly-linked list */
	struct trap *t_back;	/* .. */

	char   *t_name;		/* trap name */

	struct view  t_vu;		/* associated view */
	struct view *t_view;	/* .. */

	uint32_t  t_generics;		/* generic traps enabled */

	unsigned int *t_instance;	/* object instance */
	int	    t_insize;		/* .. */
};

extern struct trap *UHead;
