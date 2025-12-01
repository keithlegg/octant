/*************************************************************/
/*
    cnc_globals.cpp 
    
    All the configuration details and machine paramenters live here, accessed globally. 
    Also deals with the loading (saving?) of machine parameters

    Copyright (C) 2026 Keith Legg - keithlegg23@gmail.com

*/
/*************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem> 

#include <string>
#include <cstring>

#include <vector>
using std::vector;

#include "math_op.h"
#include "point_op.h"

#include "obj_model.h"
#include "cnc_plot.h"
#include "cnc_io.h"
#include "cnc_globals.h"


/*

#include <vector>
#include <iostream>
#include <fstream>

#include <algorithm> // for std::find

#include "obj_model.h"
#include "gl_setup.h"


// USE MAKEFILE TO SET THIS 
#ifdef _ISLINUX
    #include <bits/stdc++.h> 
#else
    #include "bits_stdc++.h" 
#endif
*/




char strbuffer[100][100];

std::vector<vec3> scene_drawvec3;
std::vector<vec3> scene_drawvecclr;
int num_drawvec3 = 0;

std::vector<vec3> scene_drawpoints;
std::vector<vec3> scene_drawpointsclr;
std::vector<vec3>* pt_scene_drawpoints = &scene_drawpoints;
int num_drawpoints = 0;


const int MAX_CHARS_PER_LINE = 512;
const int MAX_TOKENS_PER_LINE = 20;

extern obj_model pt_model_buffer;


/*
  std::cout
    << std::left
    << std::showbase
    << std::hex
    << std::uppercase
    << std::setfill('0')
    << std::setw(std::numeric_limits<decltype(ia)>::digits / 4)
    << ia
    << std::endl;
  std::cout.flags(save_flags);
*/

/**********************************/
void cncglobals::show( void )
{
    std::cout <<"\n\n";
    std::cout << " #### CNC GLOBALS #### " << "\n";

    //capture the state of cout 
    std::ios_base::fmtflags f( std::cout.flags() );  
    
    //print some neato 16 bit hex addresses 
    std::cout << std::hex;
    std::cout << " parport1_addr : " << (*this).parport1_addr << "\n";
    std::cout << " parport2_addr : " << (*this).parport2_addr << "\n";

    //set it back when we are done 
    std::cout.flags( f );
    std::cout <<"\n";
    
    std::cout << " ## machine params " << "\n";  
    //std::cout << " linear_unit : " << (*this).linear_unit << "\n";  

    std::cout << " ## machine travel size in 3D  " << "\n";  
    std::cout << " x_xtntx : " << (*this).x_xtntx<< "\n";
    std::cout << " y_xtntx : " << (*this).y_xtntx<< "\n";
    std::cout << " z_xtntx : " << (*this).z_xtntx<< "\n";
    std::cout <<"\n";

    std::cout << " ## waveform generation parameters " << "\n";  
    std::cout << " pp1_pulse_dly_us : " << (*this).pp1_pulse_dly_us << "\n";
    std::cout <<"\n";

    std::cout << " ## pulses per linear unit (x,y,z)" << "\n";        
    std::cout << " pp1u_x : " << (*this).pp1u_x << "\n";
    std::cout << " pp1u_y : " << (*this).pp1u_y << "\n";
    std::cout << " pp1u_z : " << (*this).pp1u_z << "\n";
    std::cout <<"\n";

    //std::cout << std::boolalpha;
    std::cout << " ## DB25 pin assignments " << "\n";  
    std::cout << " parprt1_dir_x  : " << (*this).parprt1_dir_x  << " inverted: "<< (*this).parprt1_dir_x_inv  << "\n";
    std::cout << " parprt1_step_x : " << (*this).parprt1_step_x << " inverted: "<< (*this).parprt1_step_x_inv << "\n";
    std::cout << " parprt1_dir_y  : " << (*this).parprt1_dir_y  << " inverted: "<< (*this).parprt1_dir_y_inv  << "\n";
    std::cout << " parprt1_step_y : " << (*this).parprt1_step_y << " inverted: "<< (*this).parprt1_step_y_inv << "\n";
    std::cout << " parprt1_dir_z  : " << (*this).parprt1_dir_z  << " inverted: "<< (*this).parprt1_dir_z_inv  << "\n";
    std::cout << " parprt1_step_z : " << (*this).parprt1_step_z << " inverted: "<< (*this).parprt1_step_z_inv << "\n";
    std::cout << " parprt1_dir_a  : " << (*this).parprt1_dir_a  << " inverted: "<< (*this).parprt1_dir_a_inv  << "\n";
    std::cout << " parprt1_step_a : " << (*this).parprt1_step_a << " inverted: "<< (*this).parprt1_step_a_inv << "\n";

    std::cout << "\n"; 
    /*
    std::cout << " parprt2_dir_x  : " << (*this).parprt2_dir_x  << " inverted: "<< (*this).parprt2_dir_x_inv  << "\n";
    std::cout << " parprt2_step_x : " << (*this).parprt2_step_x << " inverted: "<< (*this).parprt2_step_x_inv << "\n";
    std::cout << " parprt2_dir_y  : " << (*this).parprt2_dir_y  << " inverted: "<< (*this).parprt2_dir_y_inv  << "\n";
    std::cout << " parprt2_step_y : " << (*this).parprt2_step_y << " inverted: "<< (*this).parprt2_step_y_inv << "\n";
    std::cout << " parprt2_dir_z  : " << (*this).parprt2_dir_z  << " inverted: "<< (*this).parprt2_dir_z_inv  << "\n";
    std::cout << " parprt2_step_z : " << (*this).parprt2_step_z << " inverted: "<< (*this).parprt2_step_z_inv << "\n";
    std::cout << " parprt2_dir_a  : " << (*this).parprt2_dir_a  << " inverted: "<< (*this).parprt2_dir_a_inv  << "\n";
    std::cout << " parprt2_step_a : " << (*this).parprt2_step_a << " inverted: "<< (*this).parprt2_step_a_inv << "\n";
    */

    std::cout << "\n"; 
    std::cout << " x_limit_pin    : " << (*this).x_limitsw_pin << " inverted: "<< (*this).x_limitsw_pin_inv << "\n";
    std::cout << " y_limit_pin    : " << (*this).y_limitsw_pin << " inverted: "<< (*this).y_limitsw_pin_inv << "\n";
    std::cout << " z_limit_pin    : " << (*this).z_limitsw_pin << " inverted: "<< (*this).z_limitsw_pin_inv << "\n";
    
    //std::cout << " : " << (*this). << "\n";
    //std::cout << " : " << (*this). << "\n";    

  
};


