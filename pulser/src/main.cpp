#include <iostream>
#include <cstring>
#include <functional>

#include <unistd.h>



//#include "math_op.h"        // general math operations
#include "point_op.h"         
#include "cnc_globals.h"

#include "cnc_plot.h" 
#include "gl_setup.h" 

#include "octant.h" 



//#include "demo_cube.h" 



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







/******************************************/

/*
void parse_args(cncglobals* cg,  int argc, char **argv)
{
    if (argc < 8){
        std::cout << "Usage: pulser X1 Y1 Z1 X2 Y2 Z2 divs \n";
        abort();

    }

    //start point (vector)
    float a1 = atof(argv[1]);
    float a2 = atof(argv[2]);
    float a3 = atof(argv[3]);

    //end point (vector)
    float a4 = atof(argv[4]);
    float a5 = atof(argv[5]);
    float a6 = atof(argv[6]);

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
    //spinningCubeDemo(&argc, argv);

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

