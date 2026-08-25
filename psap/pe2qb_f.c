/* pe2qb_f.c - presentation element to qbuf, the qbuf must be one piece. */
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "psap.h"

#define moveit(c, l)        do { \
				size_t _ml; \
				if (int2sizet ((l), &_ml) != 0) \
					return NOTOK; \
				if (Qcp + _ml > Ecp) { \
				 printf("pe2qb_f: Qcp overflow len %d\n", \
					(l)); \
				 return(NOTOK); \
			    } \
				memcpy (Qcp, (char *) (c), _ml); \
				Qcp += _ml; \
				Len  += (l); \
			    } while (0)

static PElement pe_eoc = { PE_CLASS_UNIV, PE_FORM_PRIM, PE_UNIV_EOC, 0 };

char *Qcp, *Ecp;

int Len;

int pe2qb_f (PE pe) {
	PE	    p;
	int elm_len;
	byte    elmbuffer[1 + sizeof(PElementLen)];
	byte  *bp,  *ep;
	PElementForm    form;
	PElementID id;
	PElementLen len;

	if ((form = pe -> pe_form) == PE_FORM_ICONS) {
		elm_len = pe->pe_len;
		moveit(pe->pe_prim, elm_len);
		return(Len);
	}

	/* Put the id into the qbuf */

	if (int2octet (((int) (pe -> pe_class << PE_CLASS_SHIFT) & PE_CLASS_MASK)
		   | ((int) (form << PE_FORM_SHIFT) & PE_FORM_MASK), Qcp) != 0)
		return NOTOK;

	if ((id = pe -> pe_id) < PE_ID_XTND) {
		if (int2octet ((int) (as_octet (*Qcp) | id), Qcp) != 0)
			return NOTOK;
		Qcp++;
		Len++;
	} else {
		byte    idbuffer[1 + sizeof (PElementID)];
		PElementID jd;

		ep = (bp = idbuffer);
		{
			uint8_t b;

			if (int2u8 ((int) (as_octet (*Qcp) | (unsigned) PE_ID_XTND),
					&b) != 0)
				return NOTOK;
			*bp = b;
		}
		for (jd = id; jd != 0; jd >>= PE_ID_SHIFT)
			ep++;

		for (bp = ep; id != 0; id >>= PE_ID_SHIFT) {
			uint8_t b;

			if (int2u8 ((int) (id & PE_ID_MASK), &b) != 0)
				return NOTOK;
			*bp-- = b;
		}
		for (bp = idbuffer + 1; bp < ep; bp++) {
			if (int2u8 ((int) (*bp | PE_ID_MORE), bp) != 0)
				return NOTOK;
		}

		bp = ++ep;
		if (ptrdiff2int (bp - idbuffer, &elm_len) != 0)
			return NOTOK;
		moveit(idbuffer, elm_len);
	}

	/* Put the length into the qbuf */

	if ((len = pe -> pe_len) == PE_LEN_INDF) {
		if (int2octet (PE_LEN_XTND, Qcp) != 0)
			return NOTOK;
		Qcp++;
		Len++;
	} else if (len <= PE_LEN_SMAX) {
		uint8_t b;

		if (int2u8 (len, &b) != 0)
			return NOTOK;
		memcpy (Qcp, &b, 1);
		Qcp++;
		Len++;
	} else {
		uint8_t b;

		ep = elmbuffer + sizeof elmbuffer - 1;
		for (bp = ep; len != 0 && elmbuffer < bp; len >>= 8) {
			if (int2u8 (len & 0xff, &b) != 0)
				return NOTOK;
			*bp-- = b;
		}
		{
			ptrdiff_t nb = ep - bp;

			if (int2u8 (PE_LEN_XTND | (int) (nb & 0xff), &b) != 0)
				return NOTOK;
			*bp = b;
		}
			if (ptrdiff2int (ep - bp + 1, &elm_len) != 0)
				return NOTOK;
			moveit(bp, elm_len);
	}

	/* Now put the actual value into the qbuf */

	switch (pe -> pe_form) {
	case PE_FORM_PRIM:
		elm_len = pe->pe_len;
		moveit(pe->pe_prim, elm_len);
		break;

	case PE_FORM_CONS:
		if (pe -> pe_len) {
			for (p = pe -> pe_cons; p; p = p -> pe_next)
				if (pe2qb_f (p) == NOTOK)
					return NOTOK;

			if (pe -> pe_len == PE_LEN_INDF
					&& pe2qb_f (&pe_eoc) == NOTOK)
				return NOTOK;
		}
		break;

	default:
		abort();
	}

	return(Len);
}