/**********************************/

//char to string and back 
// auto my_cstr = "Hello";        // A
// std::string s(my_cstr);        // A
// // ... modify 's' ...
// auto back_to_cstr = s.c_str(); // B



/* 
// warning: untested code.

std::vector<std::string> split(std::string const &input) { 
    std::istringstream buffer(input);
    std::vector<std::string> ret;

    std::copy(std::istream_iterator<std::string>(buffer), 
              std::istream_iterator<std::string>(),
              std::back_inserter(ret));
    return ret;
}


int splitstr(char *sentence)
{
  std::stringstream ss(sentence);
  std::string to;

  if (sentence != NULL)
  {
    while(std::getline(ss,to,'\n')){
      std::cout << to << std::endl;
    }
  }

return 0;
}

*/

/**********************************/
/**********************************/
//string utility functions - experimental

//convert char * to string type 

//method 1
// std::string foo = *t;

// method 2
// const char *s = "Hello, World!";
// std::string str(s);

//parse 3 floats at a time 
//campos = Vector3( atof(token[1]), atof(token[2]), atof(token[3]) );

/**************/


char* cncglobals::copyString(char s[])
{
    char* s2;
    s2 = (char*)malloc(20);

    strcpy(s2, s);
    return (char*)s2;
}

/******************************/

std::vector<std::string> cncglobals::tokenizer( const std::string& p_pcstStr, char delim )  {
    std::vector<std::string> tokens;
    std::stringstream   mySstream( p_pcstStr );
    std::string         temp;
    while( getline( mySstream, temp, delim ) ) {
        tokens.push_back( temp );
    }
    return tokens;
} 

/******************************/


