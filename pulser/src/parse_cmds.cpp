/*****************************************************************************/
/* 
    parse_cmds.cpp
    
    A fun little command line tool inside an OpenGL window. 




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

#include <sstream>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <unistd.h>  



#include "parse_cmds.h"
#include "gl_setup.h"

#include "cnc_plot.h"

#include "octant.h"


//idx of text that invisible "cursor" sits on
int cursor = 0;

//position of extruder/quill/etc

extern Vector3 qpos;

extern cnc_plot* pt_motionplot;

extern cncglobals cg;

extern obj_model* pt_model_buffer;



void stop_machine(void)
{
    pt_motionplot->stop;
    //std::cout << "ESTOP ACTIVATED.\n";
    //mtime.reset_sim();

}


/***************************************/
/***************************************/

std::string a1,a2,a3,a4,a5,a6,a7;
float v11,v12,v13,v21,v22,v23 = 0;

int last_cmd = 0;


void parse_cmd_text(std::string *buffer)
{

    // Vector of string to save tokens
    vector <std::string> tokens;
    vector <std::string> subtokens;


    //--------------
    std::stringstream check1(*buffer);
    std::string intermediate;
    while(getline(check1, intermediate, ' '))
    {
        tokens.push_back(intermediate);
    }

    for(unsigned int i = 0; i < tokens.size(); i++)
    {
        if(i==0){ a1 = tokens[i]; }
        if(i==1){ a2 = tokens[i]; }
        if(i==2){ a3 = tokens[i]; }
        if(i==3){ a4 = tokens[i]; }
        if(i==4){ a5 = tokens[i]; }
        if(i==5){ a6 = tokens[i]; }    
        if(i==6){ a7 = tokens[i]; }         
    }

    //toggle grid
    if (a1=="help")
    {   
        std::string helpstr = "no one will save you.\n";
        buffer = &helpstr;
        std::cout << "no one will save you.\n"; 
    }    

    //--------------
    //toggle grid
    if (a1=="stop")
    {
        stop_machine(); 
    }

    //peek at internals 
    if (a1=="showcfg")
    { 
        cg.show_params();
 
    }

    //peek at internals 
    if (a1=="show")
    { 
        pt_model_buffer->show();
        pt_model_buffer->show_geom();
    }

    //--------------
    //toggle grid
    if (a1=="tog")
    {
        key_cb(103); 
    }

    //toggle gnomon at origin
    if (a1=="togg")
    {
        key_cb(71); 
    }

    //--------------
    //relative transform (from current xyz)
    if (a1=="clear" || a1=="clr")
    {
        //return head to origin 
        qpos.x = 0;qpos.y = 0;qpos.z = 0; 
        clear_scn_geom();
    }


    //--------------
    /*
    //reload cfg file 
    if (a1=="load")
    {
        //DEBUG
        std::cout << "load NOT WORKY YET\n";
    }
    */
    
    //--------------
    //absolute transform (in space)
    if (a1=="at")
    {
        v11 = std::stof(a2);
        v12 = std::stof(a3);
        v13 = std::stof(a4);

        qpos.x = v11;
        qpos.y = v12;
        qpos.z = v13;   

        Vector3 rgb = Vector3(0,1.0,1.0);
        add_vecrgb_scndrw(&qpos, &rgb);
    }

    //--------------
    //relative transform 2D (from current xyz)
    if (a1=="rt2")
    {
        Vector3 rgb = Vector3(0,1.0,1.0);
       
        //learning about try -> catch.  
        try {
                v11 = std::stof(a2);
                v12 = std::stof(a3);

                //sample the qpos 
                //Vector3 old = qpos;
                //old = qpos.operator-(old);

                //I AM SHIFTING THE AXIS TO Z UP HERE
                //set new qpos
                qpos.x = v11;
                qpos.y = 0;  
                qpos.z = v12;

            //store the difference between them
            add_vecrgb_scndrw(&qpos, &rgb);

        } catch (const std::invalid_argument& e) {  
            //std::cerr << "Error: " << e.what() << std::endl; // Handling the error
        } catch (...) { // Catch-all for any other unexpected exceptions
            std::cerr << "RT command bad input " << std::endl;
        }   


        

    }

    //--------------
    //relative transform (from current xyz)
    if (a1=="rt")
    {
        Vector3 rgb = Vector3(0,1.0,1.0);
       
        v11 = std::stof(a2);
        v12 = std::stof(a3);
        v13 = std::stof(a4);
        
        //set new qpos
        qpos.x = v11;
        qpos.y = v12;
        qpos.z = v13;  


        //sample the qpos 
        //Vector3 old = Vector3(qpos.x, qpos.y,qpos.z);
        
        //Vector3 newvec = qpos.operator-(old);
        //store the difference between them
        //add_vecrgb_scndrw(&newvec, &rgb);
        
        add_vecrgb_scndrw(&qpos, &rgb);

    }

    //--------------
    //display modes
    if (a1=="dm")
    {
        if(a2=="persp")   {key_cb(49);} 
        if(a2=="oside")   {key_cb(50);} 
        if(a2=="otop")    {key_cb(64);} 
        if(a2=="ofront")  {key_cb(51);} 
        
        if(a2=="pts")     {key_cb(36);} 
        if(a2=="wire")    {key_cb(52);} 
        if(a2=="solid")   {key_cb(53);}                 

    }

    //--------------
    if (a1=="run"||a1=="start")
    {

        //std::cout << "ESTOP DISABLED.\n";
    }

    //--------------
    /*
    //parse the second token (you cant use spaces - duh)
    std::stringstream check2(second);
    std::string intermediate2;    
    while(getline(check2, intermediate2, '_'))
    {
        subtokens.push_back(intermediate2);
    }
    // Printing the token vector
    for(int i = 0; i < subtokens.size(); i++)
    {
        std::cout << subtokens[i] << "\n";
    }
    */
    //--------
    
    //std::cout << "first:" << a1 << " second:" << a2 << '\n';

}



