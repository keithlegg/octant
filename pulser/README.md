---------
Overall structure 
   -
---------

Vectors.h
   -
   - math library (in header, will expand in the future) 

Matrices.cpp 
   -
   - Math library (will expand in the future)

point_op.cpp 
   -
   - geometry library 

timer.cpp 
   -
   - keeps track of time, drive anything that changes 

pylink.cpp 
   -
   (unused, future ) bridge to python and external tools 


 
obj_fio.cpp 
   -
   (obj_model.h) loads and saves data from disk   

obj_geom.cpp 
   -
   (obj_model.h) structure of 3d model geometry 

obj_model.cpp 
   -
   (obj_model.h) structure of 3d model - everything else, utility, etc     


globals.cpp
   -
   - loads and saves configuration files, stores parameters of machine 

io.cpp 
   -
   - placeholder for futuer IO interfaces like Mesa 5i25, etc 
     for now there is only a parallel port 

parport.cpp 
   -
   - interfaces to the parallel port(s)

plot.cpp 
   -
   - calculates motion and stores machine parameters
  
octant.cpp 
   -
   - top level program - can be run with or without GL gui 

main.cpp 
   -
   - entry point for the program 

-------------
OPTIONAL GUI LAYER 
 
framebuffer.cpp / BMP.h 
   -
   - relic from the past - load and save images


gl_gui.cpp 
   -

gl_setup.cpp
   -

gl_render.cpp
   -

parse_cmds.cpp
   -






