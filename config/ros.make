###############################################################################
#   Instructions to Make, for compilation of ISODE processes for ROS
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

OPTIONS	=	-I. -I$(TOPDIR)h -I/usr/4.2/include $(PEPYPATH) $(KRBOPT)

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
LINTDIR	=	/usr/4.2/lib/lint/

LIBISODE=	$(TOPDIR)libisode.a
LIBDSAP	=	$(TOPDIR)libdsap.a

SYSTEM	=	-ros
MANDIR	=	/usr/man/
MANOPTS	=	-ros


###############################################################################
# Programs and Libraries
###############################################################################

MAKE	=	./make DESTDIR=$(DESTDIR) $(MFLAGS) -k
SHELL	=	/bin/sh

CC      =	rc
CFLAGS  =	      $(OPTIONS)
LIBCFLAGS=	      $(CFLAGS)
LINT    =	lint
LFLAGS  =	-bhu  $(OPTIONS)
LD	=	ld
LDCC	=	$(LD)
LDFLAGS =	-V 5 -s /usr/4.2/lib/crt0.o
ARFLAGS	=

LN	=	ln

# get 4.2BSD libc
LSOCKET	=	/usr/4.2/lib/libc.a $(KRBLIB)


###############################################################################
# Generation Rules for library modules
###############################################################################

.c.o:;		$(CC) $(LIBCFLAGS) -c $*.c
