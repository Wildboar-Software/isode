###############################################################################
#   Instructions to Make, for compilation of ISODE RONOT library
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/ronot/RCS/Makefile,v 9.0 1992/06/16 12:36:36 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:36:36  isode
# Release 8.0
#
# 
###############################################################################

###############################################################################
#
#                                NOTICE
#
#    Acquisition, use, and distribution of this module and related
#    materials are subject to the restrictions of a license agreement.
#    Consult the Preface in the User's Manual for the full terms of
#    this agreement.
#
###############################################################################


PEPYPATH=	-DPEPYPATH

LIBES   =       libronot.a
LLIBS   =       $(TOPDIR)llib-lrosap $(TOPDIR)llib-lacsap $(TOPDIR)llib-lcompat
HFILES	=	$(HDIR)ronot.h $(HDIR)rosap.h $(HDIR)acsap.h \
		$(HDIR)psap2.h $(HDIR)psap.h $(HDIR)ssap.h $(HDIR)isoaddrs.h \
		$(HDIR)manifest.h $(HDIR)general.h $(HDIR)config.h


#############################################################
# Here it is...
#############################################################

all:            libronot
inst-all:	# inst-libronot
install:        inst-all clean
lint:           l-libronot


#############################################################
# libronot
#############################################################

PYFILES	=	ronot.py

CFILES  =       ronotbind1.c ronotbind2.c ronotunbind1.c \
		ronotunbind2.c ronotabort.c ronotlose.c RONOT_tables.c

OFILES  =       ronotbind1.o ronotbind2.o ronotunbind1.o \
		ronotunbind2.o ronotabort.o ronotlose.o RONOT_tables.o

inst-libronot:	$(LIBDIR)libronot.a $(LINTDIR)llib-lronot

$(LIBDIR)libronot.a:	libronot.a
		-rm -f $@
		cp libronot.a $@
		@$(UTILDIR)make-lib.sh $(SYSTEM) $@ -ranlib
		-@ls -gls $@
		-@echo ""

$(LINTDIR)llib-lronot:	llib-lronot
		- if [ -f $@ ]; then cp $@ zllib-lronot; fi
		-rm -f $@
		sed -e 's%#include "\(.*\)"%#include "$(INCDIR)\1"%' \
			< llib-lronot | \
			sed -e 's%#include "/usr/include/\(.*\)"%#include <\1>%' > $@
		@$(UTILDIR)inst-lint.sh $(SYSTEM) $(OPTIONS) $@
		-@ls -gls $@ $@.ln
		-@echo ""

libronot:	libronot.a

libronot.a:	ronotvrsn.o
		-rm -f $@
		@$(UTILDIR)make-lib.sh $(SYSTEM) $(ARFLAGS) $@ $(OFILES) \
			ronotvrsn.o
		-@rm -f $(TOPDIR)libronot.a $(TOPDIR)llib-lronot
		-@$(LN) libronot.a $(TOPDIR)libronot.a
		-@$(LN) llib-lronot $(TOPDIR)llib-lronot
		-@ls -l $@
		-@echo "RO-notation library built normally"

ronotvrsn.c:	$(OFILES)
		@$(UTILDIR)version.sh ronot > $@

l-libronot:	true
		$(LINT) $(LFLAGS) $(CFILES) ronotvrsn.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"


RONOT_tables.o:  RONOT_tables.c RONOT-types.h

RONOT_tables.c RONOT-types.h:	ronot.py $(TOPDIR)pepsy/xpepsy
		$(TOPDIR)pepsy/xpepsy -A -f -h -m ronot.py


#############################################################
# clean
#############################################################

clean:;         rm -f  *.o *.a *.ph RONOT* z* _* core ronotvrsn.c

grind:;         iprint Makefile
		tgrind -lc $(CFILES) ronotvrsn.c llib-lronot
		tgrind -lpepy -d $(TOPDIR)pepy/grindefs $(PYFILES)
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve

true:;

# DO NOT DELETE THIS LINE
# Dependencies follow
ronotabort.o: $(HDIR)tailor.h $(HDIR)logger.h $(HFILES)
ronotbind1.o: $(HDIR)tailor.h $(HDIR)logger.h RONOT-types.h $(HFILES)
ronotbind2.o: $(HDIR)tailor.h $(HDIR)logger.h RONOT-types.h $(HFILES)
ronotlose.o: $(HDIR)tailor.h $(HDIR)logger.h $(HFILES)
ronotunbind1.o: $(HDIR)tailor.h $(HDIR)logger.h RONOT-types.h $(HFILES)
ronotunbind2.o: $(HDIR)tailor.h $(HDIR)logger.h RONOT-types.h $(HFILES)
