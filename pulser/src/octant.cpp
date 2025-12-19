
/*************************************************************/
/*
   octant.cpp

   Top level Container for Octant CNC OpenGL application. 
   Contains all the parameters for the machine that dont live in external objects. 

   This contains the "render_loop" for the graphic display in OpenGL 




   Copyright (C) 2018-2026 Keith Legg - keithlegg23@gmail.com

   -------------------------------------------------------------------------------------
   Maya coordinate system    |  Blender coordinate system  |    (MOST) CNC/ 3D printers
                             |                             |         
        Y                    |        Z                    |          Z   
        |                    |        |                    |          |
        |                    |        |                    |          | 
        ______X (look on Z)  |        _____X (look on Y)   |          ______ ? (look on ?)  
   -------------------------------------------------------------------------------------
 


    //--------------------------------------------------------------------
        
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

/*************************************************************/

#include <iostream>

#include <sstream>
#include <vector>
#include <string>
#include <cmath>

#include "timer.h"
#include "gl_setup.h"
#include "point_op.h"
#include "obj_model.h"
#include "cnc_globals.h"
#include "cnc_plot.h"

#include "parse_cmds.h"
#include "gl_gui.h"
#include "gl_render.h"

#include "octant.h"

//#include "socket.h"


#define LEN(arr) ( (int) (sizeof (arr) / sizeof (arr)[0]) ) 


/***************************************/
cncglobals cg;
//cncglobals new cg;



/***************************************/
//display 3D lines and color
extern std::vector<Vector3> linebuffer1;
extern std::vector<Vector3> linebuffer1_rgb;
extern std::vector<Vector3> linebuffer2;
extern std::vector<Vector3> linebuffer2_rgb;

/***************************************/
//motion control related 

extern point_ops PG;

cnc_plot motionplot;
cnc_plot* pt_motionplot = &motionplot;



/***************************************/
//experimental TCP port 

int TCP_PORT;


/***************************************/
//Timer related 
timer mtime = timer();

/***************************************/
// data to pulse out to IO hardware
//vector<Vector3> pulsetrain;
//vector<Vector3>* pt_pulsetrain = &pulsetrain; 


/***************************************/
//I think this was from a test of the VBO 
GLfloat vertices[100];

/***************************************/
//Mouse related
extern bool view_ismoving  ;
extern bool mouseLeftDown  ;
extern bool mouseRightDown ;
extern float orbit_dist; 
extern float orbit_x;         
extern float orbit_y;  

extern float cam_posx; 
extern float cam_posy;
extern float cam_posz;

//second callback for click 
extern int on_click;
extern int clk_x_coord;
extern int clk_y_coord;

float mouse_orbit_speed      = 2.1f;


/***************************************/

// toggle - view prefs - state vars dont change  
bool DRAW_POLYS       = true; // state for toggle command
bool DRAW_GEOM        = true; // state for toggle command
bool draw_cntrgrid    = true;
bool draw_grid        = true; 

bool toglr_flatshaded = true; //DEBUG 

/***************************************/
// single view prefs - use for debugging 

bool draw_lines      = true;
bool draw_normals    = false;
bool draw_quads      = false;
bool draw_triangles  = true;
bool render_text     = true;


//DEBUG - figure the VBO crap out 
bool draw_points     = true;  
bool draw_points_vbo = false; 

//bool draw_bbox       = true;




/***************************************/
// Window related 

extern int VIEW_MODE; 
extern int window_id;
extern int scr_size_x;
extern int scr_size_y;
extern bool scr_full_toglr;



/***************************************/
//3d objects to load 
extern char* obj_filepath;
extern std::vector<std::string>  obj_filepaths;



/***************************************/
//display 3D points and color 
extern std::vector<Vector3> scene_drawpoints;
extern std::vector<Vector3> scene_drawpointsclr;
extern std::vector<Vector3>* pt_scene_drawpoints;

extern obj_model* pt_model_buffer;
extern GLuint texture[3];



/***************************************/
// data containers 


float light_posx = 0; 
float light_posy = .5;
float light_posz = 1;


float gridsquares = 10;
float gridsize = 5;
float gnomonsize = 1;


int surfce_clr_r; //read from setup.olm 
int surfce_clr_g; 
int surfce_clr_b; 




//debug - consider not using this old data type and use 3 ints in a vector?
RGBType line_color;
RGBType *pt_linecolor = &line_color;

RGBType grid_color;
RGBType *pt_gridcolor = &grid_color;

RGBType grid_color2;
RGBType *pt_gridcolor2 = &grid_color2;

 
extern GLfloat clr_linez[];
extern GLfloat emis_full[];
extern GLfloat emis_half[];
extern GLfloat emis_text[];
extern GLfloat emis_points[];
extern GLfloat emis_off[];
extern GLfloat emis_teal[];
extern GLfloat emis_red[];  
extern GLfloat emis_green[];
extern GLfloat emis_blue[];
extern GLfloat emis_lines[];
extern GLfloat clr_yellow[];
extern GLfloat clr_green[];
extern GLfloat clr_blue[];
 




void set_colors(void){
    //peripheral grid color  
    pt_gridcolor->r = 1;
    pt_gridcolor->g = 105;
    pt_gridcolor->b = 5;

    //centergrid gnomon color  
    pt_gridcolor2->r = 200;
    pt_gridcolor2->g = 200;
    pt_gridcolor2->b = 200;

    // geom color  
    pt_linecolor->r = 100;
    pt_linecolor->g = 100;
    pt_linecolor->b = 100;

    // line color 
    // pt_linecolor2->r = 0;
    // pt_linecolor2->g = 100;
    // pt_linecolor2->b = 0;
}


/***************************************/
void timer_init(void)
{
    mtime.stop();

}


