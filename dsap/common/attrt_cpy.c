#include "quipu/util.h"
#include "quipu/name.h"
#include "quipu/malloc.h"

/* Could turn this all into a macro ! */

#ifdef AttrT_cpy
#undef AttrT_cpy
#endif

AttributeType AttrT_cpy (x)
AttributeType x;
{
	return x;
}
