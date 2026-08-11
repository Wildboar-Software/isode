#include "quipu/util.h"
#include "quipu/name.h"

#ifdef AttrT_cmp
#undef AttrT_cmp
#endif

int AttrT_cmp_old (AttributeType x, AttributeType y) {
	if ( x == NULLAttrT )
		return (y ? -1 : 0);
	if ( y == NULLAttrT )
		return (1);
	if (x == y)
		return (0);
	return (x > y) ? 1 : -1;
}
