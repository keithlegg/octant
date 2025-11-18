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



void cncglobals::load_cfg_file( char* filepath )
{
    std::cout << "cncglobals loading file "<< filepath << "\n";

    std::ifstream fin;

    fin.open(filepath); // open a file
    if (!fin.good()){ 
        std::cout << "scene file \""<< filepath <<"\" appears to be missing." << std::endl;
        exit (EXIT_FAILURE); // exit if file not found
    }

    int n = 0;

    std::string line;

    int line_ct = 0;
    while (!fin.eof())
    {
        char buf[MAX_CHARS_PER_LINE];
        fin.getline(buf, MAX_CHARS_PER_LINE);
          
        const char* token[MAX_TOKENS_PER_LINE];
        token[0] = strtok(buf, " ");
        
    
        //if line has data on it ...  
        if (token[0]) 
        {

            // walk the space delineated tokens 
            for (n=1; n < MAX_TOKENS_PER_LINE; n++)
            {
                token[n] = strtok(NULL, " \t\n");
                if (!token[n]) break;  
            }

            //-------------------------------------------
            //-- MACHINE HARDWARE SETUP --------------
            if (!strcmp(token[0],"PARPORT1_ADDR"))
            {        
                parport1_addr = atof(token[1]);
            }
            //-------------------------------------------
            if (!strcmp(token[0],"PARPORT2_ADDR"))
            {        
                parport2_addr = atof(token[1]);
            }

            //-------------------------------------------
            //-- MACHINE PARAMETER SETUP --------------

            if (!strcmp(token[0],"X_XTNX"))
            {        
                //strcpy( parport2_addr, token[1]);
            }
            if (!strcmp(token[0],"Y_XTNX"))
            {        
                //strcpy( parport2_addr, token[1]);
            }
            if (!strcmp(token[0],"Z_XTNX"))
            {        
                //strcpy( parport2_addr, token[1]);
            }

            //-------------------------------------------
            //-- PULSE TIMING ---------------
            if (!strcmp(token[0],"PP1_PULSE_DLY_US"))
            {        
                // // strcpy( parport2_addr, token[1]);
            }

            // if (!strcmp(token[0],"PP1_PULSE_DLY_US"))

            //-------------------------------------------
            //-- 3D SPATIAL DIVISIONS ----------
            if (!strcmp(token[0],"PPLU1_X"))
            {        
                // // strcpy( parport2_addr, token[1]);
            }
            if (!strcmp(token[0],"PPLU1_Y"))
            {        
                // // strcpy( parport2_addr, token[1]);
            }
            if (!strcmp(token[0],"PPLU1_Z"))
            {        
                // // strcpy( parport2_addr, token[1]);
            }

            //-------------------------------------------


            //-------------------------------------------
            //-- CONTROLLER INPUTS       ----------------

            if (!strcmp(token[0],"X_LIMIT_PIN"))
            {        
                // // strcpy( parport2_addr, token[1]);
            }
            if (!strcmp(token[0],"Y_LIMIT_PIN"))
            {        
                // // strcpy( parport2_addr, token[1]);
            }
            if (!strcmp(token[0],"Z_LIMIT_PIN"))
            {        
                // // strcpy( parport2_addr, token[1]);
            }                        


            //-------------------------------------------
            //-- CONTROLLER OUTPUTS      ----------------
            if (!strcmp(token[0],"PARPRT1_XDIR"))
            {        
                    //campos = Vector3( atof(token[1]), atof(token[2]), atof(token[3]) );
            }
            if (!strcmp(token[0],"PARPRT1_XSTEP"))
            {        
                    //campos = Vector3( atof(token[1]), atof(token[2]), atof(token[3]) );
            }
            
            // -----------------

            if (!strcmp(token[0],"PARPRT1_YDIR"))
            {        
                    //campos = Vector3( atof(token[1]), atof(token[2]), atof(token[3]) );
            }    
            if (!strcmp(token[0],"PARPRT1_YSTEP"))
            {        
                    //campos = Vector3( atof(token[1]), atof(token[2]), atof(token[3]) );
            }
            
            // -----------------

            if (!strcmp(token[0],"PARPRT1_ZDIR"))
            {        
                    //campos = Vector3( atof(token[1]), atof(token[2]), atof(token[3]) );
            }
            if (!strcmp(token[0],"PARPRT1_ZSTEP"))
            {        
                    //campos = Vector3( atof(token[1]), atof(token[2]), atof(token[3]) );
            }    

            // -----------------
            if (!strcmp(token[0],"PARPRT1_ADIR"))
            {        
                    //campos = Vector3( atof(token[1]), atof(token[2]), atof(token[3]) );
            }
            if (!strcmp(token[0],"PARPRT1_ASTEP"))
            {        
                    //campos = Vector3( atof(token[1]), atof(token[2]), atof(token[3]) );
            }  


            //-------------------------------------------



            /*
                //----------------------
                if (!strcmp(token[0],"cam_pos"))
                {        
                    campos = Vector3( atof(token[1]), atof(token[2]), atof(token[3]) );
                }

                if (!strcmp(token[0],"vtx_color"))
                {   
                    //std::cout << " vtx_color is " <<  atof(token[1]) << " " <<  atof(token[2]) << " " <<  atof(token[3]) << "\n";
                    vtx_color.r = atoi(token[1]);
                    vtx_color.g = atoi(token[2]);
                    vtx_color.b = atoi(token[3]);                
                }
                if (!strcmp(token[0],"show_lines"))
                {   
                    if (!strcmp(token[1],"true"))
                    {
                        show_lines = true;
                    }
                    std::cout << "show lines "<< show_lines << "\n";
                }  
            */

            //////
            line_ct ++; 

        } 
          


    }

}






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


