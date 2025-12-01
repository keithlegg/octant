

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <functional>

#include <unistd.h>



#include "math_op.h"        // general math operations
#include "point_op.h"         
#include "cnc_globals.h"

#include "cnc_plot.h" 
#include "gl_setup.h" 

//#include "demo_cube.h" 

#include "confusion_36d.h" 




/* 
extern int PARPORT1_ADDR;
extern int PARPORT2_ADDR;
extern int PP1_PULSE_DLY_US;
extern int PP1_STEPS_PER_UNIT_X;
extern int PP1_STEPS_PER_UNIT_Y;
extern int PP1_STEPS_PER_UNIT_Z;
*/ 


// window properties
int window_id;      // ID if GLUT window 
int scr_size_x      = 512;
int scr_size_y      = 512;
bool scr_full_toglr = true;
char* obj_filepath;  

GLuint texture[3]; // storage for one texture  



/*******************************************************/

// data for animated "pong" texture 
//democube uses it - probably get rid of that 
unsigned int upos = 100;
unsigned int vpos = 320;  
unsigned int dir_u = 0;
unsigned int dir_v = 0;
unsigned int pong_speed = 5;
unsigned int pong_size  = 60;
unsigned int img_usize  = 512;
unsigned int img_vsize  = 512;

unsigned int use_tex = 0;


/******************************************/
//command line tool to generate XYZ pulses from 2 vectors 

void run_cncplot(cncglobals* cg,
                 double f_x,
                 double f_y,
                 double f_z,
                 double s_x,
                 double s_y,
                 double s_z,
                 int divs)  
{


    cnc_plot plot;
    
    vector<vec3> pulsetrain;
    vector<vec3>* pt_pulsetrain = &pulsetrain; 

    vec3 s_p = newvec3(f_x , f_y ,f_z );
    vec3 e_p = newvec3(s_x , s_y ,s_z );

    plot.calc_3d_pulses(pt_pulsetrain, s_p, e_p, divs);

    if(cg->GLOBAL_DEBUG==true)
    {
        int x=0;
        for(x=0;x<pulsetrain.size();x++)
        {
            std::cout<<pulsetrain[x].x  <<" "<<pulsetrain[x].y  <<" "<<pulsetrain[x].z   << "\n";        
        } 
    }

    if(cg->GLOBAL_DEBUG==false)
    {
       //moved to IO DEBUG   
       // plot.send_pulses(pt_pulsetrain);
    }

}


/******************************************/

/*
void parse_args(cncglobals* cg,  int argc, char **argv)
{
    if (argc < 8){
        std::cout << "Usage: pulser X1 Y1 Z1 X2 Y2 Z2 divs \n";
        abort();

    }

    //start point (vector)
    double a1 = atof(argv[1]);
    double a2 = atof(argv[2]);
    double a3 = atof(argv[3]);

    //end point (vector)
    double a4 = atof(argv[4]);
    double a5 = atof(argv[5]);
    double a6 = atof(argv[6]);

    //number of divisions in X,Y,Z space
    int a7    = atoi(argv[7]);
              
    run_cncplot( cg, a1, a2, a3, a4, a5, a6, a7 );

}
*/


/******************************************/
/******************************************/
 
int main(int argc, char **argv) 
{  

    
    /*------------------*/    
    if( argc == 2 ) {
        obj_filepath = argv[1] ;

        if( access( obj_filepath, F_OK ) != -1 ) {
            start_gui(&argc, argv); 
        } else {
            printf("# file %s does not exist.\n", obj_filepath);
            exit(0);
        }
    }

    /*------------------*/
    //spinningCubeDemo(&argc, argv);

    /*------------------*/
    // parse_args(cg, argc, argv); 
    // if(sizeof(argv[1]) == sizeof(char *))
    //    std::cout << "empty " << argc << "\n";

    /*------------------*/
    // test of limit switch reading 
    //cnc_plot plot;
    //vec3 foo;
    //plot.read_limits(&foo);
    


    return 0;
}



/******************************************/
/******************************************/

/*

    //      OLD STUFF TO PLAY WITH   


void test_cncplot(void)
{
    cnc_plot plot;

    vector<vec3> pulsetrain;
    vector<vec3>* pt_pulsetrain = &pulsetrain; 

    vec3 s_p = newvec3(0.0 , 0.0 ,0.0 );
    vec3 e_p = newvec3(1.0 , 0.5 ,0.0);
    plot.calc_3d_pulses(pt_pulsetrain, s_p, e_p, 2);

    //hopefully, we have pulses - light em up!
    plot.send_pulses(pt_pulsetrain);


}

//////////////////////////////////////






vector<vec3> scene_drawvec3;
vector<vec3> scene_drawvecclr;   
int num_drawvec3; 

void test_bezier( vec3 start, vec3 ctrl1, vec3 ctrl2, vec3 end)
{
    
    // USAGE:
    // vec3 start = newvec3(0.0 ,3.0 ,1.0 );
    // vec3 ctrl1 = newvec3(2.5  ,0.0 ,0.0 );
    // vec3 ctrl2 = newvec3(0.0 ,1.0  ,0.0 );
    // vec3 end   = newvec3(-1.0 ,0.0 ,-5.0 );
    // test_bezier(start, ctrl1, ctrl2, end);
    // int a=0;
    // for(a=0;a<scene_drawvec3.size();a++){
    //     std::cout<<scene_drawvec3[a].x  <<" "<<scene_drawvec3[a].y  <<" "<<scene_drawvec3[a].z   << "\n";
    // }

    pointgen PG;

    vector<vec3> * ptDrawvec = &scene_drawvec3;
    vector<vec3> * ptDrawClr = &scene_drawvecclr;
    int * ptnum_drawvec3 = &num_drawvec3;
    
    PG.cubic_bezier(ptDrawvec, ptDrawClr,  ptnum_drawvec3, 10, start, ctrl1, ctrl2, end);

}

void test_pt_along(void)
{

    pointgen PG;

    vector<vec3> * ptDrawvec = &scene_drawvec3;
    vec3 s_p = newvec3(0.0 ,0.0 ,0.0 );
    vec3 e_p = newvec3(7.0 ,0.1 ,15.0);
    
    PG.locate_pt_along3d(ptDrawvec, s_p, e_p, 5);
    int a=0;
    for(a=0;a<scene_drawvec3.size();a++){
        std::cout<<scene_drawvec3[a].x  <<" "<<scene_drawvec3[a].y  <<" "<<scene_drawvec3[a].z   << "\n";
    } 

}


*/
