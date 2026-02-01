/*************************************************************/
/*
    plot.cpp 

    has two associated cpp files:  
        plot.cpp (this file)
        plot_fio.cpp 

    
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
     

/*****************************************************************************/
/*************************************************************/

#include <unistd.h> //sleep()
#include <sys/io.h> //outb() ioperm()
#include <math.h>
#include <cstring>

#include <iomanip>
#include <iostream>
#include <algorithm>
#include <vector>

#include <thread>
#include <mutex>

#include "globals.h"

#include "Vectors.h"
#include "timer.h"
#include "point_op.h"
#include "parport.h"
#include "plot.h"


#if DO_BUILD_GUI == true
    #include "gl_setup.h"
    extern std::vector<Vector3> linebuffer1; 
    extern std::vector<Vector3> linebuffer1_rgb; 
    extern std::vector<Vector3> linebuffer2; 
    extern std::vector<Vector3> linebuffer2_rgb; 
#endif 

#if DO_BUILD_GUI == false
    #include "obj_model.h"   
#endif 


point_ops PG;

extern cnc_parport parport;
extern cncglobals cg;


//Timer related 
timer mtime     = timer();
timer* pt_mtime = &mtime;

/***************************************/

std::mutex mtx_p; //you can use std::lock_guard if you want to be exception safe

/***************************************/


