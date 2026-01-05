/*************************************************************/
/*
    cnc_plot.cpp 
    
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
#endif 



#define HEX(x) setw(2) << setfill('0') << hex << (int)( x )

point_ops PG;

extern cnc_parport parport;
extern cncglobals cg;


//Timer related 
timer mtime = timer();
timer* pt_mtime = &mtime;

/***************************************/

std::mutex mtx_p; //you can use std::lock_guard if you want to be exception safe

/***************************************/




/***************************************/
// just a self contained example to launch the pulser 
// it was never meant to be more than that 

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

    cnc_plot* plot     = new cnc_plot;
    cnc_parport* pport = new cnc_parport;

    float dummy = 0;

    Vector3 s_p = Vector3(f_x , f_y ,f_z );
    Vector3 e_p = Vector3(s_x , s_y ,s_z );

    plot->calc_3d_pulses(s_p, e_p, x_divs, y_divs, z_divs);

    if(DEBUG==true)
    {
        std::cout << "## run_cncplot debug mode \n";

        for(uint x=0;x<plot->pulsetrain.size();x++)
        {
            std::cout<<plot->pulsetrain[x].x  <<" "
                     <<plot->pulsetrain[x].y  <<" "
                     <<plot->pulsetrain[x].z  <<"\n";        
        } 
    }

    if(DEBUG==false)
    {
        pport->freerun_pulses(&dummy, &cg, plot);
    }

    delete pport; 
    delete plot;
}

/***************************************/

// experiment in threads - so far so good 
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
/*

    program_vecs     = main part of toolpath construction 
    tp_idxs          = index to progvectors , per polygon basis - (like faces in .OBJ file)

    loaded_file_vecs = buffer for file loading only 

*/

void cnc_plot::add_prg_vec(Vector3* nv)
{
    program_vecs.push_back(*nv);
}

/******************************************/
void cnc_plot::add_file_vec(Vector3* nv)
{
   loaded_file_vecs.push_back(*nv);
}

/******************************************/
void cnc_plot::show_vecs(std::vector<Vector3>* pt_vec)
{
    for (int vidx=0;vidx<pt_vec->size();vidx++)
    {
        std::cout <<vidx <<" "<< pt_vec->at(vidx) << "\n";
    }
}

/******************************************/
void cnc_plot::show(void)
{
    std::cout << "\n #"<< rapidmove_vecs.size()   <<" rapid vecs \n";
    std::cout << " #"  << program_vecs.size()     <<" program vecs \n";    
    std::cout << " #"  << toolpath_vecs.size()    <<" path vecs \n";    
    std::cout << " #"  << loaded_file_vecs.size() <<" file vecs \n";   
}

/******************************************/
void cnc_plot::showpthids(void)
{
    for(uint x=0;x<num_plys;x++)
    {
        std::cout << " ply id:" << x <<" size:"<< tp_idxs[x].size() << " \n";        
    }
}

