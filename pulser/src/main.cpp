
#include <iostream>
#include <cstring>
#include <functional>
#include <unistd.h>

#include "point_op.h"         

#include "globals.h"
#include "plot.h" 
#include "parport.h" 

#include "octant.h" 



//GUI RELATED 
//#include "gl_setup.h"
#include "gl_gui.h"

//GUI RELATED 



char* obj_filepath;  


/***************************************/
 
void run_cncplot(char * cfgfile, 
                 double f_x,
                 double f_y,
                 double f_z,
                 double s_x,
                 double s_y,
                 double s_z,
                 int divs)  
{

    bool DEBUG = false; 

    cnc_plot* plot = new cnc_plot;
    cnc_parport* pport = new cnc_parport;
    cncglobals cg;
    
    cg.load_cfg_file(cfgfile); 

    float dummy = 0;

    Vector3 s_p = Vector3(f_x , f_y ,f_z );
    Vector3 e_p = Vector3(s_x , s_y ,s_z );

    // std::cout << "## s vec " <<s_p.x <<" "<< s_p.y <<" "<< s_p.z <<  " \n";
    // std::cout << "## e vec " <<e_p.x <<" "<< e_p.y <<" "<< e_p.z <<  " \n";
    // std::cout << "## divs " << divs <<  " \n";

    plot->calc_3d_pulses(s_p, e_p, divs);

    if(DEBUG==true)
    {
        std::cout << "## run_cncplot debug mode \n";
 
        for(unsigned int x=0;x<plot->pulsetrain.size();x++)
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


 



/***************************************/

int main(int argc, char **argv) 
{  


    //parse_args(argc, argv); 

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