/*
    DEBUG NOT DONE 
    generate points to show the extents of the toolpaths
    use a "throw away" obj_model to do geometric calculations 
*/
void cnc_plot::toolpath_extents(void)
{

    obj_model* pt_obj2d_loader  = new obj_model;
    pt_obj2d_loader->reset();

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



/***************************************/

/*
   DEBUG NOT DONE 
   use a "throw away" obj_model to do geometric calculations 
   load a model from disk (l type obj ) and convert it to toolpaths

*/
void cnc_plot::import_path_from_obj(std::string filepath)
{
    char char_array[100];

    obj_model* pt_obj2d_loader  = new obj_model;
    pt_obj2d_loader->reset();
    strcpy(char_array, filepath.c_str()); 

    pt_obj2d_loader->load(char_array);
    
    if(pt_obj2d_loader->num_lines)
    {
        uint pcount = 0;
        for (uint x = 0;x<pt_obj2d_loader->num_lines;x++)
        {
            int pidx1 = pt_obj2d_loader->lines[x][0];
            int pidx2 = pt_obj2d_loader->lines[x][0];
            
            Vector3 pt1 = pt_obj2d_loader->points[pidx1];
            Vector3 pt2 = pt_obj2d_loader->points[pidx2];

            //copy the object3d vectors into a toolpath 
            add_file_vec(&pt1);
            add_file_vec(&pt2);
        }


        add_prgvec_ply(); 
        copy_prg_to_toolpath();
        //update_toolpaths();
    }
}

/***************************************/
// just a self contained example to launch the pulser 
// it was never meant to be more than that 


//hmm - create a new object each time? Sounds not effecient 
cnc_plot* plot     = new cnc_plot;
//cnc_parport* pport = new cnc_parport;

void run_cncplot(double f_x,
                 double f_y,
                 double f_z,
                 double s_x,
                 double s_y,
                 double s_z,
                 uint x_divs, 
                 uint y_divs,                                   
                 uint z_divs) 
{

    bool DEBUG = false; 

    Vector3 s_p = Vector3(f_x , f_y ,f_z );
    Vector3 e_p = Vector3(s_x , s_y ,s_z );

    //----
    //I was creating new objects each time. Debug ?  
    //cnc_plot* plot     = new cnc_plot;
    //cnc_parport* pport = new cnc_parport;
    //plot->clear_toolpaths();
    
    plot->calc_3d_pulses(s_p, e_p, x_divs, y_divs, z_divs);
    
    //----


    if(DEBUG == true)
    {
        std::cout << "## run_cncplot debug mode \n"; 
        std::cout << "## pulse train size " << plot->pulsetrain.size() << "\n";

        for(uint x=0;x<plot->pulsetrain.size();x++)
        {
            std::cout<<plot->pulsetrain[x].x  <<" "
                     <<plot->pulsetrain[x].y  <<" "
                     <<plot->pulsetrain[x].z  <<"\n";        
        } 
    }

    if(cg.ENABLE_MOTOR_DRIVE==0)
    {
        std::cout << "*** WARNING MOTORS DISABLED IN CFG ***\n";
    }

    if(DEBUG == false && cg.ENABLE_MOTOR_DRIVE == 1)
    {
        parport.freerun_pulses( &cg, plot);
    }

    //I was creating each time. Debug ? 
    //delete pport; 
    //delete plot;
}

/***************************************/

// experiment in multithreading - so far so good 
void pulse_thread(double f_x,
                  double f_y,
                  double f_z,
                  double s_x,
                  double s_y,
                  double s_z,
                 uint x_divs, 
                 uint y_divs,                                   
                 uint z_divs)  
{
    

    std::thread pgen_thread(run_cncplot, f_x, f_y, f_z, s_x, s_y, s_z, x_divs, y_divs, z_divs);

    //pgen_thread.detach();  
    pgen_thread.join(); 

}

/***************************************/
void cnc_plot::timer_init(void)
{
    mtime.stop();

}


/******************************************/
//rapid, prg , disp, toolpath ARE INDEXED 
void cnc_plot::add_disp_vec(Vector3* nv)
{
    disp_vecs.push_back(*nv);
}

/******************************************/

//rapid, prg , disp, toolpath ARE INDEXED 
void cnc_plot::add_prg_vec(Vector3* nv)
{
    program_vecs.push_back(*nv);
}
 

/******************************************/
//file vectros are NOT indexed 
void cnc_plot::add_file_vec(Vector3* nv)
{

    bool debug = false;
   
    if(debug)
    {  
        std::cout << "adding file vec \n";
    }

   loaded_file_vecs.push_back(*nv);
}

/******************************************/
//rapid, prg , disp, toolpath ARE INDEXED 
void cnc_plot::add_rapid_vec(Vector3* nv)
{
    bool debug = true;
   
    if(debug)
    {  
       std::cout << "adding rapid vec \n";
    }

    rapidmove_vecs.push_back(*nv);
}


/******************************************/
/******************************************/
/******************************************/
//START MOTION INDEX TOOLS (WIP)

void cnc_plot::add_motion(std::string name, 
                          std::string type, 
                          int prog_id,
                          int rapid_in, 
                          int rapid_out 
                          )
{
    
    bool debug = false;

    if(debug)
    {
        std::cout << "add_motion: name "<< name << " type "
                  << type <<" prgid " << prog_id << " ri " << rapid_in << " ro "
                  << rapid_out << "\n";
    }
    
    //std::vector<motion_idx> motion_prg[MAX_MOTION_NUM];  // "fk" style links to motion vecs
    motion_idx foo = motion_idx(name, type, prog_id, rapid_in, rapid_out);


    //clearly I have more homework to do on C++
    //https://stackoverflow.com/questions/4303513/push-back-vs-emplace-back

    motion_prg->emplace_back(foo);
    //num_motion_ids++;

}


/******************************************/
//DEBUG NEED TO THINK THIS OUT 
void cnc_plot::show_mpath(void)
{
    bool debug = true;

    if(debug)
    {
        std::cout << " we have "<< motion_prg->size() << " motion idx objects \n";
    }

    for (uint mp=0;mp<motion_prg->size();mp++)
    {
        motion_idx motionobj = motion_prg->at(mp);

        if(debug)
        {
            std::cout << " motion " << mp << " " << motionobj.name     << " " 
                                    << motionobj.type     << " "
                                    << motionobj.prog_id  << " "
                                    << motionobj.rapid_in << " "
                                    << motionobj.rapid_out << "\n";
        }//debug 

        //-------------------------------//
        if(motionobj.prog_id>=0)
        {     
              
            std::vector<uint> myply = prg_idxs[motionobj.prog_id];
            std::cout << " motion index to prgm polygon of "<< myply.size() << " size\n"; 
            //std::cout << " prog : obj id " <<  prgid   << "\n";   
            //std::cout << program_vecs[prgid].x  <<"\n";

        }

        //-------------------------------//
        // // // cache of toolpath component vectors 
        // // std::vector<Vector3> rapidmove_vecs;    
        // // std::vector<uint> rpd_idxs[MAX_NUM_PLY]; // index to rapidmove vecs
        // // uint num_rpd_plys;

        if(motionobj.rapid_in>=0)
        {     
              
            std::vector<uint> myply = rpd_idxs[motionobj.rapid_in];
            std::cout << " motion index ["<<motionobj.rapid_in<<"] :(rapid in) polygon of "<< myply.size() << " size\n"; 
        }

        //-------------------------------//
        if(motionobj.rapid_out>=0)
        {     
              
            std::vector<uint> myply = rpd_idxs[motionobj.rapid_out];
            std::cout << " motion index ["<<motionobj.rapid_out<<"] :(rapid out) polygon of "<< myply.size() << " size\n"; 
        }



    }
}


//DEBUG NEED TO THINK THIS OUT 
void cnc_plot::show_mpath_info(void)
{
      std::cout << " we have "<< motion_prg->size() << " motion idx objects \n";

    for (uint mp=0;mp<motion_prg->size();mp++)
    {
        std::cout << "  motion idxobj  " << motion_prg->at(mp).name      << " " 
                                         << motion_prg->at(mp).type      << " "
                                         << motion_prg->at(mp).prog_id   << " "
                                         << motion_prg->at(mp).rapid_in  << " "
                                         << motion_prg->at(mp).rapid_out << "\n";
    }
}

//END MOTION INDEX TOOLS (WIP)
/******************************************/
/******************************************/
/******************************************/


/******************************************/
void cnc_plot::show_vecs(std::vector<Vector3>* pt_vec)
{
    for (uint vidx=0;vidx<pt_vec->size();vidx++)
    {
        std::cout <<vidx <<" "<< pt_vec->at(vidx) << "\n";
    }
}

/******************************************/
//show pathinfo 
void cnc_plot::show_path_info(void)
{
    std::cout << "pathinfo: num path polygons: "<< num_prg_plys << "\n";
    for (uint pi=0;pi<num_prg_plys;pi++)
    {
        std::cout << "poly "<<pi <<" num pts :"<< tp_idxs[0].size() << "\n";        
    }    
}

/******************************************/

void cnc_plot::show_path(void)
{
    //std::cout << " #"  << num_motion_ids          <<" motion path obj(s) \n"; 
    std::cout << " #"  << num_prg_plys            <<" path prg polys     \n"; 
    std::cout << " #"  << num_rpd_plys            <<" path rpd polys     \n";
    std::cout << " #"  << num_disp_ids            <<" display polys     \n";

    std::cout << "  ---------------------------------------------------  \n"; 
    std::cout << " #"  << disp_vecs.size()        <<" display vecs       \n"; 
    std::cout << " #"  << loaded_file_vecs.size() <<" file vecs     \n";   
    std::cout << " #"  << program_vecs.size()     <<" program vecs  # " << num_prg_plys <<" ids \n";    
    std::cout << " #"  << rapidmove_vecs.size()   <<" rapid vecs    # " << num_rpd_plys <<" ids \n";  
    std::cout << " #"  << toolpath_vecs.size()    <<" toolpath vecs # " << num_toolpath_ids <<" ids \n";    

    #if DO_BUILD_GUI == true    
        std::cout << " #"  << linebuffer1.size()  <<" render1 vecs \n";   
        std::cout << " #"  << linebuffer2.size()  <<" render2 vecs \n"; 
    #endif   

}

/******************************************/
void cnc_plot::showpthids(void)
{
    for(uint x=0;x<num_prg_plys;x++)
    {
        std::cout << " ply id:" << x <<" size:"<< tp_idxs[x].size() << " \n";        
    }
}

/******************************************/
// a "polygon" is a sequence of line geometry used as a toolpath 
void cnc_plot::showply(uint pidx)
{
    std::cout << "------------------------\n";

    if(pidx>num_prg_plys || pidx<0)
    {
        std::cout << " # error polygon index out of range \n";
    }

    if(pidx>=0 && pidx<=num_prg_plys)
    {

        for(uint x=0;x<tp_idxs[pidx].size();x++)
        {

            uint id = tp_idxs[pidx].at(x);
            
            Vector3 v =  program_vecs[id];
            std::cout << "vec "<< x << "  " <<v.x <<" "<< v.y << " "<<v.z << "\n";

        }
    }

}

/******************************************/
// sequence of line geometries used as a toolpath(s) 
void cnc_plot::showgeom(void)
{
    std::cout << "\n #"<< rapidmove_vecs.size() <<" rapid vecs \n";
    show_vecs(&rapidmove_vecs);

    std::cout << " #" << program_vecs.size() <<" program vecs \n";    
    show_vecs(&program_vecs);

    std::cout << " #" << toolpath_vecs.size() <<" path vecs \n";    
    show_vecs(&toolpath_vecs);    

}

/******************************************/
void cnc_plot::pause(void)
{
    if(running)
    {
        running=true;
        mtime.stop();
    }
}

/******************************************/
//clear toolpaths only 
void cnc_plot::clear_toolpaths(void)
{

    if(finished==true && running==false)
    {
        //clear the old data out 
        rapidmove_vecs.clear();
        toolpath_vecs.clear();
    }else
    {
        std::cout << "clear_toolpaths: cant clear while running  \n"; 
    }

}


/******************************************/
void cnc_plot::reset(void)
{
    //need to move the head to start pos 
    
    // clear_toolpaths();
    // toolpath_vecs.push_back(Vector3(0,0,0));
    // toolpath_vecs.push_back(Vector3(1,-1,2));

    // Vector3 up_vec   = Vector3(quill_pos.x, retract_height, quill_pos.z);
    // rapidmove_vecs.push_back(quill_pos);
    // update_toolpaths();

    //this resets the internals 
    mtime.stop(); 
    vec_idx = 0;
    running  = false;
    finished = true;

    //DEBUG prg_origin may get moved to motion_idx object 
    //DEBUG - or copy it from plot-> motion_idx 
    quill_pos = Vector3(prg_origin.x,
                        prg_origin.y,
                        prg_origin.z);
            
}


/******************************************/
void cnc_plot::stop(void)
{
    if(running)
    {
        running=false;
        mtime.stop();
        //mtime.step_sim();        
    }

    finished = true;
    std::cout << " toolpath plot stopped \n";
}


/******************************************/

void cnc_plot::run_sim(void)
{
    
    //DEBUG - it crashes if you run with no data 
    //this is a hack to stop that
    if(toolpath_vecs.size()==0)  
    {
        running=false; 
        finished=true;
        return void();
    }

    //this is for cases where we are paused but still running  
    if(running==true && finished==false)
    {
        mtime.start();
    }

    //this is a fresh run from the beginning 
    if(running==false && finished==true)
    {
        mtime.reset_sim(); //0-1 time only
        mtime.start();
        running  = true;
        finished = false;
    }

}

/******************************************/
//called from update_sim()

void cnc_plot::process_vec(uint window_idx)
{
    bool debug = true;
    float low_thresh = .0001;
    
    //set up the vector to process 
    Vector3 s_p = toolpath_vecs[window_idx];
    Vector3 e_p = toolpath_vecs[window_idx+1];  

    //build the path vector 
    Vector3 offset = e_p - s_p;

    //calculate length of the vector 
    float veclen = offset.length();

    if(debug)
    {   
        std::cout << " ------------------- \n";
        std::cout << " called prcess vec [" << offset.x << ", " << offset.y << ", "<< offset.z<< "]\n";
    }


    

            
    if(veclen!=0)
    {

        if(debug)
        {   
            std::cout << " pplu          x:" << cg.pp1u_x << " y:"<< cg.pp1u_x << " z:"<< cg.pp1u_x << "\n";
         
        }

        uint numx = 0;
        uint numy = 0;
        uint numz = 0;
        
        //uint dyn_divs_x = cg.pp1u_x;
        //uint dyn_divs_y = cg.pp1u_y;
        //uint dyn_divs_z = cg.pp1u_z;

        numx = veclen * cg.pp1u_x;
        numy = veclen * cg.pp1u_y;
        numz = veclen * cg.pp1u_z;
 
        //yes I know we already checked 0 length, I dont care 
        if(veclen!=0 && veclen>low_thresh)
        {
            if(debug)
            {    
                std::cout << " length, divs  " << offset.length() <<" "<< cg.pp1u_x << " " << cg.pp1u_y << " " << cg.pp1u_z << "\n";    
                std::cout << " num calc      " << numx << " " << numy << " " << numz << "\n";
            }

            pulse_thread(s_p.x, s_p.y, s_p.z, e_p.x, e_p.y, e_p.z, numx, numy, numz ); 
        }else{
            std::cout << "vector length too low, skipping \n";
        }

    }else{
        if(debug)
        { 
            //std::cout << "  skipping zero length vector\n"; 
        }
    }//if zero length 

}


/******************************************/

//called with each "tick" of the simulation 

void cnc_plot::update_sim(void)
{
    bool debug = false;
    
    //update this to display stats in ogl
    //DEBUG - need to display number BETWEEN VECS(PTS) 
    num_simvecs = toolpath_vecs.size()-1;


    if (mtime.tm_running)
    {
        //----------------- 
        //DEBUG - need to add speed setting  
        //localsimtime = mtime.get_elapsed_simtime() * timediv;
        localsimtime = mtime.get_elapsed_simtime();        

        //----------------- 
        //the main loop where we update display and pulse the ports.
        if (vec_idx<=toolpath_vecs.size()-1 && mtime.tm_running)
        {

            if (localsimtime>0.0 && localsimtime<1.0 )
            {
                //set up the vector to process 
                Vector3 s_p = toolpath_vecs[vec_idx];
                Vector3 e_p = toolpath_vecs[vec_idx+1]; 

                //interpolate xyz posiiton over time 
                PG.lerp_along(&quill_pos, 
                            s_p, 
                            e_p, 
                            (float) localsimtime);

            }//if sim is running and in range 0-1
        }//update locator position 

        //----------------- 
        //simtime runs between 0-1 - it resets each time another vector in the stack has been processed
        if (localsimtime>=1.0)
        {
            //DEBUG MUTEX LOCK  
            mtx_p.lock();

            process_vec(vec_idx);

            //iterate the stack of vectors to process
            if (vec_idx<toolpath_vecs.size()-2)
            {
                if(debug)
                { 
                    std::cout << "finished processing vector "<< vec_idx+1 <<" of " << toolpath_vecs.size()-2 << "\n";
                }
                vec_idx++;        
                mtime.reset_sim();
                localsimtime=0;
            }
            //program finished here
            else if (vec_idx>=toolpath_vecs.size()-2)
            {
                stop();
                vec_idx = 0;
            }//stop when we processed all the data
            
            //DEBUG MUTEX UNLOCK 
            mtx_p.unlock(); 

        }//step or stop the program  


    }//if program is running  
}


/******************************************/
/*
    derived from update_toolpaths 
    this is all experimental 
*/


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

/*

    This rebuilds the main tool path (program to run)
    Can be setup and called as much as needed.


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
                
                std::cout << motionobj.name << "\n";
                
                // << mp << " " << motionobj.name     << " " 
                // << motionobj.type     << " "
                // << motionobj.prog_id  << " "
                // << motionobj.rapid_in << " "
                // << motionobj.rapid_out << "\n";

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
    
}

/******************************************/
//DEBUG - NOT TESTED 
void cnc_plot::copy_prg_to_toolpath(void)
{
   
    bool debug = true;
    
    uint num_prg_exist = program_vecs.size();

    if (num_prg_exist<=1)
    {
        std::cout << "copy_prg_to_toolpath: need at least two vectors for a line\n";
    }

    //------------
    if (num_prg_exist>1)
    {   
        toolpath_vecs = program_vecs;  

        // not sure this works in C++
        //toolpath_vecs = program_vecs; 

        // do this instead for now   
        for (uint i=0;i<program_vecs.size();i++)
        {
            toolpath_vecs[i]==program_vecs[i];
        }

        for (uint i=0;i<num_prg_plys;i++)
        {
            for (uint ii=0;ii<prg_idxs[i].size();ii++)
            {
                tp_idxs[ii] = prg_idxs[ii]; 
                num_toolpath_ids++;           
            }
        }
    }


}


/******************************************/
/*
    This dumps the file buffer into a new (line) polygon from loaded_file_vecs buffer.

    Polygons (line polygons, not to be confused with 3D object faces ) are stored in 

    move from temp file buffer and clear file buffer for next incoming polygon 
    because the polygon is CCW/CW contigous, all we need is to know how many ids to add

    Polygon vertices/vectors are:
        stored    in -  this.program_vecs  
        indexed   in -  this.tp_idxs
        displayed in -  lbuffer1, lbuffer2, etc  

*/


void cnc_plot::add_prgvec_ply(void)
{
   
    bool debug = false;
    
    uint num_prg_exist = program_vecs.size();
    uint num_filevecs  = loaded_file_vecs.size();

    //------------
    // incoming data is in file buffer - copy it to program buffer with index 
    if (loaded_file_vecs.size()==1)
    {
        std::cout << "WARNING mov_fv_to_pv - need at least two points for a line."<<"\n";
    }   

    if (loaded_file_vecs.size()>1)
    {
        for (uint p=0;p<loaded_file_vecs.size();p++)
        {   
            Vector3 tmpv = loaded_file_vecs.at(p);
            if(debug)
            {
                std::cout << "mov_fv_to_pv file vec data "<< tmpv.x << " "<< tmpv.y << " "<< tmpv.z << "\n";
            }
            add_prg_vec(&tmpv);
        }
    }
    //clear file buffer so we can load more 
    loaded_file_vecs.clear();

    //------------
    if(debug)
    {
        std::cout << "add_prgvec_ply_tp reindex "<< num_prg_exist << "\n";
    }
 
    for (uint i=0;i<num_filevecs;i++)
    {   
        prg_idxs[num_prg_plys].push_back( (num_prg_exist+i) );
    }
  
    num_prg_plys++;
}


/******************************************/

/*
    add a new polygon from loaded_file_vecs buffer.
    
    copied from add_prgvec_ply, but for display vectors (disp_vecs)
*/


void cnc_plot::add_dispvec_ply(void)
{
    bool debug = false;
    
    uint num_disp_exist = disp_vecs.size();
    uint num_filevecs  = loaded_file_vecs.size();

    //------------
    // incoming data is in file buffer - copy it to program buffer with index 
    if (loaded_file_vecs.size()==1)
    {
        std::cout << "WARNING add_dispvec_ply - need at least two points for a line."<<"\n";
    }   

    if (loaded_file_vecs.size()>1)
    {
        for (uint p=0;p<loaded_file_vecs.size();p++)
        {   
            Vector3 tmpv = loaded_file_vecs.at(p);
            if(debug)
            {
                std::cout << "mov_fv_to_pv file vec data "<< tmpv.x << " "<< tmpv.y << " "<< tmpv.z << "\n";
            }
            add_disp_vec(&tmpv);
        }
    }
    //clear file buffer so we can load more 
    loaded_file_vecs.clear();

    //------------
    if(debug)
    {
        std::cout << "add_dispvec_ply_tp reindex "<< num_disp_exist << "\n";
    }
 
    for (uint i=0;i<num_filevecs;i++)
    {   
        disp_idxs[num_disp_ids].push_back( (num_disp_exist+i) );
    }
  
    num_disp_ids++;


}


/******************************************/
/*
    The first thing called when the vectors are loaded from disk

    //DEBUG - unfinished , looks like it should set some metadata/vectors 
    about the data we loaded 

    - ??
    - extents
    - start of polygon path
    - end of plygon path 
    - length of path 
    - ??
*/

void cnc_plot::init_paths( std::vector<Vector3>* pt_anyvecs)
{
    for (uint i=0;i<pt_anyvecs->size();i++)
    {   
        //debug - should add a class method to get first and last vec 
        if(i==0)
        {
            prg_origin = pt_anyvecs->at(i);
            quill_pos = Vector3(prg_origin.x,
                                prg_origin.y,
                                prg_origin.z);
        };

        //if(i==pt_anyvecs->size()-1){prg_end = pt_anyvecs->at(i);}
        //Vector3 sv  = pt_anyvecs->at(i);
    } 

}



/******************************************/
/*
    calc_3d_pulses - translates a 3D vector into 6 electrical signals (step/dir * 3)
   
*/

void cnc_plot::calc_3d_pulses(Vector3 fr_pt, 
                              Vector3 to_pt,
                              uint numdivx,
                              uint numdivy,
                              uint numdivz)
{

    pulsetrain.clear();

    bool debug = false;

    point_ops PG;

    //calc a new 3D vector betwen the two points in 3D
    //Vector3 between   = sub(fr_pt, to_pt);     //old vector lib 
    //Vector3 between   = fr_pt.operator-(to_pt); //new vector lib 
    
    //calc the length of the path vector
    //float mag     = between.length();
    
    int xp=0;int yp=0;int zp=0;

    //calculate 3 scalars for the absolute change on each axis  
    float delta_x = fr_pt.x-to_pt.x;
    float delta_y = fr_pt.y-to_pt.y;
    float delta_z = fr_pt.z-to_pt.z;

    if (debug)
    {
        std::cout << "  delta        x "<< delta_x <<   
                                 " y "<< delta_y << 
                                 " z "<< delta_z << "\n";
    }

    //2 is a magic number to (all other data is 1 )
    //calc the direction of the vector 
    if (to_pt.x>fr_pt.x){
        xp=2;
    }else{
        xp=0; 
    }
    //calc the direction of the vector 
    if (to_pt.y>fr_pt.y){
        yp=2;
    }else{
        yp=0; 
    }
    //calc the direction of the vector 
    if (to_pt.z>fr_pt.z){
        zp=2;
    }else{
        zp=0; 
    }

    //use the amount of change times the spatial divisions to get the pulses 
    //DEBUG - we may want to use the mag of the 3d vector in here                  
    uint num_pul_x = numdivx*abs(delta_x);
    uint num_pul_y = numdivy*abs(delta_y);
    uint num_pul_z = numdivz*abs(delta_z);     


    //first element of pulse train stores the direction 
    Vector3 dirvec = Vector3(xp,yp,zp);

    //--------------------
    //DEBUG -no point in caching pulses if empty 
    //if(num_pul_x>0 && num_pul_y>0 && num_pul_z>0)
    //{
        pulsetrain.push_back(dirvec);
    //}
    //--------------------

   
    if (debug)
    {
        std::cout << "  vector dir   x " << dirvec.x << " y " << dirvec.y << " z " << dirvec.z << "\n";
    }


    // get the absolute highest number of pulses (on any axis) to calculate 
    uint tmp[] = {num_pul_x, num_pul_y, num_pul_z};
    //std::cout << "before: "<<tmp[0] << " "<< tmp[1] <<" "<< tmp[2] <<"\n";
    std::sort(std::begin(tmp), std::end(tmp)  );
    //std::cout << "after: "<<tmp[0] << " "<< tmp[1] <<" "<< tmp[2] <<"\n";
    uint most = tmp[2];

    //--------------------------------------//             
    if (debug)
    {            
        std::cout << "  num divs     " << numdivx   << " dy "<< numdivy << " dz "<< numdivz <<"\n";  
        std::cout << "  num pulses   " << num_pul_x <<" " << num_pul_y <<" " << num_pul_z <<"\n"; 
        std::cout << "  ceiling val  " << most      << "\n";
        std::cout << "#----#----#----#----# \n";
    }

    std::vector<int> calcpt_x;
    std::vector<int> calcpt_y;
    std::vector<int> calcpt_z;

    //DEBUG ADD THIS BACK LATER -no point in caching pulses if empty 
    //if(num_pul_x>0 && num_pul_y>0 && num_pul_z>0)
    if(1)
    {
        // calc the pulses using a ratio of length to divs. 
        gen_pulses(&calcpt_x, most, num_pul_x);  
        
        //the "swap axis" code below breaks things
        //it works if I do this
        gen_pulses(&calcpt_y, most, num_pul_y);  
        gen_pulses(&calcpt_z, most, num_pul_z);  

         
        //DEBUG ADD THIS BACK WHEN IT WORKS 
        //THIS SWAPS Z AXIS WITH Y 
        /*
        //Octant uses Y up (Maya 3d standard), but the CAD world uses Z up 
        if(FAKE_Z_UP_AXIS)
        {
            if (debug)
            { 
                std::cout << " # WARNING - SWAPPING Z/Y AXES!\n";
            }

            gen_pulses(&calcpt_z, most, num_pul_y);  
            gen_pulses(&calcpt_y, most, num_pul_z); 

        }else
        {
            gen_pulses(&calcpt_y, most, num_pul_y);  
            gen_pulses(&calcpt_z, most, num_pul_z);         
        }
        */
       

        //------------
        for(uint a=0;a<most;a++)
        {
            pulsetrain.push_back(Vector3(calcpt_x.at(a), calcpt_y.at(a), calcpt_z.at(a)));
            pulsetrain.push_back(Vector3(0,0,0));
        } 
    }
    

} 


/******************************************/
//DEBUG - LEGACY CODE TO REWRITE?
// this OPERATES ON AN ORPHANED PULSETRAIN so keep it out of the class
// that way it avoids the confusion of the pulsetrain inside the class 

void gen_pulses(std::vector<int>* pt_pulsetrain, int size, int num)
{
    
    if(num>size)
    {
        std::cout << "# gen_pulses: size arg may not exceed number \n";
        exit(1);
    }

    double div = (double)size/(double)num;
    //double gran = div/num;

    int a;  

    //if zero make all zeros, we want the output to be the same size
    if(num==0)
    {
        for(a=0;a<size;a++)
        {
            pt_pulsetrain->push_back(0);            
        }
    }

    //exception for integer 1, put the pulse right in the middle of output 
    if(num==1)
    {
        for(a=0;a<size;a++)
        {

            if( a == size/2)
            {
                pt_pulsetrain->push_back(1);
            }else  
            {
                pt_pulsetrain->push_back(0);
            } 

        }
    }

    // build a pulse waveform, spread it out as evenly as possible over the entirety of the data 
    // I did a true 3D solution (commented out at bottom) but this is way faster and works as far as I can tell
    if(num>1) 
    {
        for(a=0;a<size;a++)
        {
            double chunk = fmod(a,div);
            if( chunk < 1)
            {
                pt_pulsetrain->push_back(1);
            }
            if( chunk > 1)
            {
                pt_pulsetrain->push_back(0);                   
            } 
            if ( chunk==1){
                pt_pulsetrain->push_back(0);                 
            }
        }
    } 

}
      


/******************************************/

void precalc_3d_pulses(std::vector<Vector3>* pt_pulsetrain,
                              Vector3 fr_pt, 
                              Vector3 to_pt,
                              int numdivs)
{

    bool debug = false;

    point_ops PG;

    //set the pulses per linear unit (spatial unit divions) - X,Y,Z unit prescaler 
    //for now use one number for all 3 - we will add the others in later
    uint pp_lux      = numdivs;
    uint pp_luy      = numdivs;
    uint pp_luz      = numdivs;

    //calc a new 3D vector betwen the two points in 3D
    //Vector3 between   = sub(fr_pt, to_pt);     //old vector lib 
    //Vector3 between   = fr_pt.operator-(to_pt); //new vector lib 
    
    //calc the length of the path vector
    //float mag     = between.length();
    
    int xp=0;int yp=0;int zp=0;

    //calculate 3 scalars for the absolute change on each axis  
    float delta_x = fr_pt.x-to_pt.x;
    float delta_y = fr_pt.y-to_pt.y;
    float delta_z = fr_pt.z-to_pt.z;

    if (debug)
    {
        std::cout << " calc_3d_pulses: delta x "<< delta_x <<   
                                           " y "<< delta_y << 
                                           " z "<< delta_z << "\n";
    }

    //2 is a magic number to (all other data is 1 )
    //calc the direction of the vector 
    if (to_pt.x>fr_pt.x){
        xp=2;
    }else{
        xp=0; 
    }
    //calc the direction of the vector 
    if (to_pt.y>fr_pt.y){
        yp=2;
    }else{
        yp=0; 
    }
    //calc the direction of the vector 
    if (to_pt.z>fr_pt.z){
        zp=2;
    }else{
        zp=0; 
    }
    
    //first element of pulse train stores the direction 
    //pt_pulsetrain->push_back(newvec3(xp,yp,zp));
    pt_pulsetrain->push_back(Vector3(xp,yp,zp));

    //use the amount of change times the spatial divisions to get the pulses 
    //DEBUG - we may want to use the mag of the 3d vector in here                  
    uint num_pul_x = pp_lux*abs(delta_x);
    uint num_pul_y = pp_luy*abs(delta_y);
    uint num_pul_z = pp_luz*abs(delta_z); 

    if (debug)
    {
        std::cout << "# calc_3d_pulses num pulses " << num_pul_x <<" "<<num_pul_y<<" "<<num_pul_z <<"\n";
    }

    // get the absolute highest number of pulses (on any axis) to calculate 
    uint tmp[] = {num_pul_x, num_pul_y, num_pul_z};
    //std::cout << "before: "<<tmp[0] << " "<< tmp[1] <<" "<< tmp[2] <<"\n";
    std::sort(std::begin(tmp), std::end(tmp)  );
    //std::cout << "after: "<<tmp[0] << " "<< tmp[1] <<" "<< tmp[2] <<"\n";
    uint most = tmp[2];

    //--------------------------------------//             
    if (debug)
    {            
        std::cout << "#   most   " << most << " "<< numdivs << " " <<"\n";  
        std::cout << "#   numpts " << num_pul_x <<" " << num_pul_y <<" " << num_pul_z <<"\n"; 
        std::cout << "#####\n";
    }

    std::vector<int> calcpt_x;
    std::vector<int> calcpt_y;
    std::vector<int> calcpt_z;
        
    // calc the pulses using a ratio of length to divs. 
    gen_pulses(&calcpt_x, most, num_pul_x);  
    gen_pulses(&calcpt_y, most, num_pul_y);  
    gen_pulses(&calcpt_z, most, num_pul_z);  

    ////

    for(uint a=0;a<most;a++)
    {
        pt_pulsetrain->push_back(Vector3(calcpt_x.at(a), calcpt_y.at(a), calcpt_z.at(a)));
        pt_pulsetrain->push_back(Vector3(0,0,0));
    } 
    

} 


