/*****************************************************************************/
/* 
    gl_util.cpp
    
    A place for all anything GUI related without a home 

 
   
    ------------------------------------------- 

    MIT License

    Copyright (c) 2026 Keith Legg - keithlegg23@gmail.com

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.


*/
         

/*****************************************************************************/

#include <sstream>
#include <vector>
#include <iostream>
#include <string>
#include <cstring>
#include <cmath>
#include <unistd.h>  

#include "parport.h"
#include "plot.h"
#include "pylink.h"
#include "octant.h"

//GUI LAYER 
#if DO_BUILD_GUI == true
    #include "gl_util.h"
    #include "gl_setup.h"
    #include "gl_render.h"
#endif



extern std::vector<Vector3> point_buffer1;

extern cnc_parport parport;
extern cncglobals cg;

extern cnc_plot* pt_motionplot;
extern obj_model* pt_model_buffer;



/*************************************************************/
/*************************************************************/
/*
    load an OBJ - using two face indeces - ignore the 3rd face index, 
    dump the data to 3D lines back into main object buffer 
    uses a single one time scoped obj_model 

*/


void load_line_obj(std::string objfilepath)
{
    bool debug = true;

    char char_array[100];

    obj_model* pt_obj2d_loader  = new obj_model;
    
    //may not be needed on a new object?? DEBUG 
    pt_obj2d_loader->reset();
   
    strcpy(char_array, objfilepath.c_str()); 
    pt_obj2d_loader->load(char_array);
    
    //DEBUG - put this in object 3d as a class method ?
    if(debug)
    {
        std::cout << pt_obj2d_loader->num_lines << "\n";
    }

    //uint pcount = 0;
    for (uint x = 0;x<pt_obj2d_loader->num_lines;x++)
    {
        int pidx1 = pt_obj2d_loader->lines[x][0];
        int pidx2 = pt_obj2d_loader->lines[x][0];
        
        Vector3 pt1 = pt_obj2d_loader->points[pidx1];
        Vector3 pt2 = pt_obj2d_loader->points[pidx2];

        //copy the object3d vectors into a toolpath 
        pt_motionplot->add_file_vec(&pt1);
        pt_motionplot->add_file_vec(&pt2);

        //DEBUG need to fix add_new_tp_polygon() 
        //pt_motionplot->add_new_tp_polygon(pcount,2);
        //pcount++;
    }


    // I dont think this is needed, but hey
    delete pt_obj2d_loader;

    pt_motionplot->update_toolpaths();
}


/*************************************************************/

/*
   load an OBJ file ignore the 2nd and 3rd face index,
   store the data as "points" (not vertices) but a point buffer 
   represented in 3D space as multiple locators 
*/

//DEBUG NOT DONE 

void load_locs_obj(std::string objfilepath)
{
    bool debug = true;

    char char_array[100];

    obj_model* pt_obj2d_loader  = new obj_model;
    
    //may not be needed on a new object?? DEBUG 
    pt_obj2d_loader->reset();
   
    strcpy(char_array, objfilepath.c_str()); 
    pt_obj2d_loader->load(char_array);
    
    //DEBUG - put this in object 3d as a class method ?
    if(debug)
    {
        std::cout << pt_obj2d_loader->num_lines << "\n";
    }

    uint pcount = 0;
    for (uint x = 0;x<pt_obj2d_loader->num_lines;x++)
    {
        //int pidx1 = pt_obj2d_loader->pt_loc[x][0];
        //Vector3 pt1 = pt_obj2d_loader->points[pidx1];
        // ->num_locs++;

        //point_buffer1
    }


    // I dont think this is needed, but hey
    delete pt_obj2d_loader;
    //pt_motionplot->update_toolpaths();
}





/*************************************************************/

