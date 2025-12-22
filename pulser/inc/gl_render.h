 
#ifndef GL_RENDER_H    
#define GL_RENDER_H


#include "obj_model.h"




void setlight0(void);

void reset_view(void);

void render_loop(void);
void animate(void);

void grab_camera_matrix( Matrix4 *pt_mmm );
void set_view_ortho(void);
void setOrthographicProjection();
void resetPerspectiveProjection();
void renderBitmapString(float, float, void *, const char *);


void reshape_window(int width, int height);

void render_quads(obj_model* pt_model_buffer);
void render_normals(obj_model* pt_model_buffer);
void render_vbo(obj_model* pt_model_buffer);
void render_lines(obj_model* pt_model_buffer);
 


#endif


