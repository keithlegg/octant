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

//#define MAX_NUM_VERTICES 1000
//#define MAX_NUM_FACES 1000

#define MAX_NUM_VERTICES 10000
#define MAX_NUM_FACES 10000

/*
// index/indices for a line object 
typedef struct line{
    int pt1;
    int pt2;
};

// index/indices for a triangle polygon
typedef struct triangle{
    int pt1;
    int pt2;
    int pt3;    
};

// index/indices for a quad polygon 
typedef struct quad{
    int pt1;
    int pt2;
    int pt3;    
    int pt4;
};
*/


class obj_model: public polygon_ops {
    public:
        obj_model()
        {
            num_pts     = 0;
            num_vnrmls  = 0;
            num_fnrmls  = 0;
            num_uvs     = 0;
            num_lines   = 0;
            num_tris    = 0;
            num_quads   = 0; 
            num_vtxrgb  = 0;  
            
            load_as_lines = false;

        };

        ~obj_model(){};

        bool load_as_lines;

        int num_pts;
        int num_vtxrgb;
        int num_vnrmls;    
        int num_fnrmls;
        int num_uvs;
        int num_lines;
        int num_tris;
        int num_quads;    

        //------
        std::vector<double> vtx_tmp;
        std::vector<int>    fac_tmp;  
        
        //Vector3 centroid( void );
        Vector3 extents(void);
        Vector3 get_triface_normal(int);

        // --- 
        Vector3 points[MAX_NUM_VERTICES];      // vertices of model    
        Vector3 vtxrgb[MAX_NUM_VERTICES];      // vextex colors of model  

        Vector2 uvs[MAX_NUM_VERTICES];         // UV coords 
        Vector3 vnormals[MAX_NUM_VERTICES];    // veretx normals 
        Vector3 fnormals[MAX_NUM_FACES];       // face normals

        std::vector<int> lines[MAX_NUM_FACES]; // 2 sided faces 
        std::vector<int> tris [MAX_NUM_FACES]; // 3 sided faces
        std::vector<int> quads[MAX_NUM_FACES]; // 4 sided faces
        std::vector<int> faces[MAX_NUM_FACES]; // >4, N sided faces 

        // ---
        Vector3 bfr_pts[MAX_NUM_VERTICES];          // general point buffer   ( tmp work area )
        std::vector<int> bfr_faces[MAX_NUM_FACES];  // general polygon buffer ( tmp work area ) 

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

        void reset(void);
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
        void make_line(double scale); 
        void make_triangle(double scale); 
        void make_circle(int divs, double scale);
        void make_square(double scale);  
        void make_cube(double scale);

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

        void vec3_as_pt_geom(Vector3, double);
        void vec3_as_pt_geom(Vector3, Vector3, double );

        void vec3_as_geom_atpos( Vector3, Vector3, Vector3 );
        void vec3_as_geom_atpos( Vector3, Vector3 );
        void vec3_as_geom(Vector3);




};




std::vector<std::string> tokenizer( const std::string&, char);



#endif
