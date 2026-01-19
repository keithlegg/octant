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



#define MAX_NUM_VERTICES 200000
#define MAX_NUM_FACES 200000

/***************************************/
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

    //properties about our model  (inherited by obj_model)
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


    //------------------------------------/
    //------------------------------------/

    //Vector3 points[MAX_NUM_VERTICES];           // vertices of model 
    //Vector3 vtxrgb[MAX_NUM_VERTICES];           // vextex colors of model  
    //Vector2 uvs[MAX_NUM_VERTICES];              // UV coords      - storage for lookup  
    //Vector3 vnormals[MAX_NUM_VERTICES];         // vertex normals - storage for lookup 
    //Vector3 fnormals[MAX_NUM_FACES];            // face normals   - common ID with faces
    Vector3* points     = new Vector3[MAX_NUM_VERTICES];   // vertices of model 
    Vector3* vtxrgb     = new Vector3[MAX_NUM_VERTICES];   // vextex colors of model  
    Vector2* uvs        = new Vector2[MAX_NUM_VERTICES];   // UV coords      - storage for lookup  
    Vector3* vnormals   = new Vector3[MAX_NUM_VERTICES];   // vertex normals - storage for lookup 
    Vector3* fnormals   = new Vector3[MAX_NUM_FACES];      // face normals   - common ID with faces


    // --- 
    
    // //lists of ID tables for lookup 
    // std::vector<int> lines [MAX_NUM_FACES];     // 2 sided faces 
    // std::vector<int> tris  [MAX_NUM_FACES];     // 3 sided faces
    // std::vector<int> vnids [MAX_NUM_VERTICES];  // vertex normal ids
    // std::vector<int> quads [MAX_NUM_FACES];     // 4 sided faces
    // std::vector<int> faces [MAX_NUM_FACES];     // >4, N sided faces 
    uint* pt_loc               = new uint[MAX_NUM_FACES];                  // 3d "point only" location 
    std::vector<uint>* lines   = new std::vector<uint>[MAX_NUM_FACES];     // 2 sided faces 
    std::vector<uint>* tris    = new std::vector<uint>[MAX_NUM_FACES];     // 3 sided faces
    std::vector<uint>* vnids   = new std::vector<uint>[MAX_NUM_VERTICES];  // vertex normal ids
    std::vector<uint>* quads   = new std::vector<uint>[MAX_NUM_FACES];     // 4 sided faces
    std::vector<uint>* faces   = new std::vector<uint>[MAX_NUM_FACES];     // >4, N sided faces 


    // ---
    Vector3 bfr_pts[MAX_NUM_VERTICES];           // general point buffer   ( tmp work area )
    std::vector<uint> bfr_faces[MAX_NUM_FACES];  // general polygon buffer ( tmp work area ) 

    //------------------------------------/
    //------------------------------------/

    virtual void reset(void);
    void hello(void); //test of inheritance 
    int numpts(void);


    //------------------------------------/
    //functions mapped to python libraries 
    //------------------------------------/

    //bool pt_is_near( pt1, pt2, dist );

    void bbox3d(void);

    //Vector3 centroid_pts( array_of_vector3 );
    Vector3 centroid(void);
    Vector3 centroid(Vector3, Vector3, Vector3);
    void centroid(Vector3*, Vector3, Vector3, Vector3);
    //------------------------------------/
    //------------------------------------/


    Vector3 triangle_pt_vec3(Vector3, Vector3, Vector3, Vector3);
    float triangle_mean_z(Vector3, Vector3, Vector3);
    
    Vector3 three_vec3_to_normal( Vector3, Vector3, Vector3, bool);



}; 





#endif


