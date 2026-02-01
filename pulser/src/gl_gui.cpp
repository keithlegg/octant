/*************************************************************/
/*
   gl_gui.cpp 
       - experimental tools to click in window and hopefully make simple "buttons" or regions 
       of screen attached tro a callback 

  
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


#include "globals.h"
#include "plot.h"

#if DO_BUILD_GUI == true
    #include "gl_gui.h"
    #include "gl_setup.h"
    #include "gl_render.h"
#endif



extern int scr_size_x;
extern int scr_size_y;
extern int window_id;

//DEBUG - seriously conisider a better name for this!
extern GLuint texture[3];

extern cncglobals cg;
extern cnc_plot* pt_motionplot;

extern std::vector<Vector3> linebuffer1; 
extern std::vector<Vector3> linebuffer1_rgb; 

extern std::vector<Vector3> linebuffer2; 
extern std::vector<Vector3> linebuffer2_rgb; 





/********************************************/
void start_gui()
{

    // I see no reason to pass args - use dummy args instead
    int argc = 1;
    char *argv[1] = {(char*)"Something"};
    glutInit(&argc, argv ); 

    set_colors();
    pt_motionplot->timer_init();

    //------------
    //load CNC cfg (including paths to .obj files) 
    cg.load_cfg_file();
    //load the 3d models 
    cg.load_objects();

    // we should now have display vectors to process, 
    // this sets up internals like quill_pos, etc  
    pt_motionplot->init_paths(&pt_motionplot->toolpath_vecs);
    
    pt_motionplot->retract_height = cg.retract_height;
    pt_motionplot->work_height    = cg.work_height;

    //------------
    //warnings();
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  
    glutInitWindowSize(scr_size_x, scr_size_y);  //window size
    glutInitWindowPosition(0, 0);  
    
    window_id = glutCreateWindow("Octant 2026"); //create an opengl window 
 
    //------------
    reset_view();

    // Register GL callbacks       
    glutDisplayFunc(&render_loop); 
    glutIdleFunc(&animate);
    glutReshapeFunc(&reshape_window);  // register window resize callback 

    InitGL(scr_size_x, scr_size_y); // Initialize window. 

    glutMouseFunc (octant_mouse_button);
    glutMotionFunc (octant_mouse_motion);

    //------------

    // experimental draw polygon 
    //glutMouseFunc (draw_poly_mousevent);
    //loadImage("textures/generated2.bmp" , imageloaded_bfr);
    //loadImage("textures/generated2.bmp" , imageloaded_bfr2);
    
    //------------
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









/*************************************************************/
/*************************************************************/


/*

int mouseClickCount = 0;
int rectPlotted;


void add_draw_pt(float xx, float yy){
    vertices[num_pts_drw] = GLfloat(xx);
    num_pts_drw++;
    vertices[num_pts_drw] = GLfloat(yy);
    num_pts_drw++;
    vertices[num_pts_drw] = GLfloat(0);
    num_pts_drw++;
    vertices[num_pts_drw] = GLfloat(1.0);
    num_pts_drw++;
}

  void on_vertex_selected(int x, int y){

     if(mouseClickCount == 0){
       
        printf("## X %d Y %d \n", x, y );

        // glVertex2i(dr_x1, dr_y1);
        // glVertex3f (v1.x, v1.y, v1.z);
       

       add_draw_pt(2.0, 1.7);

     }
     else{
        vertices[num_pts_drw] = GLfloat(x);
        num_pts_drw++;
        vertices[num_pts_drw] = GLfloat(y);
        num_pts_drw++;
        vertices[num_pts_drw] = GLfloat(1.0);
        num_pts_drw++;
        vertices[num_pts_drw] = GLfloat(1.0);
        num_pts_drw++;

        // glVertex2i(dr_x1, dr_y1);
        // glVertex2i(dr_x1, dr_y2);
        // glVertex2i(dr_x2, dr_y2);
        // glVertex2i(dr_x2, dr_y1);

     }
}


void draw_poly_mousevent(int button, int state, int x, int y){
   if(button==GLUT_LEFT_BUTTON && state ==GLUT_DOWN && mouseClickCount == 0){
      //y = y+20; //adjusts for VM mouse tracking error

      on_vertex_selected(x, scr_size_y - y);
      rectPlotted = 0;
   }

   if(button==GLUT_LEFT_BUTTON && state ==GLUT_UP && mouseClickCount == 0){
      if(rectPlotted == 1){
       return;
      }
      else{
       mouseClickCount++;
      }
   }

   if(button==GLUT_LEFT_BUTTON && state ==GLUT_DOWN && mouseClickCount == 1){
       //y = y+20; //adjusts for VM mouse tracking error
       on_vertex_selected(x, scr_size_y - y);
       mouseClickCount = 0;
       rectPlotted = 1;
   }
}
*/






