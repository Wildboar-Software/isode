###############################################################################
#   Instructions to Make, for compilation of ISODE FTAM processes
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/ftam/RCS/Makefile,v 9.0 1992/06/16 12:14:55 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:14:55  isode
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


PEPYPATH=	-DPEPYPATH

.SUFFIXES:	.py .c .o


LIBES	=	libftam.a $(TOPDIR)libisode.a
LLIBS	=	$(TOPDIR)llib-lisode
HFILES	=	$(HDIR)ftam.h \
		$(HDIR)acsap.h $(HDIR)psap2.h $(HDIR)psap.h $(HDIR)ssap.h \
		$(HDIR)isoaddrs.h \
		$(HDIR)manifest.h $(HDIR)general.h $(HDIR)config.h
HXFILES	=	$(HFILES) $(HDIR)fpkt.h CONT-types.h FADU-types.h \
		FTAM-types.h 


##################################################################
# Here it is...
##################################################################

all:		libftam
inst-all:	inst-libftam documents manuals
install:	inst-all clean
lint:		l-libftam


##################################################################
# libftam
##################################################################

CFILES	=	ftamprovider.c ftamerror.c ftaminitiate.c ftamrespond.c \
		ftamrelease1.c ftamrelease2.c ftamabort.c ftamgroup1.c \
		ftamgroup2.c ftamaccess1.c ftamaccess2.c ftambulk.c \
		ftambulk1.c ftambulk2.c ftamselect.c ftamtrace.c ftamacl.c \
		ftamattr.c ftambits.c ftamchrg.c ftamconctl.c ftamdiag.c \
		ftamfaduid.c ftampass.c ftamshare.c ftamlose.c ftamdocument.c \
		ftamfdf.c
PYFILES	=	cont.py fadu.py ftam.py
OFILES	=	ftamprovider.o ftamerror.o ftaminitiate.o ftamrespond.o \
		ftamrelease1.o ftamrelease2.o ftamabort.o ftamgroup1.o \
		ftamgroup2.o ftamaccess1.o ftamaccess2.o ftambulk.o \
		ftambulk1.o ftambulk2.o ftamselect.o ftamtrace.o ftamacl.o \
		ftamattr.o ftambits.o ftamchrg.o ftamconctl.o ftamdiag.o \
		ftamfaduid.o ftampass.o ftamshare.o ftamlose.o ftamdocument.o \
		ftamfdf.o \
		$(OSTRINGS)

HEADERS	=	CONT.ph CONT-types.h \
		FADU.ph FADU-types.h \
		FTAM.ph FTAM-types.h 

inst-libftam:	$(LIBDIR)libftam.a inst-headers $(LINTDIR)llib-lftam

$(LIBDIR)libftam.a:	libftam.a
		-rm -f $@
		cp libftam.a $@
		@$(UTILDIR)make-lib.sh $(SYSTEM) $@ -ranlib
		-@ls -gls $@
		-@echo ""

$(LINTDIR)llib-lftam:	llib-lftam
		- if [ -f $@ ]; then cp $@ zllib-lftam; fi
		-rm -f $@
		sed -e 's%#include "\(.*\)"%#include "$(INCDIR)\1"%' \
			< llib-lftam | \
			sed -e 's%#include "/usr/include/\(.*\)"%#include <\1>%' > $@
		@$(UTILDIR)inst-lint.sh $(SYSTEM) $(OPTIONS) $@
		-@ls -gls $@ $@.ln
		-@echo ""

inst-headers:;	@cd $(TOPDIR)h; $(MAKE) inst-ftam
		@for h in $(HEADERS); do $(MAKE) TARGET=$$h inst-target; done

inst-target:	$(PEPSYDIR)$(TARGET)

$(PEPSYDIR)$(TARGET):	$(TARGET)
		- if [ -f $@ ]; then cp $@ z$(TARGET); fi
		cp $(TARGET) $@
		-@ls -gls $@
		-@echo ""

libftam:	libftam.a true
		-@rm -f $(TOPDIR)h/CONT*.h $(TOPDIR)h/FADU*.h \
			$(TOPDIR)h/FTAM*.h
		-@$(LN) CONT-types.h $(TOPDIR)h/CONT-types.h
		-@$(LN) FADU-types.h $(TOPDIR)h/FADU-types.h 
		-@$(LN) FTAM-types.h $(TOPDIR)h/FTAM-types.h

