/*****************************************************************************/
/* 
    
    obj_geom.cpp
        obj_model - geometry functions  

    "obj_model" is defined in a single header but split among 3 cpp files
    
    the files are:
        obj_model - container for top level attributes, etc
        obj_geom  - (this file) the core of geometry processing 
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
#include <vector>
using std::vector;

#include <sstream>

#include "point_op.h"
#include "obj_model.h"
 

extern obj_model* pt_model_buffer;

/*
    COPIED HERE FOR REFERENCE 
    
    uint num_pts;
    uint num_vtxrgb;
    uint num_vnrmls;    
    uint num_fnrmls;
    uint num_uvs;
    uint num_lines;
    uint num_tris;
    uint num_quads;    
    uint num_faces;
    uint num_locs;

    Vector3* points     = new Vector3[MAX_NUM_VERTICES];   // vertices of model 
    Vector3* vtxrgb     = new Vector3[MAX_NUM_VERTICES];   // vextex colors of model  
    Vector2* uvs        = new Vector2[MAX_NUM_VERTICES];   // UV coords      - storage for lookup  
    Vector3* vnormals   = new Vector3[MAX_NUM_VERTICES];   // vertex normals - storage for lookup 
    Vector3* fnormals   = new Vector3[MAX_NUM_FACES];      // face normals   - common ID with faces
    Vector3* locpos     = new Vector3[MAX_NUM_FACES];      // indexed locators 

    uint* pt_loc               = new uint[MAX_NUM_FACES];                  // 3d "point only" location 
    std::vector<uint>* lines   = new std::vector<uint>[MAX_NUM_FACES];     // 2 sided faces 
    std::vector<uint>* tris    = new std::vector<uint>[MAX_NUM_FACES];     // 3 sided faces
    std::vector<uint>* vnids   = new std::vector<uint>[MAX_NUM_VERTICES];  // vertex normal ids
    std::vector<uint>* quads   = new std::vector<uint>[MAX_NUM_FACES];     // 4 sided faces
    std::vector<uint>* faces   = new std::vector<uint>[MAX_NUM_FACES];     // >4, N sided faces 

    Vector3 bfr_pts[MAX_NUM_VERTICES];           // general point buffer   ( tmp work area )
    std::vector<uint> bfr_faces[MAX_NUM_FACES];  // general polygon buffer ( tmp work area ) 
*/

/**********************************************************/
// DEBUG - NOT WORKING?? 
// add a new 3D triangle using 3 vector3 
void obj_model::add_triangle(Vector3* pt1, Vector3* pt2, Vector3* pt3)
{
    std::cout << "starting pts " << num_tris<< " "<< num_pts << "\n";

    points[num_pts] = *pt1;
    tris[num_tris].push_back(num_pts);
    num_pts++;
    
    points[num_pts] = *pt2;
    tris[num_tris].push_back(num_pts);
    num_pts++;
    
    points[num_pts] = *pt3;
    tris[num_tris].push_back(num_pts);
    num_pts++;

    std::cout << "ending pts " << num_tris << " " << num_pts << "\n";
    num_tris++;

}

/**********************************************************/
// add a new triangle INDEX ONLY -using existing vertices
void obj_model::add_triangle(uint vid1, uint vid2, uint vid3)
{

    // debug - use fac_tmp instead? 
    vector<uint> newtri;
    newtri.push_back(vid1);
    newtri.push_back(vid2);
    newtri.push_back(vid3);

    tris[ num_tris ] = newtri;  // 3 sided 
    num_tris++;

}



/**********************************************/
// add a new triangle AND new points 
void obj_model::append_tri(Vector3 pt1, Vector3 pt2, Vector3 pt3, int vid1, int vid2, int vid3)
{

    num_pts++;
    points[num_pts] = pt1 ;
    num_pts++;
    points[num_pts] = pt2 ;
    num_pts++;
    points[num_pts] = pt3 ;    
    //--------------------
    vector<uint> newtri;
    newtri.push_back(vid1);
    newtri.push_back(vid2);
    newtri.push_back(vid3);

    tris[ num_tris ] = newtri;  // 3 sided 
    num_tris++;
}

