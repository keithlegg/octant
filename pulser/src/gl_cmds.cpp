/*****************************************************************************/
/* 
    parse_cmds.cpp
    
    A fun little command line tool inside an OpenGL window. 
    This stores the cursor state, parses both single key and text input
    and redirectes program execution to the right callbacks. 

    Some of the older, single key callbacks are called with the legacy function  
      --> key_cb()

       
    ------------------------------------------- 

    // path of text processing 
    start_gui-> render_loop-> (keypress)parser_cb-> parse_cmds-> parse_cmd_text
                                                    |---------> key_cb 
 
   
    ------------------------------------------- 

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

#include <sstream>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <unistd.h>  

#include "parport.h"
#include "plot.h"
#include "pylink.h"
#include "octant.h"

//GUI LAYER 
#if DO_BUILD_GUI == true
    #include "gl_cmds.h"
    #include "gl_setup.h"
    #include "gl_render.h"
#endif




//idx of text that invisible "cursor" sits on
int cursor = 0;

extern cnc_parport parport;

extern bool tog_testport; 
 
extern bool disp_ply_solo;
extern uint disp_ply_solo_id;


//position of extruder/quill/etc
extern cncglobals cg;

extern cnc_plot* pt_motionplot;
extern obj_model* pt_model_buffer;

/***************************************/

void load_2d_obj(void)
{
  
    obj_model* pt_obj2d_loader  = new obj_model;

    //pt_model_buffer->reset();

    //pt_model_buffer->load(char_array);
    //pt_model_buffer->calc_normals();
}


/***************************************/
void run_machine(void)
{
    pt_motionplot->run_sim();
}


void pause_machine(void)
{
    pt_motionplot->pause();
}



//void reset_machine(void)
void stop_machine(void)
{
    pt_motionplot->stop();
}


/***************************************/
/***************************************/

