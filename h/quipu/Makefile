###############################################################################
#   Instructions to Make, for compilation of ISODE QUIPU include files
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/h/quipu/RCS/Makefile,v 9.0 1992/06/16 12:23:11 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:23:11  isode
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


HEADERS =       abandon.h add.h attr.h attrvalue.h bind.h \
		common.h commonarg.h compare.h config.h \
		connection.h ds_error.h dsargument.h dsp.h \
		dua.h authen.h entry.h list.h  modify.h \
		modifyrdn.h name.h oid.h read.h remove.h \
		ds_search.h util.h sequence.h dap.h photo.h malloc.h \
		ufn.h nrs_info.h dap2.h dsap.h policy.h turbo.h \
		syntaxes.h

###################################################################
# Here it is...
###################################################################

all:;

inst-all:;      -mkdir $(INCDIR)quipu
		@for h in $(HEADERS); \
			do $(MAKE) DIR=$(INCDIR)quipu TARGET=$$h inst-target; \
			done

inst-target:    $(DIR)/$(TARGET)

$(DIR)/$(TARGET):       $(TARGET)
		- if [ -f $@ ]; then cp $@ z$(TARGET); fi
		sed -e 's%#include "\(.*\)"%#include "$(INCDIR)\1"%' \
			< $(TARGET) | \
			sed -e 's%#include "/usr/include/\(.*\)"%#include <\1>%' > $@
		-@ls -gls $@
		-@echo ""

install:        inst-all clean

lint:;

clean:;         rm -f z* _* *.orig

grind:;         tgrind -lc $(HEADERS)
