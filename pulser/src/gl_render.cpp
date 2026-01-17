/*************************************************************/
/*
   gl_render.cpp

   This contains the "render_loop" for the graphic display in OpenGL 

  
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
/*************************************************************/


 
 

#include "timer.h"
#include "point_op.h"
#include "obj_model.h"

#include "globals.h"
#include "parport.h"
#include "plot.h"

//this file should not be called anyway if no GUI, but whatever
#if DO_BUILD_GUI == true
    #include "gl_setup.h"
    #include "gl_gui.h"
    #include "gl_render.h"
    #include "gl_cmds.h"
#endif

//semaphore mouse click 
int on_click;

//mouse single click
int clk_x_coord;
int clk_y_coord;

//mouse drag
int mouseX = 0;
int mouseY = 0;


bool view_ismoving  = false;
bool mouseLeftDown  = false;
bool mouseRightDown = false;


/***************************************/

// toggle - view prefs - state vars dont change  
bool DRAW_POLYS       = true; // state for toggle command
bool DRAW_GEOM        = true; // state for toggle command
bool draw_cntrgrid    = true;
bool tog_grid         = true; 
bool tog_vtxrgb       = false; 
bool tog_testport     = false; 

bool toglr_flatshaded = true; //DEBUG 

/***************************************/
// single view prefs - use for debugging 

bool draw_lines      = false;
bool draw_normals    = false;
bool draw_quads      = false;
bool draw_triangles  = true;
bool render_text     = true;


//DEBUG - figure the VBO crap out 
bool draw_points     = true;  
bool draw_points_vbo = false; 

//bool draw_bbox       = true;

bool disp_ply_solo            = false;
uint disp_ply_solo_id = 0;

/***************************************/
//I think this was from a test of the VBO 
GLfloat vertices[100];



extern GLfloat clr_linez[]   ;
extern GLfloat emis_full[]   ;
extern GLfloat emis_half[]   ;
extern GLfloat emis_text[]   ;
extern GLfloat emis_points[] ;
extern GLfloat emis_off[]    ;
extern GLfloat emis_teal[]   ;
extern GLfloat emis_red[]    ;
extern GLfloat emis_green[]  ;
extern GLfloat emis_blue[]   ;
extern GLfloat emis_lines[]  ;
extern GLfloat clr_yellow[]  ;
extern GLfloat clr_green[]   ;
extern GLfloat clr_blue[]    ;


extern GLuint texture[3];


int line_clr_r; //read from setup.olm 
int line_clr_g; 
int line_clr_b; 
int num_pts_drw;


int VIEW_MODE = -1; 

float orbit_x;         
float orbit_y;   
float orbit_dist = 5.0; // Z zoom 

float cam_posx = 0; // camera location
float cam_posy = 0;
float cam_posz = 0;


float coefficient         = 0.005f;  
float mouse_orbit_speed   = 2.1f;



/***************************************/

char cs[100];
char s[100];


//extern timer mtime;
extern timer* pt_mtime;

extern point_ops PG;


extern cnc_plot motionplot;
extern cnc_plot* pt_motionplot;

extern cnc_parport parport;

extern cncglobals cg;

//quadrature experiment 
unsigned char portdata;



unsigned char am = 0b10000000;
unsigned char bm = 0b01000000;
int step = 0;
int dir = 0;
bool stale = true;


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

/***************************************/



/***************************************/
// Window related 

int scr_size_x      = 512;
int scr_size_y      = 512;
bool scr_full_toglr = true;
int window_id;      


extern int VIEW_MODE; 



GLuint texture[3]; // storage for textures


/***************************************/
//display 3D points and color 
extern std::vector<Vector3> scene_drawpoints;
extern std::vector<Vector3> scene_drawpointsclr;
//extern std::vector<Vector3>* pt_scene_drawpoints;

extern obj_model* pt_model_buffer;

extern std::vector<Vector3> linebuffer1; 
extern std::vector<Vector3> linebuffer1_rgb; 
extern std::vector<Vector3> linebuffer2; 
extern std::vector<Vector3> linebuffer2_rgb; 



extern RGBType *pt_linecolor;
extern RGBType *pt_gridcolor;
extern RGBType *pt_gridcolor2;


