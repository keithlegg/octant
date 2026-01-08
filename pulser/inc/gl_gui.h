#ifndef GLGUI_H
#define GLGUI_H



#ifdef __linux__
    #include <GL/glut.h>       // Header File For The GLUT Library 
    #include <GL/gl.h>       // Header File For The OpenGL32 Library
    //#include <GL/glu.h>      // Header File For The GLu32 Library
#endif



void start_gui(void);

void octant_mouse_motion(int x, int y);
void octant_mouse_button(int button, int state, int x, int y);



#endif


