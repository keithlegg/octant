
/*************************************************************/
/*
   octant.cpp

   Top level Container for Octant CNC OpenGL application. 
   Contains all the parameters for the machine that dont live in external objects. 


   Copyright (C) 2018-2026 Keith Legg - keithlegg23@gmail.com

   -------------------------------------------------------------------------------------
   Maya coordinate system    |  Blender coordinate system  |    (MOST) CNC/ 3D printers
                             |                             |         
        Y                    |        Z                    |          Z   
        |                    |        |                    |          |
        |                    |        |                    |          | 
        ______X (look on Z)  |        _____X (look on Y)   |          ______ ? (look on ?)  
   -------------------------------------------------------------------------------------
 


    //--------------------------------------------------------------------
        
    MIT License

    Copyright (c) 2026 Keith Legg - keithlegg23@gmail.com

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
         

/*****************************************************************************/

/*************************************************************/

#include <iostream>

#include <sstream>
#include <vector>
#include <string>
#include <cmath>

#include "timer.h"
#include "point_op.h"

#include "obj_model.h"

#include "globals.h"
#include "plot.h"
#include "parport.h"

#include "octant.h"

//#include "socket.h"

#if DO_BUILD_GUI == true
    // #include "gl_setup.h"
    // #include "gl_gui.h"
    // #include "gl_render.h"
    // #include "gl_cmds.h"
#endif  

#define LEN(arr) ( (int) (sizeof (arr) / sizeof (arr)[0]) ) 


/***************************************/
//SINGLETONS 

cncglobals cg;
cnc_parport parport;
cnc_parport* pt_parport;


/***************************************/
//motion control related 

extern point_ops PG;

cnc_plot motionplot;
cnc_plot* pt_motionplot = &motionplot;


Vector3 animate_locator = Vector3(0,0,0);


/***************************************/
//experimental TCP port 

int TCP_PORT;

/***************************************/
//3d objects to load 
extern char* obj_filepath;
extern std::vector<std::string>  obj_filepaths;

/***************************************/

void test_bezier( Vector3 start, Vector3 ctrl1, Vector3 ctrl2, Vector3 end)
{

    //vector<Vector3> * ptDrawvec = &linebuffer1;
    //vector<Vector3> * ptDrawClr = &linebuffer1_rgb;
    //PG.cubic_bezier(ptDrawvec, ptDrawClr,  ptnum_drawvec3, 10, start, ctrl1, ctrl2, end);

}





/**************************************************/

/*
void init_pycore(void){
    //call python from here!!

    char* pycore_cmd = "runcommand";    
    char buffer[256];

    if (active_filepath[0]=='\0')
    {
        printf("## active fp %s is blank!!\n", active_filepath );
        //strcpy(active_filepath, "foo" ); 

    }

    printf("command: python3 pycore.py %s %s\n", active_filepath, pycore_cmd);

    //there is a bug here - it demands an "active_filepath" , which wont be there if scene.olm is blank 
    snprintf(buffer, sizeof(buffer), "python3 pycore.py %s %s", active_filepath, pycore_cmd);
    int ret = system(buffer);

    //load the result in !!
    //char* newfilepath = "3d_obj/normals.obj";
    //load_objfile( newfilepath, pt_model_buffer ); 
}
*/

