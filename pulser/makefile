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
#CFLAGS   = -I$(INCDIR) -Wall   
CFLAGS   = -I$(INCDIR) -w  

LINKER   = g++

# linking flags 

#Linux
ifeq ($(UNAME), Linux)
	LFLAGS   = -g -Wall -lglut -lGL -lGLU -lX11  -lm -lz 
endif

#OSX
ifeq ($(UNAME), Darwin)
	LFLAGS   = -Wall -lm -lz -framework Glut -framework OpenGL
endif



SOURCES  := $(wildcard $(SRCDIR)/*.c)

OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f


$(TARGET): $(OBJECTS)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
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
	 
