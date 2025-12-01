#ifndef POINTOPS_H
#define POINTOPS_H


#include <iostream>
#include <vector>

#include "Vectors.h"
#include "Matrices.h"

//OLD C library - need to update 
#include "math_op.h"


//coordinate to a pixel in the scancache 
struct pt2d {
    int x;
    int y;
};


class pointgen
{
    public:
        pointgen(){
        };

        ~pointgen(){};

    void locate_pt_along3d(std::vector<vec3>*,
                           vec3, 
                           vec3, 
                           int);


    void cubic_bezier(std::vector<vec3>*, 
                      std::vector<vec3>*,
                      int*,
                      int, 
                      vec3, 
                      vec3, 
                      vec3,
                      vec3
                     );


    int get_line_intersection(float, float, float, float, float, float, float, float, float*, float *);
    void calc_circle ( pt2d *out_coords, int numdiv, int x_orig, int y_orig, float dia, int *num);
    void calc_line(  pt2d *out_coords, int *pt1, int *pt2, int *num);

};



 
class point_ops {
    public:

        point_ops(){
        };

        ~point_ops(){};
    
    //double triangle_mean_z(Vector3 pt1, Vector3 pt2, Vector3 pt3){}
        
    // cvt_2d_to_3d( points ){}
    // cvt_3d_to_2d( points ){}
    // locate_pt_along3d( x1, y1, z1, x2, y2, z2, num)


};



class polygon_ops : public point_ops 
{ 
    public: 
      int id_c; 

        polygon_ops(){
        };

        ~polygon_ops(){};

    
    void hello(void); //test of inheritance 


    int getnum_verts(void);


    //Vector3 centroid_pts( array_of_vector3 );
    //bool pt_is_near( pt1, pt2, dist );
    
    Vector3 centroid(Vector3 p1, Vector3 p2, Vector3 p3);
    void centroid(Vector3 *out, Vector3 p1, Vector3 p2, Vector3 p3);

    Vector3 triangle_pt_vec3(Vector3 p1, Vector3 p2, Vector3 p3, Vector3 dpt);
    double triangle_mean_z(Vector3 p1, Vector3 p2, Vector3 p3);


    
    Vector3 three_vec3_to_normal( Vector3 v1, Vector3 v2, Vector3 v3, bool unitlen);



}; 





#endif


