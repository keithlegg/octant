#this is not done - I am thinking about the simplest possible build system 


SUBDIRS = testport rotate_head home_axis
TOPTARGETS = all clean

$(TOPTARGETS): $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(SUBDIRS) $(TOPTARGETS)

#SUBDIRS := $(wildcard */.)
#all: $(SUBDIRS)
#$(SUBDIRS):
#		$(MAKE) -C $@
#.PHONY: all $(SUBDIRS)

 

 
