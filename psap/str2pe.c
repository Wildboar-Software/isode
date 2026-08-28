/* str2pe.c - create an Inline CONStructor PElement */

#include <stdio.h>
#include "psap.h"
#include "tailor.h"
static PElementLen str_get_len (const char *s, int len, int *result);


static PElementLen	str_get_len (const char *s, int len, int *result);
static int  str_get_start (char **sp, int *n, PElementClass *class, PElementForm *form, PElementID *id, PElementLen *plen, int *result);

#define	seterr(e,v)		(*result = (e), (v))

PE str2pe (char *s, int len, int *advance, int *result) {
#ifdef	DEBUG
	const int	    n = len;
#endif
	char   *sp;
	PElementClass   class;
	PElementForm    form;
	PElementID    id;
	PElementLen plen;
	PE	    pe;

	*result = PS_ERR_NONE;
	sp = s;
	if (str_get_start (&sp, &len, &class, &form, &id, &plen, result) == NOTOK)
		return NULLPE;
	if (form == PE_FORM_CONS)
		form = PE_FORM_ICONS;
	if (plen == PE_LEN_INDF
			&& (plen = str_get_len (sp, len, result)) == PE_LEN_INDF)
		return NULLPE;
	if ((pe = pe_alloc (class, form, id)) == NULLPE)
		return seterr (PS_ERR_NMEM, NULLPE);
	if (ptrdiff2int (sp - s, &pe -> pe_ilen) != 0) {
		pe_free (pe);
		return seterr (PS_ERR_OVERLEN, NULLPE);
	}
	if (form == PE_FORM_ICONS) {
		pe -> pe_len = pe -> pe_ilen + plen;
		pe -> pe_prim = (PElementData) s;
	} else if (pe -> pe_len = plen)
		pe -> pe_prim = (PElementData) sp;
	pe -> pe_inline = 1;
	if (advance)
		*advance = pe -> pe_ilen + plen;
#ifdef	DEBUG
	if (psap_log -> ll_events & LLOG_PDUS)
		pe2text (psap_log, pe, 1, n);
#endif
	return pe;
}

static int str_get_start (char **sp, int *n, PElementClass *class, PElementForm *form, PElementID *id, PElementLen *plen, int *result) {
	int i,
		len;
	char *s;
	byte    c,
			d;
	PElementID    jd;
	PElementLen qlen;

	s = *sp, len = *n;
	if (len-- <= 0)
		return seterr (PS_ERR_EOF, NOTOK);
	c = as_octet (*s++);
	{
		uint8_t cl,
				fo;

		if (int2u8 (((int)(c & PE_CLASS_MASK)) >> PE_CLASS_SHIFT, &cl) != 0
				|| int2u8 (((int)(c & PE_FORM_MASK)) >> PE_FORM_SHIFT, &fo)
				!= 0)
			return seterr (PS_ERR_OVERID, NOTOK);
		*class = cl;
		*form = fo;
	}
	if ((jd = (c & PE_CODE_MASK)) == PE_ID_XTND)
		for (jd = 0;; jd <<= PE_ID_SHIFT) {
			if (len-- <= 0)
				return seterr (PS_ERR_EOFID, NOTOK);
			d = as_octet (*s++);

			jd |= d & PE_ID_MASK;
			if (!(d & PE_ID_MORE))
				break;
			if (jd & (PE_ID_MASK << (PE_ID_SHIFT - 1)))
				return seterr (PS_ERR_OVERID, NOTOK);
		}
	*id = jd;
#ifdef	DEBUG
	SLOG (psap_log, LLOG_DEBUG, NULLCP,
		  ("class=%d form=%d id=%d", *class, *form, *id));
#endif
	if (len-- <= 0)
		return seterr (PS_ERR_EOFLEN, NOTOK);
	c = as_octet (*s++);
	if ((i = c) & PE_LEN_XTND) {
		{
			size_t ilen;

			i &= PE_LEN_MASK;
			if (int2sizet (i, &ilen) != 0
					|| ilen > sizeof (PElementLen))
				return seterr (PS_ERR_OVERLEN, NOTOK);
		}
		if (i) {
			for (qlen = 0; i-- > 0;) {
				if (len-- <= 0)
					return seterr (PS_ERR_EOFLEN, NOTOK);
				c = as_octet (*s++);

				qlen = (qlen << 8) | (c & 0xff);
			}

			*plen = qlen;
		} else if (*form == PE_FORM_PRIM)
			return seterr (PS_ERR_INDF, NOTOK);
		else
			*plen = PE_LEN_INDF;
	} else
		*plen = i;
#ifdef	DEBUG
	SLOG (psap_log, LLOG_DEBUG, NULLCP, ("len=%d", *plen));
#endif
	*sp = s, *n = len;
	return OK;
}

static PElementLen str_get_len (const char *s, int len, int *result) {
	char   *sp;
	PElementClass class;
	PElementForm form;
	PElementID id;
	PElementLen plen;

	for (sp = s;;) {
		if (str_get_start (&sp, &len, &class, &form, &id, &plen, result)
				== NOTOK)
			return PE_LEN_INDF;
		if (class == PE_CLASS_UNIV && id == PE_UNIV_EOC)
			return ((PElementLen) (sp - s));
		if (plen == PE_LEN_INDF
				&& (plen = str_get_len (sp, len, result)) == PE_LEN_INDF)
			return PE_LEN_INDF;
		sp += plen, len -= plen;
	}
}
