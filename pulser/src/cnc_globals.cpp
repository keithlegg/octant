/*

     all the configuration details and machine paramenters live here, accessed globally 
     also deals with the loading (saving?) of machine parameters


*/


#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <string>
#include <cstring>

#include <vector>
using std::vector;

#include "math_op.h"
#include "point_op.h"

#include "cnc_plot.h"
#include "cnc_io.h"
#include "cnc_globals.h"


/*

#include <vector>
#include <iostream>
#include <string.h>
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
std::vector<std::string> obj_filepaths;
int num_loaded_obj = 0;

std::vector<vec3> scene_drawvec3;
std::vector<vec3> scene_drawvecclr;
int num_drawvec3 = 0;

std::vector<vec3> scene_drawpoints;
std::vector<vec3> scene_drawpointsclr;
std::vector<vec3>* pt_scene_drawpoints = &scene_drawpoints;
int num_drawpoints = 0;


const int MAX_CHARS_PER_LINE = 512;
const int MAX_TOKENS_PER_LINE = 20;

//extern obj_model pt_model_buffer;

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
    std::cout << " linear_unit : " << (*this).linear_unit << "\n";  

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

    std::cout << " ## DB25 pin assignments " << "\n";  
    std::cout << " parprt1_dir_x  : " << (*this).parprt1_dir_x  << "\n";
    std::cout << " parprt1_step_x : " << (*this).parprt1_step_x << "\n";
    std::cout << " parprt1_dir_y  : " << (*this).parprt1_dir_y  << "\n";
    std::cout << " parprt1_step_y : " << (*this).parprt1_step_y << "\n";
    std::cout << " parprt1_dir_z  : " << (*this).parprt1_dir_z  << "\n";
    std::cout << " parprt1_step_z : " << (*this).parprt1_step_z << "\n";

    // std::cout << " parprt1_dir_a  : " << (*this).parprt1_dir_a  << "\n";
    // std::cout << " parprt1_step_a : " << (*this).parprt1_step_a << "\n";

    std::cout << " x_limit_pin    : " << (*this).x_limit_pin << "\n";
    std::cout << " y_limit_pin    : " << (*this).y_limit_pin << "\n";
    std::cout << " z_limit_pin    : " << (*this).z_limit_pin << "\n";
    
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

std::vector<std::string> cncglobals::tokenizer( const std::string& p_pcstStr, char delim )  {
    std::vector<std::string> tokens;
    std::stringstream   mySstream( p_pcstStr );
    std::string         temp;
    while( getline( mySstream, temp, delim ) ) {
        tokens.push_back( temp );
    }
    return tokens;
} 


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

void cncglobals::load_cfg_file( char* filepath )
{
    std::cout << "cncglobals loading file "<< filepath << "\n";

    std::ifstream filein;

    filein.open(filepath); // open a file
    if (!filein.good()){ 
        std::cout << "config file \""<< filepath <<"\" appears to be missing." << std::endl;
        exit (EXIT_FAILURE); // exit if file not found
    }

    int n = 0;

    int line_ct = 0;
    while (!filein.eof())
    {   
        //26 is a sign the pin is unassigned (only 0-24 on the plug) 
        uint parsepin = 26;

        std::string line;
        while (std::getline(filein, line)) 
        {  
            //std::cout << "FULL LINE " << line << std::endl;
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
                            //z_xtntx = std::stof( tokenized.at(1) );      
                        }

                        // if (!strcmp(token[0],"PP1_PULSE_DLY_US"))

                        //-------------------------------------------
                        //-- 3D SPATIAL DIVISIONS ----------
                        if (tokenized.at(0).find("PPLU1_X") != std::string::npos )
                        {        
                             //z_xtntx = std::stof( tokenized.at(1) );   
                        }
                        if (tokenized.at(0).find("PPLU1_Y") != std::string::npos)
                        {        
                            //z_xtntx = std::stof( tokenized.at(1) );   
                        }
                        if (tokenized.at(0).find("PPLU1_Z") != std::string::npos)
                        {        
                            //z_xtntx = std::stof( tokenized.at(1) );   
                        }

                        //------------------------------------------------
                        //-- CONTROLLER PORT INPUTS       ----------------

                        uint getpin = 26;
                        bool doinvert = false;
                        int check_for_db25 = 0; 

                        if(tokenized.size()>=2)
                        { 
                            //if you want to iterate the tokens on the line 
                            for (vector<std::string>::iterator t=tokenized.begin(); t!=tokenized.end(); ++t) 
                            {
                                
                                //std::cout << line_ct << " TOKEN "<<*t<<std::endl;

                                //you can do a direct macth if no substring  
                                if (*t=="INVERT"){
                                    std::cout << "INVERTY!\n";
                                }

                                std::string foo = *t;
                                
                                if (foo.find("DB25_") != std::string::npos)
                                {
                                 std::cout << "DB25_! " << foo <<"\n";
                                }

                            }


                            //std::vector<std::string> lookpins = (*this).tokenizer(tokenized.at(1), *"_");
                        
                             
                            // if (check_for_db25!= std::string::npos)
                            // {
                            //     if(lookpins.size()>=2)
                            //     { 
                            //         std::cout << "HEY DB25 " << lookpins.at(1) << "\n";
                            //     }
                            // } 
                        }

                        if (tokenized.at(0).find("X_LIMIT")!= std::string::npos)
                        {   
                            //z_xtntx = std::stof( tokenized.at(1) );   
                        }
                        if (tokenized.at(0).find("Y_LIMIT")!= std::string::npos)
                        {        
                            //z_xtntx = std::stof( tokenized.at(1) );   
                        }
                        if (tokenized.at(0).find("Z_LIMIT")!= std::string::npos)
                        {        
                            //z_xtntx = std::stof( tokenized.at(1) );   
                        }   

                        //-------------------------------------------
                        //-- CONTROLLER PORT OUTPUTS      ----------------
                        if (tokenized.at(0).find("PARPRT1_XDIR")!= std::string::npos)
                        {        
                                //campos = Vector3( atof(token[1]), atof(token[2]), atof(token[3]) );
                        }
                        if (tokenized.at(0).find("PARPRT1_XSTEP")!= std::string::npos)
                        {        
                                //campos = Vector3( atof(token[1]), atof(token[2]), atof(token[3]) );
                        }

                        // -----------------

                        if (tokenized.at(0).find("PARPRT1_YDIR")!= std::string::npos)
                        {        
                                //campos = Vector3( atof(token[1]), atof(token[2]), atof(token[3]) );
                        }    
                        if (tokenized.at(0).find("PARPRT1_YSTEP")!= std::string::npos)
                        {        
                                //campos = Vector3( atof(token[1]), atof(token[2]), atof(token[3]) );
                        }
                                    

                        // -----------------

                        if (tokenized.at(0).find("PARPRT1_ZDIR")!= std::string::npos)
                        {        
                                //campos = Vector3( atof(token[1]), atof(token[2]), atof(token[3]) );
                        }
                        if (tokenized.at(0).find("PARPRT1_ZSTEP")!= std::string::npos)
                        {        
                                //campos = Vector3( atof(token[1]), atof(token[2]), atof(token[3]) );
                        }    

                        // -----------------
                        if (tokenized.at(0).find("PARPRT1_ADIR")!= std::string::npos)
                        {        
                                //campos = Vector3( atof(token[1]), atof(token[2]), atof(token[3]) );
                        }
                        if (tokenized.at(0).find("PARPRT1_ASTEP")!= std::string::npos)
                        {        
                                //campos = Vector3( atof(token[1]), atof(token[2]), atof(token[3]) );
                        }  

                        //***************************************/ 
                        //***************************************/ 
                    }//if line has at least 2 sections and not commented 
                }// if line is not commented
            }

            //////
            line_ct ++; 
        }//line by line of file

    }//while data in file

}//end load file






void write_scenefile(char*objpath, char*cammatrixpath, char* scenefilepath ){};
/*
{

    //char *object_path ="3d_obj/pycore.obj";
    //char *camera_matrix_path = " ";

    FILE * fp;

    fp = fopen (scenefilepath, "w+");


    fprintf(fp, "## Generated with Gnolmec.  ##\n\n"             );

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


