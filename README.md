# FlowState - Multi axis motion control   
   
"If you're thinking of writing your own controller software and hoping it's more reliable than open source or commercially available products, you're truly out to lunch."

![alt text](https://github.com/keithlegg/octant_cnc/blob/main/images/screen.jpg) 
![alt text](https://github.com/keithlegg/octant_cnc/blob/main/images/head_normals.jpg) 
 

<br>
## DISCLAIMER
  
<br>

```

WORDS OF WISDOM FROM LINUXCNC: 

Ｔｈｅ ａｕｔｈｏｒｓ ｏｆ ｔｈｉｓ ｓｏｆｔｗａｒｅ ａｃｃｅｐｔ
ａｂｓｏｌｕｔｅｌｙ ｎｏ ｌｉａｂｉｌｉｔｙ ｆｏｒ ａｎｙ
ｈａｒｍ　ｏｒ ｌｏｓｓ ｒｅｓｕｌｔｉｎｇ ｆｒｏｍ ｉｔｓ ｕｓｅ．

Ｉｔ ｉｓ ＥＸＴＲＥＭＥＬＹ ｕｎｗｉｓｅ ｔｏ　ｒｅｌｙ
ｏｎ ｓｏｆｔｗａｒｅ ａｌｏｎｅ ｆｏｒ ｓａｆｅｔｙ．

Any machinery capable of harming persons must have
provisions for completely removing power from all
motors, etc., before persons enter any danger area.

All machinery must be designed to comply with local 
and national safety codes, and the authors of this 
software cannot and do not, take any responsibility 
for such compliance.
  
```
<br>


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