/***************************************/
//color palette 

//wireframe colors
#define pt_clr1 glColor3f(1,.3,.6)
#define pt_clr2 glColor3f(1,.3,.6)
#define pt_clr3 glColor3f(1,.3,.6)

//face normals
#define nrml_clr glColor3f(1,.3,.6)

//quill locator color
#define loc_idle_clr glColor3f(1f,.3f,.6f)
#define loc_active_clr glColor3f(1f,.3f,.6f)

//text color
#define text_clr glColor3f(0.6f, 1.0f, 0.0f)

#define green_clr glColor3f(0.0f, 1.0f, 0.0f)
#define red_clr glColor3f(1.0f, 0.0f, 0.0f)


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
//DEBUG TRIGGERS THE "OLD SEGFAULT "
void warnings(void)
{   
    
    // let us know if there is a discernible problem 
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

 


/***************************************/
/***************************************/
/********************************************/

void octant_mouse_button(int button, int state, int x, int y)
{

    mouseX = x;
    mouseY = y;

    //-----------
    //left click 
    if (button == GLUT_MIDDLE_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            std::cout << " middle click " << "\n"; 
            std::cout << " x:" <<x <<" y: "<<y << "\n"; 
        }//middle pressed

    }//middle button

    //-----------
    //left click 
    if (button == GLUT_LEFT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseLeftDown = true;
             
            on_click = true; //semaphore mouse click
            clk_x_coord = x; //capture single click
            clk_y_coord = y; //capture single click 

        }
        else if(state == GLUT_UP)
            mouseLeftDown = false;

    }

    //-----------
    // middle click
    if ((button == 3) || (button == 4)) // It's a wheel event
    {
        // Disregard redundant GLUT_UP events
        if (state == GLUT_UP) return; 

        if (button == 3)
        {
            //if (orbit_dist < -1.5){
            orbit_dist+=.1;  
            //printf("# orbit dist %f \n", orbit_dist );                                 
            //}
        }
        if (button == 4){
            //if (orbit_dist>0){ 
                orbit_dist-=.1; 
            //}
        }

        //std::cout << " zooming " << orbit_dist << "\n";

    }else
    {   
        // normal button event (not wheel)
        if (state == GLUT_DOWN)
        {


        }
    } 

    //-----------
    //Right click
    if (button == GLUT_RIGHT_BUTTON)
      {
        
        if(state == GLUT_DOWN)
        {
            mouseRightDown = true;
        }
        else if(state == GLUT_UP)
            mouseRightDown = false;

      }

}



/********************************************/

void octant_mouse_motion(int x, int y)
{
    // take offset from center of screen to get "X,Y delta"
    float center_y = (float)scr_size_y/2;
    float center_x = (float)scr_size_x/2;


    if(mouseLeftDown)
    { 

        switch (VIEW_MODE) 
        { 

            // orthographic side  (key 2)
            case 1: 
                view_ismoving = true;
                cam_posz = -(center_x-x) * coefficient; 
                cam_posy = -(center_y-y) * coefficient; 
            break; 
        
            // orthographic top   (key shift 2)
            case 2:  
                view_ismoving = true;
                cam_posx = (center_x-x) * coefficient; 
                cam_posz = (center_y-y) * coefficient;  
            break; 
        
            // orthographic front  (key 3)
            case 3:  
                view_ismoving = true;
                cam_posx = (center_x-x)  * coefficient; 
                cam_posy = -(center_y-y) * coefficient; 
            break; 
        

            default:  
                    view_ismoving = true;
                    
                    orbit_x += (x-mouseX) * coefficient; 
                    orbit_y += (y-mouseY) * coefficient; 
                    mouseX = x;
                    mouseY = y;
                    //printf("# mouse motion %d %d %f %f \n", x,y, orbit_x, orbit_y );
            break;
        }
    }

    if(mouseRightDown)
    {
        orbit_dist -= (y - mouseY) * 0.02f;
        mouseY = y;
    }

    /**************/
   

     

}



/***************************************/
/***************************************/


