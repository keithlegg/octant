/*************************************************************/
/*
    plot_fio.cpp 
    
    load and save motion caches to disk from the motion plot object 

    defined in a single header but split among 2 cpp files
    
    the two associated cpp files:  
        plot_fio.cpp (this file)
        plot.cpp



    //----------------------------------------//

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
/*************************************************************/

#include <iostream>
#include <cctype>
#include <sstream>
#include <fstream>
#include <vector>
using std::vector;



#include "obj_model.h" //tokenizer 
#include "Vectors.h"
#include "plot.h"



/******************************************/

//DEBUG not done 
void cnc_plot::save_motionfile(void)
{
    std::cout << "SAVE MOTION !\n";
};



/******************************************/

//DEBUG not done 
void cnc_plot::load_motionfile(void)
{
    std::cout << "LOAD MOTION !\n";
};



/******************************************/

//copied from obj model loader 

void cnc_plot::load_lines(char *filepath)
{
    //display stats about what we loaded when done 
    //bool showinfo  = true;
    
    //debugging tools 
    bool debug     = true;
    bool debugfile = false;

    uint pt1, pt2, pt3, pt4, ptn, ptn1 = 0;
    int vn1, vn2, vn3, vn4 = 0;

    std::cout << "## cnc_plot::load_lines - in file "<< filepath << "\n";
    
    int pofst   = 0; //DEBUG point offset indices to points if geom exists already 
    int line_ct = 0; //idx of text line we are parsing 
    
    std::ifstream obj_filein(filepath, std::ifstream::in);

    if (!obj_filein.good()){ 
        std::cout << ".obj file \""<< filepath <<"\" appears to be missing." << std::endl;
        // more polite than exiting the program
        return void();
    }

    /*
    //check to see if an object was already loaded - offset the point indeces if so 
    //point offset indices to points - if geom exists already 
    if (fio_npts>0)
    {
        pofst = fio_npts;
    }*/

    //begin the object load - walk the file parsing it 
    while (!obj_filein.eof() && !obj_filein.fail() && !obj_filein.bad())
    {  

        std::string line;
        while (std::getline(obj_filein, line)) 
        {  
            if(debugfile){ std::cout << "FULL LINE " << line << std::endl; }

            std::vector<std::string>  tokenized = tokenizer(line, *" ");

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
                            if(debug)
                            {
                                //std::cout << "vtx 3d "<< vpt.x << " "<< vpt.y <<" "<<vpt.z<<"\n";
                                std::cout << "numpts:" << fio_npts <<" "<< vpt.x << " "<< vpt.y << " " << vpt.z <<"\n";

                            }

                            fio_pts[fio_npts] = vpt;
                            fio_npts++;
                        
                                             
                        } 

                    }//end vertex loader 
                    //std::cout << "NUM PTS LOADED "<< fio_npts << "\n";

                    //-----------------------------//
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
 

                         
                        //N number of points loader - faces and line geom 
                        if (fidx>4)
                        {
                            //std::cout << " PARSING NEW LINE "<<fidx<<"\n";
                            
                            if(debug)
                            {
                                if(poly_is_linetype)
                                {
                                    std::cout << "obj_model.load line type!!\n";
                                }else{
                                    std::cout << "obj_model.load face type!!\n";
                                }
                            } 

                            //if we made it here, 
                            //we know what type of geom we are looking for 
                            //but we dont know how many indices, lets walk it again to find out 
                            
                            std::vector<uint> tmp_line_id;


                            //DEBUG starting at 2 is a bad hack it skips the "l" and the " " (space)
                            for (int a=2;a<tokenized.size();a++)
                            {   
                                ptn  = std::stoi( tokenized.at(a) );
                                tmp_line_id.push_back(ptn);
                                //std::vector<uint>* lines   = new std::vector<uint>[MAX_NUM_FACES];     // 2 sided faces 
                            }
                            
                            // std::cout << " new line idx:"<< numlines << " size:" << tmp_line_id.size() << "\n";
                            // lines[numlines] = tmp_line_id;
                            // numlines++;

                        }//end N sides loader 
                          
                    }//end face loader


                }//line is not commented out
                line_ct++;

            }//line not blank
        }//line by line
    }//obj exists   
}


/******************************************/


/******************************************/


