#ifndef GLSETUP_H    
#define GLSETUP_H


//DEBUG replace with lowercase versions 
#define TRUE 1
#define FALSE 0

//#include <stdint.h>

#include <vector>
using std::vector; 

//#include "math_op.h"
#include "obj_model.h"



//consolidated from gnolmec framebuffer.h 
//24 bit color 
typedef struct{
    unsigned int r=0;
    unsigned int g=0;
    unsigned int b=0;
}RGBType;

struct material {
  Vector3 diffuse_color;
  Vector3 specular_color;
  float shininess;
  float transparency;
};


struct directional_light
{
    Vector3 color;
    float ambient_intensity;
    Vector3 direction;
    float diffuse_intensity;
}; 





/********************************************/
#define GL_GLEXT_PROTOTYPES


#ifdef __linux__
    #include <GL/glut.h>       // Header File For The GLUT Library 
    #include <GL/gl.h>       // Header File For The OpenGL32 Library
    //#include <GL/glu.h>      // Header File For The GLu32 Library
#endif

 
#ifdef __APPLE__
	#include <GLUT/glut.h>
    //#include <GL/gl.h>      // not in use
	//#include <GL/glew.h>    // not in use 
#endif



void draw_locator( Vector3* pos, float locsize);

void glutm44_to_m44( Matrix4* , GLfloat m44_glfloat[16] );

void graticulate( bool *, bool *, RGBType *, RGBType * );

//void show_bbox(bool * pt_draw_bbox, struct obj_info* pt_obinfo, RGBType *);
void show_bbox(bool *, RGBType *);

void InitGL(int , int );
void dump_points_GLfloat( GLfloat* , obj_model* , int );
void dump_points_GLfloat( GLfloat* , vector<Vector3>* , int );

void clear_scenegeom( void);


void set_screen_square(int* sx, int* sy);



#endif