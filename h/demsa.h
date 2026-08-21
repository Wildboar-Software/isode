/* demsb.h - DEC DEMSA router interface (part a!) */

/* read write and error handlers, defined in compat/select.c */
void xhandler(int fd);
void rhandler(int fd);
void whandler(int fd);
#define   our_TCP_IP_SOCKET 1112
#define   our_X25_SOCKET    1113
#define   our_TABLE_SIZE    100
typedef struct {
	int TCP_IP_OR_X25_SOCKET;
	int descriptor;
} t_list_of_conn_desc;
