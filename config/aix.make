###############################################################################
#   Instructions to Make, for compilation of ISODE processes for
#		 IBM AIX 3.2 on RS6000 Workstation
#
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

OPTIONS	=	-I. -I$(TOPDIR)h -D_NO_PROTO $(PEPYPATH) $(KRBOPT)

HDIR	=	$(TOPDIR)h/
UTILDIR	=	$(TOPDIR)util/
BINDIR	=	/usr/bin/
SBINDIR	=	/usr/sbin/
ETCDIR	=	/etc/
LOGDIR	=	/tmp/
INCDIRM	=	/usr/include/isode
INCDIR	=	$(INCDIRM)/
PEPYDIRM=	$(INCDIR)pepy
PEPYDIR	=	$(PEPYDIRM)/
PEPSYDIRM=	$(INCDIR)pepsy
PEPSYDIR=	$(PEPSYDIRM)/
LIBDIR	=	/usr/lib/
LINTDIR	=	/usr/lib/

LIBISODE=	$(TOPDIR)libisode.a
LIBDSAP	=	$(TOPDIR)libdsap.a

SYSTEM	=	-bsd42
MANDIR	=	/usr/share/man/
MANOPTS	=	-aix

###############################################################################
# Programs and Libraries
###############################################################################

MAKE	=	./make DESTDIR=$(DESTDIR) $(MFLAGS) -k
SHELL	=	/bin/sh

CC      =	cc	
CFLAGS  =	$(OPTIONS)
LIBCFLAGS=	      $(CFLAGS)
# Need to sort lint out (somehow)
LINT    =	echo lint
LFLAGS  =	-bhu $(OPTIONS)
LD	=	ld
LDCC	=	$(CC)
LDFLAGS =	-s
ARFLAGS	=

LN	=	ln

# Add -lxti to LSOCKET if you have the OSIMF package - see aix.h
#XTILIB	=	-lxti
XTILIB	= 

LSOCKET	=	$(XTILIB) $(KRBLIB)

###############################################################################
# Generation Rules for library modules
###############################################################################

.c.o:;		$(CC) $(LIBCFLAGS) -c $*.c

