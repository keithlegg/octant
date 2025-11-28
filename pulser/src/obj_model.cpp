/*****************************************************************************/
    /* 
        obj_model.c
        
            Load and save 3D models from disk 


        Copyright (C) 2026 Keith Legg  - keithlegg23@gmail.com
    
    */
         

/*****************************************************************************/



#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iostream>

#include <fstream>
#include <vector>
using std::vector;

#include <sstream>

//#include <iomanip>

//#include <string>
//#include <cstring>

#include "obj_model.h"


int uv_cnt   = 0;  // number of UVs loaded 
 

void obj_model::clearall(void)
{

    memset(obj_model::uvs,      0, obj_model::num_pts);
    memset(obj_model::points,   0, obj_model::num_pts);
    memset(obj_model::vnormals, 0, obj_model::num_pts);
    memset(obj_model::vtxrgb,   0, obj_model::num_pts);
    memset(obj_model::fnormals, 0, obj_model::num_pts);
    memset(obj_model::tris,     0, obj_model::num_tris);
    memset(obj_model::quads,    0, obj_model::num_quads);

    obj_model::num_pts = 0;
    obj_model::num_uvs = 0;
    obj_model::num_lines = 0;
    obj_model::num_tris = 0;
    obj_model::num_quads = 0;
    obj_model::num_vnrmls = 0;
    obj_model::num_fnrmls = 0;
    obj_model::num_vtxrgb = 0;

}



void obj_model::add_triangle(vec3 pt1, vec3 pt2, vec3 pt3)
{
    //vector<int> newtri;
    int vertex_count = 0;


    // int num_pts;
    // int num_vtxrgb;
    // int num_vnrmls;    
    // int num_fnrmls;
    // int num_uvs;
    // int num_lines;
    // int num_tris;
    // int num_quads; 
    // pt_model_buffer->num_tris
    // pt_model_buffer->num_quads

    obj_model::points[obj_model::num_pts] = pt1;
    //newtri.push_back(pt_model_buffer->num_pts+1);
    obj_model::num_pts++;
    
    obj_model::points[obj_model::num_pts] = pt2;
    //newtri.push_back(pt_model_buffer->num_pts+1);
    obj_model::num_pts++;
    
    obj_model::points[ obj_model::num_pts ] = pt3;
    //newtri.push_back(pt_model_buffer->num_pts+1);
    obj_model::num_pts++;

    //triangles[ triangle_count ] = newtri;  
    //triangle_count++;

}


/*


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

*/


/*******************************************************************/


// UNTESTED  NOT WORKING YET 
//     turn a single 4 sided polygon into two 3 sided 
//     
//     +--+              +--+ 
//     |  |              | /|  
//     |  |   Becomes    |/ |  
//     +--+              +--+  

void obj_model::triangulate(void)
{

    int  p_i = 0;
    vec3 tri_cntr;

    //debug - clear any loaded normals 
    //pt_model_buffer->num_fnrmls = 0;

    printf("#number of quads %d\n", obj_model::num_quads );



    //calc normals for quads 
    for (p_i=0;p_i<obj_model::num_quads;p_i++)
    {   
           
        // fetch the pts for a triangle
        vec3 p1 = obj_model::points[obj_model::quads[p_i].pt1-1];
        vec3 p2 = obj_model::points[obj_model::quads[p_i].pt2-1];
        vec3 p3 = obj_model::points[obj_model::quads[p_i].pt3-1];

        // // calculate the face normal  
        // vec3 a = sub(p1,p2);
        // vec3 b = sub(p1,p3);
        // vec3 n = normalize(cross(a,b));
        // pt_model_buffer->fnormals[pt_model_buffer->num_fnrmls]=n;
        // pt_model_buffer->num_fnrmls++;

    }

}


/*******************************************************************/

