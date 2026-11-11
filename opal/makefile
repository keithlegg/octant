#this is not done - I am thinking about the simplest possible build system 


SUBDIRS = set_byte set_pin flash_pin

##------------------

TOPTARGETS = all clean

$(TOPTARGETS): $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(SUBDIRS) $(TOPTARGETS)


 

 
