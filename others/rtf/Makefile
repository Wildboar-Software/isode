###############################################################################
#   Instructions to Make, for compilation of the ISODE RT-file transfer utility
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/others/rtf/RCS/Makefile,v 9.0 1992/06/16 12:48:07 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:48:07  isode
# Release 8.0
#
# 
###############################################################################

###############################################################################
#
#				 NOTICE
#
#    Acquisition, use, and distribution of this module and related
#    materials are subject to the restrictions of a license agreement.
#    Consult the Preface in the User's Manual for the full terms of
#    this agreement.
#
###############################################################################


###############################################################################
# Generation Rules for program modules
###############################################################################

PEPYPATH=	-DPEPYPATH

.c.o:;		$(CC) $(CFLAGS) -c $*.c


###############################################################################
# Programs and Libraries
###############################################################################

LIBES	=	$(LIBISODE) -lcrypt
LLIBS   =	$(TOPDIR)llib-lisode


###############################################################################
# Files
###############################################################################

HFILES	=	rtf.h
CFILES	=	rtf.c rtfd.c rtfsbr.c
PYFILES	=	rtf.py


##################################################################
# Here it is...
##################################################################

all:		rtfd rtf
inst-all:	inst-rtfd inst-rtf manuals
install:	inst-all clean
lint:		l-rtfd l-rtf


##################################################################
# rtfd
##################################################################

inst-rtfd:	$(SBINDIR)iso.rtf

$(SBINDIR)iso.rtf:	xrtfd
		- if [ -f $@ ]; then cp $@ ziso.rtf; fi
		-rm -f $@
		cp xrtfd $@
		-@ls -gls $@
		-@echo ""

rtfd:		xrtfd
		cp xrtfd $@

xrtfd:		rtfd.o rtfsbr.o RTF_tables.o
		$(LDCC) $(LDFLAGS)  -o $@ rtfd.o rtfsbr.o RTF_tables.o \
			$(LIBES) $(LSOCKET)

l-rtfd:		RTF_tables.c true
		$(LINT) $(LFLAGS) rtfd.c rtfsbr.c RTF_tables.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

rtfd.o:		$(HFILES) RTF-types.h


##################################################################
# rtf
##################################################################

inst-rtf:	$(BINDIR)rtf

$(BINDIR)rtf:	xrtf
		- if [ -f $@ ]; then cp $@ zrtf; fi
		-rm -f $@
		cp xrtf $@
		-@ls -gls $@
		-@echo ""

rtf:		xrtf
		cp xrtf $@

xrtf:		rtf.o rtfsbr.o RTF_tables.o
		$(LDCC) $(LDFLAGS) -o $@ rtf.o rtfsbr.o RTF_tables.o \
			$(LIBES) $(LSOCKET)

l-rtf:		RTF_tables.c true
		$(LINT) $(LFLAGS) rtf.c rtfsbr.c RTF_tables.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

rtf.o:		$(HFILES) RTF-types.h


################################################################
# librtf
################################################################

RTF_tables.o:	RTF_tables.c RTF-types.h

RTF_tables.c RTF-types.h: rtf.py $(TOPDIR)pepsy/xpepsy
		$(TOPDIR)pepsy/xpepsy -A -f -h -m rtf.py

rtfsbr.o:	$(HFILES)


################################################################
# manual pages
################################################################

MANUALS	=	rtfd.8c rtf.1c

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


##################################################################
# clean
##################################################################

clean:;		rm -f *.o *.a RTF* x* z* _* core 

grind:;		iprint Makefile
		tgrind -lc $(HFILES) $(CFILES)
		tgrind -lpepy -d $(TOPDIR)pepy/grindefs $(PYFILES)
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve

true:;