void obj_model::calc_normals(void)
{
    int  p_i = 0;
    vec3 tri_cntr;

    //debug - clear any loaded normals 
    obj_model::num_fnrmls = 0;

    //calc normals for quads 
    for (p_i=0;p_i<obj_model::num_quads;p_i++)
    {   
            
        // fetch the pts for a triangle
        vec3 p1 = obj_model::points[obj_model::quads[p_i].pt1-1];
        vec3 p2 = obj_model::points[obj_model::quads[p_i].pt2-1];
        vec3 p3 = obj_model::points[obj_model::quads[p_i].pt3-1];

        // calculate the face normal  
        vec3 a = sub(p1,p2);
        vec3 b = sub(p1,p3);
        vec3 n = normalize(cross(a,b));

        obj_model::fnormals[obj_model::num_fnrmls]=n;
        obj_model::num_fnrmls++;


    }

    // calc normals for triangles 
    for (p_i=0;p_i<obj_model::num_tris;p_i++)
    {   
            
        // fetch the pts for a triangle
        vec3 p1 = obj_model::points[obj_model::tris[p_i].pt1-1];
        vec3 p2 = obj_model::points[obj_model::tris[p_i].pt2-1];
        vec3 p3 = obj_model::points[obj_model::tris[p_i].pt3-1];

        // calculate the face normal  
        vec3 a = sub(p1,p2);
        vec3 b = sub(p1,p3);
        vec3 n = normalize(cross(a,b));

        obj_model::fnormals[obj_model::num_fnrmls]=n;
        obj_model::num_fnrmls++;
    }

    // broken experiment to put face normals in vertex normals
    // only do so if nothing was loaded from the model 
    // DEBUG - its wrong !
    if (obj_model::num_vnrmls==0)
    {
        for (p_i=0;p_i<obj_model::num_tris;p_i++)
        {    
            // fetch the pts for a triangle
            vec3 p1 = obj_model::points[obj_model::tris[p_i].pt1-1];
            vec3 p2 = obj_model::points[obj_model::tris[p_i].pt2-1];
            vec3 p3 = obj_model::points[obj_model::tris[p_i].pt3-1];

            // calculate the face normal  
            vec3 a = sub(p1,p2);
            vec3 b = sub(p1,p3);
            vec3 n = normalize(cross(a,b));
            obj_model::vnormals[obj_model::tris[p_i].pt1-1]= n;
            obj_model::vnormals[obj_model::tris[p_i].pt2-1]= n;
            obj_model::vnormals[obj_model::tris[p_i].pt3-1]= n;      
            obj_model::num_vnrmls++;
            obj_model::num_vnrmls++;
            obj_model::num_vnrmls++;
    
        }

    }

}


/*******************************************************************/

/*
    // DEPRECATED!  - get data staight from the object class instead 
    Take a pointer to an object and populate another container object with info about it  
*/

void get_obj_info( obj_model* loader,  obj_info* obinfo)
{
    // reset the bounding box 
    obinfo->bb_min_x = 0;
    obinfo->bb_max_x = 0;
    obinfo->bb_min_y = 0;
    obinfo->bb_max_y = 0;
    obinfo->bb_min_z = 0;
    obinfo->bb_max_z = 0; 

    int x = 0;
    vec3 point;

    for(x=0;x<loader->num_pts;x++)
    {
        
        point = loader->points[x];

        if (point.x < obinfo->bb_min_x){
            obinfo->bb_min_x = point.x;    
        }
        if (point.x > obinfo->bb_max_x){
            obinfo->bb_max_x = point.x;    
        }

        if (point.y < obinfo->bb_min_y){
            obinfo->bb_min_y = point.y;    
        }
        if (point.y > obinfo->bb_max_y){
            obinfo->bb_max_y = point.y;    
        }

        if (point.z < obinfo->bb_min_z){
            obinfo->bb_min_z = point.z;    
        }
        if (point.z > obinfo->bb_max_z){
            obinfo->bb_max_z = point.z;    
        }        
        
        // print_vec3(point);

    }

    // printf("minx %f maxx %f miny %f maxy %f minz %f maxz %f \n", obinfo->bb_min_x
    //                                                            , obinfo->bb_max_x
    //                                                            , obinfo->bb_min_y
    //                                                            , obinfo->bb_max_y
    //                                                            , obinfo->bb_min_z
    //                                                            , obinfo->bb_max_z );

}

/*******************************************************************/
/*
    Print out info from an object info struct
*/


vec3 get_extents(obj_info* obinfo){

    float bb_min_x;
    float bb_max_x;
    
    float bb_min_y;
    float bb_max_y;

    float bb_min_z;
    float bb_max_z;
}




