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
#include <cstring>
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

extern uint path_render_mode;
extern bool debug_onscreen;
extern bool tog_testport; 
extern bool disp_ply_solo;
extern uint disp_ply_solo_id;
 
extern cncglobals cg;
extern cnc_parport parport;
extern cnc_plot* pt_motionplot;
extern obj_model* pt_model_buffer;


std::string a1,a2,a3,a4,a5,a6,a7,a8,a9,a10;
float v11,v12,v13,v21,v22,v23 = 0;

int last_cmd = 0;


/***************************************/

//DEBUG - WIP 
//callback from python link
//load the results back  
void load_py_obj(std::string objfilepath)
{
    std::cout << "importing obj as paths "<< objfilepath << "\n";

    pt_motionplot->import_path_from_obj( objfilepath);
}


/***************************************/

void reload_obj(void)
{
    pt_model_buffer->reset();

    //load CNC cfg (including paths to .obj files) 
    cg.load_cfg_file();
    
    //load the 3d models 
    cg.load_objects();
}

/***************************************/
void reload_paths(void)
{
   std::cout << "# reloading toolpaths \n";     
   
   //DEBUG hmm obj and vecs live in same file - decouple?
   cg.load_cfg_file();

   pt_motionplot->update_toolpaths(); 
}

/***************************************/
void unload_vec(void)
{
    std::cout << "# dumping cached vectors \n";  
    
    //stop the runing simulation
    pt_motionplot->reset();

    //---
    pt_motionplot->clear_toolpaths();
    pt_motionplot->rapidmove_vecs.clear();
    pt_motionplot->program_vecs.clear();
    
    pt_motionplot->num_prg_plys = 0;
    pt_motionplot->num_rpd_plys = 0;

    for(uint x=0;x<MAX_NUM_PLY;x++)
    {
        pt_motionplot->tp_idxs[x].clear();
    }

    clear_linebuffers();
}

/***************************************/
void unload_obj(void)
{
    pt_model_buffer->reset();
}

/***************************************/
void run_machine(void)
{
    pt_motionplot->run_sim();
}

/***************************************/
void pause_machine(void)
{
    pt_motionplot->pause();
    
    //debug - maybe a good idea?
    //cg.ENABLE_MOTOR_DRIVE = 0;

}

/***************************************/
void reset_machine(void)
{
    pt_motionplot->reset();
}

/***************************************/
void stop_machine(void)
{
    pt_motionplot->stop();
    
    //this line can lock the whole thing up - debug 
    cg.ENABLE_MOTOR_DRIVE = 0;

}


void set_motor(uint setval)
{
    if(setval==1){
        cg.ENABLE_MOTOR_DRIVE = 1;
    }
    if(setval==0){
        cg.ENABLE_MOTOR_DRIVE = 0;
    }    
}




/***************************************/
/***************************************/




