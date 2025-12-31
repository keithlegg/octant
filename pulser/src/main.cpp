
#include <iostream>
#include <cstring>

//#include <thread>

#include <unistd.h>


#include "point_op.h"         
#include "globals.h"
#include "plot.h" 
#include "parport.h" 
#include "octant.h" 



#if DO_BUILD_GUI == true
    #include <functional>
    #include "gl_gui.h"
#endif



char* obj_filepath;  

extern cnc_parport parport;
extern cncglobals cg;






/***************************************/

/*
void parse_args(int argc, char **argv)
{
    if (argc < 8){
        std::cout << "Usage: pulser X1 Y1 Z1 X2 Y2 Z2 divs \n";
        abort();

    }

    //start point (vector)
    double a1 = atof(argv[2]);
    double a2 = atof(argv[3]);
    double a3 = atof(argv[4]);

    //end point (vector)
    double a4 = atof(argv[5]);
    double a5 = atof(argv[6]);
    double a6 = atof(argv[7]);

    //number of divisions in X,Y,Z space
    int a7    = atoi(argv[8]);
              
    run_cncplot( argv[1], a1, a2, a3, a4, a5, a6, a7 );

}
*/

/***************************************/

/*
    void pulse_gen(char **argv)
    {
        
        cg.load_cfg_file(argv[1]);
        parport.speed_test(&cg, 1000000);

    }


    std::thread pgen_thread(pulse_gen, argv);

    // you should always either join or detach a thread before the flows of execution reaches the destructor.
    pgen_thread.detach();  


*/



/***************************************/

int main(int argc, char **argv) 
{  

    //call2();


    //parse_args(argc, argv); 
    /*------------------*/   

    #if DO_BUILD_GUI == true
        if( argc == 2 ) {
            obj_filepath = argv[1] ;

            if( access( obj_filepath, F_OK ) != -1 ) {
                start_gui(&argc, argv); 
            } else {
                printf("# file %s does not exist.\n", obj_filepath);
                exit(0);
            }
        }  
    #endif

    /*------------------*/

    /*
    // test of limit switch reading 
    cnc_parport plot;
    Vector3 foo = Vector3(0,0,0);
    // void cnc_parport::read_limits(cncglobals* cg, Vector3* pt_limit_switch_data)
    plot.read_limits(&foo);
    */


    /*------------------*/    
    return 0;
}

