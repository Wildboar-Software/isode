/* ufn.h - user-friendly naming routines */

#ifndef _QUIPUUFN_
#define _QUIPUUFN_

#include "quipu/util.h"
#include "quipu/name.h"
#include "quipu/entry.h"

typedef struct dn_seq * DNS;
#define NULLDNS ((struct dn_seq *) NULL)

typedef struct _envlist {
	DNS	Dns;
	int	Upper;
	int	Lower;
	struct  _envlist * Next;
} * envlist;
#define NULLEL ((envlist) NULL)
envlist	read_envlist ();

extern char ufn_notify;

extern int ufn_flags;
#define	UFN_NULL	0x00
#define	UFN_APPROX	0x01
#define	UFN_WILDHEAD	0x02
#define	UFN_ALL	(UFN_APPROX | UFN_WILDHEAD)

int	ufn_init (void);
int ufn_match (int c, char **v, DNS (*interact) (DNS, DN, char *), DNS *result, envlist el);

int aet_match (
	int c,
	char **v,
	DNS (*interact) (DNS, DN, char *),
	DNS *result,
	envlist el,
	char *context
);

int dnSelect (char *s, DNS *dlist, DNS (*interact) (DNS, DN, char *), DNS el);

#endif
