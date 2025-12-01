#include <iostream>
#include <stdio.h>


#include "math.h"

#include "math_ops.h"




/*
// create a normal vector (vec3) from 3 points that represent a polygon  
void calc_normal_triangle(Vector3 *nrml, Vector3 pt1, Vector3 pt2, Vector3 pt3)
{
    // v1=vec3();
    // v2=vec3()
    // v3=vec3();
    // v4=vec3()
    // v1.insert( three_pts[0] )
    // v2.insert( three_pts[1] )
    // v3.insert( three_pts[2] )  

    //return self.three_vec3_to_normal(v1, v2, v3, unitlen=unitlen)
}



// take 3 vec3 objects and return a face normal 
void three_vec3_to_normal(Vector3 *nrml, Vector3 v1, Vector3 v2, Vector3 v3) //, unitlen=False):
{
    // # calculate the face normal  
    // a = v1 - v2
    // b = v1 - v3
    // if unitlen:
    //     f_nrml = a.cross(b).normal
    // else:    
    //     f_nrml = a.cross(b)          
    // return f_nrml 
}
*/


/*

    // https://www.bfilipek.com/2012/06/select-mouse-opengl.html

    // pseudo code found at: 
    // http://www.gamedev.net/topic/221071-simple-raysphere-collision/ 
    Vec3d ClosestPoint(const Vec3d A, const Vec3d B, 
                       const Vec3d P, double *t) 
    { 
        Vec3d AB = B - A; 
        double ab_square = DotProduct(AB, AB); 
        Vec3d AP = P - A; 
        double ap_dot_ab = DotProduct(AP, AB); 
        // t is a projection param when we project vector AP onto AB 
        *t = ap_dot_ab / ab_square; 
        // calculate the closest point 
        Vec3d Q = A + AB * (*t); 
        return Q; 
    }  
*/

