/* mpkt.h - defines the report format for management */
#include <stdint.h>
#include "isoaddrs.h"

struct MReport {
	uint16_t type;
#define OPREQIN         1
#define OPREQOUT        2
#define USERDT          3
#define USERDR          4
#define DISCREQ         5
#define PROTERR         6
#define CONGEST         7
#define CONFIGBAD       8
#define OPREQINBAD      9
#define OPREQOUTBAD     10
#define SOURCEADDR      11
#define	STARTLISTEN	12
#define	ENDLISTEN	13

	long    id;		/* process id */
	uint16_t cid;        /* connection fd */

	union {
		struct {
			int a, b, c, d, e, f;
		} gp;

		struct {
			int	    tsel_len;
			char    tsel[TSSIZE];
			struct NSAPaddr nsap;
		} taddr;
	} u;
};

int	TManGen (unsigned int, ...);
