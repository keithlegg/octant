/*****************************************************************************/
/* 

    obj_fio.cpp
        
        Load and save 3D models from disk 
    

    "obj_model" is defined in a single header but split among 3 cpp files
    
    the files are:
        obj_model - container for top level attributes, etc
        obj_geom  - the core of geometry processing 
        obj_fio   - (this file) file in/out operations like load, save etc 



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


const int MAX_CHARS_PER_LINE = 512;
const int MAX_TOKENS_PER_LINE = 20;
const char* const DELIMITER = " ";



/**********************************************/

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
void obj_model::load(char *filepath)
{
    bool debug     = false;
    bool debugfile = false;


    uint pt1, pt2, pt3, pt4, ptn, ptn1 = 0;
    int vn1, vn2, vn3, vn4 = 0;


    std::cout << "## obj_model::load loading file "<< filepath << "\n";
    
    int pofst   = 0; //DEBUG point offset indices to points if geom exists already 
    int line_ct = 0; //idx of text line we are parsing 
    
    std::ifstream obj_filein(filepath, std::ifstream::in);

    if (!obj_filein.good()){ 
        std::cout << ".obj file \""<< filepath <<"\" appears to be missing." << std::endl;
        exit (1); // exit if file not found
    }

    //check to see if an object was already loaded - offset the point indeces if so 
    //point offset indices to points - if geom exists already 
    if (num_pts>0)
    {
        pofst = num_pts;
    }

    //begin the object load - walk the file parsing it 
    while (!obj_filein.eof() && !obj_filein.fail() && !obj_filein.bad())
    {  

        std::string line;
        while (std::getline(obj_filein, line)) 
        {  
            if(debugfile){ std::cout << "FULL LINE " << line << std::endl; }

            std::vector<std::string>  tokenized = tokenizer(line, *" ");

            std::string coords_str; // string that verts get copied to 
            std::string nrmls_str;  // string that verts get copied to 
            std::string fidx_str;   // string that faces get copied to

            //if line is not blank
            if(tokenized.size()>0)
            {

                int commentpos = tokenized.at(0).find('#');
                if ( commentpos != std::string::npos)
                {
                    // config file - line is commented out  
                }
                else
                {

                    //-----------------------------//
                    // look for V / vertices
                    if ( tokenized.at(0).find("v") != std::string::npos )
                    {
                        int vidx = 0;
                        
                        float  xc, yc, zc = 0.0;
                        float  cr, cg, cb = 0.0; //RGB float (0.0 - 1.0)

                        //walk the space delineated tokens per each line
                        for (uint a=0;a<tokenized.size();a++)
                        {   
                            if(debug){
                                std::cout << " line " << line_ct << " vtxpass " << vidx << " " << tokenized.at(a) <<"\n"; // <- vertex line 
                            }

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
                        if(debug)
                        {
                            std::cout << "vidx is "<< vidx <<"\n"; 
                        }

                        //--------------------------------//
                        //done looping vertex line, now process the data we found 
                        //std::cout << "VIDX IS "<< vidx <<"\n"; 

                        //if two points its a 2D coord ( not a standard obj file )  
                        if (vidx==2){
                            std::cout << "2D point detected!\n"; 
                        }
                        
                        //if three points its a proper vertex 
                        if (vidx>=3)
                        {
                            Vector3 vpt = Vector3( xc, yc, zc  );
                            //std::cout << "vtx 3d "<< vpt.x << " "<< vpt.y <<" "<<vpt.z<<"\n";

                            points[num_pts] = vpt;
                            num_pts = num_pts+1;
                        
                            //std::cout << "numpts:" << num_pts <<" "<< vpt.x << " "<< vpt.y << " " << vpt.z <<"\n";                                             
                        }  
                    }//end vertex loader 
                    //std::cout << "NUM PTS LOADED "<< num_pts << "\n";


                    //  look for normals
                    //-----------------------------//
                    
                    if ( tokenized.at(0).find("vn") != std::string::npos )
                    {
                        int nidx = 0;
                        float xc, yc, zc = 0.0;

                        //walk the space delineated tokens per each line
                        for (int a=0;a<tokenized.size();a++)
                        {   
                 
                            //std::cout << " line " << line_ct << " nrmlpass " << nidx << " " << tokenized.at(a) <<"\n"; // <- vertex line 
                            
                            if(nidx==1){
                                xc = std::stof(tokenized.at(a));
                            }
                            if(nidx==2){
                                yc = std::stof(tokenized.at(a));                        
                            }  
                            if(nidx==3){
                                zc = std::stof(tokenized.at(a));
                            }                                        
                            
                            if(nidx<3)
                            {
                                nidx++;
                            }
                        }

                        
                        if (nidx==3)
                        {
                            //this fills storage but we still need lookup to render  
                            Vector3 vn = Vector3( xc, yc, zc  );
                            vnormals[num_vnrmls] = vn;
                            num_vnrmls++;
                        }    
                    
                    }//end vertex normal loader 
                    
                    //-----------------------------//
                     
                    //  look for f/faces or l/lines
                    if ( tokenized.at(0).find("f") != std::string::npos || 
                         tokenized.at(0).find("l") != std::string::npos)
                    {
                        int fidx = 0;
                        bool poly_is_linetype = false;

                        if(tokenized.at(0).find("l") != std::string::npos)
                        {
                            poly_is_linetype = true;
                        }



                        //walk the space delineated tokens per each line
                        for (int a=1;a<tokenized.size();a++)
                        {   

                            // get type  
                            //if(debug)
                            //{
                            //    std::cout << " FOO  " << typeid(tokenized.at(fidx)).name()  << "\n";
                            //}

                            if( tokenized.at(a).size())
                            {
                                if(debug)
                                {
                                    std::cout << " pofst " << pofst <<" line " << line_ct << " idx:" << a << " tokenized : " << tokenized.at(a) <<"\n"; // <- vertex line 
                                }


                                //------                                   
                                //only supports 2,3,4 sided polygons  
                                if(fidx==0)
                                {
                                    //deal with "/" delineated files
                                    if ( tokenized.at(a).find("/") != std::string::npos )
                                    { 
                                        std::vector<std::string>  sl1 = tokenizer(tokenized.at(a), *"/");
                                        //Vertex id
                                        if(!sl1.at(0).empty()){
                                            //std::cout <<"SL11! vid " << sl1.at(0) << "\n"; 
                                            pt1 = std::stoi( sl1.at(0) );                                           
                                        }
                                        //UV coord
                                        if(!sl1.at(1).empty()){
                                            //std::cout <<"SL12! uv " << sl1.at(1) << "\n";                                            
                                        }
                                        //Vertex Normal
                                        if(!sl1.at(2).empty()){
                                            //std::cout <<"SL13! vn " << sl1.at(2) << "\n";
                                            vn1 = std::stoi( sl1.at(2) );  
                                        }                                        

                                    }else{
                                        pt1 = std::stoi( tokenized.at(a));
                                        if (pofst>0){ pt1 = pt1+pofst;};                                            
                                    }
                                }

                                //------ 
                                if(fidx==1)
                                {
                                    //deal with "/" delineated files                                        
                                    if ( tokenized.at(a).find("/") != std::string::npos )
                                    {
                                        std::vector<std::string>  sl2 = tokenizer(tokenized.at(a), *"/");
                                        //we need to know how many slashes .. ugh 
                                        if(!sl2.at(0).empty()){
                                            //std::cout <<"SL20! vid " << sl2.at(0) << "\n";
                                            pt2 = std::stoi( sl2.at(0) ); 
                                        }
                                        if(!sl2.at(1).empty()){
                                            //std::cout <<"SL21! uv " << sl2.at(1) << "\n"; 
                                        }
                                        if(!sl2.at(2).empty()){
                                            //std::cout <<"SL22! vn " << sl2.at(2) << "\n"; 
                                            vn2 = std::stoi( sl2.at(2) );                                              
                                        }  
                                    }else{                                         
                                        pt2 = std::stoi( tokenized.at(a));
                                        if (pofst>0){ pt2 = pt2+pofst;};   
                                    }                                             
                                }  
                                 
                                //------ 
                                if(fidx==2)
                                {
                                    //deal with "/" delineated files                                        
                                    if ( tokenized.at(a).find("/") != std::string::npos )
                                    {
                                        std::vector<std::string>  sl3 = tokenizer(tokenized.at(a), *"/");
                                        //we need to know how many slashes .. ugh 
                                        if(!sl3.at(0).empty()){
                                            //std::cout <<"SL31! vid " << sl3.at(0) << "\n"; 
                                            pt3 = std::stoi( sl3.at(0) );                                           
                                        }
                                        if(!sl3.at(1).empty()){
                                            //std::cout <<"SL32! uv " << sl3.at(1) << "\n";                                            
                                        }
                                        if(!sl3.at(2).empty()){
                                            //std::cout <<"SL33! vn " << sl3.at(2) << "\n";  
                                            vn3 = std::stoi( sl3.at(2) );                                                                                       
                                        }   
                                    }
                                    else{                                        
                                        pt3 = std::stoi( tokenized.at(a));
                                        if (pofst>0){ pt3 = pt3+pofst;};                        
                                    }
                                }   

                                //4th vertex is for 4 sided polys 
                                if(fidx==3){
                                    //deal with "/" delineated files                                        
                                    if ( tokenized.at(a).find("/") != std::string::npos )
                                    {
                                        std::vector<std::string>  sl4 = tokenizer(tokenized.at(a), *"/");
                                        //we need to know how many slashes .. ugh 
                                        if(!sl4.at(0).empty()){
                                            //std::cout <<"SL41! vid " << sl4.at(0) << "\n"; 
                                            pt4 = std::stoi( sl4.at(0) );                                           
                                        }
                                        if(!sl4.at(1).empty()){
                                            //std::cout <<"SL42! uv " << sl4.at(1) << "\n";                                            
                                        }
                                        if(!sl4.at(2).empty()){
                                            //std::cout <<"SL43! vn " << sl4.at(2) << "\n";  
                                            vn4 = std::stoi( sl4.at(2) );                                                                                       
                                        }    
                                    }else{                                        
                                        pt4 = std::stoi( tokenized.at(a));
                                        if (pofst>0){ pt4 = pt4+pofst;};                                               
                                    }
                                }  
                             
                            fidx++; 
                            }//space delineated line 
 

                        }//walk the line tokenized by spaces 

                        ///////////////////////////////////////////////////
                        ///////////////////////////////////////////////////
 
                        //if only one face index - its a point  
                        if (fidx==1)
                        {
                            // points[num_lines].push_back(pt1);
 

                        }//end line loader

                        //-------                  
                        //if two face indices - its a line  
                        if (fidx==2)
                        {
                            lines[num_lines].push_back(pt1);
                            lines[num_lines].push_back(pt2);                          
                            num_lines++;                    

                            //Vector3 points[MAX_NUM_VERTICES];           // vertices of model    
                            //std::vector<int> lines [MAX_NUM_FACES];     // 2 sided faces 

                        }//end line loader

                        //if 3 indices, its a triangle    
                        //-------
                        if (fidx==3)
                        {

                            tris[num_tris].push_back(pt1);
                            tris[num_tris].push_back(pt2);
                            tris[num_tris].push_back(pt3);
                        
                            //insert vertex normals if any
                            //there will be the same num as triangles
                            vnids[num_tris].push_back(vn1);
                            vnids[num_tris].push_back(vn2);
                            vnids[num_tris].push_back(vn3);

                            num_tris++;

                        }//end triangle loader

                        //if 4 indices, its a quad (4 sided polygon - duh) 
                        //------- 
                        if (fidx==4)
                        {
                            std::cout << " pt1 is " << pt1 << "\n";
                            std::cout << " pt2 is " << pt2 << "\n";
                            std::cout << " pt3 is " << pt3 << "\n";
                            std::cout << " pt4 is " << pt4 << "\n";
                            //DEBUG THIS IS BLOWING UP     
                            // quads[num_quads][0] = pt1;
                            // quads[num_quads][1] = pt2;                          
                            // quads[num_quads][2] = pt3;
                            // quads[num_quads][3] = pt4;
                            //num_quads++;
                        }//end quad loader 
                        
                       //---------------//
                       /*
                       //N number of points loader - faces and line geom 
                       if (fidx>4)
                       {
                            if(poly_is_linetype)
                            {
                                std::cout << "line type!!\n";
                            }else{
                                std::cout << "face type!!\n";
                            } 

                            //if we made it here, 
                            //we know what type of geom we are looking for 
                            //but we dont know how many indices, lets walk it again to find out 

                            //DEBUG starting at 2 is a bad hack it skips the "l" and the " " (space)
                            for (int a=3;a<tokenized.size();a++)
                            {   
                                //debug add better error checking 
                                //if( tokenized.at(a)[0] ) {}

                                ptn  = std::stoi( tokenized.at(a) );
                                ptn1 = std::stoi( tokenized.at(a-1) );

                                lines[num_lines].push_back(ptn);
                                lines[num_lines].push_back(ptn1);
                                num_lines++;

                            }
                        }//end N sides loader 
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
    //DEBUG INFO - PHASE THIS OUT 
    
    std::cout << "vertices loaded   "<< num_pts    << "\n";
    std::cout << "uvs loaded        "<< num_uvs    << "\n"; 
    std::cout << "vnormals loaded   "<< num_vnrmls << "\n"; 
    std::cout << "fnormals loaded   "<< num_fnrmls << "\n"; 
    std::cout << "lines loaded      "<< num_lines  << "\n";
    std::cout << "triangles loaded  "<< num_tris   << "\n";
    std::cout << "quads loaded      "<< num_quads  << "\n";  
     

}

/**********************************************/
//save geometry in memory to an OBJ file on disk 

void obj_model::save( char *filepath)
{
    std::ofstream myfile;
    myfile.open (filepath);

    myfile << "#Exported with Octant \n";
    myfile << "#number of verticies "    << num_pts  <<"\n";
    myfile << "#number of triangles     "<< num_tris <<"\n";
    myfile <<"\n";
    
    for (int xx=0;xx<num_pts;xx++)
    {
        // std::cout << vtxrgb[xx] << "\n";


        if (vtxrgb[xx][0]) 
        {
            myfile << "v " << points[xx][0] <<" "<< points[xx][1] <<" "<< points[xx][2] 
                   << " "  << vtxrgb[xx][0] <<" "<< vtxrgb[xx][1] <<" "<< vtxrgb[xx][2] 
                           <<"\n";
        }else{
            myfile << "v " << points[xx][0] <<" "<< points[xx][1] <<" "<< points[xx][2] <<"\n";
        }
    }

    myfile <<"\n";
    
    if (num_tris==0){
        std::cout << " warning - no faces to export ";
    }

    // std::cout << " triangle count is  " << num_tris << endl;
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
    if(num_lines>0)
    {
        for (uint xx=0; xx<num_lines; xx++)
        {
            myfile << "f ";
            for (uint ff=0; ff < lines[xx].size();ff++)
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
    if(num_tris>0)
    {
        for (uint xx=0; xx<num_tris; xx++)
        {
            myfile << "f ";
            for (uint ff=0; ff < tris[xx].size();ff++)
            {
                myfile << tris[xx][ff] << " "; 
            }
            myfile << "\n";
        }
    }

    //----------------------
    // export array of quads
    if(num_quads>0)
    {
        for (uint xx=0; xx<num_quads; xx++)
        {
            myfile << "f ";
            for (uint ff=0; ff < quads[xx].size();ff++)
            {
                myfile << quads[xx][ff] << " "; 
            }
            myfile << "\n";
        }
    }

    //----------------------

    myfile.close();
    std::cout << std::endl << "obj file " << filepath << " exported." << std::endl;

}




/**********************************************/
/*
   load a 4X4 matrix from disk 
   used as a projection matrix for render geometry 
*/
void obj_model::load_m44(char* filename)
{
    
    std::ifstream fin;
    fin.open(filename); // open a file
    if (!fin.good()){ 
        std::cout << "matrix file \""<< filename <<"\" appears to be missing or broken.\n";
        exit (1); // exit if file not found
    }

    int line_ct = 0; //idx of line of text we are parsing

    while (!fin.eof())
    {
        char buf[MAX_CHARS_PER_LINE];
        fin.getline(buf, MAX_CHARS_PER_LINE);

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


////

/*
//sample 3d object - may not be the "proper" way to do it 
void model::prim_cube(float scale){

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

    ///    
    faces[0] = {1,2,3,4};
    faces[1] = {1,2,6,5};
    faces[2] = {2,3,7,6};
    faces[3] = {3,7,8,4};
    faces[4] = {4,8,5,1};
    faces[5] = {5,6,7,8};

    quad_count = 6;

    model::vertex_count = 8;
}

////
//sample 3d object - may not be the "proper" way to do it
 void model::prim_circle(int divs, float scale)
 {

    float a = 0;
    int vcnt = 0;
    int step = 360/divs;
    
    std::cout << "step is " << step << endl ;

    fac_tmp.clear();

    for (a=0;a<360;a=a+step)
    {
        //std::cout << "a is " << a << endl ;
        
        //x axis 
        //obj_pts[vcnt].set(0,  sin(deg_to_rad(a))*scale, cos(deg_to_rad(a))*scale );

        //y axis 
        //obj_pts[vcnt].set( sin(deg_to_rad(a))*scale, 0, cos(deg_to_rad(a))*scale ); 

        //z axis 
        obj_pts[vcnt].set( sin(deg_to_rad(a))*scale, cos(deg_to_rad(a))*scale, 0 ); 
        vcnt++;
    } 

    // std::cout << faceindices[0] << faceindices[5];

    int i = 0; 
 
    for (int i=0; i<vcnt; i++) 
    {
        //fac_tmp.clear();
        fac_tmp.push_back(i+1);  
    }


    faces[0] = fac_tmp;

    model::vertex_count = vcnt;
 }     

////
 //sample 3d object - may not be the "proper" way to do it
 void model::prim_square(float scale)
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


////
 //sample 3d object - may not be the "proper" way to do it
 void model::prim_tri(float scale)
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
    //add_tri(1,2,3);
    //add_tri(2,1,3);

 }    



///
 //sample 3d object - may not be the "proper" way to do it
 void model::prim_line(float scale)
 {
    // vertices - (3d vectors)
    obj_pts[0].set(-scale, 0,  -scale);
    obj_pts[1].set( scale, 0,  scale);

    faces[0] = {1,2}; //notice this is only a 2 point poly (line)

    line_count = 1;

    model::vertex_count =4;
 }     



*/

