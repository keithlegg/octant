/*************************************************************/
/*
   demo_cube.cpp 

   3D demo - display a rotating cube 

   Copyright (C) 2026 Keith Legg - keithlegg23@gmail.com

*/
/*************************************************************/

#include <unistd.h>      

#include "demo_cube.h"

#include "gl_setup.h"


float xrot, yrot, zrot; 

extern int window_id;

extern GLuint texture[3];  



/*******************************************************/

// data for animated "pong" texture 
//democube uses it - probably get rid of that 
unsigned int upos = 100;
unsigned int vpos = 320;  
unsigned int dir_u = 0;
unsigned int dir_v = 0;
unsigned int pong_speed = 5;
unsigned int pong_size  = 60;
unsigned int img_usize  = 512;
unsigned int img_vsize  = 512;

unsigned int use_tex = 0;


//extern Image* main_bg_bfr      ; 
//extern Image* imageloaded_bfr2 ; 
//extern Image* imageloaded_bfr  ; 


static bool g_bButton1Down = false;
static bool g_bLightingEnabled = true;
static bool g_bFillPolygons = true;
static bool g_bTexture = false;


static int g_yClick = 0;

extern bool scr_full_toglr;

extern int scr_size_x;
extern int scr_size_y;


static float orbit_dist = -5.0;


////////////////////////////////////////////////////

//top level opengl loop 
void spinningCubeDemo(int *argc, char** argv){
    
    int screenSize = 512; //defaults to 512
    if (argv[1]){
       screenSize = atoi(argv[1]);
    }

    printf("\n\nstarting up cubedemo in %i resolution.\n", screenSize);

    // you can find documentation at http://reality.sgi.com/mjk/spec3/spec3.html   
    glutInit(argc, argv);  

    //  Select type of Display mode:   
    //  Double buffer 
    //  RGBA color
    //  Alpha components supported 
    //  Depth buffer   

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  
    glutInitWindowSize(screenSize, screenSize);  //window size

    // the window starts at the upper left corner of the screen  
    glutInitWindowPosition(0, 0);  
     
    window_id = glutCreateWindow("3D cube demo"); //create an opengl window 

    //register display callback       
    glutDisplayFunc(&drawglscene_3d);   

    // Even if there are no events, redraw our gl scene.  
    glutIdleFunc(&drawglscene_3d);

    glutReshapeFunc(&ReSizeGLScene);  //register window resize callback 
    glutKeyboardFunc(&keyPressed);    // Register key pressed callback 
    
    InitGL(scr_size_x, scr_size_y); // Initialize window. 
    
    ///////////////////////////
    //test of BMP saving 
    //create_Image("generated1.bmp");  
    //create_Image2("generated2.bmp"); 

    ///////////////////////////    
   
    glutMouseFunc (maya_mouse_button);
    //glutMotionFunc (maya_mouse_motion);

    ///////////////////////////      
    // Create our popup menu
    //BuildPopupMenu ();
    glutAttachMenu (GLUT_RIGHT_BUTTON);

    //loadImage("textures/olmec.bmp"      , imageloaded_bfr2);
    //loadImage("textures/generated3.bmp" , imageloaded_bfr);

    glutMainLoop();// Start Event Processing Engine   


}




////////////////////////////////////////////////////


