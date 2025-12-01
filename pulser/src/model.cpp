#include <iostream>
using namespace std; 

#include <fstream>
#include <cstring>
#include <vector>
#include <algorithm>


#include "math.h"
#include "Vectors.h"
#include "model.h"
#include "point_ops.h"

using namespace std;


const int MAX_CHARS_PER_LINE = 512;
const int MAX_TOKENS_PER_LINE = 20;
const char* const DELIMITER = " ";

// tokens to parse out of .OBJ file
const char* v_chr    = "v";   // vertex identifier in OBJ file 
const char* f_chr    = "f";   // face identifier in OBJ file 
const char* uv_chr   = "vt";  // UV identifier in OBJ file 
const char* nrml_chr = "vn";  // UV identifier in OBJ file 
const char* line_chr = "l";   // Line identifier in OBJ file 




/******************************/

// FIND A BETTER PLACE FOR THIS 
// FRAMEBUFFER HAS A COPY AND NEEDS TO BE MOVED 

#define DEG_TO_RAD 0.0174532925
#define RAD_TO_DEG 57.29577951

double deg_to_rad ( double deg){
   return deg * DEG_TO_RAD;
}

double rad_to_deg ( double rad){
   return rad * RAD_TO_DEG;
}

/******************************/



int model::getnum_verts(){
    //return sizeof( obj_pts ) / sizeof( obj_pts[0] );
    return vertex_count;
}


int model::getnum_faces(){
    //return sizeof( faces ) / sizeof( faces[0] );
    return line_count + triangle_count + quad_count + nsided_count;
}




/**********************************************************/

Vector3 model::get_triface_normal(int fid)
{
    
    Vector3 fac_normal;
    
    std::vector<int> tri_face;

    if (triangle_count>0){
        tri_face = triangles[fid];
    } 

    if (tri_face.size()==3)
    {
        Vector3 pt1 = obj_pts[ tri_face[0] ];
        Vector3 pt2 = obj_pts[ tri_face[1] ];
        Vector3 pt3 = obj_pts[ tri_face[2] ];
      
        fac_normal = model::three_vec3_to_normal( pt1, pt2, pt3, true);
        //cout << "pt1 idx "<< tri_face[0] << " is " << pt1 << "\n";
        //cout << "normal is " << fac_normal << "\n";
    }

    return fac_normal;

}


 


/**********************************************************/
void model::showinfo(void)
{
    cout << "\n-----------------------------------------------\n";   
    cout << " loded num vertices    "<< model::vertex_count   << endl;
    cout << " loded num lines       "<< model::line_count     << endl;
    cout << " loded num triangles   "<< model::triangle_count << endl;
    cout << " loded num quads       "<< model::quad_count     << endl;
    cout << " loded num Nsided      "<< model::nsided_count   << endl;
    cout << " loaded num vtxrgb     "<< model::vtxrgb_count   << endl;

    //cout << " getnumverts           "<< getnum_verts() << " \n";
    //cout << " getnumfaces           "<< getnum_faces() << " \n";   

    cout << "-------------------------------------------------\n\n";
}


/**********************************************************/


void model::show(void)
{

    //int numv = getnum_verts();
    for (int xx=0;xx<vertex_count;xx++)
    {
        cout << obj_pts[xx] << endl;
    }

    //----------------
    //  lines[MAX_NUM_FACES];      // 2 sided faces 

    //----------------
    cout << "## TRIANGLES \n";
    for (int xx=0;xx<triangle_count;xx++)
    {
        for (int y=0;y<triangles[xx].size();y++)
        {
           cout << triangles[xx][y] << " ";   
        }
        cout << "\n";
    }


    //----------------
    cout << "## QUADS \n";
    for (int xx=0;xx<quad_count;xx++)
    {
        for (int y=0;y<quads[xx].size();y++)
        {
           cout << quads[xx][y] << " ";   
        }
        cout << "\n";
    }
    //----------------
    cout << "## N-SIDED  \n";
    for (int xx=0;xx<nsided_count;xx++)
    {
        for (int y=0;y<faces[xx].size();y++)
        {
           cout << faces[xx][y] << " ";   
        }
        cout << "\n";
    }

    cout << "-------------------------------------------------\n\n";
}





/**********************************************************/

// obj_pts    // vertices of model    
// lines      // 2 sided faces 
// triangles  // 3 sided 
// quads      // 4 sided 
// faces      // >4, N sided faces 
// bfr_pts    // general point buffer   ( tmp work area )
// bfr_faces  // general polygon buffer ( tmp work area ) 