std::string a1,a2,a3,a4,a5,a6,a7,a8,a9,a10;
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

    for(uint i = 0; i < tokens.size(); i++)
    {
        if(i==0){ a1  = tokens[i]; }
        if(i==1){ a2  = tokens[i]; }
        if(i==2){ a3  = tokens[i]; }
        if(i==3){ a4  = tokens[i]; }
        if(i==4){ a5  = tokens[i]; }
        if(i==5){ a6  = tokens[i]; }    
        if(i==6){ a7  = tokens[i]; }         
        if(i==7){ a8  = tokens[i]; }   
        if(i==8){ a9  = tokens[i]; }   
        if(i==9){ a10 = tokens[i]; } 

    }

    //toggle grid
    if (a1=="help")
    {   
        std::cout << "------------------------------------------        \n";
        std::cout << "dm : display mode                                 \n";
        std::cout << "  wire, persp, otop, oside, etc                   \n";
        std::cout << "                                                  \n";
        
        std::cout << "------                                            \n";        
        //std::cout << "precache     -debug   build a pulsetrain cache    \n";
        //std::cout << "tp_out (INT) -debug   test parport output         \n";
        //std::cout << "tp_in        -debug   build a pulsetrain cache    \n";        

        std::cout << "                                                  \n";
        std::cout << "show                                              \n";
        std::cout << "  pt       -pulsetrain info                       \n";
        std::cout << "  cfg      - view important globals               \n";
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
    
    
    //experimental pre cache  
    if (a1=="precache")
    {
        pt_motionplot->precache_sim();  
    }
    */

    //------------------

    //test port out (how fast can we blink?)
    if (a1=="tp_out")
    {
        int num = std::stoi(a2);
        //test_port_output
        parport.test_port_output(&cg, num );
    }

    //test port in (quadrature decode) 
    if (a1=="tp_in")
    {
        if(tog_testport){
            std::cout << "PARPORT TESTER\n";
            tog_testport = false;
        }else{
            tog_testport = true;
        }
    }

    //------------------
    //run external tools 
    if (a1=="python")
    {
        exe_python(a2);    
        load_2d_obj();
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
        std::cout << "------------------------------------------        \n";

        //view globals   
        if(a2=="cfg")
        { 
            cg.show_params();         
        }
        
        //stats about plotter 
        if(a2=="path")
        { 
            pt_motionplot->show();             
        }
        
        //stats about 3D object  (not path polygons)
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
        
        //show ID and size of each paths polygon  
        if(a2=="pathids")
        { 
            pt_motionplot->showpthids();
        }
        
        //path polygon "SOLO" view and dump verts in console 
        if(a2=="ply")
        { 
            if(a3=="all")
            {
                std::cout << "display single off\n";
                disp_ply_solo = false;
            }
            else if(a3!="")
            {
                uint pidx = std::stoi(a3);
                
                disp_ply_solo = true;
                disp_ply_solo_id = pidx;

                pt_motionplot->showply(pidx); 
            }

        }        

    }

    //--------------
    // gle grid
    if (a1=="tog")
    {
        if(a2=="tris")   {key_cb(100);};
        if(a2=="grid")   {key_cb(103);};
        if(a2=="gnomon") {key_cb(71);};
        if(a2=="normals"){key_cb(110);};
        if(a2=="vtxrgb") {key_cb(56);};

    }

    //--------------
    //freerun  
    if (a1=="fr")
    {
        v11 = std::stof(a2);
        v12 = std::stof(a3);
        v13 = std::stof(a4);
        
        v21 = std::stof(a5);
        v22 = std::stof(a6);
        v23 = std::stof(a7);

        //this is not right, but its something  
        pt_motionplot->quill_pos.x = v21;
        pt_motionplot->quill_pos.y = v22;
        pt_motionplot->quill_pos.z = v23;   

        std::cout << " freerun " << a2 <<" " << a3<<" " << a4<<" " << a5<<" " << a6<<" " << a7<<" " << "\n";  

        Vector3 s_p = Vector3(v11,v12,v13);
        Vector3 e_p = Vector3(v21,v22,v23); 

        Vector3 offset = e_p - s_p;

        uint numx = 0;
        uint numy = 0;
        uint numz = 0;

        numx = offset.length()/cg.pp1u_x;
        numy = offset.length()/cg.pp1u_y;
        numz = offset.length()/cg.pp1u_z;

        //non threaded - its all broke-y
        //run_cncplot( v11, v12, v13, v21, v22, v23, std::stoi(a8));

        //threaded - it workey great
        pulse_thread( v11, v12, v13, v21, v22, v23, numx, numy, numz);

        

    }
    //--------------
    /*


    //--------------
    //relative transform (from current xyz)
    if (a1=="clear" || a1=="clr")
    {
        //return head to origin 
        pt_motionplot->quill_pos.x = 0;
        pt_motionplot->quill_pos.y = 0;
        pt_motionplot->quill_pos.z = 0; 
        clear_linebuffers();
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
        add_vec_lbuf1(&pt_motionplot->quill_pos, &rgb);
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
            add_vec_lbuf1(&pt_motionplot->quill_pos, &rgb);

        } catch (const std::invalid_argument& e) {  
            //std::cerr << "Error: " << e.what() << std::endl; // Handling the error
        } catch (...) { // Catch-all for any other unexpected exceptions
            std::cerr << "RT command bad input " << std::endl;
        }   
    }

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
        //add_vec_lbuf1(&newvec, &rgb);
        
        //add_vec_lbuf1(&pt_motionplot->quill_pos, &rgb);

    }*/

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

}//end 




/***************************************/
/*
    single key press callback freom OpenGL. 

    It has a pointer to the screen text buffer and   
    can manipulate it before sending it back.

    The "enter" acctivates the more in depth parser on the text buffer. 
    It calls parse_cmd_text() which further tokenizes and runs more advanced commands. 

*/



void parse_cmds(std::string *buffer, unsigned char *pt_key )
{
    bool show_debug = false;

    int i = static_cast<int>(*pt_key);
    
    //need a way to capture stuck keys - DEBUG - stew on it for a while
    //if(i==last_cmd)
    //{
    //    std::cout << "stop holding down the key, moron!! "<< "\n";
    //}

    last_cmd=i;

    if(show_debug)
    {
        std::cout << "cursor "<< cursor << " key val " << i  << "buf size: " << buffer->size() << std::endl;
        std::cout << " key val " << i  << std::endl;
    }


    //emergency soft stop is ` key 
    if(i==96)
    {
        pause_machine();        
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
        //todo - keep a history log of commands as txt file ??
        parse_cmd_text(buffer);
        buffer->clear();
        cursor=0;
    }    

    //-----
    //esc, +, -  
    //this is buggy - zooms while typing
    if(i==27 || i==45 || i==61)
    {
        key_cb(i); 
    }  

    //-----
    //ignore backspace, enter, `,
    if(i!=8 && i!=13 && i!=96)
    {   
        cursor = buffer->size(); 
        buffer->push_back(*pt_key);//DEBUG - why did I do this?
    }

};




