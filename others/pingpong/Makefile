###############################################################################
#   Instructions to Make, for compilation of ISODE xSAP processes
###############################################################################


.c.o:;	$(CC) $(CFLAGS) -c $*.c

LIBES	=	$(LIBISODE)
LLIBS   =	$(TOPDIR)llib-lisode

all:		pingpong
inst-all:	
install:	
lint:		l-pingpong

pingpong:	xpingpong
		cp xpingpong $@

xpingpong:	pingpong.o
		$(LDCC) -o $@ pingpong.o $(LIBES) $(LSOCKET)

l-pingpong:;	$(LINT) $(LFLAGS) pingpong.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

clean:;		rm -f x* z* _* *.o core
