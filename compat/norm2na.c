/* norm2na.c - normalize NSAPaddr to NSAP struct */

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include "psap.h"
#include "isoaddrs.h"
#include "tailor.h"
#include "internet.h"

/* Encoding on "unrealNS" addresses based on

   	"An interim approach to use of Network Addresses",
	S.E. Kille, January 16, 1989
 */

int norm2na (char *p, const int len, struct NSAPaddr *na) {
	na -> na_stack = NA_NSAP;

	if ((len == 8) && ((p[0] == 0x36) || (p[0] == 0x37))) {
		int	xlen;			/* SEK - X121 form */
		char   *cp,
			   *cp2,
			   *dp;
		char	nsap[14];

		dp = nsap;
		for (cp2 = (cp = p + 1) + 7; cp < cp2; cp++) {
			int     j;

			if ((j = ((*cp & 0xf0) >> 4)) > 9)
				goto concrete;
			if (int2char (j + '0', dp) != 0)
				return NOTOK;
			dp++;

			if ((j = (*cp & 0x0f)) > 9) {
				if (j != 0x0f)
					goto concrete;
			} else {
				if (int2char (j + '0', dp) != 0)
					return NOTOK;
				dp++;
			}
		}

		for (cp = nsap, xlen = 14; *cp == '0'; cp++, xlen--)
			continue;
		if (int2char (xlen, &na -> na_dtelen) != 0)
			return NOTOK;
		for (cp2 = na -> na_dte; xlen-- > 0; )
			*cp2++ = *cp++;
		*cp2 = 0;
		na -> na_stack = NA_X25;
		na -> na_community = SUBNET_INT_X25;
	} else {
		struct ts_interim *ts,
				   *tp;

		tp = NULL;
		for (ts = ts_interim; ts -> ts_name; ts++)
			if (len > ts -> ts_length
					&& (tp == NULL || ts -> ts_length > tp -> ts_length)
					&& bcmp_int (p, ts -> ts_prefix, ts -> ts_length) == 0)
				tp = ts;
		if (tp) {
			int	    i,
					ilen,
					rlen;
			char   *cp,
				   *dp,
				   *ep;
			char    nsap[NASIZE * 2 + 1];

			if (tp -> ts_syntax == NA_NSAP)
				goto lock_and_load;
			dp = nsap;
			for (cp = p + tp -> ts_length, ep = p + len;
					cp < ep;
					cp++) {
				int     j;

				if ((j = ((*cp & 0xf0) >> 4)) > 9) {
concrete:
					;
					LLOG (addr_log, LLOG_EXCEPTIONS,
						  ("invalid concrete encoding"));
					goto realNS;
				}
				if (int2char (j + '0', dp) != 0)
					return NOTOK;
				dp++;

				if ((j = (*cp & 0x0f)) > 9) {
					if (j != 0x0f)
						goto concrete;
				} else {
					if (int2char (j + '0', dp) != 0)
						return NOTOK;
					dp++;
				}
			}
			*dp = 0;

			cp = nsap;
lock_and_load:
			;
			na -> na_community = tp -> ts_subnet;
			switch (na -> na_stack = tp -> ts_syntax) {
			case NA_NSAP:
				goto unrealNS;

			case NA_X25:
				if ((int)strlen (cp) < 1) {
					LLOG (addr_log, LLOG_EXCEPTIONS,
						  ("missing DTE+CUDF indicator: %s", nsap));
					goto realNS;
				}
				sscanf (cp, "%1d", &i);
				cp += 1;
				switch (i) {
				case 0:	/* DTE only */
					break;

				case 1:	/* DTE+PID */
				case 2:	/* DTE+CUDF */
					if ((int)strlen (cp) < 1) {
						LLOG (addr_log, LLOG_EXCEPTIONS,
							  ("missing DTE+CUDF indicator: %s",
							   nsap));
						goto realNS;
					}
					sscanf (cp, "%1d", &ilen);
					cp += 1;
					rlen = ilen * 3;
					if ((int)strlen (cp) < rlen) {
						LLOG (addr_log, LLOG_EXCEPTIONS,
							  ("bad DTE+CUDF length: %s", nsap));
						goto realNS;
					}
					if (i == 1) {
						if (ilen > NPSIZE) {
							LLOG (addr_log, LLOG_EXCEPTIONS,
								  ("PID too long: %s", nsap));
							goto realNS;
						}
						dp = na -> na_pid;
						if (int2char (ilen, &na -> na_pidlen) != 0)
							return NOTOK;
					} else {
						if (ilen > CUDFSIZE) {
							LLOG (addr_log, LLOG_EXCEPTIONS,
								  ("CUDF too long: %s", nsap));
							goto realNS;
						}
						dp = na -> na_cudf;
						if (int2char (ilen, &na -> na_cudflen) != 0)
							return NOTOK;
					}
					for (; rlen > 0; rlen -= 3) {
						sscanf (cp, "%3d", &i);
						cp += 3;

						if (i > 255) {
							LLOG (addr_log, LLOG_EXCEPTIONS,
								  ("invalid PID/CUDF: %s", nsap));
							goto realNS;
						}
						if (int2octet (i & 0xff, dp) != 0)
							return NOTOK;
						dp++;
					}
					break;

				default:
					LLOG (addr_log, LLOG_EXCEPTIONS,
						  ("invalid DTE+CUDF indicator: %s", nsap));
					goto realNS;
				}
				strcpy (na -> na_dte, cp);
				if (sizet2char (strlen (na -> na_dte),
						&na -> na_dtelen) != 0)
					return NOTOK;
				break;

			case NA_TCP:
				if ((int)strlen (cp) < 12) {
					LLOG (addr_log, LLOG_EXCEPTIONS,
						  ("missing IP address: %s", nsap));
					goto realNS;
				}
				{
					int	    q[4];

					sscanf (cp, "%3d%3d%3d%3d", q, q + 1, q + 2,
							q + 3);
					sprintf (na -> na_domain,
							 "%d.%d.%d.%d", q[0], q[1], q[2], q[3]);
				}
				cp += 12;

				if (*cp) {
					if ((int)strlen (cp) < 5) {
						LLOG (addr_log, LLOG_EXCEPTIONS,
							  ("missing port: %s", nsap));
						goto realNS;
					}
					sscanf (cp, "%5d", &i);
					cp += 5;
					na -> na_port = htons ((uint16_t) i);

					if (*cp) {
						if ((int)strlen (cp) < 5) {
							LLOG (addr_log, LLOG_EXCEPTIONS,
								  ("missing tset: %s", nsap));
							goto realNS;
						}
						sscanf (cp, "%5d", &i);
						cp += 5;
						na -> na_tset = (uint16_t) i;

						if (*cp)
							LLOG (addr_log, LLOG_EXCEPTIONS,
								  ("extra TCP information: %s", nsap));
					}
				}
				break;

			default:
				LLOG (addr_log, LLOG_NOTICE,
					  ("unknown syntax %d for DSP: %s", ts -> ts_syntax,
					   nsap));
				goto realNS;
			}
		} else {
realNS:
			;
			na -> na_stack = NA_NSAP;
			na -> na_community = SUBNET_REALNS;
unrealNS:
			;
			if (len > sizeof na -> na_address) {
				LLOG (addr_log, LLOG_EXCEPTIONS,
					  ("NSAP address too long: %d octets", len));
				return NOTOK;
			}
			if (bcopy_int (p, na -> na_address, len) != 0)
				return NOTOK;
			if (len > (int) sizeof na -> na_address)
				return NOTOK;
			if (int2char (len, &na -> na_addrlen) != 0)
				return NOTOK;
		}
	}

	return OK;
}
