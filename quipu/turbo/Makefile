###############################################################################
#   Instructions to Make, for compilation of ISODE turbo options
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



CFILES= edb2dbm.c edbcat.c tdirname.c file2dn.c

all:		file2dn tdirname edb2dbm edbcat 

edb2dbm:	edb2dbm.o
	$(LDCC) $(LDFLAGS) -o $@ $? $(LIBGDBM)

edbcat:		edbcat.o
	$(LDCC) $(LDFLAGS) -o $@ $? $(LIBGDBM)

tdirname:	tdirname.o
	$(LDCC) $(LDFLAGS) -o $@ $?

file2dn:	file2dn.o
	$(LDCC) $(LDFLAGS) -o $@ $?

tree2dbm:

syncedb:

synctree:

install:	inst-all clean

inst-all:	all
	install -c -m 755 edb2dbm $(SBINDIR)edb2dbm
	install -c -m 755 edbcat $(SBINDIR)edbcat
	install -c -m 755 tree2dbm $(SBINDIR)tree2dbm
	install -c -m 755 syncedb $(SBINDIR)syncedb
	install -c -m 755 synctree $(SBINDIR)synctree
	install -c -m 755 tdirname $(SBINDIR)tdirname
	install -c -m 755 file2dn $(SBINDIR)file2dn

lint:
	$(LINT) $(LFLAGS) edb2dbm.c $(LLIBS)
	$(LINT) $(LFLAGS) edbcat.c $(LLIBS)
	$(LINT) $(LFLAGS) tdirname.c $(LLIBS)
	$(LINT) $(LFLAGS) file2dn.c $(LLIBS)

clean:
	rm -f *.o a.out edb2dbm edbcat tdirname file2dn