/*
  GLfloat light0_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    // Plane wave coming from +z infinity. 
    GLfloat light0_position[] = {0.0, 0.0, 1.0, 0.0};
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
*/    

/***************************************/
void setlight0(void)
{
    // FROM - https://www.khronos.org/opengl/wiki/How_lighting_works#glMaterial_and_glLight 
    // FROM - https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glLight.xml
    // FROM - https://www.cse.msu.edu/~cse872/tutorial3.html


    /* 
        GL_AMBIENT, GL_DIFFUSE, GL_SPECULAR, GL_POSITION, GL_SPOT_CUTOFF, GL_SPOT_DIRECTION, 
        GL_SPOT_EXPONENT, GL_CONSTANT_ATTENUATION, GL_LINEAR_ATTENUATION,  GL_QUADRATIC_ATTENUATION 
    */
     
    // //GLfloat lightpos[] = {1, light_posx, light_posy, light_posz}; // homogeneous coordinates
    // //GLfloat lightpos[] = {0, 0, 0, 1}; // homogeneous coordinates
    // //glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
    // // Set GL_LIGHT_0's Ambient color to 0,0,0,1
    // GLfloat lightamb[] = {0., 0., 0., 1. };
    // glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);
    // // Set GL_LIGHT_0's Diffuse color to 1,1,1,1
    // GLfloat lightdif[] = {10, 1., 1., 1. };
    // glLightfv(GL_LIGHT0, GL_DIFFUSE, lightdif);
    // // Set GL_LIGHT_0's Specular color to 1,1,1,1
    // GLfloat lightspec[] = {1., 1., 1., 1. };
    // glLightfv(GL_LIGHT0, GL_SPECULAR, lightspec);

    GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    //GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    //glLightfv(GL_LIGHT0, GL_POSITION, light_position);


    // Set the glLightModel global ambient to 0.2,0.2,0.2,1 (this is the default).
    // Don't set any other glLight or glLightModel options - just let them default.

    // Enable GL_LIGHTING and GL_LIGHT_0.
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Enable GL_COLOR_MATERIAL and set glColorMaterial to GL_AMBIENT_AND_DIFFUSE. 
    // This means that glMaterial will control the polygon's specular and emission colours 
    // and the ambient and diffuse will both be set using glColor.
    
    // glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    // glEnable(GL_COLOR_MATERIAL);  

    // Set the glMaterial Specular colour to 1,1,1,1
    // GLfloat matspec[] = {1.f, 1.f, 1.f, 1.f};
    // glMaterialfv(GL_FRONT, GL_SPECULAR, matspec);

    // GLfloat matemis[] = {1.f, 0.f, 0.f, 0.f};
    // glMaterialfv(GL_FRONT, GL_EMISSION, matemis);

    // Set the glMaterial Emission colour to 0,0,0,1
    // Set the glColor to whatever colour you want each polygon to basically appear to be. 
    // That sets the Ambient and Diffuse to the same value - which is what you generally want.
    
    //glEnable(GL_NORMALIZE);


    //  // GLfloat matdiff[] = {1.f, 1.f, 1.f, 1.f};
    //  // glMaterialfv(GL_FRONT, GL_DIFFUSE, matdiff);
     

    /*
    GLfloat light0_position[] = {0.0, 0.0, 1.0, 0.0};
    GLfloat light0_diffuse[] = {0.0, 1.0, 1.0, 0.0};

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    */

}


/***************************************/

void reshape_window(int width, int height)
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
void render_normals(obj_model* pt_model_buffer)
{

    nrml_clr;
    for (int p_i=0;p_i<pt_model_buffer->num_quads;p_i++)
    {             
        // fetch the pts for a triangle
        Vector3 p1 = pt_model_buffer->points[pt_model_buffer->quads[p_i][0]-1];
        Vector3 p2 = pt_model_buffer->points[pt_model_buffer->quads[p_i][1]-1];
        Vector3 p3 = pt_model_buffer->points[pt_model_buffer->quads[p_i][2]-1];
        
        // calculate the centroid 
        Vector3 quad_cntr;
        quad_cntr.x = (p1.x + p2.x + p3.x)/3;
        quad_cntr.y = (p1.y + p2.y + p3.y)/3;
        quad_cntr.z = (p1.z + p2.z + p3.z)/3; 
        
        //display shorter for neatness  
        //Vector3 mv =  add(quad_cntr, div(pt_model_buffer->fnormals[p_i], 20 ));
        Vector3 mv =  quad_cntr.operator+(pt_model_buffer->fnormals[p_i].operator/=(20));

        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glMaterialfv(GL_FRONT, GL_EMISSION, clr_yellow);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);
        
        glBegin(GL_LINES);
            glVertex3f(quad_cntr.x, quad_cntr.y, quad_cntr.z);
            glVertex3f(mv.x, mv.y, mv.z);
        glEnd();

        glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full);  
    }

}

