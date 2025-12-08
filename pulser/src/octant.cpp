
/*************************************************************/
/*
   octant.cpp

   Top level Container for Octant CNC OpenGL application. 



   Copyright (C) 2018-2026 Keith Legg - keithlegg23@gmail.com

   -------------------------------------------------------------------------------------
   Maya coordinate system    |  Blender coordinate system  |    (MOST) CNC/ 3D printers
                             |                             |         
        Y                    |        Z                    |          Z   
        |                    |        |                    |          |
        |                    |        |                    |          | 
        ______X (look on Z)  |        _____X (look on Y)   |          ______ ? (look on ?)  
   -------------------------------------------------------------------------------------
 

*/
/*************************************************************/

#include <iostream>

#include <sstream>
#include <vector>
#include <string>
#include <cmath>
  



#include "gl_setup.h"
#include "point_op.h"
#include "obj_model.h"
#include "cnc_globals.h"
#include "parse_cmds.h"

#include "timer.h"
#include "gl_gui.h"
#include "octant.h"

//#include "timer.h"
//#include "socket.h"





#define LEN(arr) ( (int) (sizeof (arr) / sizeof (arr)[0]) ) 



extern GLfloat emis_red;

/***********/
// mouse related
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


/***********/
// window related 
extern int VIEW_MODE; 

extern int window_id;

extern int scr_size_x;
extern int scr_size_y;
extern bool scr_full_toglr;

// toggle - view prefs - state vars dont change  
bool DRAW_POLYS       = true; // state for toggle command
bool DRAW_GEOM        = true; // state for toggle command

bool draw_cntrgrid    = true;
bool draw_grid        = true;
bool toglr_flatshaded = false;


// single view prefs - use for debugging 
bool draw_points_vbo = false; // test of VBO  
bool draw_points     = true;   
bool draw_lines      = true;
bool draw_normals    = true;
bool draw_quads      = true;
bool draw_triangles  = true;


//bool draw_bbox       = true;


//DEBUG - hilarious effect - if you turn all these off the view floats away quickly
//render_text seems to anchor it 
bool render_text     = true;

int TCP_PORT = 0;

/***************************************/

//test of a timer system 
timer mtime = timer();

/***************************************/
// object related 



extern vector<std::string>  obj_filepaths;

extern char* obj_filepath;
extern int num_drawvec3;
extern int num_drawpoints;
extern int TCP_PORT;

extern vector<Vector3> scene_drawvec3;
extern vector<Vector3> scene_drawvecclr;

extern vector<Vector3> scene_drawpoints;
extern vector<Vector3> scene_drawpointsclr;

extern vector<Vector3>* pt_scene_drawpoints;

extern obj_model* pt_model_buffer;
extern GLuint texture[3];

char active_filepath[300];


//I think this was from a test of the VBO 
int num_pts_drw = 0;
GLfloat vertices[100];

/***************************************/
// data containers 

Vector3 qpos = Vector3(0, 0, 0);

double light_intensity;

float gridsquares = 10;
float gridsize = 5;
float gnomonsize = 1;

float light_posx = 0; 
float light_posy = 3.14;
float light_posz = 0;

int surfce_clr_r; //read from setup.olm 
int surfce_clr_g; 
int surfce_clr_b; 

int line_clr_r; //read from setup.olm 
int line_clr_g; 
int line_clr_b; 


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
//extern GLfloat emis_red[]; //DEBUG WTF - confilct? where?
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

    point_ops PG;

    vector<Vector3> * ptDrawvec = &scene_drawvec3;
    vector<Vector3> * ptDrawClr = &scene_drawvecclr;
    int * ptnum_drawvec3 = &num_drawvec3;
    
    PG.cubic_bezier(ptDrawvec, ptDrawClr,  ptnum_drawvec3, 10, start, ctrl1, ctrl2, end);

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



int q_i, p_i, f_i = 0;


char cs[100];
char s[100];
        