int cncglobals::cvt_int( const std::string& s)
{
    std::size_t pos{};
    const int i{std::stoi(s, &pos)};
    std::cout << i << "; pos: " << pos << '\n';
    
       
    try
    {
        std::cout << "std::stoi(" << std::quoted(s) << "): ";
        const int i{std::stoi(s, &pos)};
        std::cout << i << "; pos: " << pos << '\n';
        return i;

    }
    catch (std::invalid_argument const& ex)
    {
        std::cout << "std::invalid_argument::what(): " << ex.what() << '\n';
    }
    /*
    catch (std::out_of_range const& ex)
    {
        std::cout << "std::out_of_range::what(): " << ex.what() << '\n';
        const long long ll{std::stoll(s, &pos)};
        std::cout << "std::stoll(" << std::quoted(s) << "): " << ll
                  << "; pos: " << pos << '\n';
    } 
    */ 
    return 0;

}

/**********************************/
/**********************************/
void cncglobals::show_obj(void)
{
    
    pt_model_buffer.show();
    pt_model_buffer.show_geom();

}



void cncglobals::load_objects(void)
{
    if(!(*this).obj_filepaths.empty()) 
    {
        char char_array[100];
        int x = 0 ; 

        //DEBUG THIS BLOWS UP (OR QUIETLY DIES)
        //for(x=0;x<(*this).num_loaded_obj;x++)
        for(x=0;x<(*this).obj_filepaths.size();x++)
        {
            //std::cout << "#### load_objects loading  " << (*this).obj_filepaths[x] <<"\n";
            strcpy(char_array, (*this).obj_filepaths[x].c_str()); 

            std::cout << "##### DEBUG load_objfile resetting file internals \n";
            pt_model_buffer.reset();
            pt_model_buffer.load(char_array);
        };
    };
}


