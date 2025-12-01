#include <iostream>
using namespace std; 

#include "math.h"

#include "point_op.h"
#include "Vectors.h"



// NOT USED YET 



int polygon_ops::getnum_verts(void){

}
/**********************************************/


/**********************************************/


//test of inheritance 
void  polygon_ops::hello(void)
{
    cout << "hello from point opszzz !! \n";
}



/*****************************************************************************/
/*  
    point_op.c

      Point Generators, create 2D/3D coordinates in all manner of fun ways. 

      AUTHOR: Keith Legg (keithlegg23@gmail.com)
 
    Copyright (C) 2026 Keith Legg   

*/

/*****************************************************************************/


#include <cmath>
#include <vector>

#include "math_op.h"
#include "point_op.h"

#include <iostream>



/*****************************/


void pointgen::locate_pt_along3d(std::vector<vec3>* output,
                                 vec3 fpos, 
                                 vec3 spos, 
                                 int num)
{
    
    int n = 0;

    for (n=0;n<num;n++)
    {
        vec3 npos;

        //X
        if(fpos.x-spos.x!=0)
        {
            npos.x =  spos.x+(((fpos.x-spos.x)/(num+1))*(n+1));
        }else{
            npos.x=0;
        }
 
        //Y
        if (fpos.y-spos.y!=0)
        { 
            npos.y =  spos.y+(((fpos.y-spos.y)/(num+1))*(n+1)); 
        }else{
            npos.y=0;
        }

        //Z
        if (fpos.z-spos.z!=0)
        { 
            npos.z =  spos.z+(((fpos.z-spos.z)/(num+1))*(n+1));
        }else{
            npos.z=0;            
        }

        //cout << n << " "<< npos.x <<" "<<npos.y<<" "<< npos.z<<"\n";

        output->push_back( npos );
        //output->insert( npos );

    }
 


};

/*****************************/

void pointgen::cubic_bezier(std::vector<vec3>* output, 
                            std::vector<vec3>* colors,
                            int* numvecs,
                            int draw_steps, 
                            vec3 start, 
                            vec3 ctrl1, 
                            vec3 ctrl2,
                            vec3 end
                        )
{
       
        double x,y,z,t,tt,ttt,u,uu,uuu = 0;
        double i;
 
        vec3 color = newvec3(1.0,0.0,0.0);

        for (i=0;i<draw_steps;i++)
        {
            t = i / draw_steps;
            tt = t * t;
            ttt = tt * t;
            u = 1.0 - t;
            uu = u * u;
            uuu = uu * u;

            x = uuu * start.x;
            x = 3.0 * uu * t * ctrl1.x +1.0;
            x = 3.0 * u * tt * ctrl2.x +1.0;
            x = ttt * end.x+1.0;

            y = uuu * start.y;
            y = 3.0 * uu * t * ctrl1.y +1;
            y = 3.0 * u * tt * ctrl2.y +1;
            y = ttt * end.y+1;

            z = uuu * start.z;
            z = 3.0 * uu * t * ctrl1.z +1.0;
            z = 3.0 * u * tt * ctrl2.z +1.0;
            z = ttt * end.z +1.0;

            vec3 out;
            out.x = x; 
            out.y = y;
            out.z = z;

            output->push_back( out );
            colors->push_back( color );
            *numvecs = *numvecs+1;

        }


        //return out;
}



/*****************************/

/*
 Returns 1 if the lines intersect, otherwise 0. In addition, if the lines 
 intersect the intersection point may be stored in the floats i_x and i_y.
 http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
*/

/*
char get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, 
    float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

    float s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        if (i_x != NULL)
            *i_x = p0_x + (t * s1_x);
        if (i_y != NULL)
            *i_y = p0_y + (t * s1_y);
        return 1;
    }

    return 0; // No collision
}
*/

int pointgen::get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, 
    float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
    float s02_x, s02_y, s10_x, s10_y, s32_x, s32_y, s_numer, t_numer, denom, t;
    s10_x = p1_x - p0_x;
    s10_y = p1_y - p0_y;
    s32_x = p3_x - p2_x;
    s32_y = p3_y - p2_y;

    denom = s10_x * s32_y - s32_x * s10_y;
    if (denom == 0)
        return 0; // Collinear
    bool denomPositive = denom > 0;

    s02_x = p0_x - p2_x;
    s02_y = p0_y - p2_y;
    s_numer = s10_x * s02_y - s10_y * s02_x;
    if ((s_numer < 0) == denomPositive)
        return 0; // No collision

    t_numer = s32_x * s02_y - s32_y * s02_x;
    if ((t_numer < 0) == denomPositive)
        return 0; // No collision

    if (((s_numer > denom) == denomPositive) || ((t_numer > denom) == denomPositive))
        return 0; // No collision
    // Collision detected
    t = t_numer / denom;
    if (i_x != NULL)
        *i_x = p0_x + (t * s10_x);
    if (i_y != NULL)
        *i_y = p0_y + (t * s10_y);

    return 1;
}

/*****************************/
//not convinced this totally works due to roundoff error, but its close enough
void pointgen::calc_circle ( pt2d *out_coords, int numdiv, int x_orig, int y_orig, float dia, int *num)
{
    int divamt = (int)(360/numdiv);
    int rotation_offset = 45;

    for (int i = 0; i <360; i=i+divamt)
    {  
        out_coords[*num].x = x_orig + (sin(dtr(i-rotation_offset))*dia);
        out_coords[*num].y = y_orig + (cos(dtr(i-rotation_offset))*dia);
        *num = *num+1;
    }

}

