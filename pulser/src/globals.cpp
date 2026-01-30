/*************************************************************/
/*
    globals.cpp 
    
    All the configuration details and machine paramenters live here, accessed globally. 
    Also deals with the loading (saving?) of machine parameters

    TODO 
        properly deal with extra spaces in file 
        error checking - check for unclosed polygoin definitions, etc 


    -----------------

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
         

/*************************************************************/
/*************************************************************/


#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem> 
#include <string>
#include <cstring>
#include <vector>

#include "globals.h"
#include "obj_model.h"
#include "plot.h"

//renamed to "parport", anticipating other interfaces in the future
#include "parport.h"



#if DO_BUILD_GUI == true
    //#include "gl_setup.h"
    #include "gl_util.h"
    #include "gl_setup.h"

    extern GLfloat bg_clr[];

#endif 


extern cnc_plot* pt_motionplot;




char strbuffer[100][100];




//linebuffer1.reserve(10000);
//linebuffer2.reserve(10000);

//
//std::vector<Vector3> linebuffer3;
//std::vector<Vector3> linebuffer3_rgb;
// //linebuffer3.reserve(10000);

//--------------------------
std::vector<Vector3> scene_drawpoints;
std::vector<Vector3> scene_drawpointsclr;

//--------------------------

//experiments is heap VS stack. idk 
// std::vector<Vector3>* linebuffer1         = new std::vector<Vector3>;
// std::vector<Vector3>* linebuffer1_rgb     = new std::vector<Vector3>;
// std::vector<Vector3>* scene_drawpoints    = new std::vector<Vector3>;
// std::vector<Vector3>* scene_drawpointsclr = new std::vector<Vector3>;



//std::vector<Vector3>* pt_scene_drawpoints = &scene_drawpoints;

obj_model* pt_model_buffer  = new obj_model;

const int MAX_CHARS_PER_LINE = 512;
const int MAX_TOKENS_PER_LINE = 20;



/*******************************************************/

//DEBUG not done - generate a square polygon basically 

//use a throw away object to build a cube 
//a lot of memory wasted for a simple task... hmm 

void cncglobals::get_machine_xtntx(void)
{   
    std::cout << " machine extnts called \n";

    //always assume a 2D rectangle on Z positive up axis 
    
    // obj_model* pt_obj2d_loader  = new obj_model;
    // pt_obj2d_loader->reset();
    // std::vector<Vector3> output_pts;
    // output_pts.reserve( 400 );  
    
    Vector3 pt1 = Vector3( xmax_xtntx, 0, ymin_xtntx); 
    Vector3 pt2 = Vector3( xmin_xtntx, 0, ymin_xtntx); 
    Vector3 pt3 = Vector3( xmin_xtntx, 0, ymax_xtntx); 
    Vector3 pt4 = Vector3( xmax_xtntx, 0, ymax_xtntx); 

    //DEBUG not what we really want, just for testing 
    pt_motionplot->add_file_vec(&pt1);
    pt_motionplot->add_file_vec(&pt2);
    pt_motionplot->add_file_vec(&pt3);
    pt_motionplot->add_file_vec(&pt4);
    //periodic    
    pt_motionplot->add_file_vec(&pt1);  

    pt_motionplot->add_prgvec_ply(); 
    pt_motionplot->copy_prg_to_toolpath();


}


/*******************************************************/
//DEBUG this may be pointless 
//trying to do memory housekeeping 
void exit_program(void)
{
    //delete pt_model_buffer;
    //delete pt_scene_drawpoints;

}