// clear geometry work area for operations 
void model::reset_buffers(void)
{
    // bfr_faces[MAX_NUM_FACES];  // faces of work area 
    // bfr_pts[MAX_NUM_VERTICES]; // vertices of work area

    vtx_cnt = 0;
    fac_cnt = 0;
    vtx_tmp.clear();
    fac_tmp.clear();
}

/**********************************************************/
// add renderable line segment between two vectors  
void model::between_2vecs_as_line(Vector3 pt1, Vector3 pt2)  
{

    vector<int> newline;

    obj_pts[vertex_count] = pt1;
    newline.push_back(vertex_count+1);
    vertex_count++;
    
    obj_pts[vertex_count] = pt2;
    newline.push_back(vertex_count+1);
    vertex_count++;

    lines[ line_count ] = newline;  
    line_count++;
    

}

// UNTESTED - same but with RGB added 
void model::between_2vecs_as_line(Vector3 pt1, Vector3 pt2, Vector3 color)  
{

    vector<int> newline;

    obj_pts[vertex_count] = pt1;
    vtx_rgb[vertex_count] = color; vtxrgb_count++;

    newline.push_back(vertex_count+1);
    vertex_count++;
    
    //--- 

    obj_pts[vertex_count] = pt2;
    vtx_rgb[vertex_count] = color; vtxrgb_count++;

    newline.push_back(vertex_count+1);
    vertex_count++;

    lines[ line_count ] = newline;  
    line_count++;
    

}

/**********************************************************/
/*
    add vector as a line segment, 
    but position it off origin at a position
*/

void model::vec3_as_geom_atpos( Vector3 pt1 , Vector3 atpos)
{

    vector<int> newline;

    obj_pts[vertex_count] = atpos;
    newline.push_back(vertex_count+1);
    vertex_count++;
    
    obj_pts[vertex_count] = atpos+pt1;
    newline.push_back(vertex_count+1);
    vertex_count++;

    lines[ line_count ] = newline;  
    line_count++;

}

//UNTESTED 
void model::vec3_as_geom_atpos( Vector3 pt1 , Vector3 atpos, Vector3 color)
{

    vector<int> newline;

    obj_pts[vertex_count] = atpos;
    vtx_rgb[vertex_count] = color;vtxrgb_count++; 
    newline.push_back(vertex_count+1);
    vertex_count++;
    
    obj_pts[vertex_count] = atpos+pt1;
    vtx_rgb[vertex_count] = color; vtxrgb_count++;    
    newline.push_back(vertex_count+1);
    vertex_count++;

    lines[ line_count ] = newline;  
    line_count++;

}

/**********************************************************/
// UNTESTED add vector as a line segment  
void model::vec3_as_geom(Vector3 pt1)
{

    vector<int> newline;

    obj_pts[vertex_count] = Vector3(0,0,0);
    newline.push_back(vertex_count+1);
    vertex_count++;
    
    obj_pts[vertex_count] = pt1;
    newline.push_back(vertex_count+1);
    vertex_count++;

    lines[ line_count ] = newline;  
    line_count++;
    

}

/**********************************************************/
// UNTESTED add vector as a line segment  
void model::vec3_as_pt_geom(Vector3 pt1, double siz)
{

    between_2vecs_as_line(pt1, pt1-Vector3( siz, 0, 0) );
    between_2vecs_as_line(pt1, pt1+Vector3( siz, 0, 0) );
    
    between_2vecs_as_line(pt1, pt1-Vector3( 0, siz, 0) );
    between_2vecs_as_line(pt1, pt1+Vector3( 0, siz, 0) );

    between_2vecs_as_line(pt1, pt1-Vector3( 0, 0, siz) );
    between_2vecs_as_line(pt1, pt1+Vector3( 0, 0, siz) );

}

void model::vec3_as_pt_geom(Vector3 pt1, Vector3 color, double siz)
{

    between_2vecs_as_line(pt1, pt1-Vector3( siz, 0, 0), color );
    between_2vecs_as_line(pt1, pt1+Vector3( siz, 0, 0), color );
    
    between_2vecs_as_line(pt1, pt1-Vector3( 0, siz, 0), color );
    between_2vecs_as_line(pt1, pt1+Vector3( 0, siz, 0), color );

    between_2vecs_as_line(pt1, pt1-Vector3( 0, 0, siz), color );
    between_2vecs_as_line(pt1, pt1+Vector3( 0, 0, siz), color );

}