void show_obj_geom(obj_model* loader)
{
    int i = 0;
    printf("\n");

    printf("\n# point indices  ---------------- %d \n", loader->num_pts);
    for (i=0;i<loader->num_pts;i++)
    {
        printf("%d pt    ",i);
        //DEBUG UNTESTED NEW PRINT IN C++
        //print_vec3( &loader->points[i]) ;
    }

    printf("\n# line indices  ------------------- %d \n", loader->num_lines);
    for (i=0;i<loader->num_lines;i++)
    {
        printf(" %d line   %d %d  \n",i ,  loader->lines[i].pt1 , loader->lines[i].pt2 );
        //print_vec3( loader->lines[i]) ;
    }

    printf("\n# triangle indices ----------------- %d \n", loader->num_tris);
    for (i=0;i<loader->num_tris;i++)
    {

        //triangle tri_buffer = from_obj->tris[i]; //start with original indices
        printf(" %d triangle   %d %d %d \n", i, loader->tris[i].pt1 , loader->tris[i].pt2, loader->tris[i].pt3);        
    }


    printf("\n# quad indices ------------------   %d \n", loader->num_quads);
    for (i=0;i<loader->num_quads;i++)
    {

        quad qbfr = loader->quads[i]; //start with original indices
        printf(" %d quad   %d %d %d %d \n", i, qbfr.pt1 , qbfr.pt2, qbfr.pt3, qbfr.pt4 );        
    }

    //printf("\n# Normals   --------  \n");

    //printf("\n# UVs   --------  \n");

    //printf("\n# Colors   --------  \n");

}



void show(obj_model* objmodel)
{
    printf("#----------------#\n");
    printf("# number  points    %d \n", objmodel->num_pts);
    printf("# number  lines     %d \n", objmodel->num_lines);
    printf("# number  triangles %d \n", objmodel->num_tris);
    printf("# number  quads     %d \n", objmodel->num_quads);
    printf("# number  UVs       %d \n", objmodel->num_uvs);
    printf("# number  vtx nrml  %d \n", objmodel->num_vnrmls);
    printf("# number  fac nrml  %d \n", objmodel->num_fnrmls);
    printf("# number  vtxcolr   %d \n", objmodel->num_vtxrgb);    
}



void show(obj_info* obinfo)
{
    printf("#----------------#\n");

    //printf("# extents           %d \n", 99);
    //printf("# centroid          %d \n", 99);
}


/*******************************************************************/
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

/*******************************************************************/
/*
    Clear an object to "reset" it
*/

void reset_objfile(obj_model* loader, obj_info* obinfo)
{

    // uv_cnt   = 0;  // number UVs loaded 

    memset(loader->uvs,      0, loader->num_pts);
    memset(loader->points,   0, loader->num_pts);
    memset(loader->vnormals, 0, loader->num_pts);
    memset(loader->vtxrgb,   0, loader->num_pts);
    memset(loader->fnormals, 0, loader->num_pts);
    memset(loader->tris,     0, loader->num_tris);
    memset(loader->quads,    0, loader->num_quads);

    //struct vec3 vtxrgb[num_vtx];        // 3 floats - color per vertex 
    //struct line lines[num_faces];       // 2 ints   - lines    idx

    loader->num_pts = 0;
    loader->num_uvs = 0;
    loader->num_lines = 0;
    loader->num_tris = 0;
    loader->num_quads = 0;
    loader->num_vnrmls = 0;
    loader->num_fnrmls = 0;
    loader->num_vtxrgb = 0;

    obinfo->bb_min_x = 0;
    obinfo->bb_max_x = 0;
    obinfo->bb_min_y = 0;
    obinfo->bb_max_y = 0;
    obinfo->bb_min_z = 0;
    obinfo->bb_max_z = 0; 

}

/*******************************************************************/


std::vector<std::string> tokenizer( const std::string& p_pcstStr, char delim )  {
    std::vector<std::string> tokens;
    std::stringstream   mySstream( p_pcstStr );
    std::string         temp;
    while( getline( mySstream, temp, delim ) ) {
        tokens.push_back( temp );
    }
    return tokens;
} 

