/*************************************************************/
/*
    cnc_globals.cpp 
    
    All the configuration details and machine paramenters live here, accessed globally. 
    Also deals with the loading (saving?) of machine parameters

 
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
using std::vector;

//#include "math_op.h"
//#include "point_op.h"

#include "obj_model.h"
#include "cnc_plot.h"

//renamed to "parport", anticipating other interfaces in the future
//#include "cnc_io.h"
#include "cnc_parport.h"

#include "cnc_globals.h"
#include "gl_setup.h"



char strbuffer[100][100];


std::vector<Vector3> scene_drawvec3;
std::vector<Vector3> scene_drawvecclr;
std::vector<Vector3> scene_drawpoints;
std::vector<Vector3> scene_drawpointsclr;

//experiments is heap VS stack. idk 
// std::vector<Vector3>* scene_drawvec3      = new std::vector<Vector3>;
// std::vector<Vector3>* scene_drawvecclr    = new std::vector<Vector3>;
// std::vector<Vector3>* scene_drawpoints    = new std::vector<Vector3>;
// std::vector<Vector3>* scene_drawpointsclr = new std::vector<Vector3>;


std::vector<Vector3>* pt_scene_drawpoints = &scene_drawpoints;
obj_model* pt_model_buffer  = new obj_model;

int num_drawvec3 = 0;
int num_drawpoints = 0;

const int MAX_CHARS_PER_LINE = 512;
const int MAX_TOKENS_PER_LINE = 20;





void exit_program(void)
{
    delete pt_model_buffer;
    //delete pt_scene_drawpoints;

}



/*******************************************************/
void cncglobals::show_params( void )
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

    if(!obj_filepaths.empty()) 
    {
        char char_array[100];
        int x = 0 ; 
            
        //std::cout << "## DEBUG load_objfile resetting obj_file internals \n";
        pt_model_buffer->reset();
        
        //for(x=0;x<(*this).num_loaded_obj;x++)
        for(x=0;x<obj_filepaths.size();x++)
        {
            //std::cout << "#### load_objects loading  " << (*this).obj_filepaths[x] <<"\n";
            strcpy(char_array, obj_filepaths[x].c_str()); 
            pt_model_buffer->load(char_array);
            //pt_model_buffer->calc_normals();

        };
        
    };
}

/*******************************************************/
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

                        // LOAD 3D OBJECT - Alias .OBJ file (CLASSIC DISPLAY)
                        if (tokenized.at(0).find("op_loadobj")!= std::string::npos)
                        {   
                            //std::cout << "\n#LOAD OPERATOR ! - 3d obj found " << tokenized.at(1) << "\n";
                            obj_filepaths.push_back(tokenized.at(1));
                            //num_loaded_obj++;

                            //std::cout <<  num_loaded_obj << "\n";
                            //std::cout << "#ADDED OBJ " << (*this).num_loaded_obj << " "<< obj_filepaths.at((*this).num_loaded_obj) << "\n";
                        }


                        // LOAD 3D VECTOR (3 floats for a vector display)
                        // load a 3d or 2d object to display as vector lines
                        if (tokenized.at(0).find("op_scenevec")!= std::string::npos)
                        {   

                            float c1,c2,c3;
                            //DEBUG - file loader counts blank spaces - need to fix
                            //if things are not spaced exactly right wont work with extras 
                            try {
                                c1 = std::stof(tokenized.at(1));
                                c2 = std::stof(tokenized.at(2));
                                c3 = std::stof(tokenized.at(3));
                                
                                Vector3 v = Vector3(c1,c2,c3); 
                                add_vec_scndrw(&v);

                                //std::cout << "NEW VEC "<< c1 <<" "<< c2 <<" "<< c3 << "\n";

                            } catch (const std::invalid_argument& e) {  
                                //std::cerr << "Error: " << e.what() << std::endl; // Handling the error
                            } catch (...) { // Catch-all for any other unexpected exceptions
                                std::cerr << "Error loading vec3 geom from cfg" << std::endl;
                            }                            
 




                            // move locator to first vector loaded to init
                            if(num_drawvec3==0){
                                 //qpos
                            } 

                        }

                        //------

                        /* 
                        // load a 3d or 2d object to display as vector lines
                        if (tokenized.at(0).find("op_scenevec")!= std::string::npos)
                        {   
                            std::cout << "\n#LOAD OPERATOR ! - vecobj found " << tokenized.at(1) << "\n";
                            obj_vecpaths.push_back(tokenized.at(1));
                            num_loaded_vecobj++;
                            std::cout <<  num_loaded_vecobj << "\n";
                            std::cout << "#ADDED VEC OBJ " << (*this).num_loaded_vecobj << " "<< obj_vecpaths.at((*this).num_loaded_vecobj) << "\n";
                        }*/


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