/**********************************************/
// add renderable line segment between two vectors  
void obj_model::between_2vecs_as_line(Vector3 pt1, Vector3 pt2)  
{

    vector<uint> newline;

    points[num_pts] = pt1;
    newline.push_back(num_pts+1);
    num_pts++;
    
    points[num_pts] = pt2;
    newline.push_back(num_pts+1);
    num_pts++;

    lines[ num_lines ] = newline;  
    num_lines++;
    

}

/**********************************************/
// UNTESTED - same but with RGB added 
void obj_model::between_2vecs_as_line(Vector3 pt1, Vector3 pt2, Vector3 color)  
{

    vector<uint> newline;

    points[num_pts] = pt1;
    vtxrgb[num_pts] = color; num_vtxrgb++;

    newline.push_back(num_pts+1);
    num_pts++;
    
    //--- 

    points[num_pts] = pt2;
    vtxrgb[num_pts] = color; num_vtxrgb++;

    newline.push_back(num_pts+1);
    num_pts++;

    lines[ num_lines ] = newline;  
    num_lines++;
    

}

/**********************************************/
/*
    add vector as a line segment, 
    but position it off origin at a position
*/

void obj_model::vec3_as_geom_atpos( Vector3 pt1 , Vector3 atpos)
{

    vector<uint> newline;

    points[num_pts] = atpos;
    newline.push_back(num_pts+1);
    num_pts++;
    
    points[num_pts] = atpos+pt1;
    newline.push_back(num_pts+1);
    num_pts++;

    lines[ num_lines ] = newline;  
    num_lines++;

}

/**********************************************/
//UNTESTED 
void obj_model::vec3_as_geom_atpos( Vector3 pt1 , Vector3 atpos, Vector3 color)
{

    vector<uint> newline;

    points[num_pts] = atpos;
    vtxrgb[num_pts] = color;num_vtxrgb++; 
    newline.push_back(num_pts+1);
    num_pts++;
    
    points[num_pts] = atpos+pt1;
    vtxrgb[num_pts] = color; num_vtxrgb++;    
    newline.push_back(num_pts+1);
    num_pts++;

    lines[ num_lines ] = newline;  
    num_lines++;

}

/**********************************************/
// UNTESTED add vector as a line segment  
void obj_model::vec3_as_geom(Vector3 pt1)
{

    vector<uint> newline;

    points[num_pts] = Vector3(0,0,0);
    newline.push_back(num_pts+1);
    num_pts++;
    
    points[num_pts] = pt1;
    newline.push_back(num_pts+1);
    num_pts++;

    lines[ num_lines ] = newline;  
    num_lines++;
    

}

/**********************************************/
// UNTESTED add vector as a line segment  
void obj_model::vec3_as_pt_geom(Vector3 pt1, float siz)
{

    between_2vecs_as_line(pt1, pt1-Vector3( siz, 0, 0) );
    between_2vecs_as_line(pt1, pt1+Vector3( siz, 0, 0) );
    
    between_2vecs_as_line(pt1, pt1-Vector3( 0, siz, 0) );
    between_2vecs_as_line(pt1, pt1+Vector3( 0, siz, 0) );

    between_2vecs_as_line(pt1, pt1-Vector3( 0, 0, siz) );
    between_2vecs_as_line(pt1, pt1+Vector3( 0, 0, siz) );

}

/**********************************************/
void obj_model::vec3_as_pt_geom(Vector3 pt1, Vector3 color, float siz)
{

    between_2vecs_as_line(pt1, pt1-Vector3( siz, 0, 0), color );
    between_2vecs_as_line(pt1, pt1+Vector3( siz, 0, 0), color );
    
    between_2vecs_as_line(pt1, pt1-Vector3( 0, siz, 0), color );
    between_2vecs_as_line(pt1, pt1+Vector3( 0, siz, 0), color );

    between_2vecs_as_line(pt1, pt1-Vector3( 0, 0, siz), color );
    between_2vecs_as_line(pt1, pt1+Vector3( 0, 0, siz), color );

}




/**********************************************/