void cncglobals::load_cfg_file( char* filepath )
{
    std::cout << "cncglobals loading file "<< filepath << "\n";

    std::ifstream cfg_filein;

    //DEBUG - need to check to make sure its a file, not a directory 
    /*
    const std::string pathString = std::to_string(filepath);
    const fs::path path(pathString); 
    std::error_code ec;  

    if (fs::is_directory(path, ec))
    { 
        // Process a directory.
    }
    if (ec) // Optional handling of possible errors.
    {
        std::cerr << "Error in is_directory: " << ec.message();
    }
    if (fs::is_regular_file(path, ec))
    {
        // Process a regular file.
    }
    if (ec) // Optional handling of possible errors. Usage of the same ec object works since fs functions are calling ec.clear() if no errors occur.
    {
        std::cerr << "Error in is_regular_file: " << ec.message();
    }*/


    cfg_filein.open(filepath); // open a file
    if (!cfg_filein.good()){ 
        std::cout << "config file \""<< filepath <<"\" appears to be missing." << std::endl;
        exit (EXIT_FAILURE); // exit if file not found
    }

    int n = 0;

    int line_ct = 0;
    while (!cfg_filein.eof())
    {   
        //26 is a sign the pin is unassigned (only 0-24 on the plug) 
        uint parsepin = 26;

        std::string line;
        while (std::getline(cfg_filein, line)) 
        {  
            // std::cout << "FULL LINE " << line << std::endl;
            std::vector<std::string>  tokenized = (*this).tokenizer(line, *" ");

            //if you want to iterate the tokens on the line 
            // for (vector<std::string>::iterator t=tokenized.begin(); t!=tokenized.end(); ++t) 
            // {
            //     std::cout << "TOKEN "<<*t<<std::endl;
            // }

            //if line is not blank
            if(tokenized.size()>0)
            {
                int commentpos = tokenized.at(0).find('#');

                //if ( commentpos != std::string::npos)
                if ( commentpos == 0)
                {
                    //line is commented out  
                }
                else
                {
                    if(tokenized.size()>=1)
                    {

                        //** MACHINE HARDWARE SETUP ************//
                        if (tokenized.at(0).find("LINEAR_UNIT") != std::string::npos )                            
                        {  
                            std::string buffer;
                            std::cout << "DEBUG parsing LINEAR_UNIT " << tokenized.at(1) << "\n";
                            buffer = tokenized.at(1);
                            
                            //linear_unit = (*this).copyString(tokenized.at(1));
                            std::cout << "DEBUG parsing LINEAR_UNIT " << linear_unit << "\n";
                        }


                        /***************************************/ 
                        //** MACHINE HARDWARE SETUP ************//
                        if (tokenized.at(0).find("PARPORT1_ADDR") != std::string::npos )                            
                        {   
                            //unfinished tokenize funtion with error catching   
                            //(*this).cvt_int(tokenized.at(1));

                            //just do this for now - no error catching 
                            parport1_addr = std::stoi( tokenized.at(1) );

                        }
                        //***************************************/ 
                        if (tokenized.at(0).find("PARPORT2_ADDR") != std::string::npos)
                        {        
                            parport2_addr = std::stoi( tokenized.at(1) );
                        }
                        
                        //***************************************/ 
                        //**  MACHINE PARAMETERS SETUP *********//

                        if (tokenized.at(0).find("X_XTNX") != std::string::npos)
                        {        
                            x_xtntx = std::stof( tokenized.at(1) );                            
                        }
                        if (tokenized.at(0).find("Y_XTNX") != std::string::npos)
                        {        
                            y_xtntx = std::stof( tokenized.at(1) );  
                        }
                        if (tokenized.at(0).find("Z_XTNX") != std::string::npos)
                        {        
                            z_xtntx = std::stof( tokenized.at(1) );  
                        }

                        //-------------------------------------------
                        //-- PULSE TIMING ---------------
                        if (tokenized.at(0).find("PP1_PULSE_DLY_US") != std::string::npos)
                        {    
                            pp1_pulse_dly_us = std::stoi( tokenized.at(1) );      
                        }

                        if (tokenized.at(0).find("PP2_PULSE_DLY_US") != std::string::npos)
                        {    
                            pp2_pulse_dly_us = std::stoi( tokenized.at(1) );      
                        }

                        //-------------------------------------------
                        //-- 3D SPATIAL DIVISIONS ----------
                        if (tokenized.at(0).find("PP1_STEPS_PER_UNIT_X") != std::string::npos )
                        {        
                            pp1u_x = std::stoi( tokenized.at(1) );   
                        }
                        if (tokenized.at(0).find("PP1_STEPS_PER_UNIT_Y") != std::string::npos)
                        {        
                            pp1u_y = std::stoi( tokenized.at(1) );   
                        }
                        if (tokenized.at(0).find("PP1_STEPS_PER_UNIT_Z") != std::string::npos)
                        {        
                            pp1u_z = std::stoi( tokenized.at(1) );   
                        }


                        //****************************************/
                        // RUN SCRIPT OPERATORS IN HERE 
                        //****************************************/                                    

                        // previously named load_scene() 
                        if (tokenized.at(0).find("op_loadobj")!= std::string::npos)
                        {   
                            std::cout << "\n#LOAD OPERATOR ! - object found " << tokenized.at(1) << "\n";

                            obj_filepaths.push_back(tokenized.at(1));

                            // //pt_model_buffer->calc_normals();
                            // //strcpy(active_filepath, char_array ); 
                            // this blows up - DEBUG  
                            std::cout <<  (*this).num_loaded_obj;
                            //std::cout << "#ADDED OBJ " << (*this).num_loaded_obj << " "<< obj_filepaths.at((*this).num_loaded_obj) << "\n";
                            


                        }

                        //------------------------------------------------
                        //-- CONTROLLER PORT INPUTS       ----------------
                        bool doinvert = false;

                        if(tokenized.size()>=2)
                        { 
                            //iterate the tokens on the line to get "inverted" parameter 
                            for (vector<std::string>::iterator t=tokenized.begin(); t!=tokenized.end(); ++t) 
                            {
                                //you can do a direct macth if no substring  
                                if (*t=="INVERT"){
                                    doinvert=true;
                                }
                            }

                            //do it one more time and put it all together now
                            for (vector<std::string>::iterator t=tokenized.begin(); t!=tokenized.end(); ++t) 
                            {
                                std::string getdbpins = *t;
        
                                if (getdbpins.find("DB25_") != std::string::npos)
                                {

                                    /******************************************/
                                    std::vector<std::string>  tokepins = (*this).tokenizer(getdbpins, *"_");  
                                    if(tokepins.size()>1)
                                    {     
                                        //----------------------------------------------
                                        // std::cout << "DB25_! " << tokenized.at(0) << " "<< tokepins.at(1) << " INVERTED  "<< doinvert << "\n";
                                        // DEBUG - THESE WILL CRASH IF N.A.N. 
                                        
                                        //std::cout << "DEBUG DB25 PIN IS " << std::stoi(tokepins.at(1))  << "\n";

                                        //----------------------------------------------

                                          
                                        if (tokenized.at(0)=="X_LIMIT_SW")
                                        {   
                                            x_limitsw_pin     = std::stoi( tokepins.at(1));
                                            x_limitsw_pin_inv = doinvert;
                                        }

                                        if (tokenized.at(0)=="Y_LIMIT_SW")
                                        {   
                                            y_limitsw_pin = std::stoi( tokepins.at(1));
                                            y_limitsw_pin_inv = doinvert;                                            
                                        }
                                        
                                        if (tokenized.at(0)=="Z_LIMIT_SW")
                                        {   
                                            z_limitsw_pin = std::stoi( tokepins.at(1));
                                            z_limitsw_pin_inv = doinvert;                                            
                                        }
                                       
                                        //----------------------------------------------
                                        //-- PARALLEL PORT1 PIN ASSIGNMENTS ------------
                                        if (tokenized.at(0).find("PP1_XDIR")!= std::string::npos)
                                        {        
                                            parprt1_dir_x = std::stoi( tokepins.at(1));
                                            parprt1_dir_x_inv = doinvert;                                                
                                        }
                                        if (tokenized.at(0).find("PP1_XSTEP")!= std::string::npos)
                                        {        
                                            parprt1_step_x = std::stoi( tokepins.at(1));
                                            parprt1_step_x_inv = doinvert;     
                                        }

                                        if (tokenized.at(0).find("PP1_YDIR")!= std::string::npos)
                                        {        
                                            parprt1_dir_y = std::stoi( tokepins.at(1));
                                            parprt1_dir_y_inv = doinvert;                                                
                                        }
                                        if (tokenized.at(0).find("PP1_YSTEP")!= std::string::npos)
                                        {        
                                            parprt1_step_y = std::stoi( tokepins.at(1));
                                            parprt1_step_y_inv = doinvert;     
                                        }

                                        if (tokenized.at(0).find("PP1_ZDIR")!= std::string::npos)
                                        {        
                                            parprt1_dir_z = std::stoi( tokepins.at(1));
                                            parprt1_dir_z_inv = doinvert;                                                
                                        }
                                        if (tokenized.at(0).find("PP1_ZSTEP")!= std::string::npos)
                                        {        
                                            parprt1_step_z = std::stoi( tokepins.at(1));
                                            parprt1_step_z_inv = doinvert;     
                                        }

                                        if (tokenized.at(0).find("PP1_ADIR")!= std::string::npos)
                                        {        
                                            parprt1_dir_a = std::stoi( tokepins.at(1));
                                            parprt1_dir_a_inv = doinvert;                                                
                                        }
                                        if (tokenized.at(0).find("PP1_ASTEP")!= std::string::npos)
                                        {        
                                            parprt1_step_a = std::stoi( tokepins.at(1));
                                            parprt1_step_a_inv = doinvert;     
                                        }
                                                                                                                        
                                        //----------------------------------------------
                                        //-- PARALLEL PORT2 PIN ASSIGNMENTS ------------
                                        /*
                                        if (tokenized.at(0).find("PP2_XDIR")!= std::string::npos)
                                        {        
                                            parprt2_dir_x = std::stoi( tokepins.at(1));
                                            parprt2_dir_x_inv = doinvert;                                                
                                        }
                                        if (tokenized.at(0).find("PP2_XSTEP")!= std::string::npos)
                                        {        
                                            parprt2_step_x = std::stoi( tokepins.at(1));
                                            parprt2_step_x_inv = doinvert;     
                                        }

                                        if (tokenized.at(0).find("PP2_YDIR")!= std::string::npos)
                                        {        
                                            parprt2_dir_y = std::stoi( tokepins.at(1));
                                            parprt2_dir_y_inv = doinvert;                                                
                                        }
                                        if (tokenized.at(0).find("PP2_YSTEP")!= std::string::npos)
                                        {        
                                            parprt2_step_y = std::stoi( tokepins.at(1));
                                            parprt2_step_y_inv = doinvert;     
                                        }

                                        if (tokenized.at(0).find("PP2_ZDIR")!= std::string::npos)
                                        {        
                                            parprt2_dir_z = std::stoi( tokepins.at(1));
                                            parprt2_dir_z_inv = doinvert;                                                
                                        }
                                        if (tokenized.at(0).find("PP2_ZSTEP")!= std::string::npos)
                                        {        
                                            parprt2_step_z = std::stoi( tokepins.at(1));
                                            parprt2_step_z_inv = doinvert;     
                                        }

                                        if (tokenized.at(0).find("PP2_ADIR")!= std::string::npos)
                                        {        
                                            parprt2_dir_a = std::stoi( tokepins.at(1));
                                            parprt2_dir_a_inv = doinvert;                                                
                                        }
                                        if (tokenized.at(0).find("PP2_ASTEP")!= std::string::npos)
                                        {        
                                            parprt2_step_a = std::stoi( tokepins.at(1));
                                            parprt2_step_a_inv = doinvert;     
                                        }
                                        */
                                    }//we can set all the DB25 pins in this loop 
  

                                }

                            }
                        }//set the DB25 pins here



                        //***************************************/ 
                        //***************************************/ 
                    }//if line has at least 2 sections and not commented 
                }// if line is not commented
            }

            //////
            line_ct ++; 
        }//line by line of file

    }//while data in file

    //test of explicit file closing 
    // if (cfg_filein.is_open()) 
    // {
    //     cfg_filein.close();  
    // }

}//end load file






