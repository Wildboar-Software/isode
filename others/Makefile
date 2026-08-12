###############################################################################
#   Instructions to Make, for compilation of other ISODE programs
###############################################################################


DIRS	=	quipu/photo quipu/image mosy tsbridge

ALL	=	callback idist listen lookup ntp osilookup pingpong quipu 

OTHERS	=	$(ALL) rfa rtf cl

all:;		@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) all"; \
			      cd $$i; $(MAKE) all); \
		    done
		cd quipu/photo; $(MAKE) Xphoto xfax
		cd quipu; $(MAKE) pilot

everything:;	@for i in $(DIRS) $(ALL); \
		    do (echo "cd $$i; $(MAKE) all"; \
			      cd $$i; $(MAKE) all); \
		    done
		cd quipu/photo; $(MAKE) Xphoto xfax
		cd quipu; $(MAKE) pilot

inst-all:;	@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) inst-all"; \
			      cd $$i; $(MAKE) inst-all); \
		    done
		cd quipu; $(MAKE) inst-pilot

install:;	@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) install"; \
			      cd $$i; $(MAKE) install); \
		    done

lint:;		@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) lint"; \
			      cd $$i; $(MAKE) lint); \
		    done

lint-everything:;	@for i in $(DIRS) $(ALL); \
		    do (echo "cd $$i; $(MAKE) lint"; \
			      cd $$i; $(MAKE) lint); \
		    done

clean:;		rm -f _*
		@for i in $(DIRS) $(OTHERS); \
		    do (echo "cd $$i; $(MAKE) clean"; \
			      cd $$i; $(MAKE) clean); \
		    done

grind:;		@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) grind"; \
			      cd $$i; $(MAKE) grind); \
		    done