static void render_loop()
{
    // Clear The Screen And The Depth Buffer
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    GLfloat lightpos[] = { light_posx, light_posy, light_posz, 0}; // homogeneous coordinates
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

    
    //------------ 
    // first attempt at animation
    //glTranslatef( sin(mtime.getElapsedTime()), 0, 0);
    //std::cout << mtime.getElapsedTime() << "\n";

    qpos.x = sin(mtime.getElapsedTime());

    //------------ 

    if (render_text)
    {
        if(on_click)
        {
            std::cout << " click! "<< clk_x_coord << " "<< clk_x_coord << "\n";
            on_click=0;
        }

        glBindTexture(GL_TEXTURE_2D, texture[0]); 

        glMaterialfv(GL_FRONT, GL_EMISSION, emis_text);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);

        glColor3d(1.0, 1.0, 1.0);
        setOrthographicProjection();
        //glPushMatrix();
        glLoadIdentity();
        
        //-----------------------------
        // render text in window 
        void *fontsm = GLUT_BITMAP_8_BY_13;     
        void *font   = GLUT_BITMAP_TIMES_ROMAN_24; 

        glutKeyboardFunc(parser_cb);
        


        // command line text 
        glColor3f(0.6f, 1.0f, 0.0f);  //text color 
        renderBitmapString(  20, scr_size_y-20  ,(void *)font,  cmd_buffer.c_str() );
      
        //--
        //X Y Z - QUILL/HEAD POSITION  

        glColor3d(0, 1.0, 1.0);        
        sprintf(cs, "X:%.2f Y:%.2f Z:%.2f", qpos.x, qpos.y, qpos.z );
        //glColor3f(1.0f, 1.0f, 1.0f);  //text color 
        renderBitmapString( ((int)(scr_size_x/2)-100), 50  ,(void *)font, cs );

        //---        
        //sprintf(s, "    %d quads ", pt_model_buffer->num_quads );
        //renderBitmapString( ((int)(scr_size_x/2)-200) , scr_size_y-20  ,(void *)font, s );
        //---
        glColor3d(1.0, 1.0, 1.0);
        sprintf(s, "camera X:%f Y:%f Z:%f", cam_posx, cam_posy, cam_posz);
        renderBitmapString( ((int)(scr_size_x/2)-250), 20  ,(void *)fontsm, s );
        //-----------------------------
        //set color back
        glColor3d(1.0, 1.0, 1.0);

        //glPopMatrix();
        resetPerspectiveProjection();

        glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full);


    }

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

    draw_locator(&qpos, .5);

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
      
        // persistant point buffer   
        // Not tested well! - I think it needs OpenGL4 amd up

        //http://ogldev.atspace.co.uk/www/tutorial02/tutorial02.html 
        //https://stackoverflow.com/questions/28849321/how-to-draw-polygon-with-3d-points-in-modern-opengl
        
        glMaterialfv(GL_FRONT, GL_EMISSION, emis_points);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);

        glPointSize(2);
 
        //-------------------------
        // KEEP THIS CODE - 
        // dump to GLFloats 
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
        //glBindBuffer(GL_ARRAY_BUFFER, VBO);

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

    /******************************************/

    // draw 3D line geometry 
    if (draw_lines)
    {

        glBindTexture(GL_TEXTURE_2D, texture[0]);

        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);
        
        // glEnable(GL_COLOR_MATERIAL);  
        // glColor3f(.5, 0, .5);
            
        for (p_i=0;p_i<pt_model_buffer->num_lines;p_i++)
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
                glColor3f(c1.x, c1.y, c1.z);   
                glVertex3f(pt1.x, pt1.y, pt1.z);

                glColor3f(c2.x, c2.y, c2.z);  
                glVertex3f(pt2.x, pt2.y, pt2.z);

            glEnd();
        }

        glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full);  
    }

    /******************************************/
    // draw the polygon geometry 
    if(draw_triangles)
    {
        //glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full); 

        //glColor3f( pt_linecolor->r, pt_linecolor->g, pt_linecolor->b); 
        glBindTexture(GL_TEXTURE_2D, texture[2]);   // choose the texture to use.
        
        if (toglr_flatshaded){
            glColor3f(1.,1.,1.);
        }
        
        //std::cout << " num tris " << pt_model_buffer->num_tris << "\n";

  
        glBegin(GL_TRIANGLES);  
            for (p_i=0;p_i<pt_model_buffer->num_tris;p_i++)
            { 
              
                // fetch the triangle indices from vertex list
                int tri1 = pt_model_buffer->tris[p_i][0];
                int tri2 = pt_model_buffer->tris[p_i][1];
                int tri3 = pt_model_buffer->tris[p_i][2];

                // //use the same vertex indices to lookup RGB 
                Vector3 rgb1 = pt_model_buffer->vtxrgb[tri1-1];
                Vector3 rgb2 = pt_model_buffer->vtxrgb[tri2-1];
                Vector3 rgb3 = pt_model_buffer->vtxrgb[tri3-1];
                
                //std::cout << "plyidx " << tri1 << " " << tri2 << " " << tri3 << "\n";

                //------------------------------//
                glColor3f(rgb1.x,rgb1.y,rgb1.z);   
                //Vector2 uv = pt_model_buffer->uvs[tri1];
                // glTexCoord2f(uv.x, uv.y);
                glTexCoord2f(0.5, 1.0);                

                Vector3 pt1 = pt_model_buffer->points[tri1-1];
                glVertex3f(pt1.x, pt1.y, pt1.z);
              
                Vector3 nrm1 = pt_model_buffer->vnormals[tri1-1];
                glNormal3f( nrm1.x, nrm1.y, nrm1.z);
                
                //std::cout <<  " nrm1 "<< nrm1.x << " "<< nrm1.y << " "<< nrm1.z << "\n";

                //------------------------------//
                glColor3f(rgb2.x,rgb2.y,rgb2.z); 
                //Vector2 uv = pt_model_buffer->uvs[tri2];
                //glTexCoord2f(uv.x, uv.y);
                glTexCoord2f(0.0, 1.0); 

                Vector3 pt2 = pt_model_buffer->points[tri2-1];
                glVertex3f(pt2.x, pt2.y, pt2.z);

                // calculated face normals 
                Vector3 nrm2 = pt_model_buffer->vnormals[tri2-1];
                glNormal3f( nrm2.x, nrm2.y, nrm2.z);
                
                //std::cout <<  " nrm2 "<< nrm2.x << " "<< nrm2.y << " "<< nrm2.z << "\n";

                //------------------------------//
                glColor3f(rgb3.x,rgb3.y,rgb3.z); 
                //Vector2 uv = pt_model_buffer->uvs[tri3];
                //glTexCoord2f(uv.x, uv.y);
                glTexCoord2f(1.0, 0.0);       

                Vector3 pt3 = pt_model_buffer->points[tri3-1];
                glVertex3f(pt3.x, pt3.y, pt3.z);

                // calculated face normals
                Vector3 nrm3 = pt_model_buffer->vnormals[tri3-1];
                glNormal3f( nrm3.x, nrm3.y, nrm3.z);
                
                //std::cout <<  " nrm3 "<< nrm3.x << " "<< nrm3.y << " "<< nrm3.z << "\n";
            }

        glEnd(); 
          

        Vector3 tri_cntr;

        // display loaded normals as lines 
        if (draw_normals)
        {
            glColor3f(.5,.5,0);

            for (p_i=0;p_i<pt_model_buffer->num_tris;p_i++)
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
                //Vector3 mv =  add(tri_cntr, div(pt_model_buffer->fnormals[p_i], 20 ));
                Vector3 mv = tri_cntr.operator+(pt_model_buffer->fnormals[p_i].operator/=(20));
                //Vector3 mv =  tri_cntr.operator+(pt_model_buffer->fnormals[p_i]);

                //print_vec3( mv );

                glBindTexture(GL_TEXTURE_2D, texture[0]);
                glMaterialfv(GL_FRONT, GL_EMISSION, clr_yellow);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);

                glBegin(GL_LINES);
                    glVertex3f(tri_cntr.x, tri_cntr.y, tri_cntr.z);
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
    Vector3 rgb = Vector3();  

    if (DRAW_GEOM)
    {

        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glMaterialfv(GL_FRONT, GL_EMISSION, emis_full);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);

        //DEBUG vertex color is off until I fix it
        for (p_i=0;p_i<num_drawvec3;p_i++)
        {   
            if(p_i==0)
            {
                sv  = scene_drawvec3[p_i];
                ev  = scene_drawvec3[p_i+1];
                //rgb = scene_drawvecclr[p_i+1];  
            }
            if(p_i>0){ 
                sv  = scene_drawvec3[p_i-1];
                ev  = scene_drawvec3[p_i];
                //rgb = scene_drawvecclr[p_i];            
            }
            glBegin(GL_LINES);
                glColor3f(1.,0,0); //hack for now
                //glColor3f(rgb.x,rgb.y,rgb.z);
                glVertex3f(sv.x, sv.y, sv.z);
                
                glColor3f(1.,0,0); //hack for now
                //glColor3f(rgb.x,rgb.y,rgb.z);
                glVertex3f(ev.x, ev.y, ev.z);
        
            glEnd();
             
        }
        
        glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full); 

    }

    //-----------------------

    //DEBUG - IF THERE IS A SPACE AT END OF FIDS, IT LOADS TRI AS QUAD
    if (draw_quads )
    {
        
        if (toglr_flatshaded){
            glColor3f(1.,1.,1.);
        }

        glBindTexture(GL_TEXTURE_2D, texture[3]);

        glBegin(GL_QUADS);                      

            for (q_i=0;q_i<pt_model_buffer->num_quads;q_i++)
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
     
                /***********************/
                glColor3f(rgb1.x,rgb1.y,rgb1.z);                 
                glTexCoord2f(0.5, 1.0);                
                glNormal3f( nrm1.x, nrm1.y, nrm1.z);
                Vector3 pt1 = pt_model_buffer->points[qu1-1];
                glVertex3f(pt1.x, pt1.y, pt1.z);

                /***********************/
                glColor3f(rgb2.x,rgb2.y,rgb2.z); 
                glTexCoord2f(0.0, 1.0); 
                glNormal3f( nrm2.x, nrm2.y, nrm2.z);
                Vector3 pt2 = pt_model_buffer->points[qu2-1];
                glVertex3f(pt2.x, pt2.y, pt2.z);

                /***********************/
                glColor3f(rgb3.x,rgb3.y,rgb3.z);                 
                glTexCoord2f(1.0, 0.0);   
                glNormal3f( nrm3.x, nrm3.y, nrm3.z);                             
                Vector3 pt3 = pt_model_buffer->points[qu3-1];
                glVertex3f(pt3.x, pt3.y, pt3.z);

                /***********************/
                glColor3f(rgb4.x,rgb4.y,rgb4.z);                 
                glTexCoord2f(1.0, 0.0);      
                glNormal3f( nrm4.x, nrm4.y, nrm4.z);
                Vector3 pt4 = pt_model_buffer->points[qu4-1];
                glVertex3f(pt4.x, pt4.y, pt4.z);


            }
        glEnd(); 


        // display loaded normals as lines 
        if (draw_normals)
        {
            glColor3f(.5,.5,0);
            for (p_i=0;p_i<pt_model_buffer->num_quads;p_i++)
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
        }//draw normals    
    }


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
    float orthzoom = 200; // orbit_dist*200;
    glOrtho( -scr_size_x/orthzoom, scr_size_x/orthzoom, -scr_size_y/orthzoom, scr_size_y/orthzoom, -1, 1 );

    glMatrixMode(GL_MODELVIEW);


    // // set viewport to be the entire window
    // glViewport(0, 0, (GLsizei)scr_size_x, (GLsizei)scr_size_y);
    // // set orthographic viewing frustum
    // glMatrixMode(GL_PROJECTION);
    // glLoadIdentity();
    // glOrtho(0, scr_size_x, 0, scr_size_y, -1, 1);
    // glMatrixMode(GL_MODELVIEW);
    // glLoadIdentity();

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
/*

   // path of text processing 
   start_gui-> render_loop-> (keypress)parser_cb-> parse_cmds-> parse_cmd_text
                                                    |---------> key_cb 
*/

