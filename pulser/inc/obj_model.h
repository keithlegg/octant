#ifndef OBJMODEL_H    
#define OBJMODEL_H


/*

    broken up into multiple C files:

    obj_model.cpp 
    obj_geom.cpp 
    obj_fio.cpp  


*/



#include <string>
#include <vector>

#include "Vectors.h"
#include "Matrices.h"
//#include "math_op.h"

//this is required - follows the same inheritance as old python tools 
#include "point_op.h"




/*
   The temrinology is confusing 

   obj_model.points should really be called vertex/verteces 

   I added a point location later , (pt_locs) - this is an indexed point (or 1 dimensional indexed vert)

*/


class obj_model: public polygon_ops 
{
    public:

        obj_model(){};

        ~obj_model(){};

        //-----------------------------------------//

        void reset(void) override;

        //------
        //buffers to be used as tmp storage 
        std::vector<float> vtx_tmp;
        std::vector<uint>   fac_tmp;  
        
        //Vector3 calc_centroid( void );
        Vector3 calc_extents(void);
        Vector3 get_triface_normal(int);

        // --- 


        //----------------
        //----------------
        // class members to deal with this as a contained object 

        // Matrix3 m33;
        Matrix4 m44;
 
        // void translate(void);
        // void rotate(void);
        // void scale(void);
        //----------------
        //----------------


        void show(void);
        void show_geom(void);
        
        //void get_obj_info(obj_model* loader, obj_info* obinfo);

        //---
        //file IO operations 
        void load_m44(char* filename);
        //void save_m44(char* filename);
        
        void load( char *);
        void save( char *);

        //------
        //prim geometry functions 
        
        void sample_data(void);
        void prim_line(float scale); 
        void prim_triangle(float scale); 
        void prim_circle(int divs, float scale);
        void prim_square(float scale);  
        void prim_cube(float scale);

        //------
        void calc_normals(void);
        void triangulate(void);
        void insert(std::vector<int>& );
    
        void add_triangle(Vector3, Vector3, Vector3 );
        void add_triangle(int, int, int );
        void append_tri(Vector3, Vector3, Vector3, int, int, int );

        //these look like they should be moved to pointgen, etc 
        void between_2vecs_as_line(Vector3, Vector3 );        
        void between_2vecs_as_line(Vector3, Vector3, Vector3 );

        void vec3_as_pt_geom(Vector3, float);
        void vec3_as_pt_geom(Vector3, Vector3, float );

        void vec3_as_geom_atpos( Vector3, Vector3, Vector3 );
        void vec3_as_geom_atpos( Vector3, Vector3 );
        void vec3_as_geom(Vector3);




};




std::vector<std::string> tokenizer( const std::string&, char);



#endif
