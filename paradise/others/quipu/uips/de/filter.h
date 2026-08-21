/* template.c - your comments here */

struct s_filter * andfilter(void);
struct s_filter * orfilter(void);
struct s_filter * eqfilter(int matchtype, char *type, char *value);
struct s_filter * subsfilter(int substrtype, char *type, char *value);
struct s_filter * presfilter(char *type);
