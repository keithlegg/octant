/*************************************************************/
/*
    cnc_plot.cpp 
    
    General motion control and path plotting.
    
    Generates a series of 3D pulses from two vectors 
    Generates the pulsetrain, a series of signals to drive the motors via the parallel port.


 


    calc_3d_pulses -  takes two points and runs gen_pules for each axis 
 
    gen_pules
        translates a scalar and divs into a valid pulsetrain to send to IO 
        run for each axis-  X,Y,Z
        Args  *pt_pulsetrain, int size, int num

    //----------------------------------------//

    MIT License

    Copyright (c) 2025 Keith Legg - keithlegg23@gmail.com

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

#include "Vectors.h"
#include "timer.h"
#include "gl_setup.h"
#include "point_op.h"
#include "cnc_plot.h"


#define HEX(x) setw(2) << setfill('0') << hex << (int)( x )

point_ops PG;



extern timer mtime;

extern vector<Vector3> linebuffer2; 
extern vector<Vector3> linebuffer2_rgb; 


/******************************************/
void cnc_plot::show_vecs(vector<Vector3>* pt_vec)
{
    for (int vidx=0;vidx<pt_vec->size();vidx++)
    {
        std::cout <<vidx <<" "<< pt_vec->at(vidx) << "\n";
    }
}

/******************************************/
void cnc_plot::show(void)
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


void cnc_plot::stop(void)
{
    if(running)
    {
        running=false;
        mtime.stop();
        //mtime.step_sim();        
    }
}


/******************************************/
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
        mtime.start();
        running = true;
        finished = false;
    }

}
 

/******************************************/
/*
    dynamically generate a tool path that lifts the head, 
    traverses and drops back down
*/
void cnc_plot::rapid_move(void)
{
    
    rapidmove_vecs.clear();
    
    linebuffer2.clear();


    Vector3 up_vec   = Vector3(quill_pos.x, retract_height, quill_pos.z);
    Vector3 hover_e  = Vector3(prg_origin.x, retract_height, prg_origin.z);
    //Vector3 trav_vec = quill_pos.operator-(prg_origin);
    //Vector3 dwn_vec  = Vector3(prg_origin.x, work_height, prg_origin.z);        
    
    /*
    rapidmove_vecs.push_back(quill_pos );
    rapidmove_vecs.push_back(up_vec );
    rapidmove_vecs.push_back(hover_e);
    rapidmove_vecs.push_back(dwn_vec);
    */

    //now we have them, add to the buffer to draw them 
    add_vec_lbuf2(&quill_pos);
    add_vec_lbuf2(&up_vec);
    add_vec_lbuf2(&hover_e);
    add_vec_lbuf2(&prg_origin);
     

}
 

/******************************************/
/*

    This rebuilds the path that moves the head. 
    It is run prior to the machine moving. 

    program_vecs  = cached vectors loaded from a file represeting a path we want to cut
                    these are seperate from the actual path we will cut so we can build 
                    more complex paths dynamically. 

    rapidmove_vecs = a path to move the head up, over, and back down 
    
    toolpath_vecs   = the actual path that will be "cut". This gets rebuilt each time we run.  

*/

void cnc_plot::update_cache(void)
{
    if(finished==true && running==false)
    {
        toolpath_vecs.clear();
        
        //rebuild the rapid move from current position back to start 
        rapid_move();

        if (rapidmove_vecs.size())
        { 
            //(re)calculate the length of all vectors
            // rapid_dist   = 0;

            //std::cout << "DEBUG - update_cache ADDING rapid vecs \n";            
            for (int v=0;v<rapidmove_vecs.size();v++)
            {
                toolpath_vecs.push_back( rapidmove_vecs.at(v) );
            }
        }

        if (program_vecs.size())
        { 
            //(re)calculate the length of all vectors
            //program_dist = 0;  

            //std::cout << "DEBUG - update_cache ADDING prog vecs \n";            
            for (int v=0;v<program_vecs.size();v++)
            {
                toolpath_vecs.push_back( program_vecs.at(v) );
            }
        
        }    
    }//if program is NOT running or paused

    /*
    std::cout << " ###############################################         \n";
    std::cout << " DEBUG number of path vecs "<< toolpath_vecs.size() << " (rapid + prog) \n";
    std::cout << " DEBUG number of rapid     "<< rapidmove_vecs.size() << "\n";
    std::cout << " DEBUG number of prog vecs "<< program_vecs.size() << "\n";        
    */
    
}