/***************************************/
void update_clk(void)
{
    //symtime

    mtime.stop();

}

/***************************************/
void toggle_polygon_draw()
{
    if (DRAW_POLYS == true){
        DRAW_POLYS = false;
        draw_quads      = false;
        draw_triangles  = false;
    }else{
        DRAW_POLYS = true;
        draw_quads      = true;
        draw_triangles  = true;
    }
}



/***************************************/
//DEBUG - GL_MODELVIEW_MATRIX and GL_PROJECTION_MATRIX seem to be the same 
//this was an artifact of the render code - I did a dump of the matrix for the renderer to use
void grab_camera_matrix( Matrix4 *pt_mmm)
{
    GLfloat model[16]; 

    glMatrixMode(GL_MODELVIEW);
    //glPushMatrix();
    glGetFloatv(GL_MODELVIEW_MATRIX, model); 
    //glPopMatrix();

    glutm44_to_m44(pt_mmm, model);

}

//DEBUG - GL_MODELVIEW_MATRIX and GL_PROJECTION_MATRIX seem to be the same 
/*
void grab_projection_matrix(m44 *pt_mpm )
{
    GLfloat model[16]; 

    glMatrixMode(GL_PROJECTION);
    // glPushMatrix();
    //gluOrtho(...);

    glGetFloatv(GL_PROJECTION_MATRIX, model); 
    
    //glPopMatrix();

    glutm44_to_m44(pt_mpm, model);

    // https://www.khronos.org/opengl/wiki/Viewing_and_Transformations
    //manipulate matrix example
    // glMatrixMode(GL_MODELVIEW);
    // glLoadIdentity();
    // glScalef(1., 1., -1.);

    //render_m44(pt_mmm);
    //print_matrix(my_model_matrix);
}
*/


/*
//DEBUG - for testing 
void tweak_matrix( void )
{
 
    GLdouble translate[16] = {1,0,0,0,
                              0,1,0,0,
                              0,0,1,0,
                              1,1,1,1};    


    //glPushMatrix();
    //glMatrixMode(GL_PROJECTION);
    
    //glMatrixMode(GL_MODELVIEW);
                              

    glMultMatrixd(translate);
    //glGetFloatv(GL_PROJECTION_MATRIX, model); 
    
    //glPopMatrix();

    //glutm44_to_m44(pt_mpm, model);

}
*/



/***************************************/

//DEBUG TRIGGERS THE "OLD SEGFAULT "
void warnings(void)
{   
    
    // let us know if there is a discernable problem 
    std::cout << "\n\n\n\n###########################################\n";

    if(!draw_lines || !pt_model_buffer->num_lines){
        std::cout << "#warn - no lines or disabled.     \n";
    }

    if(!draw_triangles || !pt_model_buffer->num_tris){
        std::cout << "#warn - no triangles or disabled. \n";
    }

    if(!draw_quads || !pt_model_buffer->num_quads){ ; }    
   
}

/***************************************/

void reset_view(void){
 
    orbit_x    = 0; //.125;  
    orbit_y    = 0; //-.06;   
    orbit_dist = 1; // 5.0;  

    // cam_rotx = 0; //camera rotation
    // cam_roty = 0;
    // cam_rotz = 0;

    cam_posx =  0; //camera location
    cam_posy =  0;
    cam_posz =  0;

}





/***************************************/

void test_bezier( Vector3 start, Vector3 ctrl1, Vector3 ctrl2, Vector3 end)
{

    //vector<Vector3> * ptDrawvec = &linebuffer1;
    //vector<Vector3> * ptDrawClr = &linebuffer1_rgb;
    //PG.cubic_bezier(ptDrawvec, ptDrawClr,  ptnum_drawvec3, 10, start, ctrl1, ctrl2, end);

}



/***************************************/

std::string cmd_buffer;

static void parser_cb(unsigned char key, int x, int y)
{
    parse_cmds(&cmd_buffer, &key);
    glutPostRedisplay();
};




/***************************************/
/***************************************/
/***************************************/



/***************************************/
/***************************************/

char cs[100];
char s[100];


double localsimtime;

