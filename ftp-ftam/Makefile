###############################################################################
#   Instructions to Make, for compilation of MITRE FTP/FTAM gateway
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/ftp-ftam/RCS/Makefile,v 9.0 1992/06/16 12:17:24 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:17:24  isode
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

.SUFFIXES:	.py .c .o

.c.o:;		$(CC) $(CFLAGS) -DBRIDGE -I$(FTAM) -c $*.c


###############################################################################
# Programs and Libraries
###############################################################################

LIBD	=	$(TOPDIR)libftam.a $(TOPDIR)libisode.a
LIBES	=	$(TOPDIR)libftam.a $(LIBISODE)
LLIBS	=	$(TOPDIR)llib-lftam $(TOPDIR)llib-lisode


###############################################################################
# Files
###############################################################################

FTAM	=	$(TOPDIR)ftam2/
HFILES	=	$(FTAM)ftamuser.h $(FTAM)ftamsbr.h $(FTAM)DOCS-types.h
CFILES	=	$(FTAM)ftam.c $(FTAM)ftamuser.c $(FTAM)ftam-assoc.c \
			$(FTAM)ftam-dir.c $(FTAM)ftam-get.c \
			$(FTAM)ftam-glob.c $(FTAM)ftam-ls.c \
			$(FTAM)ftam-mgmt.c $(FTAM)ftam-put.c $(FTAM)ftamsbr.c
OFILES	=	ftam.o ftamuser.o ftam-assoc.o ftam-dir.o ftam-get.o \
			ftam-glob.o ftam-ls.o ftam-mgmt.o ftam-put.o \
			ftamsbr.o
DOCS-O	=	$(FTAM)DOCS_tables.o
DOCS-C	=	$(FTAM)DOCS_tables.c


##################################################################
# Here it is...
##################################################################

all:		ftpd-ftam
inst-all:	inst-ftpd-ftam manuals
install:	inst-all clean
lint:		l-ftpd-ftam


##################################################################
# ftpd-ftam
##################################################################

inst-ftpd-ftam:	$(SBINDIR)in.ftpd-ftam

$(SBINDIR)in.ftpd-ftam:	xftpd-ftam
		- if [ -f $@ ]; then cp $@ zxftpd-ftam; fi
		-rm -f $@
		cp xftpd-ftam $@
		-@ls -gls $@
		-@echo ""

ftpd-ftam:	xftpd-ftam

xftpd-ftam:	ftpd.o ftpcmd.o vers.o $(OFILES) $(DOCS-O) $(LIBD)
		$(LDCC) $(LDFLAGS) -o $@ ftpd.o ftpcmd.o vers.o $(OFILES) \
			$(DOCS-O) $(LIBES) $(LSOCKET)

l-ftpd-ftam:	ftpcmd.c vers.c true
		$(LINT) $(LFLAGS) -DBRIDGE -I$(FTAM) ftpd.c ftpcmd.c \
			vers.c $(CFILES) $(DOCS-C) $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

vers.o:		vers.c
vers.c:		ftpd.c ftpcmd.y
		sh newvers.sh

ftam.o:		$(HFILES) $(FTAM)ftam.c
		$(CC) $(CFLAGS) -DBRIDGE  -I$(FTAM) -c $(FTAM)ftam.c

ftamuser.o:	$(HFILES) $(FTAM)ftamuser.c
		$(CC) $(CFLAGS) -DBRIDGE  -I$(FTAM) -c $(FTAM)ftamuser.c

ftam-assoc.o:	$(HFILES) $(FTAM)ftam-assoc.c
		$(CC) $(CFLAGS) -DBRIDGE  -I$(FTAM) -c $(FTAM)ftam-assoc.c

ftam-dir.o:	$(HFILES) $(FTAM)ftam-dir.c
		$(CC) $(CFLAGS) -DBRIDGE  -I$(FTAM) -c $(FTAM)ftam-dir.c

ftam-get.o:	$(HFILES) $(FTAM)ftam-get.c
		$(CC) $(CFLAGS) -DBRIDGE  -I$(FTAM) -c $(FTAM)ftam-get.c

ftam-glob.o:	$(HFILES) $(FTAM)ftam-glob.c
		$(CC) $(CFLAGS) -DBRIDGE  -I$(FTAM) -c $(FTAM)ftam-glob.c

ftam-ls.o:	$(HFILES) $(FTAM)ftam-ls.c
		$(CC) $(CFLAGS) -DBRIDGE  -I$(FTAM) -c $(FTAM)ftam-ls.c

ftam-mgmt.o:	$(HFILES) $(FTAM)ftam-mgmt.c
		$(CC) $(CFLAGS) -DBRIDGE  -I$(FTAM) -c $(FTAM)ftam-mgmt.c

ftam-put.o:	$(HFILES) $(FTAM)ftam-put.c
		$(CC) $(CFLAGS) -DBRIDGE  -I$(FTAM) -c $(FTAM)ftam-put.c

ftamsbr.o:	$(HFILES) $(FTAM)ftamsbr.c
		$(CC) $(CFLAGS) -DBRIDGE  -I$(FTAM) -c $(FTAM)ftamsbr.c


#################################################################
# manual pages
##################################################################

MANUALS	=	ftpd-ftam.8c

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


##################################################################
# clean
##################################################################

clean:;		rm -f *.o ftpcmd.c y.tab.h x* z* _* core vers.c

grind:		ftpcmd.c vers.c true
		iprint Makefile
		tgrind -lc $(HFILES) ftpd.c ftpcmd.c vers.c
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve

true:;
