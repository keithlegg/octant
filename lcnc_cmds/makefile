#this is not done - I am thinking about the simplest possible build system 



SUBDIRS = testport home_axis 

SUBDIRSCLEAN=$(addsuffix clean,$(SUBDIRS))




all:
	# debug - work in progress
	# run "make build" to build all commands 
 

build:
	$(MAKE) -C testport  


clean: $(SUBDIRSCLEAN)

clean_curdir:
	#rm -fv *.o *~ core .depend .*.cmd *.ko *.mod.c
	$(MAKE) clean 

%clean: %
	$(MAKE) -C $< -f $(PWD)/makefile clean_curdir