void drawglscene_3d()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear The Screen And The Depth Buffer
    glLoadIdentity();               // Reset The View


    glTranslatef(0.0f, 0.0f, -4.0f);              // move 5 units into the screen.
    glRotatef(xrot,1.0f,0.0f,0.0f);     // Rotate On The X Axis
    glRotatef(yrot,0.0f,1.0f,0.0f);     // Rotate On The Y Axis
    glRotatef(zrot,0.0f,0.0f,1.0f);     // Rotate On The Z Axis
    
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // choose the texture to use.

    glBegin(GL_QUADS);                      // begin drawing a cube
    
    // Front Face (note that the texture's corners have to match the quad's corners)
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);  // Top Left Of The Texture and Quad
    
    // Back Face
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);  // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);  // Bottom Left Of The Texture and Quad
    
    // Top Face
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);  // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);  // Top Right Of The Texture and Quad
    
    // Bottom Face       
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);  // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
    
    // Right face
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);  // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
    
    // Left Face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);  // Top Left Of The Texture and Quad
    
    glEnd();  // done with the polygon.

    if (yrot<100){
        //xrot+=.1f;  // X Axis Rotation  
        yrot+=.2f;    // Y Axis Rotation
        //zrot+=.1f;  // Z Axis Rotation
    }
    
    if (yrot>=100 && yrot<200){
        xrot+=.2f;    // X Axis Rotation  
        yrot+=.2f;    // Y Axis Rotation
        //zrot+=.1f;  // Z Axis Rotation
    }

    if (yrot>=200){
        xrot-=.2f;    // X Axis Rotation  
        //yrot+=.2f;    // Y Axis Rotation
        zrot+=.1f;  // Z Axis Rotation
 
    }

    ///////////////////////

    unsigned int u_edge = img_usize - pong_size;
    unsigned int v_edge = img_vsize - pong_size;

    if(upos<0)         { upos = 100; }
    if(vpos<0)         { vpos = 100; }
    if(upos>img_usize) { upos = 100; }
    if(vpos>img_vsize) { vpos = 100; }


    // animate some numbers 
    if(upos>=u_edge)     { dir_u = 1; }
    if(upos<=pong_size)  { dir_u = 0; }

    if(vpos>=u_edge)    { dir_v = 1; }
    if(vpos<=pong_size) { dir_v = 0; }

    if(dir_u==0)        { upos +=pong_speed;}else{ upos -=pong_speed;}
    if(dir_v==0)        { vpos +=pong_speed;}else{ vpos -=pong_speed;}

    
    //animateTextures2(main_bg_bfr);
    //animateTextures3(main_bg_bfr);

    //usleep(100000); 
    // since this is double buffered, swap the buffers to display what just got drawn.
    glutSwapBuffers();
}



////////////////////////////////////////////////////




////////////////////////////////////////////////////


 
/*
static void animateTextures3(Image *loaded_texture)
{
    //test of new framebuffer commands
    RGBType* pt_rgb_bfr =  createBuffer24(loaded_texture->sizeX, loaded_texture->sizeY);    
    
    //copyBuffer24( loaded_texture ,  pt_rgb_bfr ); //convert "Image" to "RGBType"
    
    copyBuffer24( imageloaded_bfr2 ,  pt_rgb_bfr ); //convert "Image" to "RGBType"

    // 

    //make a color for some lines 
    RGBType line_color;
    RGBType *pt_linecolor = &line_color;
    pt_linecolor->r = 255;
    pt_linecolor->g = 0;
    pt_linecolor->b = 0;

    //make a color for some lines 
    RGBType line_color2;
    RGBType *pt_linecolor2 = &line_color2;
    pt_linecolor2->r = 155;
    pt_linecolor2->g = 255;
    pt_linecolor2->b = 0;

    // int cp_tl[2] = {0};
    // int cp_br[2] = {0};
    // cp_tl[0] = upos-pong_size;
    // cp_tl[1] = vpos-pong_size;
    // cp_br[0] = upos+pong_size;
    // cp_br[1] = vpos+pong_size;
    // draw_line(pt_rgb_bfr, loaded_texture->sizeX , cp_tl[0] , cp_tl[1] , cp_br[0] , cp_br[1] , pt_linecolor); 
    // draw_line(pt_rgb_bfr, loaded_texture->sizeX , cp_tl[0] , cp_tl[1] , cp_tl[0] , cp_br[1] , pt_linecolor);
    
    //just to show its alive - bounce a ball around 
    //draw_fill_circle ( pt_rgb_bfr, loaded_texture->sizeX, upos, vpos, pong_size, pt_linecolor);

    // const char *filename = "rgb_buffer.bmp";
    // saveBMP_24bit (pt_rgb_bfr, filename , image_x, image_y);
    // free(pt_image_bfr);

    ///////////////////////
    int x = 0;
    
    int num_cells = 20;
    int cell_size = (int)loaded_texture->sizeX/num_cells;
    
    for (x=cell_size;x<=loaded_texture->sizeX-cell_size;x=x+cell_size){
        draw_fill_circle ( pt_rgb_bfr, loaded_texture->sizeX, x, vpos, cell_size/2, pt_linecolor2);
    }


    ///////////////////////
    
    // // first stab at some matrix and vector art 
    // m33 identity = new_m33(1,0,0,
    //                        0,1,0,
    //                        0,0,1);
    // vec2 v1 = newvec2(102.0, 301.0);
    // draw_line(pt_rgb_bfr, loaded_texture->sizeX , v1.x , v1.y , 0 , 0 , pt_linecolor);
    
    ///////////////////////

    //overwrite loaded buffer with lines
    copyBuffer24( pt_rgb_bfr, loaded_texture ); //convert "RGBType" to "Image"

    free(pt_rgb_bfr);
     
    // create and apply 2D texture   
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // 2d texture (x and y size)

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture

    glTexImage2D(GL_TEXTURE_2D, 0, 3, loaded_texture->sizeX, loaded_texture->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, loaded_texture->data);

}

*/