/**********************************************************/
void model::print(Vector3 in)
{
    cout  <<" "<< in.x << " " << in.y << " " << in.z <<" "; 
}


/**********************************************************/
// UNTESTED add a new triangle using 3 vector3 
void model::add_tri(Vector3 pt1, Vector3 pt2, Vector3 pt3)
{
    vector<int> newtri;

    obj_pts[vertex_count] = pt1;
    newtri.push_back(vertex_count+1);
    vertex_count++;
    
    obj_pts[vertex_count] = pt2;
    newtri.push_back(vertex_count+1);
    vertex_count++;
    
    obj_pts[vertex_count] = pt3;
    newtri.push_back(vertex_count+1);
    vertex_count++;

    triangles[ triangle_count ] = newtri;  
    triangle_count++;

}

/**********************************************************/
// add a new triangle using Face Indices to existing vertices
void model::add_tri(int vid1, int vid2, int vid3)
{

    // debug - use fac_tmp instead? 
    vector<int> newtri;
    newtri.push_back(vid1);
    newtri.push_back(vid2);
    newtri.push_back(vid3);

    triangles[ triangle_count ] = newtri;  // 3 sided 
    triangle_count++;

}

/**********************************************************/
// add a new triangle AND new points 
void model::append_tri(Vector3 pt1, Vector3 pt2, Vector3 pt3, int vid1, int vid2, int vid3)
{

    vertex_count++;
    obj_pts[vertex_count] = pt1 ;
    vertex_count++;
    obj_pts[vertex_count] = pt2 ;
    vertex_count++;
    obj_pts[vertex_count] = pt3 ;    
    //--------------------
    vector<int> newtri;
    newtri.push_back(vid1);
    newtri.push_back(vid2);
    newtri.push_back(vid3);

    triangles[ triangle_count ] = newtri;  // 3 sided 
    triangle_count++;
}

/**********************************************************/
/*
    turn a single 4 sided polygon into two 3 sided 
    
    +--+              +--+ 
    |  |              | /|  
    |  |   Becomes    |/ |  
    +--+              +--+  
*/

void model::op_triangulate(void)
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

    if (quad_count>0)
    {
        // walk the faces, assume there are ALWAYS 4 indices (quad) 
        for (int i=0; i<quad_count; i++) 
        {
            //cout << quads[i][0]-1 <<" "<< quads[i][1]-1 <<" "<< quads[i][2]-1 << "\n";
  
            add_tri(quads[i][0],quads[i][1],quads[i][2]);
            add_tri(quads[i][2],quads[i][3],quads[i][0]);           
        }
    }

   // cout << "end triangulate \n";
}

/**********************************************************/

/*
    a "render-constructor". 

    scan all loaded geom and prep for rendering 
    mostly just get things into triangles 
*/

void model::prep_render(void)
{
    
    model::op_triangulate();
    model::flatten_edits();

}

/**********************************************************/

/*
    Idea to get around having to re-order polygons each time:


    - stack up polygon operations  
    - mark deleted verts or faces with a -1 index 
    - set geom_edited to true 
    
    - wash, rinse repeat 

    - when you are done, run a cleanup function that re orders the -1 indices, 
    - set geom_edited = false
*/


void model::flatten_edits(void)
{
    cout << "begin flatten \n";



    if (model::geom_edited == true)
    {
        // check points for (??)
            /*
                iterate all points 
                if NOT -1 copy to buffer 
                swap buffer with points if any -1 found 
            */   

        // check lines for -1 

        // check triangles for -1 
            /*
                iterate all points 
                if NOT -1 copy to buffer 
                swap buffer with points if any -1 found 
            */  

        // check quads for -1 
            /*
                iterate all points 
                if NOT -1 copy to buffer 
                swap buffer with points if any -1 found 
            */  

        // check faces for -1  
        

    }

    cout << "end flatten \n";
}


/**********************************************************/

bool sort_by_zdist(const zindex_faces &lzif, const zindex_faces &rzif){ 
    // lambda for sort function 
    return lzif.distance > rzif.distance;
}

/*******/

