#ifndef CNC_GLOBALS_H    
#define CNC_GLOBALS_H

//#include <iostream>
//#include <math.h>

#include <vector>

//using std::vector; 


class cncglobals
{

    public: 
        cncglobals(){
        };

        ~cncglobals(){};


        bool GLOBAL_DEBUG           = false;
        bool DEBUG_PARAMS           = false;

        bool ENABLE_LIMIT_SWITCHES  = false;
        bool ENABLE_MOTOR_DRIVE     = false;


        //----------
        //controller hardware config 
        uint parport1_addr = 0;
        uint parport2_addr = 0;

        uint parport2_mode            = 10; // OUT, INOUT
        uint parport2_bytes_per_frame = 4;  //8 bit (byte) * frame  - per data transaction 
        //uint parport2_latency_us      = 10;

        //----------
        //machine travel size in 3D 
        //std::string linear_unit = "mm"; //"mm" or "inch" 
        char linear_unit[20] = "mm"; //"mm" or "inch" 

        uint x_xtntx = 25;
        uint y_xtntx = 25;
        uint z_xtntx = 25;

        //----------
        //waveform generation parameters
        uint pp1_pulse_dly_us = 200;
        uint pp2_pulse_dly_us = 200;

        //3d pulses per linear unit 
        uint pp1u_x          = 100;
        uint pp1u_y          = 100;
        uint pp1u_z          = 100;

        //----------
        //pin assignments
        uint parprt1_dir_x    = 26;
        uint parprt1_step_x   = 26;  
        uint parprt1_dir_y    = 26;
        uint parprt1_step_y   = 26;  
        uint parprt1_dir_z    = 26;
        uint parprt1_step_z   = 26;
        uint parprt1_dir_a    = 26;
        uint parprt1_step_a   = 26;    
        
        uint x_limit_pin      = 26;
        uint y_limit_pin      = 26;
        uint z_limit_pin      = 26;


        /*******************/
        void load_cfg_file( char*);
        void show( void );
        

};




#endif



