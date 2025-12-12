#ifndef CONFUSION36D_H    
#define CONFUSION36D_H



#include "cnc_globals.h"
#include "gl_setup.h"



//void tweak_matrix( void );

//void grab_projection_matrix(Matrix4 *pt_mpm );
void grab_camera_matrix( Matrix4 *pt_mmm );

void timer_init(void);
//void timer_reset(void)

void set_view_ortho(void);
void setOrthographicProjection();
void resetPerspectiveProjection();
void renderBitmapString(float, float, void *, const char *);

void key_cb(unsigned int key);

void run_send_pulses(cncglobals*, float, float, float,
                                  float, float, float, int);  


void start_gui(int *argc, char** argv);






#endif