Vector3 obj_model::get_triface_normal(int fid)
{
    
    Vector3 fac_normal;
    
    std::vector<uint> tri_face;

    if (num_tris>0){
        tri_face = tris[fid];
    } 

    if (tri_face.size()==3)
    {
        Vector3 pt1 = points[tri_face[0]];
        Vector3 pt2 = points[tri_face[1]];
        Vector3 pt3 = points[tri_face[2]];
      
        fac_normal = three_vec3_to_normal( pt1, pt2, pt3, true);
        //std::cout << "pt1 idx "<< tri_face[0] << " is " << pt1 << "\n";
        //std::cout << "normal is " << fac_normal << "\n";
    }

    return fac_normal;

}



/**********************************************/
/*


    void obj_model::triangulate(void)
    {
        reset_buffers();

        //cout << "begin triangulate \n";
       
        int i,j = 0;

        // bfr_faces[MAX_NUM_FACES];  // faces of work area 
        // bfr_pts[MAX_NUM_VERTICES]; // vertices of work area
        // i vtx_cnt;
        // i fac_cnt;
        // <> vtx_tmp;
        // <> fac_tmp;  

        if (num_quads>0)
        {
            // walk the faces, assume there are ALWAYS 4 indices (quad) 
            for (int i=0; i<num_quads; i++) 
            {
                //cout << quads[i][0]-1 <<" "<< quads[i][1]-1 <<" "<< quads[i][2]-1 << "\n";
      
                add_tri(quads[i][0],quads[i][1],quads[i][2]);
                add_tri(quads[i][2],quads[i][3],quads[i][0]);           
            }
        }

       // cout << "end triangulate \n";
    }

*/



// UNTESTED  NOT WORKING YET 
//     turn a single 4 sided polygon into two 3 sided 
//     
//     +--+              +--+ 
//     |  |              | /|  
//     |  |   Becomes    |/ |  
//     +--+              +--+  

void obj_model::triangulate(void)
{

    Vector3 tri_cntr;

    //debug - clear any loaded normals 
    //pt_model_buffer->num_fnrmls = 0;

    printf("#number of quads %d\n", num_quads );

    //calc normals for quads 
    for (uint p_i=0;p_i<num_quads;p_i++)
    {   
           
        // fetch the pts for a triangle
        // Vector3 p1 = points[quads[p_i][0]-1];
        // Vector3 p2 = points[quads[p_i][1]-1];
        // Vector3 p3 = points[quads[p_i][2]-1];

        // // calculate the face normal  
        // Vector3 a = sub(p1,p2);
        // Vector3 b = sub(p1,p3);
        // Vector3 n = normalize(cross(a,b));
        // pt_model_buffer->fnormals[pt_model_buffer->num_fnrmls]=n;
        // pt_model_buffer->num_fnrmls++;

    }

}


/**********************************************/

