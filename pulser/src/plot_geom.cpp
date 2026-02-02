/*************************************************************/
/*
    plot_geom.cpp 

    has three associated cpp files:  

        plot_geom.cpp (this file)
        plot_fio.cpp 
        plot.cpp 

    
    General motion control and path plotting.
    
    Generates a series of 3D pulses from two vectors 
    Generates the pulsetrain, a series of signals to drive the motors via the parallel port.


    calc_3d_pulses -  takes two points and runs gen_pulses for each axis 
 
    gen_pulses
        translates a scalar and divs into a valid pulsetrain to send to IO 
        run for each axis-  X,Y,Z
        Args  *pt_pulsetrain, int size, int num



    //----------------------------------------//

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
     



#include <sys/io.h> //outb() ioperm()
#include <math.h>
#include <iomanip>
#include <iostream>
#include <vector>

#include "globals.h"
#include "Vectors.h"
#include "point_op.h"
#include "plot.h"


#if DO_BUILD_GUI == true
    #include "gl_setup.h"
#endif 







/***************************************/
//get the bbox of the toolpaths - we need to know if they exceed machine size, etc 
//DEBUG NOT DONE 
void cnc_plot::bbox3d(void)
{
    
    bool debug = true;

    // std::vector<Vector3> toolpath_vecs;     // toolpath data (dynamically constructed)
    // std::vector<uint> tp_idxs[MAX_NUM_PLY]; // index to toolpath vecs
    // uint num_toolpath_ids;

    if(num_toolpath_ids)
    {
        // use any data just to populate 
        bb_min_x = 1.0;
        bb_max_x = 1.0;
        bb_min_y = 1.0;
        bb_max_y = 1.0;
        bb_min_z = 1.0;
        bb_max_z = 1.0;

        Vector3 tvec = Vector3();

        //toolpath is indexed - doest explicitly need to be - but this way 
        //we can just clone progam vectors and it may offer more flexibilty in the future 
        for (uint i=0;i<num_toolpath_ids;i++)
        {
            for (uint ii=0;ii<tp_idxs[i].size();ii++)
            {
                uint tpi = tp_idxs[i][ii];
                tvec  = toolpath_vecs[tpi];

                if (tvec.x<bb_min_x) { bb_min_x=tvec.x; }
                if (tvec.x>bb_max_x) { bb_max_x=tvec.x; }
                if (tvec.y<bb_min_y) { bb_min_y=tvec.y; }
                if (tvec.y>bb_max_y) { bb_max_y=tvec.y; }
                if (tvec.z<bb_min_z) { bb_min_z=tvec.z; }
                if (tvec.z>bb_max_z) { bb_max_z=tvec.z; }  

            }//iterate each polygon by point
        }//iterate tp polygon            
     

        if (debug)
        {
            std::cout << bb_min_x << " " << bb_max_x << "\n";
            std::cout << bb_min_y << " " << bb_max_y << "\n";
            std::cout << bb_min_z << " " << bb_max_z << "\n";    
        }
        
    }else{
        std::cout << "no toolpath vectors\n";
    }


}

/***************************************/
//DEBUG NOT DONE 
 
void cnc_plot::bbox3d_pts(void)
{
     
    //DEBUG - NAME OF THESE ARE SHITE  
    t_maxx = Vector3( bb_max_x, bb_min_y, bb_max_z); 
    t_maxy = Vector3( bb_min_x, bb_max_y, bb_max_z); 
    t_minx = Vector3( bb_min_x, bb_min_y, bb_max_z); 
    t_miny = Vector3( bb_max_x, bb_max_y, bb_max_z); 

    b_maxx = Vector3( bb_max_x, bb_min_y, bb_min_z); 
    b_maxy = Vector3( bb_min_x, bb_max_y, bb_min_z); 
    b_minx = Vector3( bb_min_x, bb_min_y, bb_min_z); 
    b_miny = Vector3( bb_max_x, bb_max_y, bb_min_z); 
     

    //std::cout << t_maxx.x << " " << t_maxx.y << " "<< t_maxx.z << "\n";

}