void write_scenefile(char*objpath, char*cammatrixpath, char* scenefilepath ){};
/*
{

    //char *object_path ="3d_obj/pycore.obj";
    //char *camera_matrix_path = " ";

    FILE * fp;

    fp = fopen (scenefilepath, "w+");


    fprintf(fp, "## Generated with CNCPulser.  ##\n\n"             );

    fprintf(fp, "obj_path %s \n"            , objpath                                    );
    fprintf(fp, "cam_matrix_path %s \n"     , cammatrixpath                              );
    fprintf(fp, "cam_pos %f %f %f \n"       , cam_posx, cam_posy, cam_posz               );
    fprintf(fp, "op_loadobj %s \n"          , objpath                                    );

    fprintf(fp, "\n# render prefs   \n"                                                  );
    fprintf(fp, "rendermode %s \n"          , "litshaded"                                ); 
    fprintf(fp, "showgrid true       \n"                                                   );
    fprintf(fp, "showgnomon false    \n"                                                   ); 
    fprintf(fp, "gridsize 2.5        \n"                                                   );
    fprintf(fp, "gnomonsize 1.0      \n"                                                   ); 
    fprintf(fp, "drawlines true      \n"                                                   );
    fprintf(fp, "drawquads true      \n"                                                   ); 
    fprintf(fp, "drawtriangles true  \n"                                                   );
    fprintf(fp, "drawpoints true      \n"                                                   ); 

    fprintf(fp, "\n# light setup \n"                                                     );
    fprintf(fp, "light_pos %f %f %f \n"     , light_posx, light_posy, light_posz         );
    fprintf(fp, "light_intensity %f \n"     , light_intensity                            );

    fprintf(fp, "\n# colors      \n"                                                     );
    fprintf(fp, "bg_color %s \n"            , "20 15 15"                                 ); 
    fprintf(fp, "line_color %s \n"          , "0 0 100"                                  ); 
    fprintf(fp, "fill_color %d %d %d \n"    , surfce_clr_r, surfce_clr_g, surfce_clr_b   ); 
    fprintf(fp, "vtx_color %s \n"           , "200 0 0"                                  ); 
    fprintf(fp, "show_vtx %s \n"            , "false"                                    ); 
    fprintf(fp, "show_lines %s \n"          , "false"                                    ); 
 
 
    // 


    //fprintf(fp, "\n\n## Commands to play with someday ##\n\n"     );
    //fprintf(fp, "#op_loadobj #\n"                                 );
   

    fclose(fp);


};
*/



