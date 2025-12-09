TARGET = pulser

INCDIR = inc
SRCDIR = src
OBJDIR = obj
BINDIR = bin


#CC       = g++ -D_ISLINUX 
CC       = g++ 


#detect the OS (linux or OSX, dont care about windows)
UNAME := $(shell uname)



# compiling flags  

#all the warnings (for final compiling)
#CFLAGS   = -I$(INCDIR) -Wall 

#for just making this damn thing run 
CFLAGS   = -I$(INCDIR) -w -fcompare-debug-second    


LINKER   = g++

# linking flags 

#Linux
ifeq ($(UNAME), Linux)

	#LFLAGS   = -std=c++17  

	#SHOW ME ALL THE WARNINGS 
	#LFLAGS = -Wall -lglut -lGL -lGLU 

	#NO WARNINGS 
	#LFLAGS =  -w -lglut -lGL -lGLU 
	
	LFLAGS = -g -w -lglut -lGL -lGLU 


endif

# #OSX
# ifeq ($(UNAME), Darwin)
# 	LFLAGS   = -Wall -lm -lz -framework Glut -framework OpenGL
# endif



SOURCES  := $(wildcard $(SRCDIR)/*.cpp)

OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
rm       = rm -f


$(TARGET): $(OBJECTS)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"


.PHONY: cleanimg
cleanimg:
	@$(rm) *.bmp
	@echo "image cleanup done"


.PHONY: clean
clean:
	@$(rm) $(TARGET)
	@$(rm) $(OBJECTS)
	@echo "cleanup done"


.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@$(rm) $(OBJDIR)/*.o
	@echo "Executable removed!"