/***************************************/

void render_quads(obj_model* pt_model_buffer)
{
    // if (toglr_flatshaded){
    //     glColor3f(1.,1.,1.);
    // }
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);                      
    for (int q_i=0;q_i<pt_model_buffer->num_quads;q_i++)
    { 

        int qu1 = pt_model_buffer->quads[q_i][0];
        int qu2 = pt_model_buffer->quads[q_i][1];
        int qu3 = pt_model_buffer->quads[q_i][2];
        int qu4 = pt_model_buffer->quads[q_i][3];

        //DEBUG VTX COLORS ARE BROKEN - INDEXING ISSUES 
        Vector3 rgb1 = pt_model_buffer->vtxrgb[qu1-1];
        Vector3 rgb2 = pt_model_buffer->vtxrgb[qu2-1];
        Vector3 rgb3 = pt_model_buffer->vtxrgb[qu3-1];
        Vector3 rgb4 = pt_model_buffer->vtxrgb[qu4-1];

        //DEBUG - not working or tested  
        Vector3 nrm1 = pt_model_buffer->vnormals[qu1-1];
        Vector3 nrm2 = pt_model_buffer->vnormals[qu2-1];
        Vector3 nrm3 = pt_model_buffer->vnormals[qu3-1];
        Vector3 nrm4 = pt_model_buffer->vnormals[qu4-1];

        //---------------------------//
        glColor3f(rgb1.x,rgb1.y,rgb1.z);                 
        glTexCoord2f(0.5, 1.0);                
        glNormal3f( nrm1.x, nrm1.y, nrm1.z);
        Vector3 pt1 = pt_model_buffer->points[qu1-1];
        glVertex3f(pt1.x, pt1.y, pt1.z);

        //---------------------------//
        glColor3f(rgb2.x,rgb2.y,rgb2.z); 
        glTexCoord2f(0.0, 1.0); 
        glNormal3f( nrm2.x, nrm2.y, nrm2.z);
        Vector3 pt2 = pt_model_buffer->points[qu2-1];
        glVertex3f(pt2.x, pt2.y, pt2.z);

        //---------------------------//
        glColor3f(rgb3.x,rgb3.y,rgb3.z);                 
        glTexCoord2f(1.0, 0.0);   
        glNormal3f( nrm3.x, nrm3.y, nrm3.z);                             
        Vector3 pt3 = pt_model_buffer->points[qu3-1];
        glVertex3f(pt3.x, pt3.y, pt3.z);

        //---------------------------//
        glColor3f(rgb4.x,rgb4.y,rgb4.z);                 
        glTexCoord2f(1.0, 0.0);      
        glNormal3f( nrm4.x, nrm4.y, nrm4.z);
        Vector3 pt4 = pt_model_buffer->points[qu4-1];
        glVertex3f(pt4.x, pt4.y, pt4.z);
    }
    glEnd(); 
 
}

/***************************************/