void load_matrix33(char* filename, m33 * pm33 ){};
/*
{
    ifstream fin;

    fin.open(filename); // open a file
    if (!fin.good()){ 
        cout << "matrix text file \""<< filename <<"\" appears to be missing or broken." << endl;
        exit (EXIT_FAILURE); // exit if file not found
    }

    int l_idx = 0;

    while (!fin.eof())
    {
        char buf[MAX_CHARS_PER_LINE];
        fin.getline(buf, MAX_CHARS_PER_LINE);
         
        vector <std::string> tokens; 
        stringstream check1(buf); 
        std::string intermediate; 
        
        while(getline(check1, intermediate, ' ')) 
        { 
            tokens.push_back(intermediate); 
        } 
        
        int t_idx = 0; 
        for(int i = 0; i < tokens.size(); i++)
        { 

            // omit blank spaces 
            if ( tokens[i].compare(" ") != -1)
            {   
                double elem = stod(tokens[i]);

                if (l_idx==0 && t_idx==0 )
                    pm33->m0 = elem;
                if (l_idx==0 && t_idx==1 )
                    pm33->m1 = elem;
                if (l_idx==0 && t_idx==2 )
                    pm33->m2 = elem;
                                                      

                if (l_idx==1 && t_idx==0 )
                    pm33->m3 = elem;
                if (l_idx==1 && t_idx==1 )
                    pm33->m4 = elem;
                if (l_idx==1 && t_idx==2 )
                    pm33->m5 = elem;
 

                if (l_idx==2 && t_idx==0 )
                    pm33->m6 = elem;
                if (l_idx==2 && t_idx==1 )
                    pm33->m7 = elem;
                if (l_idx==2 && t_idx==2 )
                    pm33->m8 = elem;
      
                t_idx++; // non space token index

            }

        }
       
        l_idx++; //line index
  
   }
      
}
*/



