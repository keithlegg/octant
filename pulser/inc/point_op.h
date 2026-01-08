#ifndef POINTOPS_H
#define POINTOPS_H


#include <iostream>
#include <vector>

#include "Vectors.h"
#include "Matrices.h"


//coordinate to a pixel in the scancache 
struct pt2d {
    int x;
    int y;
};


/***************************************/


float deg_to_rad (float);


/***************************************/

/*
   "point generator"
   make coordinate pairs with no geometry 

   if you want geometry -use polygon_ops()
*/
class point_ops
{
    public:

    point_ops(){
        
    };

    ~point_ops(){};

    void lerp_along( Vector3* output,
                            Vector3 fpos, 
                            Vector3 spos, 
                            float dist );


    void locate_pt_along3d(std::vector<Vector3>*,
                           Vector3, 
                           Vector3, 
                           int);


    void cubic_bezier(std::vector<Vector3>*, 
                      std::vector<Vector3>*,
                      int*,
                      int, 
                      Vector3, 
                      Vector3, 
                      Vector3,
                      Vector3
                     );


    int get_line_intersection(float, float, float, float, float, float, float, float, float*, float *);
    void calc_circle ( pt2d *out_coords, int numdiv, int x_orig, int y_orig, float dia, int *num);
    void calc_line(  pt2d *out_coords, int *pt1, int *pt2, int *num);

};
 


/***************************************/


/*
   container for polygon objects 
   store and process 3D geometry 

   if you want a complete network of polygons -use obj_model()

*/

class polygon_ops : public point_ops 
{ 
    public: 
      int id_c; 

        polygon_ops(){
            num_pts     = 0;
            num_vnrmls  = 0;
            num_fnrmls  = 0;
            num_uvs     = 0;
            num_lines   = 0;
            num_tris    = 0;
            num_quads   = 0; 
            num_vtxrgb  = 0;  

            bb_min_x = 0;
            bb_max_x = 0;
            bb_min_y = 0;
            bb_max_y = 0;
            bb_min_z = 0;
            bb_max_z = 0;              
        };

        ~polygon_ops(){};


    virtual void reset(void);

    //properties about our model 
    uint num_pts;
    uint num_vtxrgb;
    uint num_vnrmls;    
    uint num_fnrmls;
    uint num_uvs;
    uint num_lines;
    uint num_tris;
    uint num_quads;    

    // extents of model (you can derive centroid from these)
    float bb_min_x;
    float bb_max_x;
    float bb_min_y;
    float bb_max_y;
    float bb_min_z;
    float bb_max_z;

    
    void hello(void); //test of inheritance 

    int numpts(void);


    //bool pt_is_near( pt1, pt2, dist );
    
    //Vector3 centroid_pts( array_of_vector3 );
    Vector3 centroid(void);
    Vector3 centroid(Vector3, Vector3, Vector3);
    void centroid(Vector3*, Vector3, Vector3, Vector3);

    Vector3 triangle_pt_vec3(Vector3, Vector3, Vector3, Vector3);
    float triangle_mean_z(Vector3, Vector3, Vector3);
    
    Vector3 three_vec3_to_normal( Vector3, Vector3, Vector3, bool);



}; 





#endif