void start_gui(int *argc, char** argv){

    glutInit(argc, argv);  

    //shader_test();
    set_colors();

    mtime.start();

    //------------
    //load CNC cfg (including paths to .obj files) 
    cncglobals cg;
    cg.load_cfg_file(argv[1]);
    //load any optional 3d models needed for setup
    cg.load_objects();
    //cg.show_obj();
    
    //cg.show();

   
    //------------

    // Vector3 start = newvec3(0.0 ,3.0 ,1.0 );
    // Vector3 ctrl1 = newvec3(2.5  ,0.0 ,0.0 );
    // Vector3 ctrl2 = newvec3(0.0 ,1.0  ,0.0 );
    // Vector3 end   = newvec3(-1.0 ,0.0 ,-5.0 );
    // test_bezier(start, ctrl1, ctrl2, end);
    
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

    //old keyboard callback - replaced with fancier parser
    //glutKeyboardFunc(&keyPressed);     // register key pressed callback 

    InitGL(scr_size_x, scr_size_y); // Initialize window. 
  
    
    //---
    //glutMouseFunc(mouse_clk_cb);

    // experimental draw polygon 
    //glutMouseFunc (draw_poly_mousevent);

    glutMouseFunc (octant_mouse_button);
    glutMotionFunc (octant_mouse_motion);
    //---

    //loadImage("textures/generated2.bmp" , imageloaded_bfr);
    //loadImage("textures/generated2.bmp" , imageloaded_bfr2);
    
    ///////////
    // create and apply 2D texture   
    glGenTextures(4, &texture[0]);            //create 3 textures

    //grid color
    // glBindTexture(GL_TEXTURE_2D, texture[0]);  
    // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); // scale linearly when image bigger than texture
    // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // scale linearly when image smalled than texture
    
    //glTexImage2D(GL_TEXTURE_2D, 0, 3, imageloaded_bfr->sizeX, imageloaded_bfr->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, imageloaded_bfr2->data);


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

    ///////////
    
    //tb.start(); //test of timer 

    glutMainLoop();// Start Event Processing Engine   
   

}