//re order face indices of a 3D object based on distance from a point 
void model::op_zsort(Vector3 campos)
{

    // obj_pts[vcnt].set( sin(deg_to_rad(a))*scale, cos(deg_to_rad(a))*scale, 0 ); 

    int i,j = 0;

    vector <zindex_faces> sort_tris(MAX_NUM_FACES);
    
    cout << "camera pos is " << campos << "\n";
    
    // walk the triangles 
    for (int i=0; i<triangle_count; i++) 
    {
        fac_tmp.clear();
        fac_tmp = model::triangles[i];
        
        // assume triangle only (3 indices)
        Vector3 p1 = model::obj_pts[ fac_tmp[0]-1 ];
        Vector3 p2 = model::obj_pts[ fac_tmp[1]-1 ];
        Vector3 p3 = model::obj_pts[ fac_tmp[2]-1 ];                 

        Vector3 centr = centroid(p1, p2, p3);
    
        //use a struct (zipped array-ish) to sort triangles by distance to camera
        zindex_faces tmp;
        tmp.triangle = fac_tmp;
        tmp.distance = centr.distance(campos);

        sort_tris[i] = tmp;

    }


    sort(sort_tris.begin(), sort_tris.end(), sort_by_zdist);

    //cout << "sort_tris length " << sort_tris.size() << "\n";

    // overwrite sorted faces with sorted faces 
    for (int i=0; i<triangle_count; i++) 
    {
        model::triangles[i] = sort_tris[i].triangle;
        //cout << "sort_tris "<< i <<" " << sort_tris[i].face[0]<< " " << sort_tris[i].face[1] << " "<< sort_tris[i].face[2] << "\n";
    }


    //cout << "triangle count is " << triangle_count << " size of sorted is " << j <<"\n";

}


/**********************************************************/

//save geometry in memory to an OBJ file on disk 
void model::save_obj( char* filename)
{
    ofstream myfile;
    myfile.open (filename);

    myfile << "#Exported with Keith's little graphics tool\n";
    myfile << "#number of verticies "<< model::vertex_count  <<"\n";
    myfile << "#number of triangles     "<< triangle_count <<"\n";
    myfile <<"\n";

    
    for (int xx=0;xx<model::vertex_count;xx++)
    {
        // cout << vtx_rgb[xx] << "\n";


        if (vtx_rgb[xx][0]) 
        {
            myfile << "v " << obj_pts[xx][0] <<" "<< obj_pts[xx][1] <<" "<< obj_pts[xx][2] 
                   << " "  << vtx_rgb[xx][0] <<" "<< vtx_rgb[xx][1] <<" "<< vtx_rgb[xx][2] 
                           <<"\n";
        }else{
            myfile << "v " << obj_pts[xx][0] <<" "<< obj_pts[xx][1] <<" "<< obj_pts[xx][2] <<"\n";
        }
    }

    myfile <<"\n";
    
    if (triangle_count==0){
        cout << " warning - no faces to export ";
    }

    // cout << " triangle count is  " << triangle_count << endl;

    int ff = 0; 
    int xx = 0;

    //----------------------

    /*
    // export array of N sided faces
    if (nsided_count>0)
    {
        for (xx=0; xx<nsided_count; xx++)
        {
            myfile << "f ";
            for (ff=0; ff < faces[xx].size();ff++)
            {
                myfile << faces[xx][ff] << " "; 
            }
            myfile << "\n";
        }
    }*/

    // -----------------------

    // export array of lines
    if(line_count>0)
    {
        for (xx=0; xx<line_count; xx++)
        {
            myfile << "f ";
            for (ff=0; ff < lines[xx].size();ff++)
            {
                if (ff<lines[xx].size()-1){ 
                    myfile << lines[xx][ff] << " ";
                } else{
                    myfile << lines[xx][ff];                    
                }
            }
            myfile << "\n";
        }
    }

    //----------------------

    // export array of triangles
    if(triangle_count>0)
    {
        for (xx=0; xx<triangle_count; xx++)
        {
            myfile << "f ";
            for (ff=0; ff < triangles[xx].size();ff++)
            {
                myfile << triangles[xx][ff] << " "; 
            }
            myfile << "\n";
        }
    }

    //----------------------
    // export array of quads
    if(quad_count>0)
    {
        for (xx=0; xx<quad_count; xx++)
        {
            myfile << "f ";
            for (ff=0; ff < quads[xx].size();ff++)
            {
                myfile << quads[xx][ff] << " "; 
            }
            myfile << "\n";
        }
    }

    //----------------------

    myfile.close();
    cout << endl << "obj file " << filename << " exported." << endl;

}