static void render_loop()
{
    // Clear The Screen And The Depth Buffer
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //set light position 
    GLfloat lightpos[] = { light_posx, light_posy, light_posz, 0}; // homogeneous coordinates
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

    glutKeyboardFunc(parser_cb);

    //------------ 
    //DEBUG - THE MAIN LOGIC OF MOTION SHOULD NOT BE IN THE RENDER LOOP!
    //THIS NEEDS TO BE INDEPENDANT - AND UPDATE OPENGL, NOT THE OTHER WAY AROUND 
    //MOVE AS MUCH AS POSSIBLE INTO CNC_PLOT 
  
    if(mtime.running)
    {
        
        // //std::cout << mtime.getElapsedTime() << "\n";
        // std::cout << "local simtime is " << localsimtime << "\n";
        // std::cout << " pidx "<< motionplot.pidx          << "\n";

        //----

        localsimtime = mtime.get_elapsed_simtime() * motionplot.timediv;

        //simtime runs between 0-1 - it resets each time another vector in the stack has been processed
        if (localsimtime>=1.0)
        {

            //std::cout << "-----------------------------------\n";        
            //std::cout << "running index " << motionplot.pidx        << "\n";

            //iterate the stack of vectors to process
            if (motionplot.pidx<motionplot.toolpath_vecs.size())
            {
                motionplot.pidx++;        
                // start the (sim) clock over at the end of each vector segment 
                // 0.0 - 1.0 is the range - which feeds the 3D `lerp           
                mtime.step_sim();
            }

            //program finished here
            if (motionplot.pidx>=motionplot.toolpath_vecs.size()-1)
            {


                mtime.running = false;
                motionplot.stop();
                motionplot.finished = true;

                //update rebuilds the stack of vectors to process
                //this is for rapid move, etc 
                motionplot.pidx = 0;

                motionplot.update_cache();

            }
        }
        
        //----------------- 
        //the main loop where we update display and pulse the ports.
        if (motionplot.pidx<=motionplot.toolpath_vecs.size()-1 && mtime.running)
        {

            //DEBUG - get the length of the vector/spatial divs to calc proper speed 
            //vectormag   motionplot.toolpath_vecs[motionplot.pidx]

            Vector3 s_p = motionplot.toolpath_vecs[motionplot.pidx];
            Vector3 e_p = motionplot.toolpath_vecs[motionplot.pidx+1];  
            // std::cout << "start "<< s_p.x <<" "<< s_p.y << " "<< s_p.z << "\n";
            // std::cout << "end   "<< e_p.x <<" "<< e_p.y << " "<< e_p.z << "\n";                       


            PG.lerp_along(&motionplot.quill_pos, 
                           s_p, 
                           e_p, 
                           (float) localsimtime);

            glColor3d(1, .4, 1);
            draw_locator(&motionplot.quill_pos, .5);
        }

    }//end program cycle running  

    //------------ 
    //draw locator when idle 
    if(!mtime.running)
    {
        glColor3d(.7, .7, .7);
        draw_locator(&motionplot.quill_pos, .5);        
    }
    
    
    //------------ 
    //I clearly dont get the whole view matrix thing - moved out of text render
    //this is directly related and need to do homework
    setOrthographicProjection();
    glLoadIdentity();
    //glPushMatrix();
    
    if (render_text)
    {
        if(on_click)
        {
            //active but does not do anything 
            //maybe look at camera pos and rot and draw a vector for intersection with triangle?
             
            //std::cout << " click! "<< clk_x_coord << " "<< clk_x_coord << "\n";
            on_click=0;
        }

        glBindTexture(GL_TEXTURE_2D, texture[0]); 
        glMaterialfv(GL_FRONT, GL_EMISSION, emis_text);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);
    
        //-----------------------------
        // render text in window 

        //void *fontsm = GLUT_BITMAP_8_BY_13;     
        void *font   = GLUT_BITMAP_TIMES_ROMAN_24; 
        
        // command line text 
        glColor3f(0.6f, 1.0f, 0.0f);  //text color 
        renderBitmapString(  20, scr_size_y-20  ,(void *)font,  cmd_buffer.c_str() );
      
        //--
        //X Y Z - QUILL/HEAD POSITION  

        glColor3d(0, 1.0, 1.0);        
        sprintf(cs, "X:%.2f Y:%.2f Z:%.2f", motionplot.quill_pos.x, motionplot.quill_pos.y, motionplot.quill_pos.z );
        //glColor3f(1.0f, 1.0f, 1.0f);  //text color 
        renderBitmapString( ((int)(scr_size_x/2)-100), 50  ,(void *)font, cs );

        //---        
        //sprintf(s, "    %d quads ", pt_model_buffer->num_quads );
        //renderBitmapString( ((int)(scr_size_x/2)-200) , scr_size_y-20  ,(void *)font, s );
        
        
        //-----------------------------------------

        //DEBUG USING THE TIMER AS INDICATOR OF MACHINE RUNNING 
        //PROBABLY NOT WHAT YOU WANT - CONSIDER THREADS AND A MORE COMPLEX SEMAPHORE 
        if(!mtime.running)
        {
            glColor3d(1.0, 0, 0);
            renderBitmapString( ((int)(scr_size_x/2)-300) , 30  ,(void *)font, "ESTOP" ); 
        }
        else
        {
            glColor3d(0.0, 1.0, 0);
            renderBitmapString( ((int)(scr_size_x/2)-300) , 30  ,(void *)font, "ESTOP" ); 
        }

        //-----------------------------------------

        //---
        // glColor3d(1.0, 1.0, 1.0);
        // sprintf(s, "camera X:%f Y:%f Z:%f", cam_posx, cam_posy, cam_posz);
        // renderBitmapString( ((int)(scr_size_x/2)-250), 20  ,(void *)fontsm, s );
        //-----------------------------

    }
   
    //I clearly dont get the whole view matrix thing - moved out of text render
    //this is directly related to text render and I need to do homework
    //glPopMatrix();
    resetPerspectiveProjection();
    //set color back
    glColor3d(1.0, 1.0, 1.0);
    glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full);


    // --------------------------------------------

    // // glPushMatrix();

    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    //glTranslatef( sin(tb.getElapsedTime()), 0, 0);
    //glRotatef(45,0,1,0);

    //----------------------------------------------

    // // move the view without using gluLookAt
    // glMatrixMode( GL_MODELVIEW );
    // glLoadIdentity();
    // glTranslatef( 0,0,-10 );
    // glRotatef( 90, 1,0,0 );

        // timer test 
        //tb.stop();
        // print the elapsed time in millisec
 

    //----------------------------------------------
    
    switch (VIEW_MODE) 
    { 
        // orthographic side 
        case 1: 
            //I dont like doing it this way - have the target follow camera 
            gluLookAt( 1.0   , cam_posy , cam_posz,  // look from camera XYZ
                       0.0   , cam_posy , cam_posz,  // look at the origin
                       0.0   , 1.0      , 0.0        // positive Y up vector
            );        
        break; 
    
        // orthographic top   
        case 2:  
            gluLookAt( cam_posx , 1.0   , cam_posz,  // look from camera XYZ
                       cam_posx , 0.0   , cam_posz,  // look at the origin
                       0.0     , 0.0    , -1.0        // positive X (Y up when looking down)
            );   
        break; 
    
        // orthographic front  
        case 3:  
            gluLookAt( cam_posx , cam_posy  , 1.0,   // look from camera XYZ
                       cam_posx , cam_posy  , 0.0,   // look at the origin
                       0.0      , 1.0       , 0.0    // positive Y up vector
            );    
        break; 
    

        default:   
            // cheapo perspective navigation  
            cam_posx = -sin( orbit_x * mouse_orbit_speed )     * orbit_dist    ;
            cam_posy =      -orbit_y                   * (orbit_dist/2);
            cam_posz =  cos( orbit_x * mouse_orbit_speed )     * orbit_dist    ;

            //for now use gluLookAt for all view modes -  
            gluLookAt( cam_posx, cam_posy , cam_posz,  // look from camera XYZ
                       0.0     , 0.0      , 0.0,       // look at the origin
                       0.0     , 1.0      , 0.0        // positive Y up vector
            ); 
        break;   
    } 
  
    /******************************************/
    /******************************************/
    // display loaded normals as lines 
    if (draw_normals)
    {
        render_normals(pt_model_buffer);
    }//draw normals   
   

    /******************************************/
    graticulate(&draw_grid, &draw_cntrgrid, pt_gridcolor, pt_gridcolor2);

    //show_bbox(&draw_bbox, pt_gridcolor);

    //m33 foobar = m33_from_euler(45,45,45);
    //render_m33(&foobar);
    
    // m44 foobar = m44_from_euler(0,45,90);
    // render_m44(&foobar);

    /******************************************/
     
    if (draw_points_vbo)
    {
      render_vbo(pt_model_buffer);
    }
    
    /******************************************/
    // draw 3D line geometry 
    
    if (draw_lines)
    {
        render_lines(pt_model_buffer);
    } 
    
   
    //DEBUG - IF THERE IS A SPACE AT END OF FIDS, IT LOADS TRI AS QUAD
    if (draw_quads )
    {
        render_quads(pt_model_buffer);
    }
 

    /******************************************/
    // draw the polygon geometry 
   
    if(draw_triangles)
    {
        //glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
        //glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full); 
        //glBindTexture(GL_TEXTURE_2D, texture[2]);   // choose the texture to use.

        GLfloat ke[] = { 0.0, 0.0, 0.00, 1.0 };
        GLfloat ka[] = { 0.0, 0.0, 0.0, 1.0 };
        GLfloat kd[] = { 0.5, 0.5, 0.3, 1.0 };
        GLfloat ks[] = { 0.2, 0.2, 0.2, 1.0 };
        GLfloat ns[] = { 80.0 };
        glMaterialfv(GL_FRONT, GL_EMISSION, ke);
        glMaterialfv(GL_FRONT, GL_AMBIENT, ka);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, kd);
        glMaterialfv(GL_FRONT, GL_SPECULAR, ks);
        glMaterialfv(GL_FRONT, GL_SHININESS, ns);

        if (toglr_flatshaded){
            glColor3f(1.,1.,1.);
        }
        
        //std::cout << " num tris " << pt_model_buffer->num_tris << "\n";

  
        glBegin(GL_TRIANGLES);  
            for (int p_i=0;p_i<pt_model_buffer->num_tris;p_i++)
            { 
              
                // fetch the triangle indices from vertex list
                int tri1 = pt_model_buffer->tris[p_i][0];
                int tri2 = pt_model_buffer->tris[p_i][1];
                int tri3 = pt_model_buffer->tris[p_i][2];

                int vn1 = pt_model_buffer->vnids[p_i][0];
                int vn2 = pt_model_buffer->vnids[p_i][1];
                int vn3 = pt_model_buffer->vnids[p_i][2];

                // use the same vertex indices to lookup RGB 
                Vector3 rgb1 = pt_model_buffer->vtxrgb[tri1-1];
                Vector3 rgb2 = pt_model_buffer->vtxrgb[tri2-1];
                Vector3 rgb3 = pt_model_buffer->vtxrgb[tri3-1];
                

                //use one color for now
                //glColor3f(.7,.7,.5);   //use one color for now 

                //std::cout << "plyidx " << tri1 << " " << tri2 << " " << tri3 << "\n";

                //------------------------------//
                glColor3f(rgb1.x,rgb1.y,rgb1.z); 
                //Vector2 uv = pt_model_buffer->uvs[tri1];
                // glTexCoord2f(uv.x, uv.y);
                glTexCoord2f(0.5, 1.0);                
              
                Vector3 nrm1 = pt_model_buffer->vnormals[vn1-1];
                glNormal3f( nrm1.x, nrm1.y, nrm1.z);
                //std::cout <<  " vtxnrm1 "<< nrm1.x << " "<< nrm1.y << " "<< nrm1.z << "\n";
                
                Vector3 pt1 = pt_model_buffer->points[tri1-1];
                glVertex3f(pt1.x, pt1.y, pt1.z);

                //------------------------------//
                glColor3f(rgb2.x,rgb2.y,rgb2.z); 
                //Vector2 uv = pt_model_buffer->uvs[tri2];
                //glTexCoord2f(uv.x, uv.y);
                glTexCoord2f(0.0, 1.0); 

                // calculated face normals 
                Vector3 nrm2 = pt_model_buffer->vnormals[vn2-1];
                glNormal3f( nrm2.x, nrm2.y, nrm2.z);
                //std::cout <<  " vtxnrm2 "<< nrm2.x << " "<< nrm2.y << " "<< nrm2.z << "\n";

                Vector3 pt2 = pt_model_buffer->points[tri2-1];
                glVertex3f(pt2.x, pt2.y, pt2.z);

                //------------------------------//
                glColor3f(rgb3.x,rgb3.y,rgb3.z); 
                //Vector2 uv = pt_model_buffer->uvs[tri3];
                //glTexCoord2f(uv.x, uv.y);
                glTexCoord2f(1.0, 0.0);       

                // calculated face normals
                Vector3 nrm3 = pt_model_buffer->vnormals[vn3-1];
                glNormal3f( nrm3.x, nrm3.y, nrm3.z);
                //std::cout <<  " vtxnrm3 "<< nrm3.x << " "<< nrm3.y << " "<< nrm3.z << "\n";
                
                Vector3 pt3 = pt_model_buffer->points[tri3-1];
                glVertex3f(pt3.x, pt3.y, pt3.z);

            }

        glEnd(); 
          

        Vector3 tri_cntr;

        // display loaded normals as lines 
        if (draw_normals)
        {
            glColor3f(.5,.5,0);

            for (int p_i=0;p_i<pt_model_buffer->num_tris;p_i++)
            {             
                // fetch the pts for a triangle
                Vector3 p1 = pt_model_buffer->points[pt_model_buffer->tris[p_i][0]-1];
                Vector3 p2 = pt_model_buffer->points[pt_model_buffer->tris[p_i][1]-1];
                Vector3 p3 = pt_model_buffer->points[pt_model_buffer->tris[p_i][2]-1];

                // calculate the centroid 
                tri_cntr.x = (p1.x + p2.x + p3.x)/3;
                tri_cntr.y = (p1.y + p2.y + p3.y)/3;
                tri_cntr.z = (p1.z + p2.z + p3.z)/3; 

                //display shorter for neatness  
                Vector3 mv = tri_cntr.operator+(pt_model_buffer->fnormals[p_i].operator/(10));
                //Vector3 mv =  tri_cntr.operator+(pt_model_buffer->fnormals[p_i]);
    
                glBindTexture(GL_TEXTURE_2D, texture[1]);
                glMaterialfv(GL_FRONT, GL_EMISSION, clr_yellow);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);

                glBegin(GL_LINES);
                    glColor3f(1.,1.,0); //hack for now
                    glVertex3f(tri_cntr.x, tri_cntr.y, tri_cntr.z);
                    glColor3f(1.,1.,0); //hack for now
                    glVertex3f(mv.x, mv.y, mv.z);
                glEnd();


                glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full);  
            }
        }//draw normals (triangles)        
    }//draw triangles
   

    // ----------------------
    // draw scene defined geom 
    
    Vector3 sv  = Vector3();
    Vector3 ev  = Vector3();

    // draw linebuffer1  
    if (DRAW_GEOM)
    {

        glBindTexture(GL_TEXTURE_2D, texture[1]);
        glMaterialfv(GL_FRONT, GL_EMISSION, emis_full);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);

        //DEBUG vertex color is off until I fix it
        //DEBUG - size() does not catch new vectors added with the GUI?
        
        /*
        //OLD CODE THAT DRAWS EVERYTHING AS A SINGLE POLY 
        //intentionally start at 1 - skip the first point 
        //we need at least two points to indicate a line 
        for (unsigned int p_i=1;p_i<linebuffer1.size();p_i++)
        {   
            //ignore the first vector/point - off for now 
            // if(p_i==0)
            // {
            //     sv  = linebuffer1[p_i];
            //     ev  = linebuffer1[p_i+1];
            //     //rgb = linebuffer1_rgb[p_i+1];  
            // }
            if(p_i>=1){ 
                sv  = linebuffer1[p_i-1];
                ev  = linebuffer1[p_i];
                //rgb = linebuffer1_rgb[p_i];            
            }

            glBegin(GL_LINES);
                glColor3f(1.,0,0); //hack for now
                //glColor3f(rgb.x,rgb.y,rgb.z);
                glVertex3f(sv.x, sv.y, sv.z);
                
                glColor3f(1.,0,0); //hack for now
                //glColor3f(rgb.x,rgb.y,rgb.z);
                glVertex3f(ev.x, ev.y, ev.z);
        
            glEnd();
             
        }*/


        //----
        //Specifies the primitive or primitives that will be created from vertices presented between glBegin and the subsequent glEnd. 
        //Ten symbolic constants are accepted: GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP, GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS, GL_QUAD_STRIP, and GL_POLYGON. 

        //NEW CODE - USED INDEXED LOOKUP 
        /*
        for (unsigned int p_i=0;p_i<pt_motionplot->num_plys;p_i++)
        {   

            for (unsigned int ii=1;ii<pt_motionplot->tp_idxs[p_i].size();ii++)
            {
                glBegin(GL_LINES);
                    unsigned int si= pt_motionplot->tp_idxs[p_i][ii-1];
                    unsigned int ei= pt_motionplot->tp_idxs[p_i][ii];
                    
                    sv  = pt_motionplot->program_vecs[si];
                    ev  = pt_motionplot->program_vecs[ei];
                    //rgb = linebuffer1_rgb[p_i];            
                
                    glColor3f(0,1.,0); //hack for now
                    glVertex3f(sv.x, sv.y, sv.z);
                    
                    glColor3f(1.,0,0); //hack for now
                    glVertex3f(ev.x, ev.y, ev.z);

                glEnd();
            }
           
        } */
     

        //std::cout << "foo " << pt_motionplot->tp_idxs[0].size() << "\n";

        //////////////////////////////////////////////////////////
       
        int test = 1;

        if (pt_motionplot->tp_idxs[0].size()>1)
        {
            for (unsigned int ii=1;ii<pt_motionplot->tp_idxs[test].size();ii++)
            {
                glBegin(GL_LINES);
                    unsigned int si= pt_motionplot->tp_idxs[test][ii-1];
                    unsigned int ei= pt_motionplot->tp_idxs[test][ii];

                    sv  = pt_motionplot->program_vecs[si];
                    ev  = pt_motionplot->program_vecs[ei];
      
                
                    glColor3f(0,1.,0); //hack for now
                    glVertex3f(sv.x, sv.y, sv.z);
                    
                    glColor3f(1.,0,0); //hack for now
                    glVertex3f(ev.x, ev.y, ev.z);

                glEnd();
            }
        }  
         
        //////////////////////////////////////////////////////////



        //----       
        
        glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full); 

    }


    //-----------------------
    //draw linebuffer2
    if (DRAW_GEOM)
    {

        glBindTexture(GL_TEXTURE_2D, texture[2]);
        glMaterialfv(GL_FRONT, GL_EMISSION, emis_full);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);
        
        //intentionally start at 1 - skip the first point 
        //we need at least two points to indicate a line 
        for (unsigned int p_i=1;p_i<linebuffer2.size();p_i++)
        {   
            if(p_i>=1){ 
                sv  = linebuffer2[p_i-1];
                ev  = linebuffer2[p_i];
                //rgb = linebuffer1_rgb[p_i];            
            }

            glBegin(GL_LINES);
                glColor3f(.7,.7,.7); //hack for now
                //glColor3f(rgb.x,rgb.y,rgb.z);
                glVertex3f(sv.x, sv.y, sv.z);
                
                glColor3f(.7,.7,.7); //hack for now
                //glColor3f(rgb.x,rgb.y,rgb.z);
                glVertex3f(ev.x, ev.y, ev.z);
            glEnd();
             
        }
        
        glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full); 

    }

    //-----------------------

    // glPopMatrix();

    // swap the other (double) buffer to display what just got drawn.
    glutSwapBuffers();
  
    view_ismoving = false;
}


