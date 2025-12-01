#ifndef OBJMODEL_H    
#define OBJMODEL_H

#include <string>
#include <vector>

#include "math_op.h"


//Cautiously moved to the heap  
const int num_vtx   = 120000;
const int num_faces = 120000;


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



class obj_model {
    public:
        obj_model(){
            num_pts     = 0;
            num_vnrmls  = 0;
            num_fnrmls  = 0;
            num_uvs     = 0;
            num_lines   = 0;
            num_tris    = 0;
            num_quads   = 0; 
            num_vtxrgb  = 0;            
        };

        ~obj_model(){};

        int num_pts;
        int num_vtxrgb;
        int num_vnrmls;    
        int num_fnrmls;
        int num_uvs;
        int num_lines;
        int num_tris;
        int num_quads;    
      

        vec3 points[num_vtx];        // 3 floats - vertex  
        vec3 vtxrgb[num_vtx];        // 3 floats - color per vertex 
        vec2 uvs[num_vtx];           // 2 floats - UV coords 
        vec3 vnormals[num_vtx];      // 3 floats - face normal 
        vec3 fnormals[num_faces];    // 3 floats - face normal 
        line lines[num_faces];       // 2 ints   - lines    idx
        triangle tris[num_faces];    // 3 ints   - triangle idx 
        quad quads[num_faces];       // 4 ints   - quad     idx 

        // float& operator[] (size_t i)
        // {
        //   switch (i) {
        //     case 0: return x;
        //     case 1: return y;
        //     case 2: return z;
        //     default: throw "something";
        //   }
        // }
        // float operator[] (size_t i) const
        // { return (*const_cast<vector3f*>(this))[i];

        //vec3 get_obj_centroid( void );
        vec3 get_extents(void);

        void reset(void);
        void load( char *);
        void save( char *);
        void show(void);
        void calc_normals(void);
        void add_triangle(vec3 pt1, vec3 pt2, vec3 pt3);
        void triangulate(void);
        void show_geom(void);

        //void get_obj_info(obj_model* loader, obj_info* obinfo);
        //void test_loader_data( obj_model* loader);


};




std::vector<std::string> tokenizer( const std::string&, char);



//DEBUG remove this and merge into the main object?
//cant recall why the hell I did it like this 
/*
class obj_info {
    public:
        obj_info(){

        };

        ~obj_info(){};

    int num_pts;
    int num_nrmls;
    int num_uvs;

    int num_tris;
    int num_faces;
    int num_quads;

    float centroid_x;
    float centroid_y;
    float centroid_z;

    float bb_min_x;
    float bb_max_x;
    
    float bb_min_y;
    float bb_max_y;

    float bb_min_z;
    float bb_max_z;    

};
*/


#endif
