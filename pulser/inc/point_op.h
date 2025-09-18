#ifndef POINTOP_H    
#define POINTOP_H

#include <vector>

#include "math_op.h"

using namespace std;

//coordinate to a pixel in the scancache 
struct pix_coord {
    int x;
    int y;
};







class pointgen
{
    public:
        pointgen(){
        };

        ~pointgen(){};

    void locate_pt_along3d(vector<vec3>* output,
                           vec3 fpos, 
                           vec3 spos, 
                           int num);


    void cubic_bezier(vector<vec3>* output, 
                      vector<vec3>* colors,
                      int* numvecs,
                      int draw_steps, 
                      vec3 start, 
                      vec3 ctrl1, 
                      vec3 ctrl2,
                      vec3 end
                     );

};





//utilities

void calc_circle ( pix_coord *out_coords, int numdiv, int x_orig, int y_orig, float dia, int *num);
void calc_line(  pix_coord *out_coords, int *pt1, int *pt2, int *num);

int get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, 
    float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y);




#endif