void load_matrix44(char* filename, m44 * pm44 ){};
/*
{
    ifstream fin;

    fin.open(filename); // open a file
    if (!fin.good()){ 
        cout << "matrix text file \""<< filename <<"\" appears to be missing or broken." << endl;
        exit (EXIT_FAILURE); // exit if file not found
    }

    int l_idx = 0;

    while (!fin.eof())
    {
        char buf[MAX_CHARS_PER_LINE];
        fin.getline(buf, MAX_CHARS_PER_LINE);
         
        vector <std::string> tokens; 
        stringstream check1(buf); 
        std::string intermediate; 
        
        while(getline(check1, intermediate, ' ')) 
        { 
            tokens.push_back(intermediate); 
        } 
        
        int t_idx = 0; 
        for(int i = 0; i < tokens.size(); i++)
        { 

            // omit blank spaces 
            if ( tokens[i].compare(" ") != -1)
            {   
                double elem = stod(tokens[i]);

                if (l_idx==0 && t_idx==0 )
                    pm44->m0 = elem;
                if (l_idx==0 && t_idx==1 )
                    pm44->m1 = elem;
                if (l_idx==0 && t_idx==2 )
                    pm44->m2 = elem;
                if (l_idx==0 && t_idx==3 )
                    pm44->m3 = elem;                                                        

                if (l_idx==1 && t_idx==0 )
                    pm44->m4 = elem;
                if (l_idx==1 && t_idx==1 )
                    pm44->m5 = elem;
                if (l_idx==1 && t_idx==2 )
                    pm44->m6 = elem;
                if (l_idx==1 && t_idx==3 )
                    pm44->m7 = elem;  

                if (l_idx==2 && t_idx==0 )
                    pm44->m8 = elem;
                if (l_idx==2 && t_idx==1 )
                    pm44->m9 = elem;
                if (l_idx==2 && t_idx==2 )
                    pm44->m10 = elem;
                if (l_idx==2 && t_idx==3 )
                    pm44->m11 = elem;  

                if (l_idx==3 && t_idx==0 )
                    pm44->m12 = elem;
                if (l_idx==3 && t_idx==1 )
                    pm44->m13 = elem;
                if (l_idx==3 && t_idx==2 )
                    pm44->m14 = elem;
                if (l_idx==3 && t_idx==3 )
                    pm44->m15 = elem;                                      


                t_idx++; // non space token index

            }

        }
       
        l_idx++; //line index
  
   }
      
}
*/