/*******************************************************/
void cncglobals::show_params( void )
{
    std::cout <<"\n\n";
    std::cout << " #### CNC GLOBALS #### " << "\n";
    
    std::cout << " loaded cfg file    : " << cfg_filepath        << "\n";

    std::cout << " active obj file    : " << active_filepath     << "\n";
    std::cout << " python return obj  : " << obj_pycore          << "\n";
    std::cout << " ENABLE_MOTOR_DRIVE : " << ENABLE_MOTOR_DRIVE  << "\n";

    std::cout <<"\n";

    
    //--------------------------
    //capture the state of cout 
    std::ios_base::fmtflags f( std::cout.flags() );  
    
    //print some neato 16 bit hex addresses 
    std::cout << std::hex;
    std::cout << " parport1_addr : " << parport1_addr << "\n";
    std::cout << " parport2_addr : " << parport2_addr << "\n";

    //set it back when we are done 
    std::cout.flags( f );

    std::cout << " parport1_addr : " << parport1_addr << "\n";
    std::cout << " parport2_addr : " << parport2_addr << "\n";

    std::cout <<"\n";

    //--------------------------
    std::cout << " ## machine params " << "\n";  
    //std::cout << " linear_unit : " << (*this).linear_unit << "\n";  

    std::cout << " ## machine travel size in 3D  " << "\n";  
    
    std::cout << " retract height : " << retract_height<< "\n";
    std::cout << " work height    : " << work_height   << "\n";

    std::cout << " xmin_xtntx        : " << xmin_xtntx<< "\n";
    std::cout << " xmax_xtntx        : " << xmax_xtntx<< "\n";
    std::cout << " ymin_xtntx        : " << ymin_xtntx<< "\n";
    std::cout << " ymax_xtntx        : " << ymax_xtntx<< "\n";
    std::cout << " zmin_xtntx        : " << zmin_xtntx<< "\n";
    std::cout << " zmax_xtntx        : " << zmax_xtntx<< "\n";
    
    std::cout <<"\n";

    //--------------------------
    //GLOBAL_SIM_TIMER_PERIOD
    std::cout << " ## sim time clock period " << "\n";  
    std::cout << " glob_simtime_period : " << (*this).glob_simtime_period << "\n";
    std::cout <<"\n";

    //--------------------------
    std::cout << " ## waveform generation parameters " << "\n";  
    std::cout << " pp1_pulse_dly_us : " << (*this).pp1_pulse_dly_us << "\n";
    std::cout <<"\n";

    //--------------------------
    std::cout << " ## pulses per linear unit (x,y,z)" << "\n";        
    std::cout << " pp1u_x : " << pp1u_x << "\n";
    std::cout << " pp1u_y : " << pp1u_y << "\n";
    std::cout << " pp1u_z : " << pp1u_z << "\n";
    std::cout <<"\n";

    //--------------------------
    //std::cout << std::boolalpha;
    std::cout << " ## DB25 pin assignments " << "\n";  
    std::cout << " parprt1_dir_x  : " << parprt1_dir_x  << " inverted: "<< parprt1_dir_x_inv  << "\n";
    std::cout << " parprt1_step_x : " << parprt1_step_x << " inverted: "<< parprt1_step_x_inv << "\n";
    std::cout << " parprt1_dir_y  : " << parprt1_dir_y  << " inverted: "<< parprt1_dir_y_inv  << "\n";
    std::cout << " parprt1_step_y : " << parprt1_step_y << " inverted: "<< parprt1_step_y_inv << "\n";
    std::cout << " parprt1_dir_z  : " << parprt1_dir_z  << " inverted: "<< parprt1_dir_z_inv  << "\n";
    std::cout << " parprt1_step_z : " << parprt1_step_z << " inverted: "<< parprt1_step_z_inv << "\n";
    std::cout << " parprt1_dir_a  : " << parprt1_dir_a  << " inverted: "<< parprt1_dir_a_inv  << "\n";
    std::cout << " parprt1_step_a : " << parprt1_step_a << " inverted: "<< parprt1_step_a_inv << "\n";

    std::cout << "\n"; 
    /*
    std::cout << " parprt2_dir_x  : " << parprt2_dir_x  << " inverted: "<< parprt2_dir_x_inv  << "\n";
    std::cout << " parprt2_step_x : " << parprt2_step_x << " inverted: "<< parprt2_step_x_inv << "\n";
    std::cout << " parprt2_dir_y  : " << parprt2_dir_y  << " inverted: "<< parprt2_dir_y_inv  << "\n";
    std::cout << " parprt2_step_y : " << parprt2_step_y << " inverted: "<< parprt2_step_y_inv << "\n";
    std::cout << " parprt2_dir_z  : " << parprt2_dir_z  << " inverted: "<< parprt2_dir_z_inv  << "\n";
    std::cout << " parprt2_step_z : " << parprt2_step_z << " inverted: "<< parprt2_step_z_inv << "\n";
    std::cout << " parprt2_dir_a  : " << parprt2_dir_a  << " inverted: "<< parprt2_dir_a_inv  << "\n";
    std::cout << " parprt2_step_a : " << parprt2_step_a << " inverted: "<< parprt2_step_a_inv << "\n";
    */
    //--------------------------


    std::cout << "\n"; 
    std::cout << " x_limit_pin    : " << x_limitsw_pin << " inverted: "<< x_limitsw_pin_inv << "\n";
    std::cout << " y_limit_pin    : " << y_limitsw_pin << " inverted: "<< y_limitsw_pin_inv << "\n";
    std::cout << " z_limit_pin    : " << z_limitsw_pin << " inverted: "<< z_limitsw_pin_inv << "\n";
    
    //std::cout << " : " << (*this). << "\n";
    //std::cout << " : " << (*this). << "\n";    

  
};


