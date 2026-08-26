###############################################################################
#   Instructions to Make, for compilation of ISODE processes for LINUX
###############################################################################

###############################################################################
# Options
###############################################################################

#TARGET  =	-m32
# Extra -Werror=* flags compiled cleanly under ./make everything.
# Noisy historic diagnostics (type-limits, unused-*, format, empty-body,
# implicit-fallthrough, parentheses, discarded-qualifiers, ...) are omitted.
OPTIONS +=	-std=gnu17 \
		-fdiagnostics-format=sarif-stderr \
		-Werror=conversion \
		-Werror=sign-conversion \
		-Werror=missing-prototypes \
		-Werror=cast-function-type \
		-Werror=shadow \
		-Werror=old-style-definition \
		-Werror=strict-prototypes \
		-Werror=pointer-sign \
		-Werror=pedantic \
		-Werror=absolute-value \
		-Werror=alloc-zero \
		-Werror=alloca \
		-Werror=array-bounds \
		-Werror=array-compare \
		-Werror=array-parameter \
		-Werror=bool-compare \
		-Werror=bool-operation \
		-Werror=cast-align \
		-Werror=date-time \
		-Werror=duplicate-decl-specifier \
		-Werror=duplicated-cond \
		-Werror=enum-compare \
		-Werror=enum-conversion \
		-Werror=enum-int-mismatch \
		-Werror=format-signedness \
		-Werror=frame-address \
		-Werror=ignored-qualifiers \
		-Werror=infinite-recursion \
		-Werror=init-self \
		-Werror=int-in-bool-context \
		-Werror=logical-not-parentheses \
		-Werror=logical-op \
		-Werror=main \
		-Werror=memset-elt-size \
		-Werror=memset-transposed-args \
		-Werror=missing-attributes \
		-Werror=missing-declarations \
		-Werror=missing-parameter-type \
		-Werror=multistatement-macros \
		-Werror=nonnull \
		-Werror=nonnull-compare \
		-Werror=null-dereference \
		-Werror=old-style-declaration \
		-Werror=openmp-simd \
		-Werror=override-init \
		-Werror=packed \
		-Werror=packed-not-aligned \
		-Werror=pointer-arith \
		-Werror=restrict \
		-Werror=shift-negative-value \
		-Werror=shift-overflow \
		-Werror=sizeof-array-argument \
		-Werror=sizeof-array-div \
		-Werror=sizeof-pointer-div \
		-Werror=sizeof-pointer-memaccess \
		-Werror=string-compare \
		-Werror=stringop-overread \
		-Werror=stringop-truncation \
		-Werror=switch \
		-Werror=trampolines \
		-Werror=unknown-pragmas \
		-Werror=unused-label \
		-Werror=variadic-macros \
		-Werror=vla \
		-Werror=vla-parameter \
		-Werror=volatile-register-var \
		-Werror=write-strings \
		-Werror=xor-used-as-pow \
		-Werror=zero-length-bounds \
		-g -I. -I$(TOPDIR)h $(PEPYPATH) $(KRBOPT)
#OPTIONS	+=	$(TARGET) -fno-inline -fno-omit-frame-pointer

HDIR	=	$(TOPDIR)h/
UTILDIR	=	$(TOPDIR)util/
BINDIR	=	/usr/local/bin/
SBINDIR	=	/usr/local/sbin/
ETCDIR	=	/usr/local/etc/isode/
LOGDIR	=	/tmp/isodelog/
INCDIRM	=	/usr/local/include/isode
INCDIR	=	$(INCDIRM)/
PEPYDIRM=	$(INCDIR)pepy
PEPYDIR	=	$(PEPYDIRM)/
PEPSYDIRM=	$(INCDIR)pepsy
PEPSYDIR=	$(PEPSYDIRM)/
LIBDIR	=	/usr/local/lib/

LINTDIR	=	/usr/local/lib/lint/

LIBISODE=	$(TOPDIR)libisode.a
LIBDSAP	=	$(TOPDIR)libdsap.a

SYSTEM	=	-bsd42
MANDIR	=	/usr/local/share/man/
MANOPTS	=	-bsd42


###############################################################################
# Shared libraries
###############################################################################

#    You can build a shared version of the ISODE library on suns under
#    SunOS 4.0 or greater. For non sparc based architecture, any
#    release greater than 4.0 will do. However, due to a bug in the
#    compiler and the size of ISODE, you will not be able to build a
#    shared ISODE unless you have SunOS 4.1 or greater.

#    First, comment out the definitions of LIBISODE and LIBDSAP above

#    Second, uncomment these three lines:

#SHAREDLIB=	shared
#LIBISODE=	-L$(TOPDIR) -lisode
#LIBDSAP=	-L$(TOPDIR) -ldsap

#    If you are not installing the libraries in the standard place
#    (/usr/lib or /usr/local/lib) you should add a "-L$(LIBDIR)" to
#    the above two lines.

#    Third, add
#		-pic		Sun 3
#		-PIC		Sparc
#    to LIBCFLAGS below

#    Finally, remove
#		-ld -x -r $@
#		mv a.out $@
#    from the .c.o rule below.

#    Having compiled and installed ISODE, you may need to run
#    /usr/etc/ldconifg to get the system to pick up the new
#    shared libraries.

###############################################################################
# Programs and Libraries
###############################################################################

MAKE	=	./make DESTDIR=$(DESTDIR) $(MFLAGS) -k
SHELL	=	/bin/sh

LEX	=	lex
YACC	=	bison -y
CC      =	cc
LD	=	ld
CFLAGS  =       $(OPTIONS) -DPEPSY_REALS
LIBCFLAGS=      $(CFLAGS)
LINT    =	lint
LFLAGS  =	$(OPTIONS)
LDCC	=	$(CC)
LDFLAGS =	$(TARGET)	
ARFLAGS	=	

LN	=	ln

LSOCKET	=	$(KRBLIB) -lm


###############################################################################
# Generation Rules for library modules
###############################################################################

.c.o:;		$(CC) $(LIBCFLAGS) -c $*.c
