/*************************************************************/
/*
   gl_gui.c 
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

 
#include "gl_gui.h"


/*
float zoomSpeed    = 1.2f;
float rotateSpeed  = 4.0f;
Vector3 startpos = newvec3(0.0, 130.0, 60.0);

m33 capsuleObj; //represents a Unity/Maya Transform node 
quaternion orbt_rot_original;
Vector3 orbt_xform_original;
*/

int VIEW_MODE = -1; 

float orbit_x;         
float orbit_y;   
float orbit_dist = 5.0; // Z zoom 

float cam_posx = 0; // camera location
float cam_posy = 0;
float cam_posz = 0;


float coefficient = 0.005f;  


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

extern int scr_size_x;
extern int scr_size_y;


/********************************************/

// Use this for initialization
void olmecnav_start (void ) {
    // Create a transform (which will be the lookAt target and global orbit vector)
    
    //     capsuleObj = GameObject.CreatePrimitive(PrimitiveType.Capsule);
    //     capsuleObj.transform.position = Vector3.zero;

    // Snap the camera to align with the grid in set starting position (otherwise everything gets a bit wonky)
   
    // transform.position = startpos;  
    // transform.LookAt(capsuleObj.transform.position, Vector3.up);
    // capsuleObj.renderer.enabled = false; //hide the capsule object     

    //     ///
    //     orbt_xform_original = capsuleObj.transform.position;
    //     orbt_rot_original   = capsuleObj.transform.rotation;
}



/********************************************/

void octant_mouse_button(int button, int state, int x, int y)
{

    mouseX = x;
    mouseY = y;

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

        if (button == 3){
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
    }else{  // normal button event
        if (state == GLUT_DOWN){
            // printf("olmec middle click\n");  
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



/*************************************************************/
/*************************************************************/

/*


void mouse_clk_cb(int button, int state, int x, int y) 
{

    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        on_click = 1;
        clk_x_coord = x;
        clk_y_coord = y;
     }
}
*/


/***************************************/



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




/*************************************************
/**************************************************/


    //var wheelie = Input.GetAxis("Mouse ScrollWheel");
        
    // if (wheelie < 0) // back
    // {
    //     var currentZoomSpeed = 100f;
    //     transform.Translate(Vector3.forward * (wheelie * currentZoomSpeed));
    // }
    // if (wheelie > 0) // back
    // {
    //      var currentZoomSpeed = 100f;
    //      transform.Translate(Vector3.forward * (wheelie * currentZoomSpeed));
    // }

    /*
    //Input.GetAxis("Mouse ScrollWheel") < 0) // back
    if( Input.GetKey(KeyCode.RightAlt) || Input.GetKey(KeyCode.LeftAlt) ){

      // Distance between camera and orbitVector. We'll need this in a few places
      var distanceToOrbit = Vector3.Distance(transform.position, orbitVector.transform.position);
    
        //RMB - ZOOM
        if (Input.GetMouseButton(1)) {
            
            // Refine the rotateSpeed based on distance to orbitVector
            var currentZoomSpeed = Mathf.Clamp(zoomSpeed * (distanceToOrbit / 50), 0.1f, 2.0f);
            
            // Move the camera in/out
            transform.Translate(Vector3.forward * (x * currentZoomSpeed));
            
            // If about to collide with the orbitVector, repulse the orbitVector slightly to keep it in front of us
            if (Vector3.Distance(transform.position, orbitVector.transform.position) < 3) {
                orbitVector.transform.Translate(Vector3.forward, transform);
            }

        
        //LMB - PIVOT
        } else if (Input.GetMouseButton(0)) {
            
            // Refine the rotateSpeed based on distance to orbitVector
            var currentRotateSpeed = Mathf.Clamp(rotateSpeed * (distanceToOrbit / 50), 1.0f, rotateSpeed);
            
            
            // Temporarily parent the camera to orbitVector and rotate orbitVector as desired
            transform.parent = orbitVector.transform;
            orbitVector.transform.Rotate(Vector3.right * (y * currentRotateSpeed));
            orbitVector.transform.Rotate(Vector3.up * (x * currentRotateSpeed), Space.World);
            transform.parent = null;
                    
        //MMB - PAN
        else if (Input.GetMouseButton(2)) {
            
            // Calculate move speed
            var translateX = Vector3.right * (x * mouse_orbit_speed) * -1;
            var translateY = Vector3.up * (y * mouse_orbit_speed) * -1;
            
            // Move the camera
            transform.Translate(translateX);
            transform.Translate(translateY);
            
            // Move the orbitVector with the same values, along the camera's axes. In effect causing it to behave as if temporarily parented.
            orbitVector.transform.Translate(translateX, transform);
            orbitVector.transform.Translate(translateY, transform);
        }
    */        





