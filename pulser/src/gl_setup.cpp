/*************************************************************/
/*
   gl_setup.cpp
    
       - common home for an openGL application 
       - OpenGL utilities for drawing geometry and managing a display window

  
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
 
 

#include <iostream>

#include "obj_model.h"
#include "gl_setup.h"


extern float gridsquares;
extern float gridsize;
extern float gnomonsize;

extern std::vector<Vector3> linebuffer1; 
extern std::vector<Vector3> linebuffer1_rgb; 
extern std::vector<Vector3> linebuffer2; 
extern std::vector<Vector3> linebuffer2_rgb; 

extern GLuint texture[3];




//toolpath vectors
std::vector<Vector3> linebuffer1;
std::vector<Vector3> linebuffer1_rgb;
//rapid move vectors
std::vector<Vector3> linebuffer2;
std::vector<Vector3> linebuffer2_rgb;


//debug - consider not using this old data type and use 3 ints in a vector?
RGBType line_color;
RGBType *pt_linecolor = &line_color;

RGBType grid_color;
RGBType *pt_gridcolor = &grid_color;

RGBType grid_color2;
RGBType *pt_gridcolor2 = &grid_color2;




GLfloat clr_linez[]   = { 0 , 1. ,  0, 0};
GLfloat emis_full[]   = { 1 , 1  ,  1, 0};
GLfloat emis_half[]   = { .5, .5 , .5, 0};
GLfloat emis_text[]   = { .8, .8 , .9, 0};
GLfloat emis_points[] = { 0 , .6 , .2, 0};
GLfloat emis_off[]    = { 0 , 0  ,  0, 0};
GLfloat emis_teal[]   = { 0 , 1. , 1., 0};
GLfloat emis_red[]    = { 1., 0  ,  0, 0};
GLfloat emis_purple[] = { 1., 0  , 1., 0};
GLfloat emis_green[]  = { 0 , 1. ,  0, 0};
GLfloat emis_blue[]   = { 0 , 0  , 1., 0};
GLfloat emis_lines[]  = { .5, 0  , .5, 0};
GLfloat clr_yellow[]  = { 1., 1. ,  0, 0};
GLfloat clr_green[]   = { 0 , 1. ,  0, 0};
GLfloat clr_blue[]    = { 0 , 0  , 1., 0};


//GLfloat loc_active[]   = { 1, .4, 1, 0};
//GLfloat loc_idle[]    = { 1, .4, 1, 0};

// quaternion frank = quaternion_identity();




/***************************************/

