#ifndef CONFUSION36D_H    
#define CONFUSION36D_H


#include "gl_setup.h"



void tweak_matrix( void );

void grab_camera_matrix( m44 *pt_mmm );
void grab_projection_matrix(m44 *pt_mpm );

void triangulate(void);
void calc_normals(void);

void init_pycore(void);


void set_view_ortho(void);
void setOrthographicProjection();
void resetPerspectiveProjection();
void renderBitmapString(float x, float y, void *font,const char *string);

static void display_loop();
static void render_loop();

//static void animateTextures3(Image *loaded_texture);

static void ReSizeGLScene(int Width, int Height);
static void keyPressed(unsigned char key, int x, int y); 

void draw_poly_mousevent(int button, int state, int x, int y);
void olmec_mouse_button(int button, int state, int x, int y);
void olmec_mouse_motion(int x, int y);

void start_gui(int *argc, char** argv);





#endif