/**********************************************************/
void model::load_obj(char* filename){
    ifstream fin;
    fin.open(filename); // open a file
    if (!fin.good()){ 
        cout << "OBJ file \""<< filename <<"\" appears to be missing or broken." << endl;
        exit (EXIT_FAILURE); // exit if file not found
    }

    vtx_tmp.clear();
    fac_tmp.clear();

    while (!fin.eof())
    {
        char buf[MAX_CHARS_PER_LINE];
        fin.getline(buf, MAX_CHARS_PER_LINE);
        
        int i = 0;
        int n = 0; 
        fac_tmp.clear();

        const char* token[MAX_TOKENS_PER_LINE] = {};
        token[0] = strtok(buf, DELIMITER);

        //if line has data on it ...  
        if (token[0]) 
        {
            // walk the space delineated tokens 
            for (n=1; n < MAX_TOKENS_PER_LINE; n++)
            {
                token[n] = strtok(NULL, " \n\t");
                if (!token[n]) break;  
            }

        }

        /******************************/

        // process the tokens
        for (i=1; i<n; i++)
        {

            // vertex tokens "v"
            if (!strcmp(token[0],v_chr))
            {
                // first 2 of 3 vertices
                if (vtx_cnt<=2){
                    vtx_tmp.push_back( atof(token[i]) );
                }
                // third vertex 
                if (vtx_cnt==3){
                    vtx_tmp.push_back( atof(token[i]) );
                    vtx_cnt=0;
                   
                    //load 3D point 
                    obj_pts[model::vertex_count].set(vtx_tmp[0],vtx_tmp[1],vtx_tmp[2]);
                    
                    vtx_tmp.clear();
                    model::vertex_count++;
                }
                vtx_cnt++;
            }
  
            /**********/
            // face tokens "f"
            if (!strcmp(token[0],f_chr))
            {
               
                // cout << " i is "<< i << " |  num_tok  " << n << endl; 

                // if end of line is not hit, store loaded data 
                if (i < n-1 ) 
                {
                    //keep adding fids  
                    fac_tmp.push_back( atof(token[i]) );
                }

                // if end of line is hit - store loaded data and reset                
                if (i == n-1 ) 
                {
                    // add one more fid before we stop to get lest 
                    fac_tmp.push_back( atof(token[i]) );
                    
                    // 2 sided polygons (lines) 
                    if(i==2)
                    {
                        lines[line_count] = fac_tmp; 
                        fac_tmp.clear();   
                        line_count++;    
                    }

                    // 3 sided polygons 
                    if(i==3)
                    {
                        
                        triangles[triangle_count] = fac_tmp; 
                        fac_tmp.clear();   
                        triangle_count++;                           
                    }
                    
                    // 4 sided polygons 
                    if(i==4)
                    {
                        quads[quad_count] = fac_tmp;  
                        fac_tmp.clear();   
                        quad_count++;                              
                    }   

                    // N sided polygons 
                    if(i>4)
                    {
                        faces[nsided_count] = fac_tmp; 
                        fac_tmp.clear(); // tmp face buffer 
                        nsided_count++;  
                    }
                }
            }

            /**********/
            // normal tokens "vn"
            //if (!strcmp(token[0],nrml_chr)){}
            /**********/
            // UV tokens "vt"
            //if (!strcmp(token[0],uv_chr)){}
            /**********/
            // line tokens "l"
            //if (!strcmp(token[0],line_chr)){}


        }//iterate tokens
       
  
    }//iterate each line
}//load object


/**********************************************************/
/*
   load a 4X4 matrix from disk 
   used as a projection matrix for render geometry 
*/
void model::load_matrix(char* filename)
{
    
    ifstream fin;
    fin.open(filename); // open a file
    if (!fin.good()){ 
        cout << "matrix file \""<< filename <<"\" appears to be missing or broken." << endl;
        exit (EXIT_FAILURE); // exit if file not found
    }

    int line_ct = 0;
    while (!fin.eof())
    {
        char buf[MAX_CHARS_PER_LINE];
        fin.getline(buf, MAX_CHARS_PER_LINE);
        
        int i = 0;
        int n = 0; 

        const char* token[MAX_TOKENS_PER_LINE] = {};
        token[0] = strtok(buf, DELIMITER);

        //if line has data on it ...  
        if (token[0]) 
        {
            // walk the space delineated tokens 
            for (n=1; n < MAX_TOKENS_PER_LINE; n++)
            {
                token[n] = strtok(NULL, " \t\n");
                if (!token[n]) break;  
            }

        }
        
        if (line_ct==0){
            m44[0]=atof(token[0]); m44[1]=atof(token[1]); m44[2]=atof(token[2]); m44[3]=atof(token[3]);
        }
        if (line_ct==1){
            m44[4]=atof(token[0]); m44[5]=atof(token[1]); m44[6]=atof(token[2]); m44[7]=atof(token[3]);
        }
        if (line_ct==2){
            m44[8]=atof(token[0]); m44[9]=atof(token[1]); m44[10]=atof(token[2]); m44[11]=atof(token[3]);
        }
        if (line_ct==3){
            m44[12]=atof(token[0]); m44[13]=atof(token[1]); m44[14]=atof(token[2]); m44[15]=atof(token[3]);
        }

        line_ct ++; 
    }

}


