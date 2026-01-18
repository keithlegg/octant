#this is not done - I am thinking about the simplest possible build system 



all:
	# debug - work in progress
	# the "official" build is go to /pulser dir and type make 

#debug - not working yet 
#py:
#	python setupbuild.py /hard/coded/pat 	  

build:
	$(MAKE) -C pulser  


#this does not work 
.PHONY: clean
clean:
	$(MAKE)  
