###############################################################################
#   Instructions to Make, for compilation of ISODE Transport Service Bridge
###############################################################################


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

HFILES	=	
CFILES	=	tsbridge.c

all:		tsbridge
inst-all:	inst-tsbridge manuals
install:	inst-all clean
lint:		l-tsbridge

##################################################################
# tsbridge
##################################################################

inst-tsbridge:	$(SBINDIR)tsbridge

$(SBINDIR)tsbridge:	xtsbridge
		- if [ -f $@ ]; then cp $@ zxtsbridge; fi
		-rm -f $@
		cp xtsbridge $@
		-@ls -gls $@
		-@echo ""

tsbridge:	xtsbridge
		cp xtsbridge $@

xtsbridge:	tsbridge.o
		$(LDCC) $(LDFLAGS) -o $@ tsbridge.o $(LIBES) $(LSOCKET)

l-tsbridge:;	$(LINT) $(LFLAGS) tsbridge.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

################################################################
# manual pages
################################################################

MANUALS	=	tsbridge.8c

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""

##################################################################
# clean
##################################################################

clean:;		rm -f *.o x* z* _* core 

grind:;		iprint Makefile
		tgrind -lc $(HFILES) $(CFILES)

true:;