/**********************************************************/

//sample 3d object - may not be the "proper" way to do it 
void model::make_cube(double scale){

    // vertices
    obj_pts[0].set(-scale, -scale,  scale);
    obj_pts[1].set(-scale, -scale, -scale);
    obj_pts[2].set( scale, -scale, -scale);
    obj_pts[3].set( scale, -scale,  scale);
    obj_pts[4].set(-scale,  scale,  scale);
    obj_pts[5].set(-scale,  scale, -scale);
    obj_pts[6].set( scale,  scale, -scale);
    obj_pts[7].set( scale,  scale,  scale);
       
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

    quad_count = 6;

    model::vertex_count = 8;
}

/**********************************************************/
//sample 3d object - may not be the "proper" way to do it
 void model::make_circle(int divs, double scale)
 {

    double a = 0;
    int vcnt = 0;
    int step = 360/divs;
    
    cout << "step is " << step << endl ;

    fac_tmp.clear();

    for (a=0;a<360;a=a+step)
    {
        //cout << "a is " << a << endl ;
        
        //x axis 
        //obj_pts[vcnt].set(0,  sin(deg_to_rad(a))*scale, cos(deg_to_rad(a))*scale );

        //y axis 
        //obj_pts[vcnt].set( sin(deg_to_rad(a))*scale, 0, cos(deg_to_rad(a))*scale ); 

        //z axis 
        obj_pts[vcnt].set( sin(deg_to_rad(a))*scale, cos(deg_to_rad(a))*scale, 0 ); 
        vcnt++;
    } 

    // cout << faceindices[0] << faceindices[5];

    int i = 0; 
 
    for (int i=0; i<vcnt; i++) 
    {
        //fac_tmp.clear();
        fac_tmp.push_back(i+1);  
    }


    faces[0] = fac_tmp;
    nsided_count = 1;

    model::vertex_count = vcnt;
 }     

/**********************************************************/
 //sample 3d object - may not be the "proper" way to do it
 void model::make_square(double scale)
 {
    fac_tmp.clear();

    // vertices - (3d vectors)
    
    // obj_pts[vcnt].set( sin(deg_to_rad(a))*scale, cos(deg_to_rad(a))*scale, 0 ); 

    obj_pts[0].set(-scale, 1,  scale);
    obj_pts[1].set( scale, 1,  scale);
    obj_pts[2].set( scale, 1, -scale);
    obj_pts[3].set(-scale, 1, -scale);

    // face indices are NOT zero indexed 
    
    // faces[0] = {1,2,3,4};
    // face_count = 1;

    quads[0] = {1,2,3,4};
    quad_count = 1;

    vertex_count =4;
 }    


/**********************************************************/
 //sample 3d object - may not be the "proper" way to do it
 void model::make_triangle(double scale)
 {
    fac_tmp.clear();

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

    // // // Z axis 
    obj_pts[0].set( -scale ,  0    , 0 );
    obj_pts[1].set( 0      ,  scale, 0 );
    obj_pts[2].set( scale  ,  0    , 0 );
    
    // faces[0] = {1,2,3};
    // face_count = 1;
    
    vertex_count = 3;
    add_tri(1,2,3);
    //add_tri(2,1,3);

 }    



/**********************************************************/
 //sample 3d object - may not be the "proper" way to do it
 void model::make_line(double scale)
 {
    // vertices - (3d vectors)
    obj_pts[0].set(-scale, 0,  -scale);
    obj_pts[1].set( scale, 0,  scale);

    faces[0] = {1,2}; //notice this is only a 2 point poly (line)

    line_count = 1;

    model::vertex_count =4;
 }     