void render_vbo(obj_model* pt_model_buffer)
{


    // https://www.opengl-tutorial.org/beginners-tutorials/tutorial-8-basic-shading/#vertex-normals

    // persistant point buffer   
    // Not tested well! - I think it needs OpenGL4 amd up

    //https://ogldev.org/www/tutorial02/tutorial02.html
    //https://stackoverflow.com/questions/28849321/how-to-draw-polygon-with-3d-points-in-modern-opengl
    
    glMaterialfv(GL_FRONT, GL_EMISSION, emis_points);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);

    glPointSize(2);

    //-------------------------
    num_pts_drw = pt_model_buffer->num_pts;

    GLfloat vertices[num_pts_drw*4];
    GLfloat* pt_vert = vertices;
    dump_points_GLfloat( pt_vert, pt_model_buffer, num_pts_drw );
    
    // add in custom points loaded from scene.olm  
    //dump_points_GLfloat( pt_vert, pt_scene_drawpoints, num_drawpoints );

    //-------------------------
    GLuint VBO;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //glEnableVertexAttribArray(0);


    // "Enable a port" to the shader pipeline
    glEnableVertexAttribArray(0);
    
    //DEBUG COMMENTED OUT
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // pass information about how vertex array is composed
    glVertexAttribPointer(0, // same as in glEnableVertexAttribArray(0)
                          4, // # of coordinates that build a vertex
                          GL_FLOAT, // data type
                          GL_FALSE, // normalized?
                          0,        // stride
                          (void*)0);// vbo offset


    glDrawArrays(GL_POINTS, 0, num_pts_drw);
    //glDrawArrays(GL_LINES, 0, num_pts_drw);
    glDisableVertexAttribArray(0);
    
    glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full);
}



/***************************************/



void render_lines(obj_model* pt_model_buffer)
{
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);
        
    for (int p_i=0;p_i<pt_model_buffer->num_lines;p_i++)
    {   
        glBegin(GL_LINES);
            // fetch the line indices from vertex list 
            int lin1 = pt_model_buffer->lines[p_i][0];
            int lin2 = pt_model_buffer->lines[p_i][1];
            
            Vector3 pt1 = pt_model_buffer->points[lin1-1];
            Vector3 pt2 = pt_model_buffer->points[lin2-1];

            //use the same vertex indices to lookup RGB 
            Vector3 c1 = pt_model_buffer->vtxrgb[lin1-1];
            Vector3 c2 = pt_model_buffer->vtxrgb[lin2-1];

            if(c1.x==0){c1.x=line_clr_r;c2.x=line_clr_r;}
            if(c1.y==0){c1.y=line_clr_g;c2.y=line_clr_g;}
            if(c1.z==0){c1.z=line_clr_b;c2.z=line_clr_b;}

            //std::cout << "line color is set to " << c1.x <<" "<< c1.y<< " " << c1.z << "\n";
            glMaterialfv(GL_FRONT, GL_EMISSION, clr_linez);

            //point 1 
            if (tog_vtxrgb)
            {
                glColor3f(c1.x, c1.y, c1.z); 
            }else{
                pt_clr1;
            };
            glVertex3f(pt1.x, pt1.y, pt1.z);

            //point 2 
            if (tog_vtxrgb)
            {
                glColor3f(c1.x, c1.y, c1.z); 
            }else{
                pt_clr2;
            };
            glVertex3f(pt2.x, pt2.y, pt2.z);

        glEnd();
    }

    glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full);  
}



/***************************************/
void animate(void)
{
    //ticker += .01;
    render_loop();
}



/***************************************/

std::string cmd_buffer;

void parser_cb(unsigned char key, int x, int y)
{
    parse_cmds(&cmd_buffer, &key);
    glutPostRedisplay();
};


/***************************************/
//placeholder for future feature we havent invented yet
float dummy = 0;
uint top_text_y = 20;