/*******************************************************/

std::vector<std::string> cncglobals::tokenizer( const std::string& p_pcstStr, char delim )  {
    std::vector<std::string> tokens;
    std::stringstream   mySstream( p_pcstStr );
    std::string         temp;
    while( getline( mySstream, temp, delim ) ) {
        tokens.push_back( temp );
    }
    return tokens;
} 

/*******************************************************/


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
    return 0; //just to make the compiler happy 

}

/*******************************************************/
/*******************************************************/

void cncglobals::show_obj(void)
{
    
    pt_model_buffer->show();
    pt_model_buffer->show_geom();

}



/*******************************************************/
void cncglobals::load_objects(void)
{
    //std::cout <<  "load_objects called \n";
    if(obj_filepaths.empty()) 
    {
        active_filepath = "";   
    }

    //----
    if(!obj_filepaths.empty()) 
    {
        char char_array[100];
            
        std::cout << "## DEBUG load_objfile resetting obj_file internals \n";
        pt_model_buffer->reset();

        for( uint x=0;x<obj_filepaths.size();x++)
        {
            //std::cout << "#### load_objects loading  " << (*this).obj_filepaths[x] <<"\n";
            strcpy(char_array, obj_filepaths[x].c_str()); 
            
            // -- load object files from disk --
            pt_model_buffer->load(char_array);
 
            // -- post load setup functions --
            pt_model_buffer->calc_normals();
            pt_model_buffer->bbox3d();
            pt_model_buffer->bbox3d_pts();

            //DEBUG - this assumes the last obj loaded (or just 1)
            active_filepath = obj_filepaths[x];

        };


    };
}



/*******************************************************/
void cncglobals::set_cfg_path( char* filepath )
{
    cfg_filepath = filepath;
}

