###############################################################################
#   Instructions to Make, for compilation of ISODE processes for HP-UX
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

OPTIONS	=	-I. -I$(TOPDIR)h $(PEPYPATH) $(KRBOPT)

HDIR	=	$(TOPDIR)h/
UTILDIR	=	$(TOPDIR)util/
BINDIR	=	/usr/local/bin/
SBINDIR	=	/usr/etc/
ETCDIR	=	/usr/etc/
LOGDIR	=	/usr/tmp/
INCDIRM	=	/usr/include/isode
INCDIR	=	$(INCDIRM)/
PEPYDIRM=	$(INCDIR)pepy
PEPYDIR	=	$(PEPYDIRM)/
PEPSYDIRM=	$(INCDIR)pepsy
PEPSYDIR=	$(PEPSYDIRM)/
LIBDIR	=	/usr/lib/
LINTDIR	=	/usr/lib/lint/

LIBISODE=	$(TOPDIR)libisode.a
LIBDSAP	=	$(TOPDIR)libdsap.a

SYSTEM	=	-sys5
MANDIR	=	/usr/man/
MANOPTS	=	-hpux


###############################################################################
# Programs and Libraries
###############################################################################

MAKE	=	./make DESTDIR=$(DESTDIR) $(MFLAGS) -k
SHELL	=	/bin/sh

CC      =	cc
#
# You may need to add +Ns2000 +Nd3000 to CFLAGS to
# make the compiler use larger tables on the Series 300.
# The Pre-Processor symbol table needs expanding for some programs.
# add the following to CLFAGS to expand the tables to 256000 bytes...
#      -W p,-H256000
#
CFLAGS  =	-O    $(OPTIONS)
LIBCFLAGS=	      $(CFLAGS)
LINT    =	lint
LFLAGS  =	-bhuz $(OPTIONS)
LD	=	ld
LDCC	=	$(CC)
LDFLAGS =	-ns
ARFLAGS	=

LN	=	ln

# TCP/IP is native to HP-UX, but we still need a non-standard library
LSOCKET	=	-lbsdipc $(KRBLIB)


###############################################################################
# Generation Rules for library modules
###############################################################################

.c.o:;		$(CC) $(LIBCFLAGS) -c $*.c