/***************************************/
void animate(){
    //ticker += .01;
    render_loop();
}


/***************************************/

static void reshape_window(int width, int height)
{
    
    if ( height == 0 )                // Prevent A Divide By Zero If The Window Is Too Small
        height=1;
    glViewport(0, 0, width, height);  // Reset The Current Viewport And Perspective Transformation

    scr_size_x = width;
    scr_size_y = height;    


    
    // // https://www.khronos.org/opengl/wiki/Viewing_and_Transformations
    // int fov = 1;
    // int zNear = .1;
    // int zFar = 10;
    // glMatrixMode(GL_PROJECTION);
    // glLoadIdentity();
    // float aspectYScale = 1.0;
    // if ( aspect < conditionalaspect ) {
    //     aspectYScale *= (float)scr_size_x / (float)scr_size_y / conditionalaspect;
    // }
    // gluPerspective(atan(tan(fov * 3.14159 / 360.0) / aspectYScale) * 360.0 / 3.14159, (float)scr_size_x/(float)scr_size_y, zNear, zFar);

}//end resize callback

/***************************************/

void set_view_ortho(void)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();                   

    //gluOrtho2D(scr_size_x, scr_size_y, -1, 1 ); //(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top);
    float orthzoom = 200;
    //glOrtho( -scr_size_x/orthzoom, scr_size_x/orthzoom, -scr_size_y/orthzoom, scr_size_y/orthzoom, -1, 1 );
    
    glOrtho( -scr_size_x/orthzoom, scr_size_x/orthzoom, -scr_size_y/orthzoom, scr_size_y/orthzoom, -10, 10 );
    glMatrixMode(GL_MODELVIEW);
}


