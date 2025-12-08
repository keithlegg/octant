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

    std::cout << "## load_objfile loading file "<< filepath << "\n";
    
    int pofst = 0; //DEBUG point offset indices to points if geom exists already 
    int line_ct = 0;
    
    std::ifstream obj_filein(filepath, std::ifstream::in);

    if (!obj_filein.good()){ 
        std::cout << ".obj file \""<< filepath <<"\" appears to be missing." << std::endl;
        exit (1); // exit if file not found
    }

    while (!obj_filein.eof() && !obj_filein.fail() && !obj_filein.bad())
    {  

        std::string line;
        while (std::getline(obj_filein, line)) 
        {  
            // std::cout << "FULL LINE " << line << std::endl;
            std::vector<std::string>  tokenized = tokenizer(line, *" ");

            //DEBUG   
            // //point offset indices to points - if geom exists already 
            // if (num_pts>0)
            // {
            //     pofst = num_pts;
            // }

            std::string coords_str; // string that verts get copied to 
            std::string nrmls_str;  // string that verts get copied to 
            std::string fidx_str;   // string that faces get copied to

            //if line is not blank
            if(tokenized.size()>0)
            {
                int commentpos = tokenized.at(0).find('#');
                if ( commentpos != std::string::npos)
                {
                    //confige file - line is commented out  
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
                 
                            //std::cout << " line " << line_ct << " vtxpass " << vidx << " " << tokenized.at(a) <<"\n"; // <- vertex line 
                          
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
                        
                        //std::cout << "vidx is "<< vidx <<"\n"; 

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

                            ////set color to white initially  ?? DEBUG 
                            //vec3 color;
                            //color.x=1.0;   
                            //color.y=1.0;
                            //color.z=1.0; 
                            //loader->vtxrgb[loader->num_vtxrgb] = color;

                        }  
                    }//end vertex loader 
                    //std::cout << "NUM PTS LOADED "<< num_pts << "\n";


                    //  look for normals
                    //-----------------------------//
                    if ( tokenized.at(0).find("vn") != std::string::npos )
                    {
                        int a = 0;
                        int nidx = 0;
            

                        float xc, yc, zc = 0.0;

                        //walk the space delineated tokens per each line
                        for (a=0;a<tokenized.size();a++)
                        {   
                 
                            //std::cout << " line " << line_ct << " nrmlpass " << nidx << " " << tokenized.at(a) <<"\n"; // <- vertex line 
                            
                            if(nidx==0){
                                xc = std::stof(tokenized.at(a));
                            }
                            if(nidx==1){
                                yc = std::stof(tokenized.at(a));                        
                            }  
                            if(nidx==2){
                                zc = std::stof(tokenized.at(a));
                            }                                        
                            
                            nidx++;
                        }

                        if (nidx==3)
                        {
                            Vector3 vn = Vector3( xc, yc, zc  );
                            vnormals[num_vnrmls] = vn;
                            num_vnrmls++;

                        }     
                    
                    }//end vertex normal loader 

                    //-----------------------------//
                     
                    //  look for F / faces
                    if ( tokenized.at(0).find("f") != std::string::npos )
                    {
                        int a = 0;
                        int fidx = 0;
                        int pt1,pt2,pt3,pt4 = 0;

                        //walk the space delineated tokens per each line
                        for (a=1;a<tokenized.size();a++)
                        {   

                            //get type  
                            //std::cout << " FOO  " << typeid(tokenized.at(fidx)).name()  << "\n";

                            if( tokenized.at(a).size())
                            {
                                //std::cout << " pofst " << pofst <<" line " << line_ct << " idx:" << a << " tokenized : " << tokenized.at(a) <<"\n"; // <- vertex line 
                                
                                //only supports 2,3,4 sided polygons  
                                if(fidx==0){
                                    pt1 = std::stoi( tokenized.at(a));
                                    if (pofst>0){ pt1 = pt1+pofst;};
                                    
                                }
                                if(fidx==1){
                                    pt2 = std::stoi( tokenized.at(a));
                                    if (pofst>0){ pt2 = pt2+pofst;};   
                                                                                       
                                }  
                                if(fidx==2){
                                    pt3 = std::stoi( tokenized.at(a));
                                    if (pofst>0){ pt3 = pt3+pofst;};                        
                                  
                                }   
                                if(fidx==3){
                                    pt4 = std::stoi( tokenized.at(a));
                                    if (pofst>0){ pt4 = pt4+pofst;};                                               
                                }  
                            fidx++; 
                            } 
 

                        }

                        //std::cout << " num fids " << fidx << "\n";

                        //-------                  
                        //if two face indices - its a line  
                        if (fidx==2)
                        {
                            //DEBUG THIS IS BLOWING UP                             
                            // lines[num_lines][0] = pt1;
                            // lines[num_lines][1] = pt2;                          
                            num_lines++;                    
                        }//end line loader

                        //-------
                        if (fidx==3)
                        {
                            //DEBUG THIS IS BLOWING UP  
                            //std::cout << " pt1 " << pt1 << " pt2 " << pt2 << " pt3 " << pt3 << "\n";

                            vector<int> newtri;
                            newtri.push_back(pt1);
                            newtri.push_back(pt2);
                            newtri.push_back(pt3);
                            //std::cout << " newtri "<< newtri[0]<<' ' << newtri[1]<< ' '<< newtri[2]<< "\n";
                            //this->insert(newtri);
                        
                            tris[num_tris].push_back(pt1);
                            tris[num_tris].push_back(pt2);
                            tris[num_tris].push_back(pt3);
                                                                                    
                            num_tris++;

                            //add_triangle(pt1,pt2,pt3);
   

                        }//end triangle loader

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
                            num_quads++;


                        }//end quad loader 
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
    /*
    std::cout << "vertices loaded   "<< num_pts    << "\n";
    std::cout << "uvs loaded        "<< num_uvs    << "\n"; 
    std::cout << "lines loaded      "<< num_lines  << "\n";
    std::cout << "triangles loaded  "<< num_tris   << "\n";
    std::cout << "quads loaded      "<< num_quads  << "\n";  
    */ 

}

/**********************************************/
//save geometry in memory to an OBJ file on disk 

void obj_model::save( char *filepath)
{
    std::ofstream myfile;
    myfile.open (filepath);

    myfile << "#Exported with Keith's little graphics tool\n";
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
    if(num_lines>0)
    {
        for (xx=0; xx<num_lines; xx++)
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
    if(num_tris>0)
    {
        for (xx=0; xx<num_tris; xx++)
        {
            myfile << "f ";
            for (ff=0; ff < tris[xx].size();ff++)
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
        for (xx=0; xx<num_quads; xx++)
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


////

/*
//sample 3d object - may not be the "proper" way to do it 
void model::prim_cube(double scale){

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
 void model::prim_circle(int divs, double scale)
 {

    double a = 0;
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
 void model::prim_square(double scale)
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
 void model::prim_tri(double scale)
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
 void model::prim_line(double scale)
 {
    // vertices - (3d vectors)
    obj_pts[0].set(-scale, 0,  -scale);
    obj_pts[1].set( scale, 0,  scale);

    faces[0] = {1,2}; //notice this is only a 2 point poly (line)

    line_count = 1;

    model::vertex_count =4;
 }     



*/

