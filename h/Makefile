###############################################################################
#   Instructions to Make, for compilation of ISODE header files
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




DIRS	=	quipu

HEADERS	=	config.h \
		sys.dirent.h usr.dirent.h \
		general.h manifest.h isoaddrs.h sys.file.h \
		internet.h x25.h tp4.h dgram.h \
		cmd_srch.h isoservent.h logger.h tailor.h \
		mpkt.h \
		tpkt.h tsap.h \
		spkt.h ssap.h \
		pepsy.h \
		ppkt.h psap2.h psap.h \
		acpkt.h acsap.h \
		rtpkt.h rtsap.h \
		ropkt.h rosap.h \
		ronot.h \
		rosy.h \
		osisec-stub.h

FHEADERS=	fpkt.h ftam.h

# Don't install by default...
DEMSA=		demsa.h demsb.h 



all:;

inst-all:;	-mkdir $(INCDIRM)
		@for h in $(HEADERS); do $(MAKE) TARGET=$$h inst-target; done

inst-ftam:;	@for h in $(FHEADERS); do $(MAKE) TARGET=$$h inst-target; done

inst-target:	$(INCDIR)$(TARGET)

$(INCDIR)$(TARGET):	$(TARGET)
		- if [ -f $@ ]; then cp $@ z$(TARGET); fi
		sed -e 's%#include "\([^/]*\)"%#include "$(INCDIR)\1"%' \
			< $(TARGET) | \
			sed -e 's%#include "/usr/include/\(.*\)"%#include <\1>%' > $@
		-@ls -gls $@
		-@echo ""

install:	inst-all clean

lint:;

clean:;		rm -f z* _*
		@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) clean"; \
			      cd $$i; $(MAKE) clean); \
		    done

grind:;		tgrind -lc $(HEADERS)
		@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) grind"; \
			      cd $$i; $(MAKE) grind); \
		    done