//text demo 
void setOrthographicProjection() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
       gluOrtho2D(0, scr_size_x, 0, scr_size_y);
    glScalef(1, -1, 1);
    glTranslatef(0, -scr_size_y, 0);
    glMatrixMode(GL_MODELVIEW);
} 

//text demo 
void resetPerspectiveProjection() {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
} 

//text demo 
void renderBitmapString(float x, float y, void *font,const char *string){
    const char *c;
    glRasterPos2f(x, y);
    for (c=string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
} 




/***************************************/
void set_view_persp(void)
{
    // set perspective viewing frustum
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)(scr_size_x)/scr_size_y, 1.0f, 1000.0f); // FOV, AspectRatio, NearClip, FarClip
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();    

}



/**************************************************/
/**************************************************/
/**************************************************/
void start_gui(int *argc, char** argv){

    glutInit(argc, argv);  

    //shader_test();
    set_colors();

    timer_init();
    //mtime.start();

    //------------

    //load CNC cfg (including paths to .obj files) 

    //setup filepaths and paths to cut 
    cg.load_cfg_file(argv[1]);

    //load the 3d models 
    cg.load_objects();

    // we have vectors in display - calcluate the head path from them   
    motionplot.loadpath(&linebuffer1);
    motionplot.retract_height = cg.retract_height;
    motionplot.work_height    = cg.work_height;



    //------------
    
    //warnings();
         
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  
    glutInitWindowSize(scr_size_x, scr_size_y);  //window size
    glutInitWindowPosition(0, 0);  
    
    window_id = glutCreateWindow("CNC_Pulser v.a0001"); //create an opengl window 
 
    /////////////////////////////////////////////////
    reset_view();

    // Register GL callbacks       
    glutDisplayFunc(&render_loop); 
    glutIdleFunc(&animate);
    glutReshapeFunc(&reshape_window);  // register window resize callback 

    InitGL(scr_size_x, scr_size_y); // Initialize window. 

    glutMouseFunc (octant_mouse_button);
    glutMotionFunc (octant_mouse_motion);

    //---

    // experimental draw polygon 
    //glutMouseFunc (draw_poly_mousevent);

    //loadImage("textures/generated2.bmp" , imageloaded_bfr);
    //loadImage("textures/generated2.bmp" , imageloaded_bfr2);
    
    ///////////
    // create and apply 2D texture   
    glGenTextures(4, &texture[0]);            //create 3 textures

    //general line color 
    glBindTexture(GL_TEXTURE_2D, texture[1]);  
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); // scale linearly when image bigger than texture
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // scale linearly when image smalled than texture
    //glTexImage2D(GL_TEXTURE_2D, 0, 3, imageloaded_bfr2->sizeX, imageloaded_bfr2->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, imageloaded_bfr2->data);

    //polygon color 
    glBindTexture(GL_TEXTURE_2D, texture[2]);  
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); // scale linearly when image bigger than texture
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // scale linearly when image smalled than texture
    //glTexImage2D(GL_TEXTURE_2D, 0, 3, imageloaded_bfr->sizeX, imageloaded_bfr->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, imageloaded_bfr2->data);


    //polygon color 2
    glBindTexture(GL_TEXTURE_2D, texture[3]);  
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); // scale linearly when image bigger than texture
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // scale linearly when image smalled than texture
    //glTexImage2D(GL_TEXTURE_2D, 0, 3, imageloaded_bfr->sizeX, imageloaded_bfr->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, imageloaded_bfr->data);

    glutMainLoop();// Start Event Processing Engine   
   
    //DEBUG - polymorphic class type ‘obj_model’ which has non-virtual destructor might cause undefined behavior
    //delete pt_model_buffer;

}





