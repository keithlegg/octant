#this is not done - I am thinking about the simplest possible build system 


#SUBDIRS = testport test_coms rotate_head home_axis pwm_pump

SUBDIRS = testport pumpctrl

##------------------




TOPTARGETS = all clean

$(TOPTARGETS): $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(SUBDIRS) $(TOPTARGETS)


 

 
