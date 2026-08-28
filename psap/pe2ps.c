/* pe2ps.c - presentation element to presentation stream */

#include <stdio.h>
#include "psap.h"
#include "tailor.h"

static PElement pe_eoc = { PE_CLASS_UNIV, PE_FORM_PRIM, PE_UNIV_EOC, 0 };
static int  pe2ps_aux2 (PS ps, PE pe, const int eval);
int  ps_write_id (PS ps, PE pe);
int  ps_write_len (PS ps, PE pe);

int pe2ps_aux (PS ps, PE pe, const int eval) {
	int     result;

	if (eval > 0)
		switch (pe -> pe_form) {
		case PE_FORM_PRIM:
		case PE_FORM_ICONS:
			break;
		case PE_FORM_CONS:
			ps_get_abs (pe);
			break;
		}
	if ((result = pe2ps_aux2 (ps, pe, eval)) != NOTOK)
		result = ps_flush (ps);
	return result;
}

static int pe2ps_aux2 (PS ps, PE pe, const int eval) {
	PE p;

	if (pe -> pe_form == PE_FORM_ICONS) {
		if (ps_write_aux (ps, pe -> pe_prim, pe -> pe_len, 1) == NOTOK)
			return NOTOK;
		return OK;
	}
	if (ps_write_id (ps, pe) == NOTOK || ps_write_len (ps, pe) == NOTOK)
		return NOTOK;
	switch (pe -> pe_form) {
	case PE_FORM_PRIM:
		if (ps_write_aux (ps, pe -> pe_prim, pe -> pe_len, 1) == NOTOK)
			return NOTOK;
		break;
	case PE_FORM_CONS:
		if (eval < 0)
			return OK;
		if (pe -> pe_len) {
			for (p = pe -> pe_cons; p; p = p -> pe_next)
				if (pe2ps_aux2 (ps, p, 0) == NOTOK)
					return NOTOK;
			if (pe -> pe_len == PE_LEN_INDF
					&& pe2ps_aux2 (ps, &pe_eoc, 0) == NOTOK)
				return NOTOK;
		}
		break;
	}
	return OK;
}

int ps_write_id (PS ps, PE pe) {
	byte    buffer[1 + sizeof (PElementID)];
	byte  *bp = buffer;
	PElementForm    form;
	PElementID id;
	int n;

	if ((form = pe -> pe_form) == PE_FORM_ICONS)
		form = PE_FORM_CONS;
	if (int2u8 (((int) (pe -> pe_class << PE_CLASS_SHIFT) & PE_CLASS_MASK)
		  | ((int) (form << PE_FORM_SHIFT) & PE_FORM_MASK), bp) != 0)
		return NOTOK;
	if ((id = pe -> pe_id) < PE_ID_XTND) {
		if (int2u8 ((int) (*bp | id), bp) != 0)
			return NOTOK;
		bp++;
	} else {
		byte *ep;
		PElementID jd;
		if (int2u8 ((int) (*bp | PE_ID_XTND), bp) != 0)
			return NOTOK;
		ep = buffer;
		for (jd = id; jd != 0; jd >>= PE_ID_SHIFT)
			ep++;
		for (bp = ep; id != 0; id >>= PE_ID_SHIFT) {
			if (int2u8 ((int) (id & PE_ID_MASK), bp) != 0)
				return NOTOK;
			bp--;
		}
		for (bp = buffer + 1; bp < ep; bp++) {
			if (int2u8 ((int) (*bp | PE_ID_MORE), bp) != 0)
				return NOTOK;
		}
		bp = ++ep;
	}
	if (ptrdiff2int (bp - buffer, &n) != 0)
		return NOTOK;
	if (ps_write (ps, buffer, n) == NOTOK)
		return NOTOK;
	return OK;
}

/* probably should integrate the non-PE_LEN_SMAX case with the algorithm in
   num2prim() for a single, unified routine */

int ps_write_len (PS ps, PE pe) {
	byte    buffer[1 + sizeof (PElementLen)];
	byte  *bp = buffer,
		   *ep;
	PElementLen len;
	int n;

	if ((len = pe -> pe_len) == PE_LEN_INDF) {
		if (int2u8 (PE_LEN_XTND, bp) != 0)
			return NOTOK;
		bp++;
	} else if (len <= PE_LEN_SMAX) {
		if (int2u8 (len & 0xff, bp) != 0)
			return NOTOK;
		bp++;
	} else {
		ep = buffer + sizeof buffer - 1;
		for (bp = ep; len != 0 && buffer < bp; len >>= 8) {
			if (int2u8 (len & 0xff, bp) != 0)
				return NOTOK;
			bp--;
		}
		{
			const ptrdiff_t nb = ep - bp;

			if (int2u8 (PE_LEN_XTND | (int) (nb & 0xff), bp) != 0)
				return NOTOK;
		}
		if (ptrdiff2int (ep - bp + 1, &n) != 0)
			return NOTOK;
		if (ps_write (ps, bp, n) == NOTOK)
			return NOTOK;

		return OK;
	}
	if (ptrdiff2int (bp - buffer, &n) != 0)
		return NOTOK;
	if (ps_write (ps, buffer, n) == NOTOK)
		return NOTOK;
	return OK;
}