/**************************************************/
/**************************************************/

//define keyboard events 
void key_cb(unsigned int key) 
{


    if (key == 52) //4 - display as wire 
    { 
        glDisable(GL_TEXTURE_2D);        
        glDisable(GL_LIGHTING);
        glPolygonMode (GL_FRONT_AND_BACK,  GL_LINE);

    }

    if (key == 53) //5 - display as solid, no texture  
    { 

        //glDisable(GL_TEXTURE_2D);        
        //glDisable(GL_LIGHTING);
        //glShadeModel(GL_SMOOTH);              
        //glDisable(GL_TEXTURE_2D);
        //glDisable(GL_COLOR_MATERIAL);
        // glDisable(GL_LIGHT0);
        // glDisable(GL_LIGHT1);
        //------------------------
                        
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
        setlight0();        
    }
    
    if (key == 54) //6 - display as solid, with texture 
    { 
        glEnable(GL_TEXTURE_2D);
        setlight0();
    }


    if (key == 55) // 7
    { 
        glEnable(GL_LIGHTING);
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    }



    /*

    // PYLINK TO IMPLEMENT 

    if (key == 83) //shift s key - open socket  
    { 
        printf("opening socket on port %i \n", TCP_PORT); 
        //sockettest3();
    }

    if (key == 112) //p
    { 

        //go ahead and dump camera matrix automatically 
        m44 foo = identity44();
        grab_camera_matrix(&foo);
        negate_y_axis(&foo);
        save_matrix44(cam_matrix_filepath, &foo );

        //launch python3 
        init_pycore(); 
    }
    if (key == 111) // o
    { 

        clear_scenegeom();
        reset_objfile(pt_model_buffer, pt_obinfo);  

        //char* file2 = "3d_obj/PYCORE.obj";
        char* pycorepath = "3d_obj/PYCORE.obj";
        //reset_objfile(pt_model_buffer, pt_obinfo);

        load_objfile(pycorepath, pt_model_buffer );
        pt_model_buffer->calc_normals();
        get_obj_info( pt_model_buffer, pt_obinfo);


    }
    */

    //ESCAPE KEY
    if (key == 27) 
    { 
        glutDestroyWindow(window_id); 
        exit_program();
        exit(0);                   
    }

    //d key 
    if (key == 100) 
    { 
        toggle_polygon_draw();
    }

    if (key == 49) //1 - perspective mode
    { 
      
        VIEW_MODE = -1; 

        // // set viewport to be the entire window
        // glViewport(0, 0, (GLsizei)scr_size_x, (GLsizei)scr_size_y);
        // // set perspective viewing frustum
        // glMatrixMode(GL_PROJECTION);
        // glLoadIdentity();
        // gluPerspective(45.0f, (float)(scr_size_x)/scr_size_y, 1.0f, 1000.0f); // FOV, AspectRatio, NearClip, FarClip
        // // switch to modelview matrix in order to set scene
        // glMatrixMode(GL_MODELVIEW);
        // glLoadIdentity();
        
        set_view_persp();
    }

    if (key == 50) //2 - orthographic side 
    { 
        reset_view();
        VIEW_MODE = 1; 
        set_view_ortho();
    }

    if (key == 64) //shift 2 - orthographic top  
    {
        reset_view();
        VIEW_MODE = 2; 
        set_view_ortho();    
    }

    if (key == 51) //3 - orthographic front 
    { 
        reset_view();
        VIEW_MODE = 3; 
        set_view_ortho();
    }

    if (key == 35) //shift 3 - ???
    {
        reset_view();        
        VIEW_MODE = 4; 
        set_view_ortho();        
    }


    //------

    if (key == 45) // minus
    { 
        orbit_dist-=.1;  
    }
    
    if (key == 61) //plus
    { 
        orbit_dist+=.1;  
    }

    if (key == 105) //i - draw bbox
    {   
        /*
        std::cout << "\n\n\n"; 
        get_obj_info( pt_model_buffer, pt_obinfo);        
        if (draw_bbox == true){
            draw_bbox = false;
        }else{
            draw_bbox = true;
            show(pt_model_buffer);            
        }*/
    }

    if (key == 114) //r
    { 
        //glColor3f( 1., 1., 1.); 
        //load_scene(obj_filepath);
    }


    if (key == 102) //f
    { 
        //TODO - set orbit_dist to X2 model size 
        //set_screen_square(&scr_size_x, &scr_size_y);
        //reset_view();
    }


    //----
    if (key == 32) // space
    { 

        if (scr_full_toglr == true){
            glutFullScreen();
            scr_full_toglr = false;
        }else{
            glutReshapeWindow(800, 800);
            glutPositionWindow(0,0);  
            scr_full_toglr = true;
        }

    }

    //----
    if (key == 110) // n
    { 

        if (draw_normals == true){
            draw_normals = false;
        }else{
            draw_normals = true;
            //pt_model_buffer->calc_normals();

        }
        //std::cout << "draw normals " << draw_normals << "\n"; 
    }

    //----
    if (key == 70) //shift f
    { 
        if (DRAW_GEOM == true){
            DRAW_GEOM = false;

        }else{
            DRAW_GEOM = true;
        }
    }

    //----
    if (key == 71) //shift g
    { 
        if (draw_cntrgrid == true){
            draw_cntrgrid = false;
        }else{
            draw_cntrgrid = true;
        }

    }

    //----
    if (key == 103) //g
    { 
        if (draw_grid == true){
            draw_grid = false;
        }else{
            draw_grid = true;
        }
    }


    //----
    if (key == 36) //Shift 4 - display as points 
    { 
        // to draw points  
        //glPolygonMode (GL_FRONT_AND_BACK, GL_POINT);
        //glPointSize(4);
    
        if (draw_points == true){
            draw_points_vbo = false;
            glPolygonMode (GL_FRONT_AND_BACK,  GL_POINT);
            draw_points = false;
        }else{
            draw_points_vbo = true;
            glPolygonMode (GL_FRONT_AND_BACK,  GL_LINE);
            draw_points = true;
        }
    }

    //----
    if (key == 37) //shift 5 , ignore lights  
    { 
        //trying to get the flat, no shaded ambient look 
        if (toglr_flatshaded == true){
            glDisable(GL_TEXTURE_2D);        
            glDisable(GL_LIGHTING);
            toglr_flatshaded = false;
        }else{
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_LIGHTING);
            toglr_flatshaded = true;
        }

    } 

}