/***************************************/
/*
    single key press callback freom OpenGL. 

    It has a poinmter to the screen text buffer and   
    can manipulate it before sending it back.

    The "enter" acctivates the more in depth parser on the text buffer. 
    It calls parse_cmd_text() which further tokenizes and runs more advanced commands. 

*/



void parse_cmds(std::string *buffer, unsigned char *pt_key )
{
    int i = static_cast<int>(*pt_key);
    
    //need a way to capture stuck keys - DEBUG - stew on it for a while
    //if(i==last_cmd)
    //{
    //    std::cout << "stop holding down the key, moron!! "<< "\n";
    //}

    last_cmd=i;

    //debug info 
    //std::cout << "cursor "<< cursor << " key val " << i  << "buf size: " << buffer->size() << std::endl;
    //std::cout << " key val " << i  << std::endl;

    //emergency soft stop is ` key 
    if(i==96)
    {
        stop_machine();
    }

    //-----
    //backspace key
    if(i==8)
    {
        if (buffer->size()>0 && cursor>0)
        {
            cursor--;
            buffer->erase(cursor, buffer->size());
        }
    }

    //-----
    //enter key
    if(i==13)
    {
        //todo - keep a log of commands as txt file ??
        
        //std::cout << "command to parse \n" << *buffer << "\n";

        parse_cmd_text(buffer);
        buffer->clear();
        cursor=0;
        //std::cout << "ENTER PRESSED\n";        
    }    

    //-----
    //esc, +, -  
    //this is buggy - zooms while typing
    if(i==27 || i==45 || i==61)
    {
        key_cb(i); 
    }  

    //-----
    //ignore backspace, enter, ` ,
    if(i!=8 && i!=13 && i!=96)
    {   
        cursor = buffer->size(); 
        buffer->push_back(*pt_key);//DEBUG - why did I do this?
    }

};