/******************************************/
// a "polygon" is a sequence of line geometry used as a toolpath 
void cnc_plot::showply(uint pidx)
{
    std::cout << "------------------------\n";

    if(pidx>num_plys || pidx<0)
    {
        std::cout << " # error polygon index out of range \n";
    }

    if(pidx>=0 && pidx<=num_plys)
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
//ideally this all happens in this class
//the problem is the while(running) loop locks up the GUI
//until I learn threads, just cnc_plot::run() and cnc_plot::update() from render_loop 

void cnc_plot::run_sim(void)
{
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
void cnc_plot::process_vec(uint window_idx)
{
    bool debug = false;

    if(debug)
    {   
        std::cout << "called prcess vec \n";
    }


    //set up the vector to process 
    Vector3 s_p = toolpath_vecs[window_idx];
    Vector3 e_p = toolpath_vecs[window_idx+1];  

    //calculate length of the vector 
    Vector3 offset = e_p - s_p;

    
    uint numx = 0;
    uint numy = 0;
    uint numz = 0;

    numx = offset.length()*cg.pp1u_x;
    numy = offset.length()*cg.pp1u_y;
    numz = offset.length()*cg.pp1u_z;
    
    if(debug)
    {    
        //std::cout << "num calc " << offset.length() <<" "<< cg.pp1u_x << " " << cg.pp1u_y << " " << cg.pp1u_z << "\n";    
        std::cout << "num calc " << numx << " " << numy << " " << numz << "\n";
    }

    pulse_thread(s_p.x, s_p.y, s_p.z, e_p.x, e_p.y, e_p.z, numx, numy, numz ); 

}


/******************************************/
void cnc_plot::update_sim(void)
{
    bool debug = false;

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

    This rebuilds the path that moves the head. 
    It is run prior to the machine moving. 

    program_vecs    = cached vectors loaded from a file 
    rapidmove_vecs  = a path to move the head up, over, and back down 
    toolpath_vecs   = the actual path that will be "cut". This gets rebuilt each time we run.  

*/

void cnc_plot::update_toolpaths(void)
{
    if(finished==true && running==false)
    {
        //clear the old data out 
        toolpath_vecs.clear();
        rapidmove_vecs.clear();
        
        #if DO_BUILD_GUI == true
            //clear display geom 
            clear_linebuffers();
        #endif

        //---- 
        //std::cout << tp_idxs.size() << "\n";
        if(program_vecs.size()>0)
        {

            //move head up at current quill pos
            //Vector3 up_vec   = Vector3(quill_pos.x, retract_height, quill_pos.z);
            
            //add_vec_lbuf2(&quill_pos);
            //toolpath_vecs.push_back(quill_pos);

            //add_vec_lbuf2(&up_vec);
            //toolpath_vecs.push_back(up_vec);

            //iterate all polygons     
            for (uint pl=0;pl<num_plys;pl++)
            {
                Vector3 this_st = program_vecs[tp_idxs[pl][0]];
                Vector3 hover = Vector3(this_st.x, retract_height, this_st.z);

                //add_vec_lbuf2(&hover);
                //toolpath_vecs.push_back(hover);
                //add_vec_lbuf2(&hover);//add to display buffer  

                //add_vec_lbuf2(&this_st);
                //toolpath_vecs.push_back(this_st);
                //add_vec_lbuf2(&this_st);//add to display buffer  


                //add each vec3 for the polygon 
                for (uint vid=0;vid<tp_idxs[pl].size();vid++)
                {
                    Vector3 seg = program_vecs[tp_idxs[pl][vid]];

                    //add_vec_lbuf2(    );
                    toolpath_vecs.push_back(seg);  
                };

            }//iterate polygons 

            //----------------------
            // //now we have them, add to the buffer to draw them 

        }//if data exists

        //-------------------------------------------//
        /*     
        //(re)calculate the length of all vectors
        //program_dist = 0;  
        //std::cout << "DEBUG - update_toolpaths ADDING prog vecs \n";            
        for (int v=0;v<program_vecs.size();v++)
        {
            toolpath_vecs.push_back( program_vecs.at(v) );
        }*/
        
          
    }//if program is NOT running or paused
    
}


/******************************************/
/*
    add a new polygon to program and display buffer.

    This dumps the filebuffer into a new polygon, or set of line vectors.

    Polygons (line polygons, not to be confused with 3D object faces ) are stored in 
    
    Polygon vertices/vectors are:
        stored    in -  this.program_vecs  
        indexed   in -  this.tp_idxs
        displayed in -  lbuffer1, lbuffer2, etc  

    move from temp file buffer and clear file buffer for next incoming polygon 
    because the polygon is CCW/CW contigous, all we need is to know how many ids to add
*/

void cnc_plot::add_new_polygon(int numply, int numids)
{
    //std::cout << "add ply cont called # "<< numply  << " "<<  numids << "\n";
         
    int reindex = 0;
    
     
    //if data already loaded - increase index to new incoming data 
    if(numply==0)
    {
        reindex = numply; 
    }else{
        reindex = program_vecs.size(); 
    }
    //std::cout << "add ply reindex "<< reindex << "\n";
 


    //dynamically add more indices 
    //we just iterate a sequence of ids up to N verteces
    for (uint i=0;i<numids;i++)
    {   
        tp_idxs[numply].push_back( (reindex+i) );
    }

    // incoming data is in file buffer - copy it to program buffer with index 
    if (loaded_file_vecs.size()==1)
    {
        std::cout << "WARNING add_new_polygon - need at least two points for a line."<<"\n";
    }    
    if (loaded_file_vecs.size()>1)
    {
        for (uint p=0;p<loaded_file_vecs.size();p++)
        {   
            // Vector3 foo = loaded_file_vecs.at(p);
            // std::cout << "ADDING "<< foo.x << " "<< foo.y << " "<< foo.z << "\n";

            //add to program_vecs (toolpath) 
            add_prg_vec(&loaded_file_vecs.at(p));
            
            #if DO_BUILD_GUI == true
                //add to display buffer 
                add_vec_lbuf1(&loaded_file_vecs.at(p)); 
            #endif
        }
    }

    //clear file buffer so we can load more 
    loaded_file_vecs.clear();

    num_plys++;

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

void cnc_plot::loadpath( std::vector<Vector3>* pt_drawvecs)
{
    for (int i=0;i<pt_drawvecs->size();i++)
    {   
        //debug - should add a class method to get first and last vec 
        if(i==0)
        {
            prg_origin = pt_drawvecs->at(i);
            quill_pos = Vector3(prg_origin.x,
                                prg_origin.y,
                                prg_origin.z);
        };

        //if(i==pt_drawvecs->size()-1){prg_end = pt_drawvecs->at(i);}
        //Vector3 sv  = pt_drawvecs->at(i);
    } 
    
    update_toolpaths();
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

    bool debug = true;

    point_ops PG;

    //calc a new 3D vector betwen the two points in 3D
    //Vector3 between   = sub(fr_pt, to_pt);     //old vector lib 
    Vector3 between   = fr_pt.operator-(to_pt); //new vector lib 
    
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
    //DEBUG -no point in cachine pulses if empty 
    //if(num_pul_x>0 && num_pul_x>0 && num_pul_x>0)
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

    //DEBUG -no point in cachine pulses if empty 
    if(num_pul_x>0 && num_pul_x>0 && num_pul_x>0)
    {
        // calc the pulses using a ratio of length to divs. 
        gen_pulses(&calcpt_x, most, num_pul_x);  
        
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
    double gran = div/num;

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
    unsigned int pp_lux      = numdivs;
    unsigned int pp_luy      = numdivs;
    unsigned int pp_luz      = numdivs;

    //calc a new 3D vector betwen the two points in 3D
    //Vector3 between   = sub(fr_pt, to_pt);     //old vector lib 
    Vector3 between   = fr_pt.operator-(to_pt); //new vector lib 
    
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
    unsigned int num_pul_x = pp_lux*abs(delta_x);
    unsigned int num_pul_y = pp_luy*abs(delta_y);
    unsigned int num_pul_z = pp_luz*abs(delta_z); 

    if (debug)
    {
        std::cout << "# calc_3d_pulses num pulses " << num_pul_x <<" "<<num_pul_y<<" "<<num_pul_z <<"\n";
    }

    // get the absolute highest number of pulses (on any axis) to calculate 
    unsigned int tmp[] = {num_pul_x, num_pul_y, num_pul_z};
    //std::cout << "before: "<<tmp[0] << " "<< tmp[1] <<" "<< tmp[2] <<"\n";
    std::sort(std::begin(tmp), std::end(tmp)  );
    //std::cout << "after: "<<tmp[0] << " "<< tmp[1] <<" "<< tmp[2] <<"\n";
    unsigned int most = tmp[2];

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

    for(unsigned int a=0;a<most;a++)
    {
        pt_pulsetrain->push_back(Vector3(calcpt_x.at(a), calcpt_y.at(a), calcpt_z.at(a)));
        pt_pulsetrain->push_back(Vector3(0,0,0));
    } 
    

} 





/******************************************/

//probably will be obsolete with threading ?
/*
void cnc_plot::show_pt(void)
{

    std::cout << " pulsetrain size     "<< pulsetrain.size() << "\n";
    std::cout << " pulsetrain idx size "<< pt_idxs->size()    << "\n";

    for(unsigned int x=0;x<pt_idxs->size();x++)
    {
        std::cout <<" idx: " << x <<" "<< pt_idxs->at(x) << "\n";      
    }

}
*/

/******************************************/

//walk each vector in the toolpath 
//calc the pulsetrain, store the index at start of each 
//cache the step and direction data to speed up the running "simulation"
/*
void cnc_plot::precache_sim(void)
{
    bool debug = false;
      
    pulsetrain.clear();
    pt_idxs->clear();

    std::vector<Vector3> * pt_pulsetrain = &pulsetrain;

    std::cout << " NOT DONE DEBUG - precache num vecs:"<<toolpath_vecs.size()<<"\n";

    //----------------- 
    //iterate all coords and run calc_3d_pulses for them
    for (unsigned int pc=0;pc<toolpath_vecs.size()-1;pc++)
    {
        //----- 
        //set up the vector to process 
        Vector3 s_p = toolpath_vecs[pc];
        Vector3 e_p = toolpath_vecs[pc+1];  
        if(debug)
        {
            std::cout << "precache_sim \n"; 
            std::cout << s_p.x <<" "<< s_p.y << " "<< s_p.z << "\n";
            std::cout << e_p.x <<" "<< e_p.y << " "<< e_p.z << "\n";
        }

        //----- 
        //std::cout << "pulsetrain size - new index " << pt_pulsetrain->size() << "\n";
        pt_idxs->push_back( pt_pulsetrain->size() );

        //precalc_3d_pulses appends each time we run it  
        //DEBUG  also need to calc length/time to get num divs 
        precalc_3d_pulses(pt_pulsetrain, s_p, e_p, 10);
        
         
        for (unsigned int pi=0;pi<pt_idxs->size();pi++)
        {
            std::cout << "PT IDX " << pt_idxs->at(pi) << "\n";
            std::cout << "START OF PULSETRAIN VEC - IDX " << pt_pulsetrain->at(pt_idxs->at(pi))  << "\n";
        } 


    }//iterate all calculated toolpath vectors 


}
*/


/*


void cnc_plot::run(void)
{
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

    //-------------------------------//
    while (mtime.tm_running)
    {
        //localsimtime = mtime.get_elapsed_simtime() * timediv;
        localsimtime = mtime.get_elapsed_simtime();        


        //std::cout << "--------------------------\n";
        //std::cout << "cnc_plot -> local simtime " << localsimtime <<"\n";
        // std::cout << "real time " << mtime.getElapsedTimeInSec() << "\n";

        //simtime runs between 0-1 - it resets each time another vector in the stack has been processed
        if (localsimtime>=1.0)
        {
             
            //std::cout << "-----------------------------------\n";        
            //std::cout << "running index " << vec_idx        << "\n";

            //iterate the stack of vectors to process
            if (vec_idx<toolpath_vecs.size())
            {
                vec_idx++;        

                // start the (sim) clock over at the end of each vector segment 
                // 0.0 - 1.0 is the range - which feeds the 3D `lerp  
                mtime.reset_sim();
                localsimtime=0;
            }
            //program finished here
            else if (vec_idx>=toolpath_vecs.size()-1)
            {
                stop();

                //std::cout << "-----------------------------------\n";        
                //std::cout << "mtime stopped  "  << "\n";
                vec_idx = 0;
                //update_toolpaths();

            }
           
        }
        //-----------------
        std::cout<< localsimtime << "\n";

        //----------------- 
        //the main loop where we update display and pulse the ports.
        if (vec_idx<=toolpath_vecs.size()-1 && mtime.tm_running)
        {
            Vector3 s_p = toolpath_vecs[vec_idx];
            Vector3 e_p = toolpath_vecs[vec_idx+1];  

            PG.lerp_along(&quill_pos, 
                           s_p, 
                           e_p, 
                           (float) localsimtime);
        }

    }//end program cycle running  



}
*/



/******************************************/


 