void set_colors(void)
{
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


/******************************************/
//void m44_to_glm44( m44* pt_m44, GLfloat m44_glfloat[16] ){}

void glutm44_to_m44( Matrix4* pt_m44, GLfloat m44_glfloat[16] ){

    //Note the Matrix4 class uses column major order.
    //My old library m44 uses row major order.

    /*
    //ROW MAJOR for (Keiths old m44 Class)
    pt_m44->m0  = m44_glfloat[0];
    pt_m44->m1  = m44_glfloat[1];
    pt_m44->m2  = m44_glfloat[2];
    pt_m44->m3  = m44_glfloat[3];

    pt_m44->m4  = m44_glfloat[4];
    pt_m44->m5  = m44_glfloat[5];
    pt_m44->m6  = m44_glfloat[6];
    pt_m44->m7  = m44_glfloat[7];
    
    pt_m44->m8  = m44_glfloat[8];
    pt_m44->m9  = m44_glfloat[9];
    pt_m44->m10 = m44_glfloat[10];
    pt_m44->m11 = m44_glfloat[11];
    
    pt_m44->m12 = m44_glfloat[12];
    pt_m44->m13 = m44_glfloat[13];
    pt_m44->m14 = m44_glfloat[14];
    pt_m44->m15 = m44_glfloat[15];
    */

    //COLUMN MAJOR using "new" (ironically older) Matrix4 class    
    pt_m44->set( m44_glfloat[0], m44_glfloat[4],  m44_glfloat[8],  m44_glfloat[12], 
                 m44_glfloat[1], m44_glfloat[5], m44_glfloat[9],  m44_glfloat[13], 
                 m44_glfloat[2], m44_glfloat[6], m44_glfloat[10], m44_glfloat[14], 
                 m44_glfloat[3], m44_glfloat[7], m44_glfloat[11], m44_glfloat[15]);

} 

/******************************************/
//draw a locator representing a "quill", "extruder", etc 
//Basically the thing that moves around doing stuff on your machine

void draw_locator( Vector3* pos, float locsize)
{

    glBindTexture(GL_TEXTURE_2D, texture[0]);    

    float id = 0;

    // glMaterialfv(GL_FRONT, GL_EMISSION, emis_green);
    // glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);
    // glColor3f(.4,.4,.4);
    //glColor3f(rgb->r,rgb->g,rgb->b);    
    
    float hl = locsize/2; //halfline

    glMaterialfv(GL_FRONT, GL_EMISSION, emis_purple);
    glBegin(GL_LINES);
        glVertex3f(  pos->x-hl, pos->y , pos->z );
        glVertex3f(  pos->x+hl, pos->y , pos->z );

        glVertex3f(  pos->x, pos->y-hl, pos->z );
        glVertex3f(  pos->x, pos->y+hl, pos->z );        

        glVertex3f(  pos->x, pos->y, pos->z-hl);
        glVertex3f(  pos->x, pos->y, pos->z+hl); 

    glEnd();

}

/******************************************/
//draw a 3D grid on the "floor" and an indicator to show XYZ axis  
void graticulate( bool *tog_grid, bool *draw_cntrgrid, RGBType *pt_gridcolor, RGBType *pt_gridcolor2 )
{
 
    //int grd_num       = 10;
    
    float grd_height  = 0.0;

    float arrow_sml = gnomonsize/10;
    float arrow_med = gnomonsize/8;    

    float gspac = gridsize/(gridsquares/2);

    // printf("### %f \n",gridsquares);
    // printf("%f \n",gridsize);
    // printf("%f \n",gspac);

    glBindTexture(GL_TEXTURE_2D, texture[0]);    

    float id = 0;

    glMaterialfv(GL_FRONT, GL_EMISSION, emis_half);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);
    
    glColor3f(.4,.4,.4);

    glBegin(GL_LINES);
  
        for(id=-gspac; id<=gridsize; id+=gspac)
        {
            if(id==0)
            {  
                if (*draw_cntrgrid == 1)
                {
                    
                    glMaterialfv(GL_FRONT, GL_EMISSION, emis_red);
                    glColor3f(.5,0,0);    
                    glVertex3f(  gnomonsize, grd_height, id );
                    glVertex3f( 0          , grd_height, id ); 

                    glMaterialfv(GL_FRONT, GL_EMISSION, emis_green);
                    glColor3f(0,.5,0); 
                    glVertex3f( id, gnomonsize , id );
                    glVertex3f( id, id       , id ); 

                    glMaterialfv(GL_FRONT, GL_EMISSION, emis_blue); 
                    glColor3f(0,0,.5);                    
                    glVertex3f( id, grd_height,   gnomonsize );
                    glVertex3f( id, grd_height,  0 );  


                }
                if (*tog_grid == 1)
                {
                    glMaterialfv(GL_FRONT, GL_EMISSION, emis_half);
                    glColor3f(.4,.4,.4);
                    glVertex3f( id, grd_height,   gridsize );
                    glVertex3f( id, grd_height,  -gridsize );  

                    glVertex3f(  gridsize, grd_height, id );
                    glVertex3f( -gridsize, grd_height, id ); 
                }   


            }else if (*tog_grid == 1) 
            {
                glMaterialfv(GL_FRONT, GL_EMISSION, emis_half);
                glColor3f(.4,.4,.4);
                
                glVertex3f(-id, grd_height,  gridsize);
                glVertex3f(-id, grd_height, -gridsize);  
                glVertex3f( id, grd_height,  gridsize);
                glVertex3f( id, grd_height, -gridsize); 
                
                glVertex3f(-gridsize, grd_height, -id);
                glVertex3f( gridsize, grd_height, -id);                
                glVertex3f(-gridsize, grd_height,  id);
                glVertex3f( gridsize, grd_height,  id);                 
            }
            

        }

    glEnd();

    if (*draw_cntrgrid==1)
    {
        glColor3i(pt_gridcolor2->r, pt_gridcolor2->g, pt_gridcolor2->b);  
        glBegin(GL_TRIANGLES); 

            glMaterialfv(GL_FRONT, GL_EMISSION, emis_red); 
            glColor3f(.5,0,0); 
  
            glVertex3f(gnomonsize           , grd_height , -arrow_sml );
            glVertex3f(gnomonsize           , grd_height ,  arrow_sml );
            glVertex3f(gnomonsize+arrow_med , grd_height ,   0        );

            glMaterialfv(GL_FRONT, GL_EMISSION, emis_blue); 
            glColor3f(0,0,.5); 
            glVertex3f( -arrow_sml, grd_height, gnomonsize            );
            glVertex3f(  arrow_sml, grd_height, gnomonsize            );
            glVertex3f(          0, grd_height, gnomonsize+arrow_med  );

        glEnd();
    }
    

    glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full);
}


