




#include "quipu/util.h"
#include "quipu/attrvalue.h"

int avs_cmp (AV_Sequence a, AV_Sequence b);

int as_cmp_comp (Attr_Sequence a, Attr_Sequence b) {
	int i;
	if (( i= AttrT_cmp (a->attr_type,b->attr_type)) != 0)
		return (i);
	return (avs_cmp (a->attr_value,b->attr_value));
}

int as_cmp (Attr_Sequence a, Attr_Sequence b) {
	int i;
	for (; (a != NULLATTR) && (b != NULLATTR) ; a = a->attr_link, b = b->attr_link)
		if ( (i =as_cmp_comp (a,b)) != 0)
			return (i);
	if ( (a == NULLATTR) && (b == NULLATTR) )
		return (0);
	else
		return (a ? 1  : -1);
}