void render_loop(void)
{
    // Clear The Screen And The Depth Buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //set light position 
    GLfloat lightpos[] = { light_posx, light_posy, light_posz, 0}; // homogeneous coordinates
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

    glutKeyboardFunc(parser_cb);

    //------------ 
 
    if(pt_mtime->tm_running)
    {

        //I tried to make this sepereate from the GUI but then we need threads. Sigh. 
        //this seems like a good compromise
        pt_motionplot->update_sim();
            
        glColor3d(1, .4, 1);
        draw_locator(&motionplot.quill_pos, .5);

    }//end program cycle running  
    
   

    //------------ 
    //draw locator when idle 
    if(!pt_mtime->tm_running)
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
            #if DEBUG_MOUSE_CLICK == true 
                //active but does not do anything 
                //maybe look at camera pos and rot and draw a vector for intersection with triangle?

                std::cout << " click! "<< clk_x_coord << " "<< clk_x_coord << "\n";
            #endif 

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
        text_clr;
        renderBitmapString(  20, scr_size_y-20  ,(void *)font,  cmd_buffer.c_str() );
      
        //--
        //X Y Z - QUILL/HEAD POSITION  
             
                
        green_clr;        
        sprintf(cs, "X:%.2f Y:%.2f Z:%.2f", motionplot.quill_pos.x, motionplot.quill_pos.y, motionplot.quill_pos.z );
        renderBitmapString( ((int)(scr_size_x/2)-100), 50  ,(void *)font, cs );
         
        if (tog_testport)
        {
             
            //get the byte from IO
            parport.test_inputs(&cg, &portdata);

            //unsigned char   // pin10 - 0b10000000 - 0x80
            //unsigned char   // pin11 - 0b01000000 - 0x40
            //unsigned char   // pin12 - 0b00100000 - 0x20
            //unsigned char   // pin13 - 0b00010000 - 0x10
            //unsigned char   // pin15 - 0b00001000 - 0x08

            /*                
            parport.decode_quadrature(&cg, &step, &dir, &am, &bm, &stale);
            if(!stale){
                std::cout <<  " step  " << step << " dir " << dir << "\n";
                if(dir)
                {
                    animate_locator.x += step;
                }else{
                    animate_locator.x -= step;                    
                }
            }           
            */
             
            glColor3d(0, 1.0, 0);        
            sprintf(cs, BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(portdata)  );
            renderBitmapString( ((int)(scr_size_x/2)-50), 75  ,(void *)GLUT_BITMAP_HELVETICA_18, cs );
        }


        //---        
        //sprintf(s, "    %d quads ", pt_model_buffer->num_quads );
        //renderBitmapString( ((int)(scr_size_x/2)-200) , scr_size_y-top_text_y  ,(void *)font, s );
        
        
        //-----------------------------------------

        if(!cg.ENABLE_MOTOR_DRIVE)
        {
            red_clr;
            renderBitmapString( ((int)(scr_size_x/2)-50) , top_text_y  ,(void *)font, "DISABLED" ); 
        }
        else
        {
            green_clr;
            renderBitmapString( ((int)(scr_size_x/2)-50) , top_text_y  ,(void *)font, "ENABLED" ); 
        }

        //-----------------------------------------
        //DEBUG USING THE TIMER AS INDICATOR OF MACHINE RUNNING 
        //PROBABLY NOT WHAT YOU WANT - CONSIDER THREADS AND A MORE COMPLEX SEMAPHORE 
        if(!pt_mtime->tm_running)
        {
            red_clr;
            renderBitmapString( ((int)(scr_size_x/2)-200) , top_text_y  ,(void *)font, "ESTOP" ); 
        }
        else
        {
            green_clr;
            renderBitmapString( ((int)(scr_size_x/2)-200) , top_text_y  ,(void *)font, "ESTOP" ); 
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
    graticulate(&tog_grid, &draw_cntrgrid, pt_gridcolor, pt_gridcolor2);

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

        if (toglr_flatshaded)
        {
            glColor3f(1.,1.,1.);
        }
        
        //std::cout << " num tris " << pt_model_buffer->num_tris << "\n";

  
        glBegin(GL_TRIANGLES);  
            for (uint p_i=0;p_i<pt_model_buffer->num_tris;p_i++)
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
                if (tog_vtxrgb)
                {
                    glColor3f(rgb1.x,rgb1.y,rgb1.z); 
                }else{
                    glColor3f(.3,.3,.6);
                };

                //Vector2 uv = pt_model_buffer->uvs[tri1];
                // glTexCoord2f(uv.x, uv.y);
                glTexCoord2f(0.5, 1.0);                
              
                Vector3 nrm1 = pt_model_buffer->vnormals[vn1-1];
                glNormal3f( nrm1.x, nrm1.y, nrm1.z);
                //std::cout <<  " vtxnrm1 "<< nrm1.x << " "<< nrm1.y << " "<< nrm1.z << "\n";
                
                Vector3 pt1 = pt_model_buffer->points[tri1-1];
                glVertex3f(pt1.x, pt1.y, pt1.z);

                //------------------------------//
                if (tog_vtxrgb)
                {                
                    glColor3f(rgb2.x,rgb2.y,rgb2.z); 
                }else{
                    glColor3f(1,.3,.6);
                };

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

                if (tog_vtxrgb)
                {
                    glColor3f(rgb3.x,rgb3.y,rgb3.z); 
                }else{
                    //glColor3f(.3,.3,.6);
                };                
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
        for (uint p_i=1;p_i<linebuffer1.size();p_i++)
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
       

        // disp_ply_solo  
        // disp_ply_solo_id = pidx;
        if (disp_ply_solo)
        {
  
            for (uint ii=1;ii<pt_motionplot->tp_idxs[disp_ply_solo_id].size();ii++)
            {
                glBegin(GL_LINES);
                    uint si= pt_motionplot->tp_idxs[disp_ply_solo_id][ii-1];
                    uint ei= pt_motionplot->tp_idxs[disp_ply_solo_id][ii];
                    sv  = pt_motionplot->program_vecs[si];
                    ev  = pt_motionplot->program_vecs[ei];
                    glColor3f(0,1.,0); //hack for now
                    glVertex3f(sv.x, sv.y, sv.z);
                    glColor3f(1.,0,0); //hack for now
                    glVertex3f(ev.x, ev.y, ev.z);
                glEnd();
           }//iterate all ids in single polygon
                   
        }//single poly display 
        else
        {
            for (uint p_i=0;p_i<pt_motionplot->num_plys;p_i++)
            {   
                for (uint ii=1;ii<pt_motionplot->tp_idxs[p_i].size();ii++)
                {
                    glBegin(GL_LINES);
                        uint si= pt_motionplot->tp_idxs[p_i][ii-1];
                        uint ei= pt_motionplot->tp_idxs[p_i][ii];
                        sv  = pt_motionplot->program_vecs[si];
                        ev  = pt_motionplot->program_vecs[ei];
                        //rgb = linebuffer1_rgb[p_i];            
                        glColor3f(0,1.,0); //hack for now
                        glVertex3f(sv.x, sv.y, sv.z);
                        glColor3f(1.,0,0); //hack for now
                        glVertex3f(ev.x, ev.y, ev.z);
                    glEnd();
               }//iterate all ids in each polygon
            }//iterate all polygons
           
        }//display all polygons 
     
        //////////////////////////////////////////////////////////
        
        //std::cout << "foo " << pt_motionplot->tp_idxs[0].size() << "\n";

        /*
        int test = 1;

        if (pt_motionplot->tp_idxs[test].size()>1)
        {
            for (uint ii=1;ii<pt_motionplot->tp_idxs[test].size();ii++)
            {
                glBegin(GL_LINES);
                    uint si= pt_motionplot->tp_idxs[test][ii-1];
                    uint ei= pt_motionplot->tp_idxs[test][ii];

                    sv  = pt_motionplot->program_vecs[si];
                    ev  = pt_motionplot->program_vecs[ei];
      
                
                    glColor3f(0,1.,0); //hack for now
                    glVertex3f(sv.x, sv.y, sv.z);
                    
                    glColor3f(1.,0,0); //hack for now
                    glVertex3f(ev.x, ev.y, ev.z);

                glEnd();
            }
        }  
        */ 
        //////////////////////////////////////////////////////////

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
        for (uint p_i=1;p_i<linebuffer2.size();p_i++)
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





/**************************************************/
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


//define keyboard events 
void key_cb(uint key) 
{

    //9 - toggle vertex color
    if (key == 56) 
    { 
        if (tog_vtxrgb == true){
            tog_vtxrgb = false;
        }else{
            tog_vtxrgb = true;
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

    //4 - display as wire 
    if (key == 52) 
    { 
        glDisable(GL_TEXTURE_2D);        
        glDisable(GL_LIGHTING);
        glPolygonMode (GL_FRONT_AND_BACK,  GL_LINE);

    }

    //5 - display as solid, no texture 
    if (key == 53)  
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
        if (tog_grid == true){
            tog_grid = false;
        }else{
            tog_grid = true;
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



}


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