/**************************************************/
/**************************************************/
/**************************************************/
/**************************************************/

/*
void init_pycore(void){
    //call python from here!!

    char* pycore_cmd = "runcommand";    
    char buffer[256];

    if (active_filepath[0]=='\0')
    {
        printf("## active fp %s is blank!!\n", active_filepath );
        //strcpy(active_filepath, "foo" ); 

    }

    printf("command: python3 pycore.py %s %s\n", active_filepath, pycore_cmd);

    //there is a bug here - it demands an "active_filepath" , which wont be there if scene.olm is blank 
    snprintf(buffer, sizeof(buffer), "python3 pycore.py %s %s", active_filepath, pycore_cmd);
    int ret = system(buffer);

    //load the result in !!
    //char* newfilepath = "3d_obj/normals.obj";
    //load_objfile( newfilepath, pt_model_buffer ); 
}
*/


/**************************************************/

/*
       working - Draw example 

        glPointSize(4);
        
        GLfloat vertices[] =
        {
            1.0f , 3.0f, 1.0f, 1.0f,
            2.0f , 2.0f, 1.0f, 1.0f,
            3.0f , 1.0f, 2.0f, 1.0f
        };

        GLuint VBO;

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        //glEnableVertexAttribArray(0);


        // "Enable a port" to the shader pipeline
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // pass information about how vertex array is composed
        glVertexAttribPointer(0, // same as in glEnableVertexAttribArray(0)
                              4, // # of coordinates that build a vertex
                              GL_FLOAT, // data type
                              GL_FALSE, // normalized?
                              0,        // stride
                              (void*)0);// vbo offset

 
        glDrawArrays(GL_POINTS, 0, 3);
        //glDrawArrays(GL_LINES, 0, 2);
        glDisableVertexAttribArray(0);
*/


