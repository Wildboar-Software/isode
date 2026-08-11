/* py_advise.c - standard "advise" routine for pepsy/pepy */







#include <stdio.h>
#include <stdarg.h>

#ifndef	lint
char   PY_pepy[BUFSIZ] = "";

void	PY_advise (char* what, char* fmt, ...) {
	va_list	ap;

	va_start (ap, fmt);

	_asprintf (PY_pepy, what, fmt, ap);

	va_end (ap);
}
#else
/* VARARGS */

void
PY_advise (char *what, char *fmt) {
	PY_advise (what, fmt);
}
#endif
