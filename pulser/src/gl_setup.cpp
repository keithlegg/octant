/*************************************************************/
/*
   gl_setup.c 
       -  common home for an openGL application 
      
   Copyright (C) 2026 Keith Legg - keithlegg23@gmail.com

*/
/*************************************************************/
 

#include <iostream>

#include <stdlib.h>
#include <stdio.h>

#include "obj_model.h"

#include "gl_setup.h"


extern GLuint texture[3];

extern float gridsquares;
extern float gridsize;
extern float gnomonsize;


static GLfloat emis_full[]   = { 1, 1, 1, 0};
static GLfloat emis_off[]    = { 0, 0, 0, 0};
static GLfloat emis_half[]   = { .5, .5, .5, 0};
static GLfloat emis_teal[]   = { 0, 1., 1., 0};
static GLfloat emis_red[]    = { 1., 0, 0, 0};
static GLfloat emis_green[]  = { 0, 1., 0, 0};
static GLfloat emis_blue[]   = { 0, 0, 1., 0};

// quaternion frank = quaternion_identity();



//////////////////////////////////////////////////////////////
//void m44_to_glm44( m44* pt_m44, GLfloat m44_glfloat[16] ){}

void glutm44_to_m44( m44* pt_m44, GLfloat m44_glfloat[16] ){

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

} 


//////////////////////////////////////////////////////////////
//draw a 3D grid on the "floor" and an indicator to show XYZ axis  
void graticulate( bool *draw_grid, bool *draw_cntrgrid, RGBType *pt_gridcolor, RGBType *pt_gridcolor2 )
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
                if (*draw_grid == 1)
                {
                    glMaterialfv(GL_FRONT, GL_EMISSION, emis_half);
                    glColor3f(.4,.4,.4);
                    glVertex3f( id, grd_height,   gridsize );
                    glVertex3f( id, grd_height,  -gridsize );  

                    glVertex3f(  gridsize, grd_height, id );
                    glVertex3f( -gridsize, grd_height, id ); 
                }   


            }else if (*draw_grid == 1) 
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


//////////////////////////////////////////////////////////////

void show_bbox(bool *pt_draw_bbox, struct obj_info* pt_obinfo, RGBType *pt_gridcolor){

   if (*pt_draw_bbox == 1)
   {
        glMaterialfv(GL_FRONT, GL_EMISSION, emis_half);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);

        glBindTexture(GL_TEXTURE_2D, texture[0]);    

        float id = 0;

        glBegin(GL_LINES);
            
            glColor3f(pt_gridcolor->r, pt_gridcolor->g, pt_gridcolor->b);  


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

        glEnd();

        glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full);
    }

}


//////////////////////////////////////////////////////////////

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


//////////////////////////////////////////////////////////////


//    function to convert internal geom into GLfloat array 
//    this is used to draw points into a Vertex buffer object 


void dump_points_GLfloat( GLfloat* verts, obj_model* pt_object, int numpts)
{
    
    int y = 0;
    int num_gl_pts = 0;
   
    for (y=0;y<numpts;y++)
    {
        
        vec3 point = pt_object->points[y];
        
        verts[num_gl_pts] =  (GLfloat)point.x; num_gl_pts++;
        verts[num_gl_pts] =  (GLfloat)point.y; num_gl_pts++;
        verts[num_gl_pts] =  (GLfloat)point.z; num_gl_pts++;
        verts[num_gl_pts] =  (GLfloat)1.0f; num_gl_pts++; // dont know what the 4th is ??????  

    }

}

//////////////////////////////////////////////////////////////

//void dump_points_GLfloat( GLfloat* verts, vector<vec3>* pt_vecarray, int numpts);
//overloaded dump points to accept array of points instead of object 
void dump_points_GLfloat( GLfloat* verts, vector<vec3>* pt_vecarray, int numpts)
{
    
    int y = 0;
    int num_gl_pts = numpts;


    // vector<vec3> vvex;
    // vvex.push_back(newvec3(1,2,3));
    // cout << vvex[0].y << "\n";

    // vec3 dv  = scene_drawvec3[p_i];
    // vec3 rgb = scene_drawvecclr[p_i];            
    // glVertex3f(dv.x, dv.y, dv.z);
    
    // vector<vec3> * ptDrawvec = &scene_drawvec3;
    // vector<vec3> * ptDrawClr = &scene_drawvecclr;
    // int * ptnum_drawvec3 = &num_drawvec3;

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



//////////////////////////////////////////////////////////////
void clear_scenegeom( void)
{

    //scene_drawvec3.clear();
    //scene_drawvecclr.clear();
    //num_drawvec3 = 0;
}


//////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////

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






//////////////////////////////////////////////////////////////

void drawTriangle (GLfloat x1, GLfloat y1, GLfloat x2, 
    GLfloat y2, GLfloat x3, GLfloat y3, GLfloat z)
{
   glBegin (GL_TRIANGLES);
       glVertex3f (x1, y1, z);
       glVertex3f (x2, y2, z);
       glVertex3f (x3, y3, z);
   glEnd ();
}


//////////////////////////////////////////////////////////////

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









//////////////////////////////////////////////////////////////
void set_screen_square(int* sx, int* sy){
    
    if (*sx>*sy){
        *sy = *sx;
        glutReshapeWindow(*sx, *sx);
    }else{
        *sx = *sy;
        glutReshapeWindow(*sy, *sy);
    }

}


//////////////////////////////////////////////////////////////

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