/////////////////////////////

/*


//send_pulses


/*
void run_send_pulses(cncglobals* cg,
                 float f_x,
                 float f_y,
                 float f_z,
                 float s_x,
                 float s_y,
                 float s_z,
                 int divs)  
{

    Vector3 s_p = Vector3(f_x , f_y ,f_z );
    Vector3 e_p = Vector3(s_x , s_y ,s_z );

    vector<Vector3> pulsetrain;
    vector<Vector3>* pt_pulsetrain = &pulsetrain; 

    motionplot.calc_3d_pulses(pt_pulsetrain, s_p, e_p, divs);

    if(cg->GLOBAL_DEBUG==true)
    {
        for(int x=0;x<pulsetrain.size();x++)
        {
            std::cout<<pulsetrain[x].x  <<" "<<pulsetrain[x].y  <<" "<<pulsetrain[x].z   << "\n";        
        } 
    }

    if(cg->GLOBAL_DEBUG==false)
    {
       // motionplot.send_pulses(pt_pulsetrain);
    }

 }   
*/


/******************************************/
//command line tool to generate XYZ pulses from 2 vectors 
/*

RELIC FROM THE OLDER TOOL - THIS WILL GO AWAY 

void run_cncplot(cncglobals* cg,
                 float f_x,
                 float f_y,
                 float f_z,
                 float s_x,
                 float s_y,
                 float s_z,
                 int divs)  
{


    cnc_plot plot;
    
    vector<Vector3> pulsetrain;
    vector<Vector3>* pt_pulsetrain = &pulsetrain; 

    Vector3 s_p = Vector3(f_x , f_y ,f_z );
    Vector3 e_p = Vector3(s_x , s_y ,s_z );

    plot.calc_3d_pulses(pt_pulsetrain, s_p, e_p, divs);

    if(cg->GLOBAL_DEBUG==true)
    {
        int x=0;
        for(x=0;x<pulsetrain.size();x++)
        {
            std::cout<<pulsetrain[x].x  <<" "<<pulsetrain[x].y  <<" "<<pulsetrain[x].z   << "\n";        
        } 
    }

    if(cg->GLOBAL_DEBUG==false)
    {
       //moved to IO DEBUG   
       // plot.send_pulses(pt_pulsetrain);
    }

    delete pt_pulsetrain;
    

}

*/




