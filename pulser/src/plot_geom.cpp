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
    bool debug = false;

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


/***************************************/
/*
    DEBUG NOT DONE 
    
    generate primitive shapes and make into pts to use 

    Args:
    
        shape:
            0 - circle 
            1 - square

*/


void cnc_plot::prim_shape(uint shape, uint axis, float size)
{

    obj_model* pt_obj2d_loader  = new obj_model;
    pt_obj2d_loader->reset();
    
    std::vector<Vector3> output_pts;
    output_pts.reserve( 400 );      
     

    uint divs      = 8;
    float ox,oy,oz = 0;

    //-----------//

    if(shape==0)
    {
        // calc_circle args = (numdiv axis x_orig y_orig dia )
        //DEBUG convert origin to Vector3*
        if(axis==0){ pt_obj2d_loader->calc_circle(&output_pts, divs, 0, ox, oy, size); }
        if(axis==1){ pt_obj2d_loader->calc_circle(&output_pts, divs, 1, ox, oy, size); }
        if(axis==2){ pt_obj2d_loader->calc_circle(&output_pts, divs, 2, ox, oy, size); }                

        for (uint v=0;v<output_pts.size();v++)
        {
            Vector3 pt = output_pts[v];
            add_file_vec(&pt);
        }
        
        add_prgvec_ply(); 
        copy_prg_to_toolpath();
    
    }//circle prim 


    //-----------//
    if(shape==1)
    {
        Vector3 orig = Vector3(0,0,0);
        
        // calc_square args = (origin,  dia )
        if(axis==0){ pt_obj2d_loader->calc_square(&output_pts, &orig, 0, size); }
        if(axis==1){ pt_obj2d_loader->calc_square(&output_pts, &orig, 1, size); }
        if(axis==2){ pt_obj2d_loader->calc_square(&output_pts, &orig, 2, size); }  
     
        for (uint v=0;v<output_pts.size();v++)
        {
            Vector3 pt = output_pts[v];
            add_file_vec(&pt);
        }
        
        add_prgvec_ply(); 
        copy_prg_to_toolpath();

    }//square prim

    //-----------//

    // void point_ops::calc_line( pt2d *out_coords, int *pt1, int *pt2, int *num)

    //-----------//
      
    // int point_ops::get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, 
    //     float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)

 

}


/******************************************/
/*
    derived from update_toolpaths 
    this is all experimental 

void cnc_plot::bake_motion(void)
{

    
    bool debug = false;
    
    if(debug)
    {
        std::cout << " num polys " << num_prg_plys << "\n"; 
    } 
    
    if(finished==true && running==false)
    {
        #if DO_BUILD_GUI == true
            clear_linebuffers(); //clear display geom 
        #endif
        //clear the old data out 
        toolpath_vecs.clear();

        //---- 
        if(program_vecs.size()>0)
        {
            //iterate all polygons     
            for (uint pl=0;pl<num_prg_plys;pl++)
            {

                //add each vec3 for the polygon 
                for (uint vid=0;vid<tp_idxs[pl].size();vid++)
                {
                    Vector3 seg = program_vecs[tp_idxs[pl][vid]];
                    
                    if(debug)
                    {
                        std::cout << " update debug " << vid << " "<< seg.x << "\n";
                    }

                    toolpath_vecs.push_back(seg);  

                    #if DO_BUILD_GUI == true
                        add_vec_lbuf2(&seg);
                    #endif 

                };

            }//iterate polygons 

        }//if data exists
      
    }//if program is NOT running or paused
    
}
*/


/*

    This rebuilds the main tool path (program to run)
    Can be setup and called as much as needed.

    if no motion_idx object exists, 
    just use prg_vectors 

    this is buggy and probably wrong 



    program_vecs   - vectors loaded from a file 
    rapidmove_vecs - a path to move the head up, over, and back down 
    toolpath_vecs  - dynamically built path for head to move - gets rebuilt each time    

*/

void cnc_plot::update_toolpaths(void)
{

    //DEBUG _ NOT EVEN CLOSE TO DONE
    
    bool debug = true;
    
    if(debug)
    {
        
        std::cout << " update_toolpaths called \n";
        std::cout << " num polys " << num_prg_plys << "\n"; 
    } 


    if(finished==true && running==false)
    {
        #if DO_BUILD_GUI == true
            clear_linebuffers(); //clear display geom 
        #endif
        
        //clear the old toolpath data out 
        toolpath_vecs.clear();
        num_toolpath_ids=0;

        //CHECK FOR MOTION OBJECTS FIRST 
        //IF NONE THEN BUILD TOOLPATHS FROM PRG VECS
        if( motion_prg->size() )
        {
            std::cout << "update_toolpaths - MOTION IDX OBJ EXISTS  " << "\n";
            
            //for (uint mp=0;mp<num_motion_ids;mp++)
            for (uint mp=0;mp<motion_prg->size();mp++)
            {
                motion_idx motionobj = motion_prg->at(mp);

                // std::cout << "motion id is "<< mp << "\n";                
                // std::cout << motionobj.name << "\n";
                
                //scan for rapid_in (index to an index of rapid move vectors)
                //reconstruct the vectors, if any
                if(motionobj.rapid_in>=0)
                {
                    //std::cout << "rapid in found\n";
                    std::vector<uint> rp_ids = rpd_idxs[motionobj.rapid_in];
                    for(uint rv=0;rv<rp_ids.size();rv++)
                    {
                        //std::cout << rapidmove_vecs[rv].x << " "<< rapidmove_vecs[rv].y<<" "<< rapidmove_vecs[rv].z <<"\n";
                        Vector3 rapvec = Vector3(rapidmove_vecs[rv].x, rapidmove_vecs[rv].y, rapidmove_vecs[rv].z );
                        
                        toolpath_vecs.push_back(rapvec);
                    }
                }

                //scan for program
                if(motionobj.prog_id>=0)
                {
                    std::cout << "rapid in found\n";
                }
                
                //scan for rapid_out                
                if(motionobj.rapid_out>=0)
                {
                    std::cout << "rapid in found\n";
                }

                //if(motionobj.prog_id>=0)
                //{     
                //    std::vector<uint> myply = prg_idxs[motionobj.prog_id];
                //    std::cout << " motion index to prgm polygon of "<< myply.size() << " size\n"; 
                //}
            }
        }
        else if(program_vecs.size()>0)
        {
            std::cout << "update_toolpaths - MOTION IDX OBJ DO NOT EXIST - USE PRG VECS  " << "\n";

            //iterate all polygons     
            for (uint pl=0;pl<num_prg_plys;pl++)
            {
                //add each vec3 for the polygon 
                for (uint vid=0;vid<prg_idxs[pl].size();vid++)
                {
                    Vector3 seg = program_vecs[prg_idxs[pl][vid]];
                    
                    if(debug)
                    {
                        std::cout << " update debug " << vid << " "<< seg.x << "\n";
                    }
                };
            }//iterate polygons 
            
            copy_prg_to_toolpath();

        }//if data exists
      
    }//if program is NOT running or paused
 

    //update the num toolpaths for display in window 
    //DEBUG - need to display number BETWEEN VECS(PTS) 
    num_simvecs = toolpath_vecs.size()-1;
}
