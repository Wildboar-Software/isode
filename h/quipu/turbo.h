/* turbo.h - your comments here */

#ifndef QUIPUTURBO
#define QUIPUTURBO

#include "quipu/config.h"
#include "quipu/name.h"
#include "quipu/entry.h"

/*
 * this structure represents a generic avl tree node.
 */

typedef struct avlnode {
	caddr_t		avl_data;
	char		avl_bf;
	struct avlnode	*avl_left;
	struct avlnode	*avl_right;
} Avlnode;

#define NULLAVL	((Avlnode *) NULL)

/* balance factor values */
#define LH 	-1
#define EH 	0
#define RH 	1

/* avl routines */
#define avl_getone(x)	(x == 0 ? 0 : (x)->avl_data)
#define avl_onenode(x)	(x == 0 || ((x)->avl_left == 0 && (x)->avl_right == 0))
extern int	avl_insert(Avlnode **root, caddr_t data, int (*fcmp)(caddr_t data1, caddr_t data2), int (*fdup)(caddr_t data1, caddr_t data2));
extern caddr_t	avl_delete(Avlnode **root, caddr_t data, int (*fcmp)(caddr_t data1, caddr_t data2));
extern caddr_t	avl_find(Avlnode *root, caddr_t data, int (*fcmp)(caddr_t data1, caddr_t data2));
extern caddr_t	avl_getfirst(Avlnode *root);
extern caddr_t	avl_getnext(void);
extern int	avl_dup_error(void);

int turbo_isoptimized(AttributeType attr);

/* apply traversal types */
#define AVL_PREORDER	1
#define AVL_INORDER	2
#define AVL_POSTORDER	3
/* what apply returns if it ran out of nodes */
#define AVL_NOMORE	-6

#ifdef TURBO_INDEX

/*
 * this structure represents an attribute index.  the index is composed
 * of the attribute type, a count of the number of different values in
 * the tree, and a pointer to the root of the tree of attribute values.
 * these nodes are linked together (one node for each attribute type being
 * optimized) in an avl tree in each index node.  there is also
 * a tree and count for soundex values of the attribute.  and now there's
 * also a tree and count for reversed values of the attribute (for final
 * substring queries).
 */

typedef struct index {
	/* entry associated with this index */
	/* sibling => parent		    */
	DN		i_dn;		/* subtree => base		    */

	/* for subtree index: descendants   */
	/* not held locally		    */
	struct entry	**i_nonleafkids;

	/* for both: aliases that escape    */
	/* the scope of the index	    */
	struct entry	**i_nonlocalaliases;

	AttributeType	i_attr;		/* the attribute type		    */
	int		i_count;	/* number of entries in this tree   */
	int		i_rcount;	/* number of ents in reverse tree   */
	int		i_scount;	/* number of ents in soundex tree   */
	Avlnode		*i_root;	/* tree of values		    */
	Avlnode		*i_rroot;	/* tree of reverse values	    */
	Avlnode		*i_sroot;	/* tree of soundex values	    */
} Index;

int idn_cmp(DN a, Index *b);
int idn_cmp_from_caddrs(caddr_t data1, caddr_t data2);
int th_prefix(DN a, DN b);

#define NULLINDEX	((Index *) 0)

typedef struct {
	struct entry	*ep_entry;
	int		ep_count;
} eptr_node;

typedef struct index_node {
	caddr_t		in_value;
	struct entry	**in_entries;
	int		in_num;
	int		in_max;
} Index_node;

#define NULLINDEXNODE	((Index_node *) 0)

#define get_subtree_index(x) \
	((Index *) avl_find( subtree_index, (caddr_t) (x), idn_cmp_from_caddrs ))

#define get_sibling_index(x) \
	((Index *) avl_find( sibling_index, (caddr_t) (x), idn_cmp_from_caddrs ))

int avl_free(
	Avlnode *root,
	void (*dfree)(caddr_t data)
);

caddr_t avl_getfirst(Avlnode *root);
caddr_t avl_getnext (void);
int avl_dup_error (void);
caddr_t avl_find(
	Avlnode *root,
	caddr_t data,
	int (*fcmp)(caddr_t data1, caddr_t data2)
);
int avl_prefixapply(
	Avlnode *root,
	caddr_t data,
	int (*fmatch)(caddr_t data1, caddr_t data2),
	caddr_t marg,
	int (*fcmp)(caddr_t data1, caddr_t data2, caddr_t carg),
	caddr_t carg,
	int stopflag
);
int avl_apply(
	Avlnode *root,
	int (*fn)(caddr_t data, caddr_t arg),
	caddr_t arg,
	int stopflag,
	int type
);
caddr_t avl_delete(
	Avlnode **root,
	caddr_t data,
	int (*fcmp)(caddr_t data1, caddr_t data2)
);
int avl_insert(
	Avlnode **root,
	caddr_t data,
	int (*fcmp)(caddr_t data1, caddr_t data2),
	int (*fdup)(caddr_t data1, caddr_t data2)
);

#endif /* TURBO_INDEX */
#endif /* QUIPUTURBO */