libftam.a:	ftamvrsn.o
		-rm -f $@
		@$(UTILDIR)make-lib.sh $(SYSTEM) $(ARFLAGS) $@ $(OFILES) \
			CONT_tables.o FADU_tables.o FTAM_tables.o ftamvrsn.o
		-@rm -f $(TOPDIR)libftam.a $(TOPDIR)llib-lftam
		-@$(LN) libftam.a $(TOPDIR)libftam.a
		-@$(LN) llib-lftam $(TOPDIR)llib-lftam
		-@ls -l $@
		-@echo "FTAM library built normally"

FTAM_tables.o:	FTAM_tables.c FTAM-types.h CONT-types.h FADU-types.h

FTAM_tables.c FTAM-types.h: ftam.py $(TOPDIR)pepsy/xpepsy
		$(TOPDIR)pepsy/xpepsy -A -f -h -m ftam.py

FADU_tables.o:	FADU_tables.c FADU-types.h 

FADU_tables.c FADU-types.h: fadu.py $(TOPDIR)pepsy/xpepsy
		$(TOPDIR)pepsy/xpepsy -A -f -h -m fadu.py

CONT_tables.o:	CONT_tables.c CONT-types.h 

CONT_tables.c CONT-types.h: cont.py $(TOPDIR)pepsy/xpepsy
		$(TOPDIR)pepsy/xpepsy -A -f -h -m cont.py


ftamvrsn.c:	$(OFILES) CONT_tables.o FADU_tables.o FTAM_tables.o
		@$(UTILDIR)version.sh ftam > $@

l-libftam:	CONT_tables.c FADU_tables.c FTAM_tables.c true
		$(LINT) $(LFLAGS) $(CFILES) CONT_tables.c FADU_tables.c \
			FTAM_tables.c ftamvrsn.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

ftamprovider.o:	$(HXFILES)
ftamerror.o:	$(HFILES)
ftaminitiate.o:	$(HXFILES)
ftamrespond.o:	$(HXFILES)
ftamrelease1.o:	$(HXFILES)
ftamrelease2.o:	$(HXFILES)
ftamabort.o:	$(HXFILES)
ftamgroup1.o:	$(HXFILES)
ftamgroup2.o:	$(HXFILES)
ftamaccess1.o:	$(HXFILES)
ftamaccess2.o:	$(HXFILES)
ftambulk.o:	$(HXFILES)
ftambulk1.o:	$(HXFILES)
ftambulk2.o:	$(HXFILES)
ftamselect.o:	$(HXFILES)
ftamtrace.o:	$(HXFILES)
ftamacl.o:	$(HXFILES)
ftambits.o:	$(HXFILES)
ftamattr.o:	$(HXFILES)
ftamchrg.o:	$(HXFILES)
ftamconctl.o:	$(HXFILES)
ftamdiag.o:	$(HXFILES)
ftamfaduid.o:	$(HXFILES)
ftampass.o:	$(HXFILES)
ftamshare.o:	$(HXFILES)
ftamlose.o:	$(HXFILES)
ftamdocument.o:	$(HFILES) $(HDIR)tailor.h $(HDIR)logger.h
ftamfdf.o:	$(HXFILES)


################################################################
# documents
################################################################

documents:	$(ETCDIR)isodocuments

$(ETCDIR)isodocuments:	isodocuments
		- if [ -f $@ ]; then cp $@ zisodocuments; fi
		cp isodocuments $@
		-@ls -gls $@
		-@echo ""


##################################################################
# manual pages
##################################################################

MANUALS	=	libftam.3n isodocuments.5

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


##################################################################
# clean
##################################################################

clean:;		rm -f *.o *.a *.ph CONT* FADU* FTAM* z* _* core  \
			ftamvrsn.c \
			$(TOPDIR)h/CONT*.h $(TOPDIR)h/FADU*.h \
			$(TOPDIR)h/FTAM*.h

grind:;		iprint Makefile isodocuments
		tgrind -lc $(CFILES) ftamvrsn.c llib-lftam
		tgrind -lpepy -d $(TOPDIR)pepy/grindefs $(PYFILES)
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve

true:;