/******************************************/
/*
    The first thing called when the vectors are loaded from disk
    Esentially loads the vectors from the disk, copies them into 

    toolpath_vecs   = the actual path that will be cut 

    program_vecs = cached vectors loaded from a file represeting a path we want to cut
                     these are seperate from the actual path we will cut so we can build 
                     more complex paths dynamically. 

    rapidmove_vecs = a path to move the head up, over, and back down 

*/

void cnc_plot::loadpath( vector<Vector3>* pt_drawvecs, int numdivs)
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

        if(i==pt_drawvecs->size()-1){prg_end = pt_drawvecs->at(i);}

        Vector3 sv  = pt_drawvecs->at(i);
        program_vecs.push_back(sv);
        
    } 
    
    update_cache();
}



/******************************************/
/*
    calc_3d_pulses - translates a 3D vector into 6 electrical signals (step/dir * 3)
   
*/

void cnc_plot::calc_3d_pulses(Vector3 fr_pt, 
                              Vector3 to_pt,
                              int numdivs)
{

    bool debug = true;

    point_ops PG;

    //set the pulses per linear unit (spatial unit divions) - X,Y,Z unit prescaler 
    //for now use one number for all 3 - we will add the others in later
    int pp_lux      = numdivs;
    int pp_luy      = numdivs;
    int pp_luz      = numdivs;

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
    pulsetrain.push_back(Vector3(xp,yp,zp));

    //use the amount of change times the spatial divisions to get the pulses 
    //DEBUG - we may want to use the mag of the 3d vector in here                  
    int num_pul_x = pp_lux*abs(delta_x);
    int num_pul_y = pp_luy*abs(delta_y);
    int num_pul_z = pp_luz*abs(delta_z); 

    if (debug)
        std::cout << "# calc_3d_pulses num pulses " << num_pul_x <<" "<<num_pul_y<<" "<<num_pul_z <<"\n";

    // get the absolute highest number of pulses (on any axis) to calculate 
    int tmp[] = {num_pul_x, num_pul_y, num_pul_z};
    //std::cout << "before: "<<tmp[0] << " "<< tmp[1] <<" "<< tmp[2] <<"\n";
    std::sort(std::begin(tmp), std::end(tmp)  );
    //std::cout << "after: "<<tmp[0] << " "<< tmp[1] <<" "<< tmp[2] <<"\n";
    int most = tmp[2];

    //--------------------------------------//             
    if (debug)
    {            
        std::cout << "#   most   " << most << " "<< numdivs << " " <<"\n";  
        std::cout << "#   numpts " << num_pul_x <<" " << num_pul_y <<" " << num_pul_z <<"\n"; 
        std::cout << "#####\n";
    }

    cnc_plot plot;

    vector<int> calcpt_x;
    vector<int> calcpt_y;
    vector<int> calcpt_z;
               
    // just calc the pulses using a ratio of length to divs. 
    // I experimented with a true 3D method below but this seems to work fine (?)                       
    plot.gen_pules(&calcpt_x, most, num_pul_x);  
    plot.gen_pules(&calcpt_y, most, num_pul_y);  
    plot.gen_pules(&calcpt_z, most, num_pul_z);  

    int a=0;
    for(a=0;a<most;a++)
    {
        pulsetrain.push_back(Vector3(calcpt_x.at(a), calcpt_y.at(a), calcpt_z.at(a)));
        pulsetrain.push_back(Vector3(0,0,0));
    }
    

} 


/******************************************/
/*
   translates two 3d vectors into a pulsetrain , a series of on/off to send to IO 
*/