void obj_model::calc_normals(void)
{
    uint  p_i = 0;
    Vector3 tri_cntr;

    //debug - clear any loaded normals 
    num_fnrmls = 0;

    //calc normals for quads 
    for (p_i=0;p_i<num_quads;p_i++)
    {   
            
        // fetch the pts for a triangle
        Vector3 p1 = points[quads[p_i][0]-1];
        Vector3 p2 = points[quads[p_i][1]-1];
        Vector3 p3 = points[quads[p_i][2]-1];

        // calculate the face normal  
        Vector3 a = p1.operator-(p2);
        Vector3 b = p1.operator-(p3);
        Vector3 n = a.cross(b).normalize();

        fnormals[num_fnrmls]=n;
        num_fnrmls++;


    }

    // calc normals for triangles 
    for (p_i=0;p_i<obj_model::num_tris;p_i++)
    {   
            
        // fetch the pts for a triangle
        Vector3 p1 = points[tris[p_i][0]-1];
        Vector3 p2 = points[tris[p_i][1]-1];
        Vector3 p3 = points[tris[p_i][2]-1];

        // calculate the face normal  
        Vector3 a = p1.operator-(p2);
        Vector3 b = p1.operator-(p3);
        Vector3 n = a.cross(b).normalize()/4;

        fnormals[obj_model::num_fnrmls]=n;
        num_fnrmls++;
    }

    //----------
    
    //https://stackoverflow.com/questions/23048271/difference-between-glnormal3f-and-glnormalpointer
    // Vec3d normal = Vec3d.vectorProduct(
    //         Vec3d.sub(tri.getB(), tri.getA()),
    //         Vec3d.sub(tri.getC(), tri.getA())); //calculating normalvector
    // glNormal3d(normal.x, normal.y, normal.z);

    // broken experiment to put face normals in vertex normals
    // only do so if nothing was loaded from the model 
    // DEBUG - its wrong !
     
    if (obj_model::num_vnrmls==0)
    {
        for (p_i=0;p_i<obj_model::num_tris;p_i++)
        {    
            // fetch the pts for a triangle
            Vector3 p1 = points[tris[p_i][0]-1];
            Vector3 p2 = points[tris[p_i][1]-1];
            Vector3 p3 = points[tris[p_i][2]-1];

            // calculate the face normal  
            Vector3 a = p2.operator-(p1);
            Vector3 b = p1.operator-(p3);
            Vector3 n = b.cross(a).normalize();; 
                       
            //fill the vn lookup data
             vnormals[tris[p_i][0]-1]= n;
             vnormals[tris[p_i][1]-1]= n;
             vnormals[tris[p_i][2]-1]= n; 
             num_vnrmls++;
             num_vnrmls++;
             num_vnrmls++;

            //doesnt crash, doesnt work 
            // //set the idx of the lookup
            // std::vector<int> vnlup;
            // vnlup.push_back(num_vnrmls-2);
            // vnlup.push_back(num_vnrmls-1);
            // vnlup.push_back(num_vnrmls);                
            // vnids[p_i] = vnlup;

    
        }
    }   

}


/**********************************************/
/*
    DEBUG - NOT DONE -  Print out info from an object info struct
    debug - add axis option for 2D, otherwise return 3D

*/

/*
Vector3 obj_model::calc_extents(void)
{
    //set up the containers to store data 
    //iterate all point geom - storing highest and lowest for each axis 
    
}
*/








/**********************************************/
/*
    NOT WORKING YET 

       append one object into another 
     - points, triangles, quads, UVs, Normals
     - TODO add filter for only transferring some components 
*/
/*
void insert_geom(obj_model* from_obj, obj_model* to_obj)
{

    int x = 0;

    int pt_idx    = 0;
    int tri_idx   = 0;
    int quad_idx  = 0;
    int line_idx  = 0;

    //printf("## ##  %d %d \n", from_obj->num_pts, from_obj->num_tris);

    //points
    for (x=0;x<from_obj->num_pts;x++)
    {
        to_obj->points[pt_idx] = from_obj->points[x];
        pt_idx++; 
    }

    //triangles
    for (x=0;x<from_obj->num_tris;x++)
    {
        triangle tri_buffer = from_obj->tris[x]; //start with original indices

        printf("\n #tri1 %d %d %d \n", from_obj->tris[x].pt1 , from_obj->tris[x].pt2, from_obj->tris[x].pt3);        

        tri_buffer.pt1 = tri_buffer.pt1 +pt_idx;  //shift index in new geom
        tri_buffer.pt2 = tri_buffer.pt2 +pt_idx;  //shift index in new geom
        tri_buffer.pt3 = tri_buffer.pt3 +pt_idx;  //shift index in new geom

        printf("   #tri2 %d %d %d \n", tri_buffer.pt1 , tri_buffer.pt2, tri_buffer.pt3);
        
        to_obj->tris[tri_idx] = tri_buffer;
        

        tri_idx++; 
    }

    //quads 
    for (x=0;x<from_obj->num_quads;x++)
    {
        to_obj->quads[quad_idx] = from_obj->quads[x];
        quad_idx++; 
    }

    //lines
    for (x=0;x<from_obj->num_lines;x++)
    {
        to_obj->lines[line_idx] = from_obj->lines[x];
        line_idx++; 
    }

    //normals 

    //UVs 


    to_obj->num_pts = pt_idx;
    //to_obj->num_uvs = 0;
    to_obj->num_lines = line_idx;
    to_obj->num_tris = tri_idx;
    to_obj->num_quads = quad_idx;  
   

    //printf("# %d %d \n", to_obj->num_pts, to_obj->num_tris);

}
*/

