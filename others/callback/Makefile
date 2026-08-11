###############################################################################
#   Instructions to Make, for compilation of the ISODE callback demo
###############################################################################

###############################################################################
#
# 
#
#
# 
#
#
#
# 
###############################################################################




###############################################################################
# Options
###############################################################################

BINDIR	=	/usr/new/


###############################################################################
# Generation Rules for program modules
###############################################################################

.c.o:;		$(CC) $(CFLAGS) -c $*.c


###############################################################################
# Programs and Libraries
###############################################################################

LIBES	=	$(LIBISODE)
LLIBS   =	$(TOPDIR)llib-lisode


###############################################################################
# Files
###############################################################################

HFILES	=	callback.h
CFILES	=	initiator.c responder.c report.c




all:		initiator responder
inst-all:	inst-initiator inst-responder
install:	inst-all clean
lint:		l-initiator l-responder


##################################################################
# initiator
##################################################################

inst-initiator:	$(BINDIR)callback

$(BINDIR)callback:	xinitiator
		- if [ -f $@ ]; then cp $@ zcallback; fi
		-rm -f $@
		cp xinitiator $@
		-@ls -gls $@
		-@echo ""

initiator:	xinitiator
		cp xinitiator $@

xinitiator:	initiator.o report.o
		$(LDCC) $(LDFLAGS) -o $@ initiator.o report.o $(LIBES) \
			$(LSOCKET)

l-initiator:;	$(LINT) $(LFLAGS) initiator.c report.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

initiator.o:	$(HFILES)


##################################################################
# responder
##################################################################

inst-responder:	$(SBINDIR)iso.callback

$(SBINDIR)iso.callback:	xresponder
		- if [ -f $@ ]; then cp $@ ziso.callback; fi
		-rm -f $@
		cp xresponder $@
		-@ls -gls $@
		-@echo ""

responder:	xresponder
		cp xresponder $@

xresponder:	responder.o report.o
		$(LDCC) $(LDFLAGS)  -o $@ responder.o report.o $(LIBES) \
			$(LSOCKET)

l-responder:;	$(LINT) $(LFLAGS) responder.c report.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

responder.o:	$(HFILES)


##################################################################
# clean
##################################################################

clean:;		rm -f *.o x* z* _* core 

grind:;		iprint Makefile
		tgrind -lc $(HFILES) $(CFILES)

true:;
