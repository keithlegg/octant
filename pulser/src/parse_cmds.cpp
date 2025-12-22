/*****************************************************************************/
/* 
    parse_cmds.cpp
    
    A fun little command line tool inside an OpenGL window. 
    This stores the cursor state, parses both single key and text input
    and redirectes program execution to the right callbacks. 

    Some of the older, single key callbacks are called with the legacy function  
      --> key_cb()


    DEBUG - we need a way to call commands from the headless, CLI only. 
    Need to rethink and rebuild the whole command structure. 


       
    ------------------------------------------- 

    // path of text processing 
    start_gui-> render_loop-> (keypress)parser_cb-> parse_cmds-> parse_cmd_text
                                                    |---------> key_cb 
 
   
    ------------------------------------------- 

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


#include "cnc_parport.h"

#include "parse_cmds.h"
#include "gl_setup.h"

#include "cnc_plot.h"
#include "pylink.h"

#include "octant.h"


//idx of text that invisible "cursor" sits on
int cursor = 0;

extern bool tog_testport; 

//position of extruder/quill/etc
extern cncglobals cg;

extern cnc_plot* pt_motionplot;
extern obj_model* pt_model_buffer;


void run_machine(void)
{
    pt_motionplot->run();
}



void stop_machine(void)
{
    pt_motionplot->pause();
}


/***************************************/
/***************************************/

std::string a1,a2,a3,a4,a5,a6,a7;
float v11,v12,v13,v21,v22,v23 = 0;

int last_cmd = 0;


void parse_cmd_text(std::string *buffer)
{

    // Vector of string to save tokens
    std::vector <std::string> tokens;
    std::vector <std::string> subtokens;


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
        std::cout << "------------------------------------------        \n";
        std::cout << "dm : display mode                                 \n";
        std::cout << "  wire, persp, otop, oside, etc                   \n";
        std::cout << "                                                  \n";
        std::cout << "show                                              \n";
        std::cout << "  obj      - stats about loaded 3d object         \n";
        std::cout << "  path     - stats about toolpath                 \n";
        std::cout << "  pathids  -                                      \n";
        std::cout << "  pathgeom - show coordinates for toolpaths       \n";                
        std::cout << "  objgeom  - show coords for polygonsm etc        \n"; 
        std::cout << "                                                  \n"; 
        std::cout << "lup (look up by ID)                               \n";
        std::cout << "   pathid (int),                                  \n";
        std::cout << "------------------------------------------        \n";

    }    
    
    /**********************************************************************/
  
    /*
    //export internal vectors to obj file 
    if (a1=="walk")
    {
        if(a2=="extents")
        {
        } 
    }
    //export internal vectors to obj file 
    if (a1=="export")
    {
        if(a2=="rapid")
        {
        }   
      
        if(a2=="paths")
        {
        }             
    } 
    */
    //run external tools 
    if (a1=="testport")
    {
        if(tog_testport){
            std::cout << "PARPORT TESTER\n";
            tog_testport = false;
        }else{
            tog_testport = true;
        }
    }


    //run external tools 
    if (a1=="python")
    {
        exe_python();    
    }

    /*
    if (a1=="readbyte")
    { 
    }
 
    if (a1=="sendbyte")
    { 
    }
    */

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
    if (a1=="lup")
    { 
        if(a2=="pathid")
        {
            if(a3!="")
            {
                std::cout << "WE HAVE IDS :"<<a3<<":\n";
            }
        }
    }

    //peek at internals 
    if (a1=="show")
    { 
        if(a2=="path")
        { 
            pt_motionplot->show();             
        }
        
        if(a2=="obj")
        { 
            pt_model_buffer->show();
        }

        //-------------------------
        //these are probably only needed for debugging 
        
        if(a2=="objgeom")
        { 
            pt_model_buffer->show_geom();
        }

        if(a2=="pathgeom")
        { 
            pt_motionplot->showgeom();
        }

        if(a2=="pathids")
        { 
            pt_motionplot->showpthids();
        }

        if(a2=="ply")
        { 
            if(a3!="")
            {
                unsigned int pidx = std::stoi(a3);
                pt_motionplot->showply(pidx); 
            }
        }        

    }

    //--------------
    // gle grid
    if (a1=="tog")
    {
        if(a2=="tris"){key_cb(100);};
        if(a2=="grid"){key_cb(103);};
        if(a2=="gnomon"){key_cb(71);};
        if(a2=="normals"){key_cb(110);};
        if(a2=="vtxrgb"){key_cb(56);};

    }



    //--------------
    //relative transform (from current xyz)
    if (a1=="clear" || a1=="clr")
    {
        //return head to origin 
        pt_motionplot->quill_pos.x = 0;
        pt_motionplot->quill_pos.y = 0;
        pt_motionplot->quill_pos.z = 0; 
        clear_scn_geom();
    }

    //--------------
    //absolute transform (in space)
    if (a1=="at")
    {
        v11 = std::stof(a2);
        v12 = std::stof(a3);
        v13 = std::stof(a4);

        pt_motionplot->quill_pos.x = v11;
        pt_motionplot->quill_pos.y = v12;
        pt_motionplot->quill_pos.z = v13;   

        Vector3 rgb = Vector3(0,1.0,1.0);
        add_vecrgb_scndrw(&pt_motionplot->quill_pos, &rgb);
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
                pt_motionplot->quill_pos.x = v11;
                pt_motionplot->quill_pos.y = 0;  
                pt_motionplot->quill_pos.z = v12;

            //store the difference between them
            add_vecrgb_scndrw(&pt_motionplot->quill_pos, &rgb);

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
        //Vector3 rgb = Vector3(0,1.0,1.0);
       
        v11 = std::stof(a2);
        v12 = std::stof(a3);
        v13 = std::stof(a4);
        
        //set new qpos
        pt_motionplot->quill_pos.x = v11;
        pt_motionplot->quill_pos.y = v12;
        pt_motionplot->quill_pos.z = v13;  


        //sample the qpos 
        //Vector3 old = Vector3(qpos.x, qpos.y,qpos.z);
        
        //Vector3 newvec = qpos.operator-(old);
        //store the difference between them
        //add_vecrgb_scndrw(&newvec, &rgb);
        
        //add_vecrgb_scndrw(&pt_motionplot->quill_pos, &rgb);

    }

    //--------------
    //display modes
    if (a1=="dm")
    {
        if(a2=="persp")  {key_cb(49);} 
        if(a2=="side")   {key_cb(50);} 
        if(a2=="top")    {key_cb(64);} 
        if(a2=="front")  {key_cb(51);} 
        
        if(a2=="pts")     {key_cb(36);} 
        if(a2=="wire")    {key_cb(52);} 
        if(a2=="s1")      {key_cb(53);}  
        if(a2=="s2")      {key_cb(5);}
        if(a2=="s3")      {key_cb(6);}
        if(a2=="s4")      {key_cb(7);}          

    }

    //--------------
    if (a1=="run"||a1=="start")
    {
        run_machine();
    }



    //--------------
    //clear for next transaction
    //a1,a2,a3,a4,a5,a6,a7 = "";
    //--------------

}//end 




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