/*****************************/

void pointgen::calc_line( pt2d *out_coords, int *pt1, int *pt2, int *num)
{

    int x1 = pt1[0];
    int y1 = pt1[1];
    int const x2 = pt2[0];
    int const y2 = pt2[1];

    int const foobar = 100;

    short xy_idx     = 0;
    int delta_x(x2 - x1);

    // if x1 == x2, then it does not matter what we set here
    signed char const ix((delta_x > 0) - (delta_x < 0));
    delta_x = abs(delta_x) << 1;
    int delta_y(y2 - y1);
    // if y1 == y2, then it does not matter what we set here
    signed char const iy((delta_y > 0) - (delta_y < 0));
    delta_y = abs(delta_y) << 1;
    
    //dump a point
    
    out_coords[xy_idx].x = x1;
    out_coords[xy_idx].y = y1;
    xy_idx++;


    if (delta_x >= delta_y)
    {
      // error may go below zero
      int error(delta_y - (delta_x >> 1));
      while (x1 != x2)
      {
          if ((error >= 0) && (error || (ix > 0)))
          {
              error -= delta_x;
              y1 += iy;
          }
          // else do nothing
          error += delta_y;
          x1 += ix;
            //dump  a point
            out_coords[xy_idx].x = x1;
            out_coords[xy_idx].y = y1;
            xy_idx++;

      }
    }
    else
    {
      // error may go below zero
      int error(delta_x - (delta_y >> 1));
      while (y1 != y2)
      {
          if ((error >= 0) && (error || (iy > 0)))
          {
              error -= delta_y;
              x1 += ix;
          }
          // else do nothing
          error += delta_x;
          y1 += iy;

            //dump  a point
            out_coords[xy_idx].x = x1;
            out_coords[xy_idx].y = y1;
            xy_idx++;
      }
    }

   *num = xy_idx;
}



/**********************************************/

// // create a normal vector (vec3) from 3 points that represent a polygon  
// // take 3 point (positions), convert to vectors 
// // and run three_vec3_to_normal on them  
// Vector3 calc_tripoly_normal( Vector3 v1, Vector3 v2, Vector3 v3, bool unitlen=false): 
// {
//     Vector3 v1;
//     Vector3 v2;
//     Vector3 v3;
// 
//     //v1.insert( three_pts[0] )
//     //v2.insert( three_pts[1] )
//     //v3.insert( three_pts[2] )  
// 
//     return self.three_vec3_to_normal(v1, v2, v3, unitlen=unitlen)
// }


/**********************************************/


/*
    def between(self, pt2):
        """ given 2 points in 3D, create a 3D vector 
            representing the offset between them 

            doesnt get much easier than this, just subtract 

        """
   
        #if isinstance(pt1, tuple):
        #    pt1 = vec3( pt1[0], pt1[1], pt1[2]) 


        if isinstance(pt2, tuple):
            pt2 = vec3( pt2[0], pt2[1], pt2[2])

        return pt2 - self
*/

/**********************************************/

// take 3 vec3 objects and return a face normal 
Vector3 polygon_ops::three_vec3_to_normal( Vector3 v1, Vector3 v2, Vector3 v3, bool unitlen)
{

    // calculate the face normal  
    Vector3 a = v1 - v2;
    Vector3 b = v1 - v3;

    Vector3 f_nrml;

    if(unitlen == true){
        f_nrml = a.cross(b).normalize();
    }
    else {    
        f_nrml = a.cross(b);          
    }

    return f_nrml; 
   
}


/**********************************************/
// UNTESTED - vector from a triangle centroid to a point
Vector3 polygon_ops::triangle_pt_vec3(Vector3 p1, Vector3 p2, Vector3 p3, 
                                                              Vector3 dpt
                                     )
{
    Vector3 tmp; 
    
    tmp.x = (p1.x + p2.x + p3.x)/3;
    tmp.y = (p1.y + p2.y + p3.y)/3;
    tmp.z = (p1.z + p2.z + p3.z)/3;    

    return dpt-tmp;
}

/**********************************************/
// Z axis only centroid 
double  polygon_ops::triangle_mean_z(Vector3 p1, Vector3 p2, Vector3 p3)
{
    double z1 = p1.z;
    double z2 = p2.z;
    double z3 = p3.z;
    return (z1+z2+z3)/3;
}


/**********************************************/

// XYZ centroid of a triangle 
Vector3 polygon_ops::centroid(Vector3 p1, Vector3 p2, Vector3 p3)
{
    Vector3 out;
    out.x = (p1.x + p2.x + p3.x)/3;
    out.y = (p1.y + p2.y + p3.y)/3;
    out.z = (p1.z + p2.z + p3.z)/3; 
    return out;
}

// XYZ centroid of a triangle 
void polygon_ops::centroid(Vector3 *out, Vector3 p1, Vector3 p2, Vector3 p3)
{

    out->x = (p1.x + p2.x + p3.x)/3;
    out->y = (p1.y + p2.y + p3.y)/3;
    out->z = (p1.z + p2.z + p3.z)/3; 

}



