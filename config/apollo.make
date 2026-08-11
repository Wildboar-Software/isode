###############################################################################
#   Instructions to Make, for compilation of ISODE processes for Apollo
###############################################################################

###############################################################################
#
# 
#
# Contributed by John Brezak, Apollo Computer, Inc.
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

TOPDIR	=	/isode/
HDIR	=	$(TOPDIR)h/
UTILDIR	=	$(TOPDIR)util/
BINDIR	=	/isode/bin/
SBINDIR	=	/isode/etc/
ETCDIR	=	/isode/etc/
LOGDIR	=	/isode/logs/
MANDIR  =       /isode/man/
INCDIRM	=	/usr/include/isode
INCDIR	=	$(INCDIRM)/
PEPYDIRM=	$(INCDIR)pepy
PEPYDIR	=	$(PEPYDIRM)/
PEPSYDIRM=	$(INCDIR)pepsy
PEPSYDIR=	$(PEPSYDIRM)/
LIBDIR	=	/isode/lib/
LINTDIR	=	/isode/lib/lint/

LIBISODE=	$(TOPDIR)libisode.a
LIBDSAP	=	$(TOPDIR)libdsap.a

SYSTEM	=	-bsd42
MANDIR	=	/usr/man/
MANOPTS	=	-bsd42

OPTIONS	=	-I/isode/h -A systype,any -A cpu,3000 $(PEPYPATH) $(KRBOPT)


###############################################################################
# Programs and Libraries
###############################################################################

MAKE	=	./make DESTDIR=$(DESTDIR) $(MFLAGS) -k
SHELL	=	/bin/sh

CC      =	cc
CFLAGS	=	      $(OPTIONS)
LIBCFLAGS=	      $(CFLAGS)
LINT    =	lint
LFLAGS  =	-bhuz $(OPTIONS)
LD	=	ld
LDCC	=	$(CC)
LDFLAGS =	#-s
ARFLAGS	=

LN	=	ln

LSOCKET	=	$(KRBLIB)


###############################################################################
# Generation Rules for library modules
###############################################################################

.c.o:;		$(CC) $(LIBCFLAGS) -c $*.c