void parse_cmd_text(std::string *buffer)
{

    // Vector of string to save tokens
    std::vector <std::string> tokens;
    std::vector <std::string> subtokens;

    std::stringstream check1(*buffer);
    std::string intermediate;

    while(getline(check1, intermediate, ' '))
    {
        tokens.push_back(intermediate);
    }

    /**********************************************************************/
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

    /**********************************************************************/
    //toggle grid
    if (a1=="help")
    {   
        std::cout << "------------------------------------------        \n"; 
        std::cout << "         Octant help                              \n";
        std::cout << "------------------------------------------        \n"; 
        std::cout << "                                                  \n";
        std::cout << "-------- SETUP COMMANDS ------------------        \n"; 
        std::cout << "dm : (display operations)                         \n";
        std::cout << "                                                  \n";
        std::cout << "togg, toggle                                      \n";
        std::cout << "                                                  \n";
        
        std::cout << "-------- PROGRAM COMMANDS ----------------        \n"; 
        std::cout << "run, stop, reset                                  \n";
        std::cout << "                                                  \n";   
        std::cout << "unload  (path,obj)                                \n";
        std::cout << "reload  (path,obj)                                \n";
        std::cout << "                                                  \n";

        std::cout << "show (sho)                                        \n";
        std::cout << "mtr : (motor enable)                              \n";

        std::cout << "-------- DEBUGGING COMMANDS --------------        \n";               

        std::cout << "mh - move head(quill)                             \n";          
        std::cout << "am - addmotion                                    \n";        
        std::cout << "                                                  \n";

        std::cout << "lup (lookup by ID)                                \n";
        std::cout << "   pathid (int),                                  \n";
        std::cout << "                                                  \n";    


        std::cout << "------------------------------------------        \n";
        //std::cout << "precache     -debug   build a pulsetrain cache    \n";
        //std::cout << "tp_out (INT) -debug   test parport output         \n";
        //std::cout << "tp_in        -debug   build a pulsetrain cache    \n";  
        //std::cout << "------------------------------------------        \n";  
        std::cout << "                                                  \n";
         
        /*****************/
        if(a2=="dm")
        {
            std::cout << "dm : (display operations)                        \n";
            std::cout << "  wire                                           \n";
            std::cout << "  persp                                          \n";
            std::cout << "  top                                            \n";
            std::cout << "  side                                           \n";                        
            std::cout << "  p0                                             \n";
            std::cout << "  p1                                             \n";            
        }

        /*****************/
        if(a2=="togg")
        {
            std::cout << "togg, toggle                                      \n";
            std::cout << " tris,                                            \n";        
            std::cout << " grid      (alias command - tg)                   \n";  
            std::cout << " gnomon    (alias command - tgg)                  \n";  
            std::cout << " normals                                          \n";                          
            std::cout << " vtxrgb                                           \n";  
        }

        /*****************/
        if(a2=="mtr")
        {
            std::cout << "mtr : (motor enable)                              \n";
            std::cout << "  on,off                                          \n";
            std::cout << "                                                  \n";
        }

        /*****************/
        if(a2=="show")
        {
            std::cout << "show (sho)                                        \n";
            std::cout << "  cfg      - view important globals               \n";
            std::cout << "  motion   - show motion index objs               \n";
            std::cout << "  pt       - pulsetrain info                      \n";
            std::cout << "  obj      - stats about loaded 3d object         \n";
            
            std::cout << "  path     - stats about toolpath                 \n";
            std::cout << "  path     - stats about toolpath                 \n";

            std::cout << "  pathids  -                                      \n";
            std::cout << "  pathgeom - show coordinates for toolpaths       \n";                
            std::cout << "  objgeom  - show coords for polygonsm etc        \n"; 
            std::cout << "  debug    -camera stats HUD                      \n";
            std::cout << "                                                  \n";  
        }   

    }    
    
    /**********************************************************************/


    if(a1=="prim")
    {
        
        //args - prim circle [size] [axis]
        if(a2=="circle")
        {
            if(a3!="")
            {  
                v11 = std::stof(a3);
                //std::cout << " args "<< a2<<" "<< a3<< " " << a4<<"\n";

                if(a4=="x"||a4=="0"){pt_motionplot->prim_shape(0, 0, v11);}    
                if(a4=="y"||a4=="1"){pt_motionplot->prim_shape(0, 1, v11);}  
                if(a4=="z"||a4=="2"){pt_motionplot->prim_shape(0, 2, v11);}                                                 
            }
        } 
        
        //-----------------
        //args - prim square [size]          
        if(a2=="square")
        {
            if(a3!="")
            {  
                v11 = std::stof(a3);
                //std::cout << " args "<< a2<<" "<< a3<< " " << a4<<"\n";
        
                if(a4=="x"||a4=="0"){ pt_motionplot->prim_shape(1, 0, v11); }   
                if(a4=="y"||a4=="1"){ pt_motionplot->prim_shape(1, 1, v11); }  
                if(a4=="z"||a4=="2"){ pt_motionplot->prim_shape(1, 2, v11); }                                                                  
            }
        } 


    }
    
    //------------------
    //run external tools 
    if (a1=="python")
    {
         
        exe_python(a2);    
        std::string path = "3d_obj/PYCORE.obj";
        load_py_obj(path);

    }

    //------------------

    //run external tools 
    if (a1=="test")
    {


    }

    //------------------
    /*
    if (a1=="readbyte")
    { 
    }
 
    if (a1=="sendbyte")
    { 
    }
    */

    //------------------
    if (a1=="save")
    {
        if(a2=="motion")
        {
            pt_motionplot->save_motionfile();
        }         

    }

    //------------------
    if (a1=="load")
    {
        if(a2=="motion")
        {
            pt_motionplot->load_motionfile();
        }         

    }
 
    //------------------
    if (a1=="mx")
    {
        cg.get_machine_xtntx();
    }

    //------------------
    if (a1=="extent"||a1=="extents")
    {
                

        /* CENTROID  
           Vector3 foocntr = pt_model_buffer->centroid();
           pt_model_buffer->add_locator(foocntr);
        */

        //DEBUG - IF NO OBJECT - USE TOOLPATHS INSTEAD 

        //  BBOX (WIP)
        pt_model_buffer->add_locator(pt_model_buffer->t_maxx);
        pt_model_buffer->add_locator(pt_model_buffer->t_minx);
        pt_model_buffer->add_locator(pt_model_buffer->t_maxy);
        pt_model_buffer->add_locator(pt_model_buffer->t_miny);

        pt_model_buffer->add_locator(pt_model_buffer->b_maxx);
        pt_model_buffer->add_locator(pt_model_buffer->b_minx);
        pt_model_buffer->add_locator(pt_model_buffer->b_maxy);
        pt_model_buffer->add_locator(pt_model_buffer->b_miny);
         

    }

    //------------------
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

    //------------------
    //test port out (how fast can we blink?)
    if (a1=="reset")
    {
        std::cout << " resetting simulation \n";

        reset_machine();
    }

    //------------------
    //test port out (how fast can we blink?)
    if (a1=="mtr" || a1=="motor")
    {
        if (a2=="1")
        {    
            set_motor(1);
        }

        if (a2=="0")
        {    
            set_motor(0);
        }        

    }


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
    //clear scene cache 
    if (a1=="unload")
    {
        if(a2=="")
        {
            std::cout << " \n";
            std::cout << "unload: needs args  \n";
            std::cout << "   possible args:   \n";
            std::cout << "       paths, obj   \n";
        }

        if(a2=="obj")
        {    
            unload_obj();
        }

        if(a2=="paths"||a2=="path"||a2=="vec")
        {    
            unload_vec();
        }        
    }

    //------------------
    if (a1=="reload")
    {

        if(a2=="")
        {
            std::cout << "reload: needs args\n";
        }

        if(a2=="obj")
        {    
            reload_obj();
        }

        if(a2=="paths"||a2=="path"||a2=="vec")
        {    
            reload_paths();
        }          
    }


    //--------------
    //not really used - the preferred method is the '``' key 
    if (a1=="stop")
    {
        stop_machine(); 
    }

    //peek at internals 
    if (a1=="lookup"||a1=="lup")
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
    if (a1=="show"|| a1=="sho")
    { 
        std::cout << "------------------------------------------        \n";

        // view camera info onscreen (no toggle)   
        if(a2=="debug")
        { 
            debug_onscreen = true;
        }

        // view globals   
        if(a2=="cfg")
        { 
            cg.show_params();         
        }
        
        //-- 
        // view motion   
        if(a2=="mpath"||a2=="mpaths")
        { 
            pt_motionplot->show_mpath(); 
        }
        // show motionpath indices 
        if(a2=="mpathinfo")
        { 
            pt_motionplot->show_mpath_info();             
        }

        //-- 
        // stats about plotter 
        if(a2=="path"||a2=="paths")
        { 
            pt_motionplot->show_path();             
        }
        // show path indices 
        if(a2=="pathinfo")
        { 
            pt_motionplot->show_path_info();             
        }

        //-- 
        // stats about 3D object  (not path polygons)
        if(a2=="obj")
        { 
            pt_model_buffer->show();
        }

        //-------------------------
        // these are probably only needed for debugging 
        
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
    //toggle grid shortcut
    if (a1=="tg")
    {
        key_cb(103);
    }
    
    //toggle gnomon shortcut
    if (a1=="tgg")
    {
        key_cb(71);
    }

    //--------------
    // toggle stuff
    if (a1=="toggle"||a1=="tog")
    {
        if(a2=="tris")   {key_cb(100);};
        if(a2=="grid")   {key_cb(103);};
        if(a2=="gnomon") {key_cb(71);};
        if(a2=="normals"){key_cb(110);};
        if(a2=="vtxrgb") {key_cb(56);};

    }


    /////////////////////////////////////////
    // MOTION COMMANDS

    //home head
    //if (a1=="hh") {};

    //move head (and store as a new program??)
    if (a1=="mh")
    {

        if(a2!="" && a3!="" && a4!="")
        {
            std::cout << "move head cmd " << a2 << " " << a3 << " "<< a4 << "\n";

            v11 = std::stoi(a2);
            v12 = std::stoi(a3);
            v13 = std::stoi(a4);
            
            Vector3 dest = Vector3(v11, v12, v13); 
            pt_motionplot->add_rapid_vec( &pt_motionplot->quill_pos );
            pt_motionplot->add_rapid_vec( &dest );

            uint rapid_idx = pt_motionplot->rapidmove_vecs.size();


            // add vector (quill) @ retractheight  
            // add vector (Vector3) @ retractheight  
            
            // void add_motion( name, type, prog_id, rapid_in, rapid_out);
            pt_motionplot->add_motion("mh_cmd", "rapid", v11, rapid_idx, v13 );


        }

    }

    /////////////////////////////////////////
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

        uint numx, numy, numz = 0;

        if(a8 != "")
        {
            std::cout << "freerun a8 detected " << a8 << "\n";

            numx = std::stoi(a8);
            numy = std::stoi(a8);
            numz = std::stoi(a8);            
        }
        else
        {
            numx = offset.length()/cg.pp1u_x;
            numy = offset.length()/cg.pp1u_y;
            numz = offset.length()/cg.pp1u_z;
        }

        //non threaded - older 
        run_cncplot( v11, v12, v13, v21, v22, v23, numx, numy, numz);

        //threaded - it seems to work
        //pulse_thread( v11, v12, v13, v21, v22, v23, numx, numy, numz);
    }
    /////////////////////////////////////////
    /*


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

    /////////////////////////////////////////
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

    }
    */
    /////////////////////////////////////////



    //--------------
    //display modes
    if (a1=="dm")
    {
        if(a2=="persp")  {key_cb(49);} 
        if(a2=="side")   {key_cb(50);} 
        if(a2=="top")    {key_cb(64);} 
        if(a2=="front")  {key_cb(51);} 
        
        if(a2=="p0")      {path_render_mode=0;} 
        if(a2=="p1")      {path_render_mode=1;} 

        if(a2=="pts")                  {key_cb(36);} 
        if(a2=="wire")                 {key_cb(52);} 
        if(a2=="s1"||a2=="solid")      {key_cb(53);}  
        if(a2=="s2")                   {key_cb(5);}
        if(a2=="s3")                   {key_cb(6);}
        if(a2=="s4")                   {key_cb(7);}          

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

//parse_cmds(&cmd_buffer, &key);



void parse_cmds(std::string *buffer, unsigned char *pt_key, int* keyint )
{
    bool debug = false;

    int i = static_cast<int>(*pt_key);
    
    //need a way to capture stuck keys - DEBUG - stew on it for a while
    //if(i==last_cmd)
    //{
    //    std::cout << "stop holding down the key, moron!! "<< "\n";
    //}

    last_cmd=i;

    if(debug)
    {
        std::cout << "cursor "<< cursor << " key val " << i  << "buf size: " << buffer->size() << std::endl;
        std::cout << " key val " << i  << std::endl;
    }


    //emergency soft stop is ` key 
    if(i==96)
    {
        //the folowing locks up the program 
        //I am guessing its thread related  
        //set_motor(0);
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