/******************************************/
 
void show_bbox(bool *pt_draw_bbox, RGBType *pt_gridcolor)
{
   
   if (*pt_draw_bbox == 1)
   {
        glMaterialfv(GL_FRONT, GL_EMISSION, emis_half);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);

        glBindTexture(GL_TEXTURE_2D, texture[0]);    
        
        //gDEBUG rab object extents from class method 
        //pt_obinfo

        glBegin(GL_LINES);
            
            glColor3f(pt_gridcolor->r, pt_gridcolor->g, pt_gridcolor->b);  

            /*
            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_min_z);
            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_min_z);

            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_min_z);
            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_min_z);            
            
            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_min_z);
            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_max_z); 

            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_min_z);
            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_min_z);
              
            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_max_z);
            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_max_z);            

            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_max_z);
            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_max_z); 

            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_max_z);
            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_max_z); 

            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_min_z);
            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_min_z); 

            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_min_z);
            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_max_z); 

            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_max_z);
            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_max_z); 

            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_min_z);
            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_max_z);

            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_min_z);
            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_max_z);
            */
            
        glEnd();

        glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full);
    }
   
}


/******************************************/

// initialize OpenGL  
void InitGL(int Width, int Height)           
{
        
    glEnable(GL_TEXTURE_2D);                  // Enable Texture Mapping
    glClearColor(0.15f, 0.15f, 0.25f, 0.0f);  // Clear The Background Color To Blue 
    
    glClearDepth(1.0);                        // Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS);                     // The Type Of Depth Test To Do
    glEnable(GL_DEPTH_TEST);                  // Enables Depth Testing
    
    glShadeModel(GL_SMOOTH);                  // Enables Smooth Color Shading
    

    glMatrixMode(GL_PROJECTION);
    
    glLoadIdentity();                     
    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);   // Calculate The Aspect Ratio Of The Window
    
    //glMatrixMode(GL_MODELVIEW);
}


/******************************************/


//    function to convert internal geom into GLfloat array 
//    this is used to draw points into a Vertex buffer object 


void dump_points_GLfloat( GLfloat* verts, obj_model* pt_object, int numpts)
{
    
    int y = 0;
    int num_gl_pts = 0;
   
    for (y=0;y<numpts;y++)
    {
        
        Vector3 point = pt_object->points[y];
        
        verts[num_gl_pts] =  (GLfloat)point.x; num_gl_pts++;
        verts[num_gl_pts] =  (GLfloat)point.y; num_gl_pts++;
        verts[num_gl_pts] =  (GLfloat)point.z; num_gl_pts++;
        verts[num_gl_pts] =  (GLfloat)1.0f; num_gl_pts++; // dont know what the 4th is ??????  

    }

}

