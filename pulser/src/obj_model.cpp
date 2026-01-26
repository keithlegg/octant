/*****************************************************************************/
/* 

    obj_model.cpp
    
        obj_model - base container for non geometry and non file IO functions 

    
    "obj_model" is defined in a single header but split among 3 cpp files
    the three associated cpp files:  
    
        obj_model - (this file) - container for top level attributes, etc
        obj_geom  - the core of geometry processing 
        obj_fio   - file in/out operations like load, save etc 

         

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





#include <string>
#include <cstring>

#include <iostream>

#include <cctype>

#include <fstream>
//#include <vector>


#include <sstream>
#include "obj_model.h"

 

extern obj_model* pt_model_buffer;



/***************************************/

void obj_model::add_locator(Vector3 pos)
{
    //std::cout << "add locator called \n";

    locpos[num_locs] = pos;
    pt_loc[num_locs] = num_locs;
    num_locs++;


}

/***************************************/
/*

    Overridden function - inherited from polygon_ops  
    Clear an object to "reset" it
*/

void obj_model::reset(void)
{
    //DEBUG - FIX THIS 


    // Vector3* points     = new Vector3[MAX_NUM_VERTICES];   // vertices of model 
    // Vector3* vtxrgb     = new Vector3[MAX_NUM_VERTICES];   // vextex colors of model  
    // Vector2* uvs        = new Vector2[MAX_NUM_VERTICES];   // UV coords      - storage for lookup  
    // Vector3* vnormals   = new Vector3[MAX_NUM_VERTICES];   // vertex normals - storage for lookup 
    // Vector3* fnormals   = new Vector3[MAX_NUM_FACES];      // face normals   - common ID with faces
    
    lines->clear();  
    tris->clear(); 
    vnids->clear(); 
    quads->clear(); 
    faces->clear();  

    num_pts = 0;
    num_uvs = 0;
    num_lines = 0;
    num_tris = 0;
    num_quads = 0;
    num_vnrmls = 0;
    num_fnrmls = 0;
    num_vtxrgb = 0;

}


/***************************************/

void obj_model::show(void)
{
    std::cout<< "#-----------------------#\n";
    std::cout<< "#---- objmodel show ----#\n";
    std::cout<< "# number  points    " << num_pts     << "\n";
    std::cout<< "# number  lines     " << num_lines   << "\n";
    std::cout<< "# number  triangles " << num_tris    << "\n";
    std::cout<< "# number  quads     " << num_quads   << "\n";
    std::cout<< "# number  UVs       " << num_uvs     << "\n";
    std::cout<< "# number  vtx nrml  " << num_vnrmls  << "\n";
    std::cout<< "# number  fac nrml  " << num_fnrmls  << "\n";
    std::cout<< "# number  vtxcolr   " << num_vtxrgb  << "\n"; 
}

/***************************************/


void obj_model::show_geom(void)
{
    int i;

    std::cout << "\n";

    //std::cout << "\n# point indices  ---------------- %d \n", (*this).num_pts);
    for (i=0;i<num_pts;i++)
    {
        std::cout << " pt idx "<< i  << " is :";
        //std::cout << points[i].x << " " << points[i].y << " "<< points[i].z ; 
        
        std::cout << "\n";
    }

    // std::cout << "\n# line indices  ------------------- %d \n", (*this).num_lines);
    for (i=0;i<num_lines;i++)
    {
        std::cout << " line idx "<< i  << " is :";
        //std::cout << lines[i][0] << " " << lines[i][1] << "\n";  
        std::cout << "\n";        
    }

    //std::cout << "\n# triangle indices ----------------- %d \n", (*this).num_tris);
    for (i=0;i<num_tris;i++)
    {
        std::cout << " tri idx "<< i  << " is :";
        std::cout << tris[i][0] << " " << tris[i][1] << " "<< tris[i][2];
        std::cout << "\n";


    }

    //std::cout << "\n# quad indices ------------------   %d \n", (*this).num_quads);
    for (i=0;i<num_quads;i++)
    {
        std::cout << " quad idx "<< i  << " is :";
        //std::cout << quads[i][0] << " " << quads[i][1] << " "<< quads[i][2] << " "<< quads[i][3];
        std::cout << "\n";
    }

    //printf("\n# Normals   --------  \n");

    //printf("\n# UVs   --------  \n");

    //printf("\n# Colors   --------  \n");

}