/*******************************************************/
void cncglobals::load_cfg_file( void )
{
    std::ifstream cfg_filein;

    uint line_ct       = 0;
    uint local_vec_idx = 0; //remember how many new points to add 

    std::cout << "cncglobals loading file "<< cfg_filepath << "\n";

    //DEBUG - need to check to make sure its a file, not a directory 
    /*
    const std::string pathString = std::to_string(cfg_filepath);
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


    cfg_filein.open(cfg_filepath); // open a file
    if (!cfg_filein.good()){ 
        std::cout << "config file \""<< cfg_filepath <<"\" appears to be missing." << std::endl;
        exit (EXIT_FAILURE); // exit if file not found
    }

    while (!cfg_filein.eof())
    {   

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

                        //****************************************/
                        // RUN SCRIPT OPERATORS IN HERE 
                        //****************************************/                                    
                        
                        // PYTHON RETURN 3D OBJECT - (OUTPUT OF PYCORE) 
                        if (tokenized.at(0).find("ENABLE_MOTOR_DRIVE")!= std::string::npos)
                        {   
                            ENABLE_MOTOR_DRIVE = std::stoi(tokenized.at(1));
                        }
    
                        /**************************************/  

                        
                        if (tokenized.at(0).find("op_loadpathobj")!= std::string::npos)
                        {   
                            std::string buffer;
                            buffer = tokenized.at(1);
                            std::cout << "DEBUG LOAD 3D OBJECT PATH OBJECT  \n";
                            
                            pt_motionplot->import_path_from_obj( buffer );

                        }

                        /**************************************/  
                        // DEBUG - NOT DONE -
                        // location - OBJ file   
                        if (tokenized.at(0).find("op_loadlocs")!= std::string::npos)
                        {   
                            std::string buffer;
                            buffer = tokenized.at(1);
                            std::cout << "DEBUG LOAD 3D POINT (LOCs) OBJECT  \n";
                        }

                        /**************************************/ 
                        /**************************************/ 
                        //GUI exclusive params

                        #if DO_BUILD_GUI == true
                            // DEBUG - NOT DONE -
                            // lines only - OBJ file  
                            if (tokenized.at(0).find("op_loadpaths")!= std::string::npos)
                            {   
                                std::string buffer;
                                buffer = tokenized.at(1);

                                std::cout << "DEBUG LOAD 3D LINE OBJECT  \n";
                                load_line_obj(buffer);

                            }

                            /**************************************/ 
                            if (tokenized.at(0).find("SCREEN_BG_COLOR") != std::string::npos )                            
                            {  
       
                                //std::cout << "DEBUG BG COLOR " << buffer1 << " "<< buffer2 << " "<< buffer3 << "\n";
                            
                                bg_clr[0] = std::stof(tokenized.at(1));
                                bg_clr[1] = std::stof(tokenized.at(2));
                                bg_clr[2] = std::stof(tokenized.at(3));

                            }

                        #endif 
    
                        //end exclusive GUI params 
                        /**************************************/ 
                        /**************************************/ 

                        //----- 
                        // DEBUG - NOT DONE - PYTHON RETURN 3D OBJECT - (OUTPUT OF PYCORE) 
                        if (tokenized.at(0).find("op_py_obj")!= std::string::npos)
                        {   

                            std::string buffer;
                            //std::cout << "DEBUG parsing LINEAR_UNIT " << tokenized.at(1) << "\n";
                            buffer = tokenized.at(1);
                            std::cout << "PYTHON NEEDS SETTING OBJ DEBUG         \n";


                            //obj_pycore = buffer;  
                        }

                        /**************************************/  
                        // LOAD 3D OBJECT - Alias .OBJ file (CLASSIC DISPLAY)
                        if (tokenized.at(0).find("op_loadobj")!= std::string::npos)
                        {   
                            //std::cout << "\n#LOAD OPERATOR ! - 3d obj found " << tokenized.at(1) << "\n";
                            obj_filepaths.push_back(tokenized.at(1));
                            //std::cout <<  num_loaded_obj << "\n";
                            //std::cout << "#ADDED OBJ " << (*this).num_loaded_obj << " "<< obj_filepaths.at((*this).num_loaded_obj) << "\n";
                        }

                        /**************************************/ 
                        /**************************************/ 
                        //MOTION PATH - ( WIP )

                        //keep track of the polygon index - open a new poly  
                        if (tokenized.at(0).find("new_polygon")!= std::string::npos)
                        {
                            if(active_polygon_load==true)
                            {
                                std::cout << "load_cfg_file - error - polygon load already open \n";
                            }else{   
                                std::cout << "load_cfg_file - debug - opening new polygon load \n";                         
                                active_polygon_load=true;
                                

                                /*********/ 
                                //DEBUG - still figuring this out 
                                // create a new motion_idx object for each polygon 

                                //pt_motionplot->add_motion(tokenized.at(1), tokenized.at(2), 0, 0, 0 );
                                /*********/ 


                            };
                        } 

                        /*********/                         
                        //keep track of the polygon index - close a new poly  
                        if (tokenized.at(0).find("end_polygon")!= std::string::npos)
                        {   
                            if(active_polygon_load==false)
                            {
                                std::cout << "load_cfg_file - error - polygon load already closed. \n";
                            }else{
                                if( pt_motionplot->loaded_file_vecs.size()==0 )
                                {
                                    std::cout << "load_cfg_file - NO DATA TO LOAD - SKIPPING " <<"\n"; 
                                }else{                          
                                    std::cout << "load_cfg_file - adding new polygon  " << "\n"; 

                                    pt_motionplot->add_prgvec_ply(); 
                                    
                                    //DEBUG this is a stub 
                                    pt_motionplot->copy_prg_to_toolpath(); //hack to just render something now 

                                    
                                }//if data to load 

                                active_polygon_load=false;
                            };
                        } 
                        
                        /*********/  
                        if(active_polygon_load)
                        { 
                            // LOAD 3D VECTOR (3 floats for a vector display)
                            // load a 3d or 2d object to display as vector lines
                            if (tokenized.at(0).find("new_ply_vtx")!= std::string::npos)
                            {   
                                float c1,c2,c3;
                                //DEBUG - file loader counts blank spaces - need to fix
                                //if things are not spaced exactly right wont work with extras 
                                try 
                                {
                                    c1 = std::stof(tokenized.at(1));
                                    c2 = std::stof(tokenized.at(2));
                                    c3 = std::stof(tokenized.at(3));
                                    
                                    Vector3 v = Vector3(c1,c2,c3); 
                                    pt_motionplot->add_file_vec(&v);
 
                                } catch (const std::invalid_argument& e) {  
                                    //std::cerr << "Error: " << e.what() << std::endl; // Handling the error
                                } catch (...) { // Catch-all for any other unexpected exceptions
                                    std::cerr << "Error loading vec3 geom from cfg" << std::endl;
                                }                            
                            }//load a 3D vector 
                        }//active polygon load 

                        //END MOTION PATH 
                        /**************************************/ 
                        /**************************************/ 


                        //***************************************/ 
                        //** MACHINE HARDWARE SETUP ************//
                        
                        //DEBUG - NOT DONE 
                        if (tokenized.at(0).find("LINEAR_UNIT") != std::string::npos )                            
                        {  
                            std::string buffer;
                            //std::cout << "DEBUG parsing LINEAR_UNIT " << tokenized.at(1) << "\n";
                            buffer = tokenized.at(1);
                            //std::cout << "DEBUG parsing LINEAR_UNIT " << linear_unit << "\n";

                        }

                        //***************************************/ 
                        if (tokenized.at(0).find("RETRACT_HEIGHT") != std::string::npos )                            
                        {  
                            std::string buffer;
                            retract_height = std::stof( tokenized.at(1) );  
                        }

                        //***************************************/ 
                        if (tokenized.at(0).find("WORK_HEIGHT") != std::string::npos )                            
                        {  
                            std::string buffer;
                            //std::cout << "DEBUG parsing WORK_HEIGHT " << tokenized.at(1) << "\n";
                            work_height = std::stof( tokenized.at(1) );  
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

                        if (tokenized.at(0).find("XMIN_XTNX") != std::string::npos)
                        {        
                            xmin_xtntx = std::stof( tokenized.at(1) );                            
                        }
                        if (tokenized.at(0).find("XMAX_XTNX") != std::string::npos)
                        {        
                            xmax_xtntx = std::stof( tokenized.at(1) );                            
                        }

                        if (tokenized.at(0).find("YMIN_XTNX") != std::string::npos)
                        {        
                            ymin_xtntx = std::stof( tokenized.at(1) );  
                        }
                        if (tokenized.at(0).find("YMAX_XTNX") != std::string::npos)
                        {        
                            ymax_xtntx = std::stof( tokenized.at(1) );  
                        }

                        if (tokenized.at(0).find("ZMIN_XTNX") != std::string::npos)
                        {        
                            zmin_xtntx = std::stof( tokenized.at(1) );  
                        }
                        if (tokenized.at(0).find("ZMAX_XTNX") != std::string::npos)
                        {        
                            zmax_xtntx = std::stof( tokenized.at(1) );  
                        }
                        //-------------------------------------------
                        
                        //-- GLOBAL SIM TIME PERIOD  ----------------
                        if (tokenized.at(0).find("GLOBAL_SIM_TIMER_PERIOD") != std::string::npos)
                        {    
                            glob_simtime_period = std::stoi( tokenized.at(1) );      
                        }

                        //-- PULSE TIMING ---------------------------
                        if (tokenized.at(0).find("PP1_PULSE_DLY_US") != std::string::npos)
                        {    
                            pp1_pulse_dly_us = std::stoi( tokenized.at(1) );      
                        }

                        if (tokenized.at(0).find("PP2_PULSE_DLY_US") != std::string::npos)
                        {    
                            pp2_pulse_dly_us = std::stoi( tokenized.at(1) );      
                        }

                        //-------------------------------------------
                        //-- 3D SPATIAL DIVISIONS -------------------
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




                        //------------------------------------------------
                        //-- CONTROLLER PORT INPUTS       ----------------
                        bool doinvert = false;

                        if(tokenized.size()>=2)
                        { 
                            //iterate the tokens on the line to get "inverted" parameter 
                            for (std::vector<std::string>::iterator t=tokenized.begin(); t!=tokenized.end(); ++t) 
                            {
                                //you can do a direct macth if no substring  
                                if (*t=="INVERT"){
                                    doinvert=true;
                                }
                            }

                            //do it one more time and put it all together now
                            for (std::vector<std::string>::iterator t=tokenized.begin(); t!=tokenized.end(); ++t) 
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
            //***********/ 
            line_ct ++; 
        }//line by line of file

    }//while data in file

    //***************************************/ 
    /*
        if polygon load is still open , Houston we have a problem
        Okay, look, this hardly passes for error che checking but at lest its something
        im doing my best here
    */
    if(active_polygon_load)
    {
        std::cout << "IT APPEARS THERE IS AN OPEN POLYGON IN CFG \n";
        abort();
    }

    //test of explicit file closing 
    // if (cfg_filein.is_open()) 
    // {
    //     cfg_filein.close();  
    // }

}//end load file







