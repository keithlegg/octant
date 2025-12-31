#ifndef CNC_GLOBALS_H    
#define CNC_GLOBALS_H

#include <vector>

//**********************************//
//option for a gui-less, "headless" tool  
/*
   if you do this, remove:
       all files prefixed with "gl_XXX"
       parse_cmds.cpp
*/
#define DO_BUILD_GUI true
#define DEBUG_MOUSE_CLICK false


//**********************************//






void exit_program(void);




class cncglobals
{

    public: 
        cncglobals()
        {
            active_filepath="";
            GLOBAL_DEBUG           = true;
            //DEBUG_PARAMS           = false;
            ENABLE_LIMIT_SWITCHES  = false;
            ENABLE_MOTOR_DRIVE     = false;

            ply_count              = 0; 
            active_polygon_load    = false; 

        };

        ~cncglobals(){};
        
        unsigned int ply_count;
        bool active_polygon_load;

        bool GLOBAL_DEBUG;
        //bool DEBUG_PARAMS;
        bool ENABLE_LIMIT_SWITCHES;
        bool ENABLE_MOTOR_DRIVE;

        //----------
        //std::vector<std::string> obj_vecpaths;
        std::vector<std::string> obj_filepaths;

        std::string active_filepath;

        //----------
        //controller hardware config 
        uint parport1_addr = 0;
        uint parport2_addr = 0;

        uint parport2_mode            = 10; // OUT, INOUT
        uint parport2_bytes_per_frame = 4;  //8 bit (byte) * frame  - per data transaction 
        //uint parport2_latency_us      = 10;

        //----------
        //machine travel size in 3D 
        std::string linear_unit = "mm"; //"mm" or "inch" 

        float retract_height = 0;
        float work_height    = 0;

        float x_xtntx = 25;
        float y_xtntx = 25;
        float z_xtntx = 25;

        //----------
        //waveform generation parameters
        uint pp1_pulse_dly_us = 200;
        uint pp2_pulse_dly_us = 200;

        //3d pulses per linear unit 
        uint pp1u_x           = 100;
        uint pp1u_y           = 100;
        uint pp1u_z           = 100;

        //----------
        //pin assignments port1
        uint parprt1_dir_x    = 26;
        uint parprt1_step_x   = 26;  
        uint parprt1_dir_y    = 26;
        uint parprt1_step_y   = 26;  
        uint parprt1_dir_z    = 26;
        uint parprt1_step_z   = 26;
        uint parprt1_dir_a    = 26;
        uint parprt1_step_a   = 26;  
        uint parprt1_dir_x_inv   = false;
        uint parprt1_step_x_inv  = false;  
        uint parprt1_dir_y_inv   = false;
        uint parprt1_step_y_inv  = false;  
        uint parprt1_dir_z_inv   = false;
        uint parprt1_step_z_inv  = false;
        uint parprt1_dir_a_inv   = false;
        uint parprt1_step_a_inv  = false;    

        //pin assignments port2
        //these are not used for now - port1 is for axis control 
        /*
        uint parprt2_dir_x    = 26;
        uint parprt2_step_x   = 26;  
        uint parprt2_dir_y    = 26;
        uint parprt2_step_y   = 26;  
        uint parprt2_dir_z    = 26;
        uint parprt2_step_z   = 26;
        uint parprt2_dir_a    = 26;
        uint parprt2_step_a   = 26;           
        bool parprt2_dir_x_inv    = false;
        bool parprt2_step_x_inv   = false;  
        bool parprt2_dir_y_inv    = false;
        bool parprt2_step_y_inv   = false;  
        bool parprt2_dir_z_inv    = false;
        bool parprt2_step_z_inv   = false;
        bool parprt2_dir_a_inv    = false;
        bool parprt2_step_a_inv   = false;   
        */


        // Output pins 1, 2, 3, 4, 5, 6, 7, 8, 9, 14, 16, 17. Or 1, 14, 16, 17.
        // Input pins 10, 11, 12, 13, 15. Or 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15.
        // Input pins 10, 11, 12, 13, 15.
        /*
        uint parprt2_GPIO1     = 26;
        uint parprt2_GPIO1_inv = false;
        uint parprt2_GPIO1_ddr = true;
        uint parprt2_GPIO2     = 26;
        uint parprt2_GPIO2_inv = false;
        uint parprt2_GPIO2_ddr = true;
        uint parprt2_GPIO3     = 26;
        uint parprt2_GPIO3_inv = false;
        uint parprt2_GPIO3_ddr = true;        
        uint parprt2_GPIO4     = 26;
        uint parprt2_GPIO4_inv = false;
        uint parprt2_GPIO4_ddr = true;        
        uint parprt2_GPIO5     = 26;
        uint parprt2_GPIO5_inv = false;
        uint parprt2_GPIO5_ddr = true;        
        uint parprt2_GPIO6     = 26;
        uint parprt2_GPIO6_inv = false;
        uint parprt2_GPIO6_ddr = true;        
        uint parprt2_GPIO7     = 26;
        uint parprt2_GPIO7_inv = false;
        uint parprt2_GPIO7_ddr = true;        
        uint parprt2_GPIO8     = 26;
        uint parprt2_GPIO8_inv = false;
        uint parprt2_GPIO8_ddr = true;        
        uint parprt2_GPIO9     = 26;
        uint parprt2_GPIO9_inv = false;  
        uint parprt2_GPIO9_ddr = true;                                                       
        uint parprt2_GPIO10     = 26;
        uint parprt2_GPIO10_inv = false; 
        uint parprt2_GPIO10_ddr = true;         
        uint parprt2_GPIO11     = 26;
        uint parprt2_GPIO11_inv = false; 
        uint parprt2_GPIO11_ddr = true;
        uint parprt2_GPIO12     = 26;
        uint parprt2_GPIO12_inv = false; 
        uint parprt2_GPIO12_ddr = true;
        */

        //pin assignments limit switches
        uint x_limitsw_pin  = 26;
        bool x_limitsw_pin_inv = false;
        uint y_limitsw_pin  = 26;
        bool y_limitsw_pin_inv = false;
        uint z_limitsw_pin  = 26;
        bool z_limitsw_pin_inv = false;


        /*******************/
        std::vector<std::string> tokenizer( const std::string&, char);
        int cvt_int( const std::string&);

        //void copy_file_vecs_display(void);

        void load_objects(void);
        void load_cfg_file( char*);
        void show_params( void );
        void show_obj( void );

        // check that all axis are set after running  
        //void verify_axis_set( void );

        // build bitmasks from loded parameters  
        //void build_xlimit_inputs ( void );                
        //void build_ylimit_inputs ( void );  
        //void build_zlimit_inputs ( void );  
        //void build_xlimit_inputs ( void );                
        //void build_ylimit_inputs ( void );  
        //void build_zlimit_inputs ( void );  


};




#endif