/**************************************************/
/**************************************************/



/*
    // a - frame all objects 
    // q - select tool         
    // w - move 
    // e - rotate 
    // r - scale 
    // f - frame selected 
    // t - show manipulator 
    // p - parent 
    // shft p - unparent 
*/

void setlight0(void){
    // FROM - https://www.khronos.org/opengl/wiki/How_lighting_works#glMaterial_and_glLight 
    // FROM - https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glLight.xml
    // FROM - https://www.cse.msu.edu/~cse872/tutorial3.html


    /* 
        GL_AMBIENT, GL_DIFFUSE, GL_SPECULAR, GL_POSITION, GL_SPOT_CUTOFF, GL_SPOT_DIRECTION, 
        GL_SPOT_EXPONENT, GL_CONSTANT_ATTENUATION, GL_LINEAR_ATTENUATION,  GL_QUADRATIC_ATTENUATION 
    */

    // //GLfloat lightpos[] = {1, light_posx, light_posy, light_posz}; // homogeneous coordinates
    // GLfloat lightpos[] = {0, 1, 0, 0}; // homogeneous coordinates
    // glLightfv(GL_LIGHT0, GL_POSITION, lightpos);


    // Set GL_LIGHT_0's Ambient color to 0,0,0,1
    GLfloat lightamb[] = {0., 0., 0., 1. };
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);

    // Set GL_LIGHT_0's Diffuse color to 1,1,1,1
    GLfloat lightdif[] = {1., 1., 1., 1. };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightdif);

    // Set GL_LIGHT_0's Specular color to 1,1,1,1
    GLfloat lightspec[] = {1., 1., 1., 1. };
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightspec);


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


    //  // GLfloat matdiff[] = {1.f, 1.f, 1.f, 1.f};
    //  // glMaterialfv(GL_FRONT, GL_DIFFUSE, matdiff);


}



