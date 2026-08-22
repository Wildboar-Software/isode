/* photo.h - */

typedef  struct {
	char length;            /* number of bits in the pattern  */
	short pattern;          /* the bit pattern
						     (starting at bit 13) */
} code_word;

typedef struct {
	code_word  make,        /* make up code */
			   /* if make.length  = 0  then there
			   		 is no make up code */
			   term;        /* terminal code */
} full_code;

typedef struct {
	unsigned char pos;     /* pointer into the string */
	int   * run_top;       /* run length pointer */
	int   * run_pos;       /* pointer to above */
	unsigned char mask;    /* mask to get at one bit of the string */
	char  * dbuf_top;
	char  * dbuf;
} bit_string;

typedef struct {
	char  r_type;           /* descibes type of run */
	int   run_length;       /* a run_length         */
} run_type;

typedef struct nd {
	char n_type;            /* describe the type of node */
	struct nd * one;        /* one branch */
	struct nd * zero;       /* zero branch */
	int     value;          /* value arrived at */
} node ;

/* now some constant definitions */
#define MSB_MASK 010000  /* octal. Mask to get at first bit
						of code_word  pattern  */
#define BIT_MASK 0x80     /* i.e. set bit 8 of a char */

#define WHITE   0
#define WT_TERM 0
#define BLACK   1
#define BL_TERM 1
#define MAKE    2
#define EOLN    127
#define INTERNAL 7

#define P   1
#define H   2
#define V0  6
#define VR1 7
#define VR2 8
#define VR3 9
#define VL1 5
#define VL2 4
#define VL3 3

#define BUFLEN  75000
#define LINEBUF 4096

#define FIXED_OFFSET 6

full_code get_code(int run, char colour);
int photo_start (char * name);
int photo_end (char * name);
void hide_picture (void);
int decode_t4 (char *data, char *name, int size);
void photo_black (int length);
void photo_white (int length);
void photo_line_end (bit_string * line);
int build_trees (void);
void set_bit (bit_string *lineptr);
void clr_bit (bit_string *lineptr);
char get_bit (bit_string *lineptr);
node * get_node (void);
run_type next_run (bit_string *lineptr, char xcolour);