/******************************************/

//void dump_points_GLfloat( GLfloat* verts, vector<vec3>* pt_vecarray, int numpts);
//overloaded dump points to accept array of points instead of object 
void dump_points_GLfloat( GLfloat* verts, std::vector<Vector3>* pt_vecarray, int numpts)
{
    
    int y = 0;
 
    // vector<vec3> vvex;
    // vvex.push_back(newvec3(1,2,3));
    // cout << vvex[0].y << "\n";

    // vec3 dv  = linebuffer1[p_i];
    // vec3 rgb = linebuffer1_rgb[p_i];            
    // glVertex3f(dv.x, dv.y, dv.z);
    
    // vector<vec3> * ptDrawvec = &linebuffer1;
    // vector<vec3> * ptDrawClr = &linebuffer1_rgb;


    // output->push_back( out );
    // colors->push_back( color );
    // *numvecs = *numvecs+1;
    //cout << pt_vecarray[0].x << "\n";

    for (y=0;y<numpts;y++)
    {

        // verts[num_gl_pts] =  (GLfloat)pt_vecarray->at(y).x; num_gl_pts++;
        // verts[num_gl_pts] =  (GLfloat)pt_vecarray->at(y).y; num_gl_pts++;
        // verts[num_gl_pts] =  (GLfloat)pt_vecarray->at(y).z; num_gl_pts++;        
        // verts[num_gl_pts] =  (GLfloat)1.0; num_gl_pts++;        
    }

}





/******************************************/
void clear_linebuffers( void)
{

    linebuffer1.clear();
    linebuffer1_rgb.clear();

    linebuffer2.clear();
    linebuffer2_rgb.clear();

}

/******************************************/
void add_vec_lbuf1( Vector3* insert)
{   
    Vector3 r = Vector3(1,1,1);
    linebuffer1.push_back(*insert);
    linebuffer1_rgb.push_back(r);
}

void add_vec_lbuf1( Vector3* insert, Vector3* rgb)
{
    linebuffer1.push_back(*insert);
    linebuffer1_rgb.push_back(*rgb);

}

/******************************************/

void add_vec_lbuf2( Vector3* insert)
{   
    Vector3 r = Vector3(1,1,1);
    linebuffer2.push_back(*insert);
    linebuffer2_rgb.push_back(r);
}
 


 


/******************************************/
void drawTriangle (GLfloat x1, GLfloat y1, GLfloat x2, 
                   GLfloat y2, GLfloat x3, GLfloat y3, GLfloat z)
{
   glBegin (GL_TRIANGLES);
       glVertex3f (x1, y1, z);
       glVertex3f (x2, y2, z);
       glVertex3f (x3, y3, z);
   glEnd ();
}


/******************************************/
void drawViewVolume (GLfloat x1, GLfloat x2, GLfloat y1, 
                     GLfloat y2, GLfloat z1, GLfloat z2)
{
   glColor3f (1.0, 1.0, 1.0);

   glBegin (GL_LINE_LOOP);
       glVertex3f (x1, y1, -z1);
       glVertex3f (x2, y1, -z1);
       glVertex3f (x2, y2, -z1);
       glVertex3f (x1, y2, -z1);
   glEnd ();

   glBegin (GL_LINE_LOOP);
       glVertex3f (x1, y1, -z2);
       glVertex3f (x2, y1, -z2);
       glVertex3f (x2, y2, -z2);
       glVertex3f (x1, y2, -z2);
   glEnd ();

   glBegin (GL_LINES);  //  4 lines    
       glVertex3f (x1, y1, -z1);
       glVertex3f (x1, y1, -z2);
       glVertex3f (x1, y2, -z1);
       glVertex3f (x1, y2, -z2);
       glVertex3f (x2, y1, -z1);
       glVertex3f (x2, y1, -z2);
       glVertex3f (x2, y2, -z1);
       glVertex3f (x2, y2, -z2);
   glEnd ();
}