void save_matrix44(char* filepath, m44 *input ){};

/*

   FILE * fp;

   fp = fopen (filepath, "w+");
   
   fprintf(fp, "%f %f %f %f\n", input->m0  , input->m1  , input->m2  , input->m3  );
   fprintf(fp, "%f %f %f %f\n", input->m4  , input->m5  , input->m6  , input->m7  );
   fprintf(fp, "%f %f %f %f\n", input->m8  , input->m9  , input->m10 , input->m11 );
   fprintf(fp, "%f %f %f %f\n", input->m12 , input->m13 , input->m14 , input->m15 );      
   
   fclose(fp);
      
}
*/


void save_matrix33(char* filepath, m33 *input ){};
/*

   FILE * fp;

   fp = fopen (filepath, "w+");
 
   fprintf(fp, "%f %f %f \n", input->m0  , input->m1  , input->m2   );
   fprintf(fp, "%f %f %f \n", input->m3  , input->m4  , input->m5   );
   fprintf(fp, "%f %f %f \n", input->m6  , input->m7  , input->m8   );
      
   fclose(fp);
      
}



*/





/*****************************************************/
/*
void write_scenefile(char*objpath, char*cammatrixpath, char* scenefilepath )
{

    //char *object_path ="3d_obj/pycore.obj";
    //char *camera_matrix_path = " ";

    FILE * fp;

    fp = fopen (scenefilepath, "w+");


    fprintf(fp, "## Generated with CNCPulser.  ##\n\n"             );

    fprintf(fp, "obj_path %s \n"            , objpath                                    );
    fprintf(fp, "cam_matrix_path %s \n"     , cammatrixpath                              );
    fprintf(fp, "cam_pos %f %f %f \n"       , cam_posx, cam_posy, cam_posz               );
    fprintf(fp, "op_loadobj %s \n"          , objpath                                    );

    fprintf(fp, "\n# render prefs   \n"                                                  );
    fprintf(fp, "rendermode %s \n"          , "litshaded"                                ); 
    fprintf(fp, "showgrid true       \n"                                                   );
    fprintf(fp, "showgnomon false    \n"                                                   ); 
    fprintf(fp, "gridsize 2.5        \n"                                                   );
    fprintf(fp, "gnomonsize 1.0      \n"                                                   ); 
    fprintf(fp, "drawlines true      \n"                                                   );
    fprintf(fp, "drawquads true      \n"                                                   ); 
    fprintf(fp, "drawtriangles true  \n"                                                   );
    fprintf(fp, "drawpoints true      \n"                                                   ); 

    fprintf(fp, "\n# light setup \n"                                                     );
    fprintf(fp, "light_pos %f %f %f \n"     , light_posx, light_posy, light_posz         );
    fprintf(fp, "light_intensity %f \n"     , light_intensity                            );

    fprintf(fp, "\n# colors      \n"                                                     );
    fprintf(fp, "bg_color %s \n"            , "20 15 15"                                 ); 
    fprintf(fp, "line_color %s \n"          , "0 0 100"                                  ); 
    fprintf(fp, "fill_color %d %d %d \n"    , surfce_clr_r, surfce_clr_g, surfce_clr_b   ); 
    fprintf(fp, "vtx_color %s \n"           , "200 0 0"                                  ); 
    fprintf(fp, "show_vtx %s \n"            , "false"                                    ); 
    fprintf(fp, "show_lines %s \n"          , "false"                                    ); 


    fclose(fp);

};


//////////////////////////////////////////


class interrupt_flag
    {
    public:
    void set();
        bool is_set() const;
    };
    thread_local interrupt_flag this_thread_interrupt_flag;
    class interruptible_thread
    {
        std::thread internal_thread;
        interrupt_flag* flag;
    public:
        template<typename FunctionType>
        interruptible_thread(FunctionType f)
        {
            std::promise<interrupt_flag*> p;
            internal_thread=std::thread([f,&p]{
                ￼￼￼￼    p.set_value(&this_thread_interrupt_flag);
                    f(); 
                });
        flag=p.get_future().get();
    }
    ￼￼void interrupt()
    {
        if(flag) {
            flag->set();
        }
    ￼￼￼￼} 
};  



*/