//define keyboard events 
void key_cb(unsigned int key) 
{

    /*
    if (key == 83) //shift s key - open socket  
    { 
        printf("opening socket on port %i \n", TCP_PORT); 
        //sockettest3();
    }*/

    //ESCAPE KEY
    if (key == 27) 
    { 
        glutDestroyWindow(window_id); 
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

    if (key == 52) //4 - display as wire 
    { 
        glDisable(GL_TEXTURE_2D);        
        glDisable(GL_LIGHTING);
        glPolygonMode (GL_FRONT_AND_BACK,  GL_LINE);

    }

    if (key == 53) //5 - display as solid, no texture  
    { 

        setlight0();

        // glShadeModel(GL_SMOOTH);              
        glDisable(GL_TEXTURE_2D);

        //------------------------

        //glDisable(GL_COLOR_MATERIAL);

        // glDisable(GL_LIGHT0);
        // glDisable(GL_LIGHT1);
        // glDisable(GL_LIGHT2);
        // glDisable(GL_LIGHT3);
        // glDisable(GL_LIGHT4);
           
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

    //------

    if (key == 45) // minus
    { 
        orbit_dist-=.1;  
    }
    
    if (key == 61) //plus
    { 
        orbit_dist+=.1;  
    }


    //------
    if (key == 73) //shift i - show obj info 
    { 

        //get_obj_info( pt_model_buffer, pt_obinfo);
        //show_obj_geom(pt_model_buffer);
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
            pt_model_buffer->calc_normals();

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

