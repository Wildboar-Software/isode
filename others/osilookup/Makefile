###############################################################################
#   Instructions to Make, for compilation of ISODE/SunLink osilookup program
###############################################################################


###############################################################################
# Generation Rules for program modules
###############################################################################

.c.o:;		$(CC) $(CFLAGS) -c $*.c

###############################################################################
# Programs and Libraries
###############################################################################

LIBES	=	
LLIBS   =	

###############################################################################
# Files
###############################################################################

HFILES	=	
CFILES	=	osilookup.c

all:		osilookup
inst-all:	inst-osilookup
install:	inst-all clean
lint:		l-osilookup

##################################################################
# osilookup
##################################################################

inst-osilookup:	$(SBINDIR)osilookup

$(SBINDIR)osilookup:	xosilookup
		- if [ -f $@ ]; then cp $@ zxosilookup; fi
		-rm -f $@
		cp xosilookup $@
		-@ls -gls $@
		-@echo ""

osilookup:	xosilookup
		cp xosilookup $@

xosilookup:	osilookup.o
		$(LDCC) $(LDFLAGS) -o $@ osilookup.o $(LIBES) $(LSOCKET)

l-osilookup:;	$(LINT) $(LFLAGS) osilookup.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

##################################################################
# clean
##################################################################

clean:;		rm -f *.o x* z* _* core 

grind:;		iprint Makefile
		tgrind -lc $(HFILES) $(CFILES)

true:;