void cnc_plot::gen_pules(vector<int>* pt_pulsetrain, int size, int num)
{
    
    if(num>size)
    {
        std::cout << "# gen_pules: size arg may not exceed number \n";
        exit(1);
    }

    float div = size/num;

    int a;  

    //if num==zero, pad data with all zeros instead of doing nothing 
    //we want the output to be the same size
    if(num==0)
    {
        for(a=0;a<size;a++)
        {
            pt_pulsetrain->push_back(0);            
        }
    }

    //if only 1 pulse, do something special, put the pulse right in the middle of output 
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
            float chunk = fmod(a,div);
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
/******************************************/

/*

void cnc_plot::calc_3d_pulses(vector<Vector3>* pt_pulsetrain,
                              Vector3 fr_pt, 
                              Vector3 to_pt,
                              int numdivs)
{

    bool debug = false;

    point_ops PG;

    //set the pulses per linear unit (spatial unit divions) - X,Y,Z unit prescaler 
    //for now use one number for all 3 - we will add the others in later
    // int pp_lux      = 10;
    // int pp_luy      = 10;
    // int pp_luz      = 10;
    int pp_lux      = numdivs;
    int pp_luy      = numdivs;
    int pp_luz      = numdivs;


    //make some storage for the data to work in 
    vector<Vector3> x_pts;
    vector<Vector3> y_pts;
    vector<Vector3> z_pts;
    vector<Vector3> samples;

    //make some pointers to those data.
    //(people who say THOSE data are technically correct, but they are pedantic dillholes) 
    vector<Vector3>* pt_xpts    = &x_pts;
    vector<Vector3>* pt_ypts    = &y_pts;
    vector<Vector3>* pt_zpts    = &z_pts;
    vector<Vector3>* pt_samples = &samples;


    //set up variables to do vector-y stuff
    Vector3 between   = sub(fr_pt, to_pt);
    float mag     = length(between);
    float gran    = 0;  //granularity 
    float thresh  = 0;  //threshold 
    //std::cout << mag <<"\n";
    
    int xp=0;int yp=0;int zp=0;

    //calculate the absolute change for each axis  
    float delta_x = fr_pt.x-to_pt.x;
    float delta_y = fr_pt.y-to_pt.y;
    float delta_z = fr_pt.z-to_pt.z;

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
    pt_pulsetrain->push_back(newvec3(xp,yp,zp));

    
    //not totally sure this is right 
    int num_pul_x = (mag*pp_lux)*abs(delta_x);
    int num_pul_y = (mag*pp_luy)*abs(delta_y);
    int num_pul_z = (mag*pp_luz)*abs(delta_z);            

    if (debug)
        std::cout << "# num pulses " << num_pul_x <<" "<<num_pul_y<<" "<<num_pul_z <<"\n";

    // get the absolute highest number of pulses (on any axis) to calculate 
    int tmp[] = {num_pul_x, num_pul_y, num_pul_z};
    //std::cout << "before: "<<tmp[0] << " "<< tmp[1] <<" "<< tmp[2] <<"\n";
    std::sort(std::begin(tmp), std::end(tmp)  );
    //std::cout << "after: "<<tmp[0] << " "<< tmp[1] <<" "<< tmp[2] <<"\n";
    int most = tmp[2];
                         
    // get the smallest division to use for sampling (granularity) 
    if (mag!=0 && most !=0)
    {
        gran = (mag/most);
    }else{
        gran = 0;
    }


    // calculate a series of points along vector for each axis 
    if(num_pul_x!=0)
    {
        PG.locate_pt_along3d(pt_xpts, to_pt, fr_pt, num_pul_x);
    }
    if(num_pul_y!=0) 
    {
        PG.locate_pt_along3d(pt_ypts, to_pt, fr_pt, num_pul_y);
    }
    if(num_pul_z!=0)
    {            
        PG.locate_pt_along3d(pt_zpts, to_pt, fr_pt, num_pul_z);
    }

    //# build a sampleset of all points along the vector - then iterate and match each axis to those points
    //# converting the spatial points into a pulse train 
    if (most!=0 && gran!=0)
    {
        //take the smallest possible sample.. and cut it in half for good measure
        thresh = gran/2;
        
        PG.locate_pt_along3d(pt_samples, to_pt, fr_pt, most);
        
        //DEBUG this "works" but does not distribute the pulses properly due to the 3 loops in a loop
        //the pulses should be evenly spaced across the entirety, and this makes clumps of pulses 
        int a=0;int i=0;
        for(a=0;a<samples.size();a++)
        {

            xp=0;
            yp=0;
            zp=0;

            Vector3 spt = samples[a]; 

            //X 
            for (i=0;i<x_pts.size();i++)
            {
                Vector3 xpt = x_pts[i];
                Vector3 ss = sub(xpt,spt);
                if( length(ss)<thresh)
                {
                    xp=1;
                    //break;
                }
            }

            //Y
            for (i=0;i<y_pts.size();i++)
            {
                Vector3 ypt = y_pts[i];
                Vector3 ss = sub(ypt,spt);
                if( length(ss)<thresh)
                {
                    yp=1;
                    //break;
                }
            }


            //Z
            for (i=0;i<z_pts.size();i++)
            {
                Vector3 zpt = z_pts[i];
                Vector3 ss = sub(zpt,spt);
                if( length(ss)<thresh)
                {
                    zp=1;
                    //break;
                }
            }

            pt_pulsetrain->push_back(newvec3(xp,yp,zp));
            pt_pulsetrain->push_back(newvec3(0,0,0));
        }
     

   }


}

*/