/*******************************************************************/
void load_objfile( char *filepath, obj_model* loader)
{


    std::cout << "load_objfile loading file "<< filepath << "\n";
    
    int pofst = 0; //pointoffset indices to points if geom exists already 
    int line_ct = 0;

    // std::string s;
    // std::ifstream in(filepath);
    // if(!in) {
    //     std::cerr << "File not opened" << std::endl;
    //     exit(1);
    // }
    // while(in >> s) 
    // {

    //std::ifstream obj_filein;
    //obj_filein.open(filepath); // open a file
    
    std::ifstream obj_filein(filepath, std::ifstream::in);

    if (!obj_filein.good()){ 
        std::cout << "config file \""<< filepath <<"\" appears to be missing." << std::endl;
        exit (EXIT_FAILURE); // exit if file not found
    }

    //while (!obj_filein.eof())
    while (!obj_filein.eof() && !obj_filein.fail() && !obj_filein.bad())
    {  

        std::string line;
        while (std::getline(obj_filein, line)) 
        {  
            // std::cout << "FULL LINE " << line << std::endl;
            std::vector<std::string>  tokenized = tokenizer(line, *" ");

            //point offset indices to points - if geom exists already 
            if (loader->num_pts>0){
                pofst = loader->num_pts;
            }

            std::string coords_str; // string that verts get copied to 
            std::string nrmls_str;  // string that verts get copied to 
            std::string fidx_str;   // string that faces get copied to

            //if line is not blank
            if(tokenized.size()>0)
            {
                int commentpos = tokenized.at(0).find('#');
                if ( commentpos != std::string::npos)
                {
                    //line is commented out  
                }
                else
                {
                    //-----------------------------//
                    // look for V / vertices
                    if ( tokenized.at(0).find("v") != std::string::npos )
                    {
                        int a = 0;
                        int vidx = 0;
                        
                        float  xc, yc, zc = 0.0;
                        float  cr, cg, cb = 0.0; //RGB float (0.0 - 1.0)

                        //walk the space delineated tokens per each line
                        for (a=0;a<tokenized.size();a++)
                        {   
                 
                            //std::cout << " line " << line_ct << " vertex " << vidx << " " << tokenized.at(a) <<"\n"; // <- vertex line 
                          
                            if(vidx==1){
                                xc = std::stof(tokenized.at(a));
                            }
                            if(vidx==2){
                                yc = std::stof(tokenized.at(a));                        
                            }  
                            if(vidx==3){
                                zc = std::stof(tokenized.at(a));
                            }                                        
                              
                            //optional vertex color 
                            if(vidx==4){
                                cr = std::stof(tokenized.at(a));
                            }
                            if(vidx==5){
                                cg = std::stof(tokenized.at(a));                        
                            }  
                            if(vidx==6){
                                cb = std::stof(tokenized.at(a));
                            } 

                            vidx++; 
                        }
                        
                        //--------------------------------//
                        //done looping vertex line, now process the data we found 
                        
                        //std::cout << "VIDX IS "<< vidx <<"\n"; 

                        //if two points its a 2D coord ( not a standard obj file )  
                        if (vidx==3){
                            std::cout << "2D point detected!\n"; 
                        }
                        
                        //if three points its a proper vertex 
                        if (vidx==3 || vidx==7)
                        {
                            vec3 vpt = newvec3( xc, yc, zc  );

                            //print_vec3(&vpt); //to view output 
                            std::cout << "NUM PTS LOADED "<< loader->num_pts << "\n";

                            loader->points[loader->num_pts] = vpt;
                            loader->num_pts++;
         
                            ////set color to white initially  ?? DEBUG 
                            //vec3 color;
                            //color.x=1.0;   
                            //color.y=1.0;
                            //color.z=1.0; 
                            //loader->vtxrgb[loader->num_vtxrgb] = color;

                        }  

                    }//end vertex loader 


                    //-----------------------------//

                    //  look for normals
                    if ( tokenized.at(0).find("vn") != std::string::npos )
                    {
                        /*
                        strcpy (nrmls_str, tok_spacs+4);

                        //walk the tokens on the line (a copy of it)
                        char* tok_line = strtok(nrmls_str, " ");
                        int nidx = 0;
                        
                        float xc, yc, zc = 0.0;

                        while (tok_line) 
                        {
                            // printf("%s \n", tok_line );   
                            
                            if(nidx==0){
                                xc = atof(tok_line);
                            }
                            if(nidx==1){
                                yc = atof(tok_line);                        
                            }  
                            if(nidx==2){
                                zc = atof(tok_line);
                            }                                        
                            
                            nidx++;tok_line = strtok(NULL, " \t\n");
                        }

                        if (nidx==3)
                        {
                            vec3 vn = newvec3( xc, yc, zc  );
                            loader->vnormals[loader->num_vnrmls] = vn;
                            loader->num_vnrmls++;

                        }     

                        */
                    }//end vertex normal loader 

                    //-----------------------------//

                    //  look for F / faces
                    if ( tokenized.at(0).find("f") != std::string::npos )
                    {
                        /*
                        strcpy (fidx_str, tok_spacs+2);
                        char* tok_line = strtok(fidx_str, " ");
                        int fidx = 0;
                        
                        int pt1,pt2,pt3,pt4 = 0;;

                        // walk the tokens on the line 
                        // ASSUME TRIANGLES ONLY! (3 coords per vertex)
                        // STUPID BUG - IF THERE IS EMPTY SPACE AT END OF FIDS IT COUNTS ONE MORE fidx
                        while (tok_line) 
                        {
                            //printf("%d %s\n", fidx, tok_line); // <- face line                  

                            //only supports 2,3,4 sided polygons  
                            if(fidx==0){
                                pt1 = atoi( tok_line);
                                if (pofst>0){ pt1 = pt1+pofst;};
                            }
                            if(fidx==1){
                                pt2 = atoi( tok_line);
                                if (pofst>0){ pt2 = pt2+pofst;};                                               
                            }  
                            if(fidx==2){
                                pt3 = atoi( tok_line);
                                if (pofst>0){ pt3 = pt3+pofst;};                         
                            }   
                            if(fidx==3){
                                pt4 = atoi( tok_line);
                                if (pofst>0){ pt4 = pt4+pofst;};                                               
                            }  
                            ////////////////////

                            //n = atoi (buffer);
                            tok_line = strtok(NULL, " \t\n");fidx++;

                        }
                        // STUPID BUG - IF THERE IS EMPTY SPACE AT END OF FIDS IT COUNTS ONE MORE fidx

                        //-------                  
                        //if two face indices - its a line  
                        if (fidx==2)
                        {
                            loader->lines[loader->num_lines].pt1 = pt1;
                            loader->lines[loader->num_lines].pt2 = pt2;                          
                            loader->num_lines++;                    
                        }//end line loader

                        //-------
                        if (fidx==3)
                        {

                            // if you want the actual point data from this index
                            // print_vec3(loader->points[pt1]);

                            //or just store the indices
                            loader->tris[loader->num_tris].pt1 = pt1;
                            loader->tris[loader->num_tris].pt2 = pt2;                          
                            loader->tris[loader->num_tris].pt3 = pt3;

                            loader->num_tris++;

                        }//end triangle loader

                        //------- 

                        if (fidx==4)
                        {
                            loader->quads[loader->num_quads].pt1 = pt1;
                            loader->quads[loader->num_quads].pt2 = pt2;                          
                            loader->quads[loader->num_quads].pt3 = pt3;
                            loader->quads[loader->num_quads].pt4 = pt4;
                            loader->num_quads++;
                        }//end quad loader 
                    */
                    }//end face loader

                    //-----------------------------//
                    //-----------------------------//
                }//line is not commented out
                line_ct++;

            }//line not blank
        }//line by line
    }//obj exists   

    // ---------------------------------------------
    //loader->num_uvs = 0;

    /* 
    printf("\n\n---------------------------\n"  ) ;
    printf("%d vertices loaded   \n", loader->num_pts    ) ;
    printf("%d uvs loaded        \n", loader->num_uvs    ) ; 
    printf("%d lines loaded      \n", loader->num_lines  ) ;
    printf("%d triangles loaded  \n", loader->num_tris   ) ;
    printf("%d quads loaded      \n", loader->num_quads  ) ;  
    */

}