/******************************************/
void set_screen_square(int* sx, int* sy){
    
    if (*sx>*sy){
        *sy = *sx;
        glutReshapeWindow(*sx, *sx);
    }else{
        *sx = *sy;
        glutReshapeWindow(*sy, *sy);
    }

}



/******************************************/

// m33_from_euler
// m44_from_euler

/*
   X - R
   Y - G
   Z - B 
*/

/*
void render_m33(m33 *t33)
{
    // if transpose:
    //     v1 = vec3(m33[0], m33[3], m33[6])
    //     v2 = vec3(m33[1], m33[4], m33[7])
    //     v3 = vec3(m33[2], m33[5], m33[8])  

    glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    // Not transposed
    vec3 v1 = newvec3(t33->m0, t33->m1, t33->m2);
    vec3 v2 = newvec3(t33->m3, t33->m4, t33->m5);
    vec3 v3 = newvec3(t33->m6, t33->m7, t33->m8);

    glBegin(GL_LINES);
      
       glMaterialfv(GL_FRONT, GL_EMISSION, emis_red);
       glVertex3f (0   ,    0,    0);
       glVertex3f (v1.x, v1.y, v1.z);
      
       glMaterialfv(GL_FRONT, GL_EMISSION, emis_green);
       glVertex3f (0   ,    0,    0);
       glVertex3f (v2.x, v2.y, v2.z);

       glMaterialfv(GL_FRONT, GL_EMISSION, emis_blue);       
       glVertex3f (0   ,    0,    0);
       glVertex3f (v3.x, v3.y, v3.z);

        
    glEnd ();

    glMaterialfv(GL_FRONT, GL_EMISSION, emis_off); 
    glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full); 
}

/////

void render_m44(m44 *t44)
{
  
    // ⎡0   1   2   3 ⎤     ⎡xx  xy  xz  0⎤
    // ⎢4   5   6   7 ⎥     ⎢yx  yy  yz  0⎥
    // ⎢8   9   10  11⎥     ⎢zx  zy  zz  0⎥
    // ⎣12  13  14  15⎦     ⎣0   0   0   0⎦

    glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    // Not transposed
    vec3 v1 = newvec3(t44->m0, t44->m1, t44->m2);
    vec3 v2 = newvec3(t44->m4, t44->m5, t44->m6);
    vec3 v3 = newvec3(t44->m8, t44->m9, t44->m10);

    glBegin(GL_LINES);
       glMaterialfv(GL_FRONT, GL_EMISSION, emis_red);    
       glVertex3f (0   ,    0,    0);
       glVertex3f (v1.x, v1.y, v1.z);

       glMaterialfv(GL_FRONT, GL_EMISSION, emis_green);       
       glVertex3f (0   ,    0,    0);
       glVertex3f (v2.x, v2.y, v2.z);

       glMaterialfv(GL_FRONT, GL_EMISSION, emis_blue);        
       glVertex3f (0   ,    0,    0);
       glVertex3f (v3.x, v3.y, v3.z);

        
    glEnd ();

    glMaterialfv(GL_FRONT, GL_EMISSION, emis_off); 
    glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full); 

}



///



void swimbag_tile(void)
{
    int grd_num       = 10;

    float grd_size    = 2.5;
    float gspac = grd_size/(grd_num/2);

    float id = 0;
    glBegin(GL_LINES);
        for(id=0; id<=grd_size; id+=gspac)
        {
            glVertex3f(-id, 0,  grd_size);
            glVertex3f(-id, 0, -grd_size);  
            glVertex3f(-id, 0, -id);
            glVertex3f( id, 0, -id);                
        }
    glEnd();
}


*/