//DEBUG - prototype function not used yet 
void obj_model::insert(std::vector<int>& input)
{
    //EMPTY 
}




/**********************************************/

void obj_model::sample_data(void)
{

    uvs[0]    = Vector2(  0.0 , 0.0         );  
    points[0] = Vector3( -1.0 , -1.0,  1.0  );
    uvs[1]    = Vector2( 1.0  , 0.0         );  
    points[1] = Vector3( 1.0 , -1.0,  1.0   );
    uvs[2]    = Vector2( 1.0, 1.0           );  
    points[2] = Vector3( 1.0,  1.0,  1.0    );

    // Back Face
    uvs[3]     = Vector2( 1.0, 0.0           );  
    points[3]  = Vector3( -1.0, -1.0, -1.0   );
    uvs[4]     = Vector2( 1.0, 1.0           );  
    points[4]  = Vector3( -1.0,  1.0, -1.0   );
    uvs[5]     = Vector2( 0.0, 1.0           );  
    points[5]  = Vector3( 1.0,  1.0, -1.0    );

    //  uvs     // UV coords 
    //  points  // 3 floats  
    //  faces   // triangles only 
                    
}






/**********************************************************/

//sample 3d object - may not be the "proper" way to do it 
void obj_model::prim_cube(float scale)
{

    // vertices
    points[0].set(-scale, -scale,  scale);
    points[1].set(-scale, -scale, -scale);
    points[2].set( scale, -scale, -scale);
    points[3].set( scale, -scale,  scale);
    points[4].set(-scale,  scale,  scale);
    points[5].set(-scale,  scale, -scale);
    points[6].set( scale,  scale, -scale);
    points[7].set( scale,  scale,  scale);
       
    //fac_tmp = {1,2,3,4};
    //faces[0] = fac_tmp;

    // faces - NOT zero indexed
    // faces[0] = {1,2,3,4};
    // faces[1] = {1,2,6,5};
    // faces[2] = {2,3,7,6};
    // faces[3] = {3,7,8,4};
    // faces[4] = {4,8,5,1};
    // faces[5] = {5,6,7,8};

    /********************/    
    faces[0] = {1,2,3,4};
    faces[1] = {1,2,6,5};
    faces[2] = {2,3,7,6};
    faces[3] = {3,7,8,4};
    faces[4] = {4,8,5,1};
    faces[5] = {5,6,7,8};

    num_quads = 6;
    num_pts = 8;
}

/**********************************************************/
 
 //DEBUG - NOT DONE AT ALL 
 //sample 3d object - may not be the "proper" way to do it
 void obj_model::prim_triangle(float scale)
 {
    //fac_tmp.clear();

    // vertices - (3d vectors)
    
    // obj_pts[vcnt].set( sin(deg_to_rad(a))*scale, cos(deg_to_rad(a))*scale, 0 ); 


    // //X axis 
    // obj_pts[0].set( 0, -scale ,  0     );
    // obj_pts[1].set( 0, 0      ,  scale );
    // obj_pts[2].set( 0, scale  ,  0     );

    // Y axis 
    // obj_pts[0].set( -scale ,0 ,  0     );
    // obj_pts[1].set( 0      ,0 ,  scale );
    // obj_pts[2].set( scale  ,0 ,  0     );

    // Z axis 
    points[0].set( -scale ,  0    , 0 );
    points[1].set( 0      ,  scale, 0 );
    points[2].set( scale  ,  0    , 0 );
    
    // faces[0] = {1,2,3};
    // face_count = 1;

    add_triangle(1,2,3);

 }    

 


/**********************************************************/


//DEBUG - THIS IS NOT 3D - taken from renderer 
//sample 3d object - may not be the "proper" way to do it
void obj_model::prim_line(float scale)
{
    // vertices - (3d vectors)
    points[0].set(-scale, 0,  -scale);
    points[1].set( scale, 0,  scale);
    
    faces[0] = {1,2}; //notice this is only a 2 point poly (line)
    
    num_lines = 1;
    num_pts   = 4;
}     
  




