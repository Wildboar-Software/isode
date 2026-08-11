###############################################################################
#    Instructions to Make, for compilation of ISODE processes for 
#                                                             Olivetti LSX 30xx
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

OPTIONS	=	-I. -I$(TOPDIR)h -I$(TOPDIR)hstubssap $(PEPYPATH) -DDEBUG $(KRBOPT)
PROOF	=	/usr2/proof/ISODE-6.7b/

HDIR    =       $(TOPDIR)h/
UTILDIR =       $(TOPDIR)util/
BINDIR  =       $(PROOF)bin/
SBINDIR =       $(PROOF)etc/
ETCDIR  =       $(PROOF)etc/
LOGDIR	=	/usr/tmp/
INCDIRM =       $(PROOF)src/include/isode
INCDIR  =       $(INCDIRM)/
LOCINC	=	$(TOPDIR)hstubssap/
PEPYDIRM=       $(INCDIR)pepy
PEPYDIR =       $(PEPYDIRM)/
PEPSYDIRM=	$(INCDIR)pepsy
PEPSYDIR=	$(PEPSYDIRM)/
LIBDIR  =       $(PROOF)src/lib/
LINTDIR =       $(PROOF)src/lib/

LIBISODE=	$(TOPDIR)libisode.a
LIBDSAP=	$(TOPDIR)libdsap.a
 
SYSTEM  =       -bsd42
MANOPTS =       -bsd42


###############################################################################
# Programs and Libraries
###############################################################################

MAKE    =       ./make DESTDIR=$(DESTDIR) $(MFLAGS) -k
SHELL   =       /bin/sh

CC      =       cc
CFLAGS  =       -O    $(OPTIONS)
LIBCFLAGS=      $(CFLAGS)
LINT    =       lint
LFLAGS  =       -bhuz $(OPTIONS)
LD      =       ld
LDCC    =       $(CC)
LDFLAGS =       -s
ARFLAGS =

LN      =       ln

LSOCKET =	$(KRBLIB)


###############################################################################
# Generation Rules for library modules
###############################################################################

.c.o:;          $(CC) $(LIBCFLAGS) -c $*.c
		-ld -x -r $@
		mv a.out $@
