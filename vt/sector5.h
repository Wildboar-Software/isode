/* sector5.h - VTPM: FSM sector 5 definitions */





typedef struct expl_ptr {
#define NULLCOORD  -1
	int xval;	/* if they don't exist = NULLCOORD */
	int yval;
	int zval;
} EXPL_PTR;

#define NOBKTOK	   -1  		/* for token in S mode */

typedef struct bkq_content {
	int token_val; /* 0 initiator, 1 acceptor, 2 accChoice or nobktok */
	EXPL_PTR ExplPtr;
} BKQ_content;

typedef struct bkr_content {
	int token_val; /* 0 initiator, 1 acceptor or nobktok */
	EXPL_PTR ExplPtr;
} BKR_content;

typedef struct br_cnt {
	BKQ_content	BKQcont;
	BKR_content	BKRcont;
	EXPL_PTR 	ExPtr;
} BRcnt;

