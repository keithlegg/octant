

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>



#include "math_op.h"        // general math operations
#include "point_op.h"         
#include "cnc_plot.h"         

//#include "gl_setup.h"     // common to all - moved to reduce size 
//char* obj_filepath;  


/******************************************/

void run_cncplot(double f_x,
                 double f_y,
                 double f_z,
                 double s_x,
                 double s_y,
                 double s_z,
                 int divs)  
{
    cnc_plot plot;

    bool DEBUG = false; 

    vector<vec3> pulsetrain;
    vector<vec3>* pt_pulsetrain = &pulsetrain; 

    vec3 s_p = newvec3(f_x , f_y ,f_z );
    vec3 e_p = newvec3(s_x , s_y ,s_z );

    plot.calc_3d_pulses(pt_pulsetrain, s_p, e_p, divs);

    if(DEBUG==true)
    {
        int x=0;
        for(x=0;x<pulsetrain.size();x++)
        {
            cout<<pulsetrain[x].x  <<" "<<pulsetrain[x].y  <<" "<<pulsetrain[x].z   << "\n";        
        } 
    }

    if(DEBUG==false)
    {
        plot.send_pulses(pt_pulsetrain);
    }

}

/******************************************/

void parse_args(int argc, char **argv)
{
    if (argc < 8){
        cout << "Usage: pulser X1 Y1 Z1 X2 Y2 Z2 divs \n";
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
              
    run_cncplot( a1, a2, a3, a4, a5, a6, a7 );

}


/******************************************/
/******************************************/

int main(int argc, char **argv) 
{  
    
    parse_args(argc, argv); 
    
    
    /*
    //test of limit switch reading 
    cnc_plot plot;
    vec3 foo;
    plot.read_limits(&foo);
    */


    /*
    //test of config file 
    cnc_plot plot;
    plot.load_config_file("testcfg.txt");
    */


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
    //     cout<<scene_drawvec3[a].x  <<" "<<scene_drawvec3[a].y  <<" "<<scene_drawvec3[a].z   << "\n";
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
        cout<<scene_drawvec3[a].x  <<" "<<scene_drawvec3[a].y  <<" "<<scene_drawvec3[a].z   << "\n";
    } 

}


*/