/*******************************************************************/
void save_objfile( char *filepath, obj_model* loader)
{

   FILE * fp;

   fp = fopen (filepath, "w+");
   fprintf(fp, "%s %s %s %d", "i", "an", "not working yet", 555);
   
   fclose(fp);
      
}


/*******************************************************************/

void test_loader_data( obj_model* loader){

    loader->uvs[0]    = newvec2(  0.0 , 0.0         );  
    loader->points[0] = newvec3( -1.0 , -1.0,  1.0  );
    loader->uvs[1]    = newvec2( 1.0  , 0.0         );  
    loader->points[1] = newvec3( 1.0 , -1.0,  1.0   );
    loader->uvs[2]    = newvec2( 1.0, 1.0           );  
    loader->points[2] = newvec3( 1.0,  1.0,  1.0    );

    // Back Face
    loader->uvs[3]     = newvec2( 1.0, 0.0           );  
    loader->points[3]  = newvec3( -1.0, -1.0, -1.0   );
    loader->uvs[4]     = newvec2( 1.0, 1.0           );  
    loader->points[4]  = newvec3( -1.0,  1.0, -1.0   );
    loader->uvs[5]     = newvec2( 0.0, 1.0           );  
    loader->points[5]  = newvec3( 1.0,  1.0, -1.0    );

    //  uvs     // UV coords 
    //  points  // 3 floats  
    //  faces   // triangles only 
                    
}