///////////////////////

static void maya_mouse_button(int button, int state, int x, int y)
{
    // Respond to mouse button presses.
    // If button1 pressed, mark this state so we know in motion function.

    // printf("maya button x %d y %d \n",x , y);

    //left click 
    if (button == GLUT_LEFT_BUTTON)
      {
        g_bButton1Down = (state == GLUT_DOWN) ? true : false;

      }

      // middle click
      if ((button == 3) || (button == 4)) // It's a wheel event
      {
           // Disregard redundant GLUT_UP events
           if (state == GLUT_UP) return; 

           if (button == 3){
               if (orbit_dist < -1){
                   orbit_dist++;                
               }
  
           }
           if (button == 4){
               orbit_dist--; 
           }
        
           //std::cout << " zooming " << orbit_dist << "\n";

      }else{  // normal button event
           if (state == GLUT_DOWN){
               // printf("maya middle click\n");  
           }
      }


    //Right click
    if (button == GLUT_RIGHT_BUTTON)
      {
        
        // printf("maya right click \n");

      }

}



///////////////////////
//SHOW_GUI_MOUSECLICK
static void maya_mouse_motion(int x, int y)
{

}


///////////////////////
// callback when window is resized (which shouldn't happen in fullscreen) 
static void ReSizeGLScene(int Width, int Height)
{
    
    if (Height==0)  // Prevent A Divide By Zero If The Window Is Too Small
    Height=1;

    glViewport(0, 0, Width, Height);    // Reset The Current Viewport And Perspective Transformation

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //gluOrtho2D(0, 100, 0, 100);
    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
    
    glMatrixMode(GL_MODELVIEW);
}//end resize callback

//define keyboard events 
static void keyPressed(unsigned char key, int x, int y) 
{

    usleep(100);

    //ESCAPE KEY
    if (key == 27) 
    { 
        glutDestroyWindow(window_id); 
        exit(0);                   
    }

    if (key == 32) //space
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

    if (key == 97) //a
    { 
        if (pong_speed<20){
            pong_speed +=1;
        }

    }
 
    if (key == 98) //b
    { 
        //printf("you pressed b\n");
        if (pong_speed>0){
            pong_speed -=1;
        }

    }
  
    if (key == 99) //c
    { 
        if (pong_size<200){
            pong_size +=1;
        }      
    }

    if (key == 100) //d
    { 
 
        if (pong_size>4){
            pong_size -=1;
        }

    }

    if (key == 101) //e
    { }        

    if (key == 102) //f
    { }

    if (key == 119) //w
    { 
        if(use_tex==0){
            use_tex=1;
        }
        else{
            use_tex=0;
        }  
    }

    if (key == 113) //q
    { 
        //printf("you pressed q\n");
        //testLoadBinary();      
    }

}


