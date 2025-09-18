/*************************************************************/
/*
   math_op.c

       This is an attempt to port the python math library I wrote into the C language

       The original library supports:
           vector2
           vector3
           vector4
           Quaternion 
           Matrix2X2
           Matrix3X3
           Matrix4X4
           Spherical/Polar coords 


       The original can be found here:

           https://github.com/keithlegg/gnelscript/blob/master/pygfx/math_ops.py

   Copyright (C) 2019 Keith Legg - keithlegg23@gmail.com

*/
/*************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include <cmath>

#include "math_op.h"


/************************************************/



//degree to radian 
double dtr ( double deg){
   return deg * DEG_TO_RAD;
}

//radian to degree
double rtd ( double rad){
   return rad * RAD_TO_DEG;
}

/************************************************/
double dotProduct ( vec2 v1, vec2 v2){
    return (v1.x*v2.x + v1.y*v2.y ); 
}


double dotProduct ( vec3 v1, vec3 v2){
    return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z); 
}


/************************************************/
// distance between 2 double points in 2D
double fcalc_distance(double pt1[2], double pt2[2]){
    return sqrt( ( (pt1[0]-pt2[0])*(pt1[0]-pt2[0])) + ((pt1[1]-pt2[1])*(pt1[1]-pt2[1])) ) ;
}

// distance between 2 int points in 2D
double fcalc_distance(int pt1[2], int pt2[2]){
    return sqrt( ( (pt1[0]-pt2[0])*(pt1[0]-pt2[0])) + ((pt1[1]-pt2[1])*(pt1[1]-pt2[1])) ) ;
}

double length( vec2 input){
    return sqrt( (input.x * input.x) + (input.y * input.y) );
}

double length( vec3 input){
    return sqrt( (input.x * input.x) + (input.y * input.y) + (input.z * input.z) );
}


/************************************************/

double calc_theta_vert ( double start_x, double start_y, double end_x, double end_y)
{
    //#get corner to build a right triangle
    double a_x = end_x-start_x;  
    double a_y = end_y-start_y;

    double r = 0;
    
    //#relative offset (depending on order of start-end)
    if (a_x != 0 && a_y !=0){
         r = ( rtd( atan(a_x/a_y) ) );
    }
  
    //printf(" angle is %f\n" , r);
    return r;
}

/************************************************/

double angle_between( vec2 v_one, vec2 v_two )
{
    vec2 v1 ;
    vec2 v2 ;
    v1 = normalize(v_one);
    v2 = normalize(v_two);
    double dot = dotProduct(v1,v2);
    return rtd( acos(dot) );
}


/************************************************/

/*
    vec2 copy(vec2)
    vec3 copy(vec2)
    quaternion copy(quaternion)
    m33 copy(m33)
    m44 copy(m44)
    

*/


/************************************************/


vec2 newvec2( int x, int y ){
    vec2 output;
    output.x = (double)x;
    output.y = (double)y;
    return output; 
}

vec2 newvec2( double x, double y ){
    vec2 output;
    output.x = x;
    output.y = y;
    return output; 
}

/************************************************/

/* normalize a vector */
vec2 normalize( vec2 input )
{
     double vlen  = length(input);

     vec2 output;

     if(vlen != 0)
     {
        output.x = input.x/vlen;
        output.y = input.y/vlen;
     }

     return output; 
}

/************************************************/
vec2 scale_vec( vec2 input, double amount ){
    vec2 output;
    output.x = input.x/amount;
    output.y = input.y/amount;
    return output; 
}

/************************************************/
vec2 mult( vec2 input, double amount ){
    vec2 output;
    output.x = input.x*amount;
    output.y = input.y*amount;
    return output; 
}

/************************************************/

/*
  convert an arbitrary line segment to a true vector from origin.
*/
vec2 line2vect(double start_x, double start_y, double end_x, double end_y)
{
    vec2 out;
    out.x = end_x-start_x;
    out.y = end_y-start_y;
    return out;
}

vec2 line2vect(int start_x, int start_y, int end_x, int end_y)
{
    vec2 out;
    out.x = (double)(end_x-start_x);
    out.y = (double)(end_y-start_y);
    return out;
}


/************************************************/

/* multiply two 2d vectors */
vec2 vmul_2d ( vec2 v1, vec2 v2 )
{
     vec2 output;
     
     output.x = v1.x * v2.x;
     output.y = v1.y * v2.y;

     return output; 
}


/************************************************/

vec3 add (vec3 v1, vec3 v2){
    vec3 output;
    output.x = v1.x + v2.x;
    output.y = v1.y + v2.y;
    output.z = v1.z + v2.z;
    
    return output;

}

vec3 sub (vec3 v1, vec3 v2){
    vec3 output;
    output.x = v1.x - v2.x;
    output.y = v1.y - v2.y;
    output.z = v1.z - v2.z;
    
    return output;    
}

vec3 mult (vec3 v1, vec3 v2){
    vec3 output;
    output.x = v1.x * v2.x;
    output.y = v1.y * v2.y;
    output.z = v1.z * v2.z;
    
    return output;
}

 
vec3 div (vec3 v1, double sclr){
     // UNTESTED/ BROKEN /NORMALIZE 

     vec3 output;

     if(sclr != 0)
     {
        output.x = v1.x/sclr;
        output.y = v1.y/sclr;
        output.z = v1.z/sclr;
     }

     return output; 


}
 

vec3 div (vec3 v2){
    // UNTESTED - normalized? 
    vec3 output;

    output.x = v2.x / length(v2);
    output.y = v2.y / length(v2);
    output.z = v2.z / length(v2);

    return output;

}


/************************************************/
/************************************************/
/************************************************/


vec3 cross(vec3 v1, vec3 v2){
    // UNTESTED! 
    vec3 output;
    
    output.x =  v1.y*v2.z-v1.z*v2.y; 
    output.y =  v1.z*v2.x-v1.x*v2.z; 
    output.z =  v1.x*v2.y-v1.y*v2.x;
    
    return output;
}

vec3 newvec3( int x, int y, int z ){
    vec3 output;
    output.x = (double)x;
    output.y = (double)y;
    output.z = (double)z;
    return output; 
}

vec3 newvec3( double x, double y, double z ){
    vec3 output;
    output.x = x;
    output.y = y;
    output.z = z;
    return output; 
}

/* normalize a 3d vector */
vec3 normalize( vec3 input )
{
     double vlen  = length(input);

     vec3 output;

     if(vlen != 0)
     {
        output.x = input.x/vlen;
        output.y = input.y/vlen;
        output.z = input.z/vlen;

     }

     return output; 
}


vec3 dtr_vec3(vec3 invec){
    vec3 output;

    //return ( dtr( invec[0] ),
    //         dtr( invec[1] ),
    //         dtr( invec[2] ),
    //       )
    
    return output;

}

/************************************************/

vec3 rtd_vec3( vec3 invec ){
    vec3 output;    
    // return ( rtd( invec[0] ),
    //          rtd( invec[1] ),
    //          rtd( invec[2] ),
    //        )
    return output;
}        

/************************************************/
vec3 mult( double scalar, vec3 v ){
    vec3 output; 
    // return [v[0]*scalar, v[1]*scalar, v[2]*scalar ]
    return output;
}

/************************************************/
/* multiplies a 3X3 matrix by a vec3 
   returns a vec3 */

vec3 mult( m33 m, vec3 v ){
    vec3 output;
    
    output.x = m.m0 * v.x + m.m3 * v.y + m.m6 * v.z; 
    output.y = m.m1 * v.x + m.m4 * v.y + m.m7 * v.z; 
    output.z = m.m2 * v.x + m.m5 * v.y + m.m8 * v.z; 

    return output;
}


/************************************************/
/* multiplies a 4X4 matrix by a 3D vector 
   returns a vector tuple */

vec3 mult( m44 m, vec3 v ){
    vec3 output;

    output.x = m.m0*v.x + m.m4*v.y + m.m8  * v.z+m.m12;
    output.y = m.m1*v.x + m.m5*v.y + m.m9  * v.z+m.m13;
    output.z = m.m2*v.x + m.m6*v.y + m.m10 * v.z+m.m14;
       
    return output;
}


/************************************************/

// given 2 points in 3D, create a 3D vector 
// representing the offset between them 
// doesnt get much easier than this, just subtract 
////////////
// vec3 between(double pt1_x, double pt1_y, double pt1_z, 
//                  double pt2_x, double pt2_y, double pt2_z){
//   
//         #if isinstance(pt1, tuple):
//         #    pt1 = vec3( pt1[0], pt1[1], pt1[2]) 
// 
//         if isinstance(pt2, tuple):
//             pt2 = vec3( pt2[0], pt2[1], pt2[2])
// 
//         return pt2 - self
// }

/************************************************/


/*

    def project_pt(self, offset):
        """ project a point along a vector """
        
        # nX = B.x - A.x
        # nY = B.y - A.y
        # nZ = B.z - A.z 

        # distX = pow( (A.x - B.x ) , 2.0 ) 
        # distY = pow( (A.y - B.y ) , 2.0 ) 
        # distZ = pow( (A.z - B.z ) , 2.0 ) 
        # vecLength = math.sqrt(distX + distY + distZ )

        # normalized vector  
        # calcX = nX / self.length
        # calcY = nY / self.length
        # calcZ = nZ / self.length

        normal = self.normal

        # project point along vector with offset (can use negative too)
        ptX = self.x + (normal.x * offset)
        ptY = self.y + (normal.y * offset)
        ptZ = self.z + (normal.z * offset)
        return type(self)(ptX, ptY, ptZ)


    def angle_between(self, other):
        """ 
            result is in radians
            derived from law of cosines 
            range from 0 (colinear) to PI radians (180 deg)  

            FAILS WHEN 0 or 180 ?
        """
        try:
            o = math.acos( self.dot(other) / (self.length*other.length) ) 
            return o 
        except:
            return 0

    def vector_mean(self, p_vecs, mode='vec3'):
        """ 
            UNTESTED 
         
            take a list of vectors and return a new vector that is an average

        """
        tmp_x = 0;tmp_y = 0;tmp_z = 0;

        count = 0
        a = 0 
        for v in p_vecs: 

            if mode=='angle':            
                a = a + self.np_angle_between( upvec, v )

            if mode=='vec3':
                if isinstance(v, vec3):
                    tmp_x = tmp_x+v.x
                    tmp_y = tmp_x+v.y
                    tmp_z = tmp_x+v.z 

                if isinstance(v, tuple) or isinstance(v, list):                
                    tmp_x = tmp_x+v[0]
                    tmp_y = tmp_x+v[1]
                    tmp_z = tmp_x+v[2] 

            count += 1 
        
        if mode=='angle':
            return a/count 
        if mode=='vec3':
            return type(self)(tmp_x/count, tmp_y/count, tmp_z/count)


        return None 

*/      

/************************************************/
 
void print_vec2(vec2 input){
    printf("%f %f \n", input.x, input.y );
}

void print_vec3(vec3 input){
    printf("%f %f %f\n", input.x, input.y, input.z );
}
 

/************************************************/
/************************************************/
/************************************************/


m33 new_m33( double m0 , double m1 , double m2,  
             double m3 , double m4 , double m5, 
             double m6 , double m7 , double m8
    ){
    m33 output;
    return output; 
}


m44 new_m44( double m0 , double m1 , double m2 , double m3, 
             double m4 , double m5 , double m6 , double m7,
             double m8 , double m9 , double m10, double m11,
             double m12, double m13, double m14, double m15
    ){
    m44 output;
    return output; 
}


m33 identity33(void){

    m33 output;    
     
    output.m0 =  1.0;
    output.m1 =  0;
    output.m2 =  0;
    
    output.m3 =  0;
    output.m4 =  1.0;
    output.m5 =  0;

    output.m6 =  0;
    output.m7 =  0;
    output.m8 =  1;   
                
    return output;  

}


m44 identity44(void){
    m44 output;
    
    output.m0  = 1.0;
    output.m1  = 0;
    output.m2  = 0;
    output.m3  = 0;

    output.m4  = 0;
    output.m5  = 1.0;
    output.m6  = 0;
    output.m7  = 0;

    output.m8  = 0;
    output.m9  = 0;
    output.m10 = 1.0;
    output.m11 = 0;

    output.m12 = 0;
    output.m13 = 0;
    output.m14 = 0;
    output.m15 = 1.0;
    
    return output;
}

/************************************************/

m33 matrix_add(m33 other){
    // self.m[0]+n[0], self.m[1]+n[1], self.m[2]+n[2],
    // self.m[3]+n[3], self.m[4]+n[4], self.m[5]+n[5],
    // self.m[6]+n[6], self.m[7]+n[7], self.m[8]+n[8]     
}

m44 matrix_add(m44 other){
    //  self.m[0] +n[0] , self.m[1]+n[1]  , self.m[2]+n[2]  , self.m[3]+n[3]  , 
    //  self.m[4] +n[4] , self.m[5]+n[5]  , self.m[6]+n[6]  , self.m[7]+n[7]  ,
    //  self.m[8] +n[8] , self.m[9]+n[9]  , self.m[10]+n[10], self.m[11]+n[11],
    //  self.m[12]+n[12], self.m[13]+n[13], self.m[14]+n[14], self.m[15]+n[15]

}

m33 matrix_sub(m33 other){
    //   self.m[0]-n[0], self.m[1]-n[1], self.m[2]-n[2],
    //   self.m[3]-n[3], self.m[4]-n[4], self.m[5]-n[5],
    //   self.m[6]-n[6], self.m[7]-n[7], self.m[8]-n[8]     
}

m44 matrix_sub(m44 other){
    //  self.m[0] -n[0] , self.m[1]-n[1]  , self.m[2]-n[2]  , self.m[3]-n[3]  , 
    //  self.m[4] -n[4] , self.m[5]-n[5]  , self.m[6]-n[6]  , self.m[7]-n[7]  ,
    //  self.m[8] -n[8] , self.m[9]-n[9]  , self.m[10]-n[10], self.m[11]-n[11],
    //  self.m[12]-n[12], self.m[13]-n[13], self.m[14]-n[14], self.m[15]-n[15]

}


/************************************************/
/* multiply two 3X3 matricies together */

m33 mult( m33 m, m33 n ){
    // UNTESTED
    m33 output;    
     
    output.m0 =  m.m0*n.m0 + m.m1*n.m3 + m.m2*n.m6;
    output.m1 =  m.m0*n.m1 + m.m1*n.m4 + m.m2*n.m7;
    output.m2 =  m.m0*n.m2 + m.m1*n.m5 + m.m2*n.m8;
    output.m3 =  m.m3*n.m0 + m.m4*n.m3 + m.m5*n.m6;
    output.m4 =  m.m3*n.m1 + m.m4*n.m4 + m.m5*n.m7;
    output.m5 =  m.m3*n.m2 + m.m4*n.m5 + m.m5*n.m8;
    output.m6 =  m.m6*n.m0 + m.m7*n.m3 + m.m8*n.m6;
    output.m7 =  m.m6*n.m1 + m.m7*n.m4 + m.m8*n.m7;
    output.m8 =  m.m6*n.m2 + m.m7*n.m5 + m.m8*n.m8;   
                
    return output;        
}


/************************************************/
/* multiply two 4X4 matricies together */

m44 mult( m44 m, m44 n ){
    // UNTESTED - NOT DONE 
    m44 output;
    
    output.m0  = m.m0*n.m0  + m.m1*n.m4  + m.m2*n.m8   + m.m3*n.m12;
    output.m1  = m.m0*n.m1  + m.m1*n.m5  + m.m2*n.m9   + m.m3*n.m13;
    output.m2  = m.m0*n.m2  + m.m1*n.m6  + m.m2*n.m10  + m.m3*n.m14;
    output.m3  = m.m0*n.m3  + m.m1*n.m7  + m.m2*n.m11  + m.m3*n.m15;
    output.m4  = m.m4*n.m0  + m.m5*n.m4  + m.m6*n.m8   + m.m7*n.m12;
    output.m5  = m.m4*n.m1  + m.m5*n.m5  + m.m6*n.m9   + m.m7*n.m13;
    output.m6  = m.m4*n.m2  + m.m5*n.m6  + m.m6*n.m10  + m.m7*n.m14;
    output.m7  = m.m4*n.m3  + m.m5*n.m7  + m.m6*n.m11  + m.m7*n.m15;
    output.m8  = m.m8*n.m0  + m.m9*n.m4  + m.m10*n.m8  + m.m11*n.m12;
    output.m9  = m.m8*n.m1  + m.m9*n.m5  + m.m10*n.m9  + m.m11*n.m13;
    output.m10 = m.m8*n.m2  + m.m9*n.m6  + m.m10*n.m10 + m.m11*n.m14;
    output.m11 = m.m8*n.m3  + m.m9*n.m7  + m.m1*n.m11  + m.m11*n.m15;
    output.m12 = m.m12*n.m0 + m.m13*n.m4 + m.m14*n.m8  + m.m15*n.m12;
    output.m13 = m.m12*n.m1 + m.m1*n.m5  + m.m14*n.m9  + m.m15*n.m13;
    output.m14 = m.m12*n.m2 + m.m13*n.m6 + m.m14*n.m10 + m.m15*n.m14;
    output.m15 = m.m12*n.m3 + m.m13*n.m7 + m.m14*n.m11 + m.m15*n.m15;
    
    return output;
}

/************************************************/

m33 copy_matrix( m33 input ){

    m33 output;    
     
    output.m0 =  input.m0;
    output.m1 =  input.m1;
    output.m2 =  input.m2;
    
    output.m3 =  input.m3;
    output.m4 =  input.m4;
    output.m5 =  input.m5;

    output.m6 =  input.m6;
    output.m7 =  input.m7;
    output.m8 =  input.m8;   
                
    return output;  

}

/************************************************/

m44 copy_matrix(m44 input){
    m44 output;
    
    output.m0  = input.m0;
    output.m1  = input.m1;
    output.m2  = input.m2;
    output.m3  = input.m3;

    output.m4  = input.m4;
    output.m5  = input.m5;
    output.m6  = input.m6;
    output.m7  = input.m7;

    output.m8  = input.m8;
    output.m9  = input.m9;
    output.m10 = input.m10;
    output.m11 = input.m11;

    output.m12 = input.m12;
    output.m13 = input.m13;
    output.m14 = input.m14;
    output.m15 = input.m15;
    
    return output;
}

/************************************************/

m33 test_indices33(void){
     // create a useless matrix with incrementing numbers  
     // for testing transpose and more 

    m33 output;    
     
    output.m0 =  0;
    output.m1 =  1;
    output.m2 =  2;
    
    output.m3 =  3;
    output.m4 =  4;
    output.m5 =  5;

    output.m6 =  6;
    output.m7 =  7;
    output.m8 =  8;   
                
    return output;      
}

/************************************************/

m44 test_indices44(void){
     // create a useless matrix with incrementing numbers  
     // for testing transpose and more 
    m44 output;    
     
    output.m0 =  0;
    output.m1 =  1;
    output.m2 =  2;
    output.m3 =  3;
    
    output.m4 =  4;
    output.m5 =  5;
    output.m6 =  6;
    output.m7 =  7;

    output.m8  = 8;   
    output.m9  = 9;
    output.m10 = 10; 
    output.m11 = 11;

    output.m12 = 12; 
    output.m13 = 13;
    output.m14 = 14; 
    output.m15 = 15;

    return output;   
}
        
/************************************************/
double determinant(m33 input){
    //     https://www.mathsisfun.com/algebra/matrix-determinant.html
    //           a b c  |  0 1 2  |        
    //      A =  d e f  |  3 4 5  |       
    //           g h i  |  6 7 8  |     
    //     |A| = a(ei − fh) − b(di − fg) + c(dh − eg)
    //     |A| = 0(48 − 57) − 1(38 − 56) + 2(37 − 46)

    double o = 0;
    o = input.m0 * ((input.m4 * input.m8)-(input.m5*input.m7)) - input.m1*((input.m3*input.m8)-(input.m5*input.m6)) +  input.m2*((input.m3*input.m7)-(input.m4*input.m6));
                
    return o;  

}

/************************************************/


double determinant(m44 input){
    
    double o; 

    // Laplace expansion method 
    // https://www.mathsisfun.com/algebra/matrix-determinant.html
    // plus  a times the determinant of the matrix that is not in a's row or column,
    // minus b times the determinant of the matrix that is not in b's row or column,
    // plus  c times the determinant of the matrix that is not in c's row or column,
    // minus d times the determinant of the matrix that is not in d's row or column,
    //            |               | A*        -     B*      +        C*    -         D*|
    // a  b  c  d | 0   1  2   3  | a -->     |  <--b-->    |     <--c-->  |      <--d |  
    // e  f  g  h | 4   5  6   7  | |  f g h  |  e  |  g  h |  e  f  |  h  | e  f  g | |
    // i  j  k  l | 8   9  10  11 |    j k l  |  i     k  l |  i  j     l  | i  j  k   |
    // m  n  o  p | 12  13 14  15 |    n o p  |  m     o  p |  m  n     p  | m  n  o   | 
    m44 a = copy_matrix(input); 
    m33 a33 = new_m33(a.m5,a.m6,a.m7,a.m9,a.m10,a.m11,a.m13,a.m14,a.m15);
    m33 b33 = new_m33(a.m4,a.m6,a.m7,a.m8,a.m10,a.m11,a.m12,a.m14,a.m15);
    m33 c33 = new_m33(a.m4,a.m5,a.m7,a.m8,a.m9 ,a.m11,a.m12,a.m13,a.m15);
    m33 d33 = new_m33(a.m4,a.m5,a.m6,a.m8,a.m9 ,a.m10,a.m12,a.m13,a.m14);
    o = (a.m0 * determinant(a33)) - (a.m1*determinant(b33)) + (a.m2*determinant(c33)) - (a.m3*determinant(d33));
    return o;   
}

/************************************************/

// standard indicies  |   transposed indicies
// 1  2  3            |   1 4 7
// 4  5  6            |   2 5 8
// 7  8  9            |   3 6 9  

void transpose(m33 *input){

    m33 tmp;  
    
    tmp.m0 = input->m0;
    tmp.m1 = input->m3;
    tmp.m2 = input->m6;
    tmp.m3 = input->m1;
    tmp.m4 = input->m4;
    tmp.m5 = input->m7;
    tmp.m6 = input->m2;
    tmp.m7 = input->m5;
    tmp.m8 = input->m8;    

    input->m0 = tmp.m0;
    input->m1 = tmp.m1;
    input->m2 = tmp.m2;
    input->m3 = tmp.m3;
    input->m4 = tmp.m4;
    input->m5 = tmp.m5;
    input->m6 = tmp.m6;
    input->m7 = tmp.m7;
    input->m8 = tmp.m8;  
    
}

/************************************************/
void transpose(m44 *input){
    m44 tmp; 

    tmp.m0  = input->m0;
    tmp.m1  = input->m4;
    tmp.m2  = input->m8;
    tmp.m3  = input->m12;
    tmp.m4  = input->m1;
    tmp.m5  = input->m5;
    tmp.m6  = input->m9;
    tmp.m7  = input->m13;
    tmp.m8  = input->m2;
    tmp.m9  = input->m6;
    tmp.m10 = input->m10;
    tmp.m11 = input->m14;
    tmp.m12 = input->m3;
    tmp.m13 = input->m7;
    tmp.m14 = input->m11;
    tmp.m15 = input->m15;

    input->m0  = tmp.m0;
    input->m1  = tmp.m1;
    input->m2  = tmp.m2;
    input->m3  = tmp.m3;
    input->m4  = tmp.m4;
    input->m5  = tmp.m5;
    input->m6  = tmp.m6;
    input->m7  = tmp.m7;
    input->m8  = tmp.m8;
    input->m9  = tmp.m9;
    input->m10 = tmp.m10;
    input->m11 = tmp.m11;
    input->m12 = tmp.m12;
    input->m13 = tmp.m13;
    input->m14 = tmp.m14;
    input->m15 = tmp.m15;

}

/************************************************/
void print_matrix(m33 input){
    printf("%f %f %f\n", input.m0, input.m1, input.m2 );
    printf("%f %f %f\n", input.m3, input.m4, input.m5 );
    printf("%f %f %f\n", input.m6, input.m7, input.m8 );    
}

void print_matrix(m44 input){
    printf("%f %f %f %f\n", input.m0  , input.m1  , input.m2  , input.m3 );
    printf("%f %f %f %f\n", input.m4  , input.m5  , input.m6  , input.m7 );
    printf("%f %f %f %f\n", input.m8  , input.m9  , input.m10 , input.m11 );
    printf("%f %f %f %f\n", input.m12 , input.m13 , input.m14 , input.m15 );        
}

/************************************************/

m33 m33_from_euler(double xrot, double yrot, double zrot){
    // build rotationY  
    m33 y_matrix = identity33();
    y_matrix.m0  =  cos( dtr( yrot ) );
    y_matrix.m2  = -sin( dtr( yrot ) );
    y_matrix.m6  =  sin( dtr( yrot ) );
    y_matrix.m8  =  cos( dtr( yrot ) );
               
    // build rotationZ  
    m33 z_matrix = identity33();
    z_matrix.m0 =  cos( dtr( zrot ) );
    z_matrix.m1 =  sin( dtr( zrot ) );
    z_matrix.m3 = -sin( dtr( zrot ) );
    z_matrix.m4 =  cos( dtr( zrot ) );
    
    m33 tmp_matr =  mult( y_matrix , z_matrix); 

    // build rotationX   
    m33 x_matrix = identity33();
    x_matrix.m4  =  cos( dtr( xrot ) ); 
    x_matrix.m5  =  sin( dtr( xrot ) ); 
    x_matrix.m7  = -sin( dtr( xrot ) );
    x_matrix.m8  =  cos( dtr( xrot ) );
    
    m33 output = mult( x_matrix , tmp_matr); 
    return output;
}

/************************************************/

m44 m44_from_euler(double xrot, double yrot, double zrot){
    // build rotationY  
    m44 y_matrix = identity44();
    y_matrix.m0  =  cos( dtr( yrot ) );
    y_matrix.m2  = -sin( dtr( yrot ) );
    y_matrix.m8  =  sin( dtr( yrot ) );
    y_matrix.m10 =  cos( dtr( yrot ) );

    // build rotationZ (see diagram above) 
    m44 z_matrix = identity44();
    z_matrix.m0 =  cos( dtr( zrot ) );
    z_matrix.m1 =  sin( dtr( zrot ) );
    z_matrix.m4 = -sin( dtr( zrot ) );
    z_matrix.m5 =  cos( dtr( zrot ) );
    m44 tmp_matr = mult(y_matrix, z_matrix );

    // build rotationX (see diagram above) 
    m44 x_matrix = identity44();
    x_matrix.m5  =  cos( dtr( xrot ) ); 
    x_matrix.m6  =  sin( dtr( xrot ) ); 
    x_matrix.m9  = -sin( dtr( xrot ) );
    x_matrix.m10 =  cos( dtr( xrot ) );
 
    m44 output = mult( x_matrix, tmp_matr );

    return output;
}

/************************************************/
    // m33 rotate_pts_3d(points, xrot, yrot, zrot):
    //     //  The "standard" 9 element, Row major, 3X3 rotation matrix used by Maya
    //     //   ⎡0  1  2⎤      xx xy xz 
    //     //   ⎢3  4  5⎥      yx yy yz 
    //     //   ⎣6  7  8⎦      zx zy zz 
    //     //   ------------------------------
    //     //        Rotate Y matrix     
    //     //   ⎡cos(y)  0      -sin(y) ⎤ 
    //     //   ⎢0       1       0      ⎥ 
    //     //   ⎣sin(y)  0       cos(y) ⎦ 
    //     //   ------------------------------
    //     //        Rotate Z  matrix 
    //     //   ⎡  cos(z)  sin(z)  0    ⎤ 
    //     //   ⎢ -sin(z)  cos(z)  0    ⎥
    //     //   ⎣  0       0       1    ⎦
    //     //   ------------------------------
    //     //        Rotate X matrix  
    //     //   ⎡ 1       0         0   ⎤    
    //     //   ⎢ 0    cos(x)   sin(x)  ⎥  
    //     //   ⎣ 0   -sin(x)   cos(x)  ⎦  
    //     //   ------------------------------            
    //     dtr = self.mu.dtr
    //     # build rotationY (see diagram above) 
    //     y_matrix =  self.identity
    //     y_matrix[0]  =  math.cos(dtr( yrot ))
    //     y_matrix[2]  = -math.sin(dtr( yrot ))
    //     y_matrix[6]  =  math.sin(dtr( yrot ))
    //     y_matrix[8]  =  math.cos(dtr( yrot ))
    //     ####                
    //     # build rotationZ (see diagram above) 
    //     z_matrix    =  self.identity
    //     z_matrix[0] =  math.cos(dtr( zrot ))
    //     z_matrix[1] =  math.sin(dtr( zrot ))
    //     z_matrix[3] = -math.sin(dtr( zrot ))
    //     z_matrix[4] =  math.cos(dtr( zrot ))
    //     tmp_matr =  y_matrix * z_matrix 
    //     ####
    //     # build rotationX (see diagram above) 
    //     x_matrix =  self.identity
    //     x_matrix[4]  =   math.cos(dtr( xrot )) 
    //     x_matrix[5]  =   math.sin(dtr( xrot )) 
    //     x_matrix[7]  =  -math.sin(dtr( xrot ))
    //     x_matrix[8]  =   math.cos(dtr( xrot ))
    //     rotation_33 = x_matrix * tmp_matr 
    //     ############ 
    //     return rotation_33.batch_mult_pts(points)


/************************************************/
    // m33 batch_mult_pts(self, pts):
    //     // iterate a list of points and multiply them by this matrix 
    //     tmp_buffer = []
    //     out = None
    //     for pvec in pts:  
    //         tmp_buffer.append( self * pvec )
    //     return tmp_buffer
/************************************************/

    // m33 align_two_vec3(self, a, b):
    //     //UNFINISHED
    //     //     https://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d
    //     //      ⎛cosθ −sinθ  0 ⎞
    //     //  G = ⎜sinθ  cosθ  0 ⎟ 
    //     //      ⎝0      0    1 ⎠
    //     //       --------------------------------------------------------------
    //     //       Given our unit vectors, we note that cosθ=A⋅B, and sinθ=||A×B||
    //     //      ⎛ A.B    -||A×B||    0 ⎞
    //     //  G = ⎜||A×B||   A.B       0 ⎟ 
    //     //      ⎝0          0        1 ⎠
    //     theta = self.mu.dtr(angle)
    //     #axis = vec3.normal
    //     axis = vec3.as_np
    //     #tmpm33 = self.identity
    //     tmpm33 = expm(np.cross(np.eye(3), axis / np.linalg.norm(axis) * theta)) 
    //     return self.from_np(tmpm33)


/************************************************/
    /*       


    def batch_mult_pts(self, pts):
        """ sub component of matrix rotate function 
            multiply a 4X4 matrix by a list of points 
        """
        #debug - make work with other types??

        tmp_buffer = []
        out = None
        for pvec in pts:  
            #total experiment for perpective/homogeneous coordinates  
            #pvec = vec4(pt[0], pt[1], pt[2], 1)
            #print(pvec)
            tmp_buffer.append( self * pvec )
 
        return tmp_buffer

    def rotate_pts_3d(self, points, xrot, yrot, zrot):
        """
           The "standard" 16 element, Row major, 4X4 rotation matrix 

           ⎡0   1   2   3  ⎤    ⎡   XVEC    0 ⎤
           ⎢4   5   6   7  ⎥    ⎢   YVEC    0 ⎥
           ⎢8   9   10  11 ⎥    ⎢   ZVEC    0 ⎥
           ⎣12  13  14  15 ⎦    ⎣ 0  0  0   0 ⎦

           ⎡0   1   2   3 ⎤     ⎡xx  xy  xz  0⎤
           ⎢4   5   6   7 ⎥     ⎢yx  yy  yz  0⎥
           ⎢8   9   10  11⎥     ⎢zx  zy  zz  0⎥
           ⎣12  13  14  15⎦     ⎣0   0   0   0⎦
           ------------------------------
           rotate Y matrix     
           ⎡  cos(y)  0      -sin(y)  0 ⎤ 
           ⎢  0       1       0       0 ⎥ 
           ⎢  sin(y)  0       cos(y)  0 ⎥ 
           ⎣  0       0       0       1 ⎦
           ------------------------------
           rotate Z  matrix 
           ⎡  cos(z)  sin(z)  0       0 ⎤ 
           ⎢ -sin(z)  cos(z)  0       0 ⎥
           ⎢  0       0       1       0 ⎥
           ⎣  0       0       0       1 ⎦
           ------------------------------
           rotate X matrix  
           ⎡  1       0       0       0 ⎤  
           ⎢  0       cos(x)  sin(x)  0 ⎥  
           ⎢  0      -sin(x)  cos(x)  0 ⎥  
           ⎣  0       0       0       1 ⎦ 
        """
        dtr = self.mu.dtr

        ####
        #build rotationY (see diagram above) 
        y_matrix     =  self.identity
        y_matrix[0]  =  math.cos(dtr( yrot ))
        y_matrix[2]  = -math.sin(dtr( yrot ))
        y_matrix[8]  =  math.sin(dtr( yrot ))
        y_matrix[10] =  math.cos(dtr( yrot ))

        ####                
        #build rotationZ (see diagram above) 
        z_matrix    =  self.identity
        z_matrix[0] =  math.cos(dtr( zrot ))
        z_matrix[1] =  math.sin(dtr( zrot ))
        z_matrix[4] = -math.sin(dtr( zrot ))
        z_matrix[5] =  math.cos(dtr( zrot ))
        tmp_matr = y_matrix * z_matrix 

        ####
        #build rotationX (see diagram above) 
        x_matrix     =  self.identity
        x_matrix[5]  =   math.cos(dtr( xrot )) 
        x_matrix[6]  =   math.sin(dtr( xrot )) 
        x_matrix[9]  =  -math.sin(dtr( xrot ))
        x_matrix[10] =   math.cos(dtr( xrot ))
        rotation_44 = x_matrix * tmp_matr

        ############ 
        return rotation_44.batch_mult_pts(points)


/************************************************/

quaternion new_quaternion( double x, double y, double z, double w ){
    quaternion output;

    output.x = x;
    output.y = y;
    output.z = z;
    output.w = w;

    return output; 
}

/************************************************/
quaternion quaternion_identity(){
    return new_quaternion(1.0,0,0,0);
}

/************************************************/

void set(  quaternion *input, double x, double y, double z, double w){
    input->w = w;
    input->x = x;
    input->y = y;
    input->z = z;    
}

/************************************************/

void print_quaternion(quaternion input){
    printf("%f %f %f %f\n", input.w, input.x, input.y, input.z );
}

/************************************************/


void quaternion_rotx(quaternion *input, double theta){
    double theta_over2 = theta * .5;
    input->w = cos( theta_over2 );
    input->x = sin( theta_over2 );
}

/************************************************/

void quaternion_roty(quaternion *input, double theta){
    double theta_over2 = theta * .5;
    input->w = cos( theta_over2 );
    input->y = sin( theta_over2 );
}


/************************************************/
void quaternion_rotz(quaternion *input, double theta){
    double theta_over2 = theta * .5;
    input->w = cos( theta_over2 );
    input->z = sin( theta_over2 );
}

/************************************************/
void quaternion_fr_euler(quaternion *input, double h, double p, double b){
    double sp,sb,sh,cp,cb,ch = 0;

    sp = sin( p*.5); 
    sb = sin( b*.5); 
    sh = sin( h*.5); 
    cp = cos( p*.5); 
    cb = cos( b*.5); 
    ch = cos( h*.5); 
       
    // if (trans_type == 'obj2inertial'){
    input->w =  ch * cp * cb + sh * sp * sb;
    input->x =  ch * sp * cb + sh * cp * sb;
    input->y = -ch * sp * sb + sh * cp * cb;
    input->z = -sh * sp * cb + ch * cp * sb;
    //}

    // elif (trans_type == 'inertial2ob'){
    // input->w =  ch * cp * cb + sh * sp * sb;
    // input->x = -ch * sp * cb - sh * cp * sb;
    // input->y =  ch * sp * sb - sh * cp * cb;
    // input->z =  sh * sp * cb - ch * cp * sb;
    //}
 
    // else:
    //     printf( "Invalid trans_type!" ) 

}

/************************************************/
double quaternion_mag(quaternion *input){
    double mag = float( sqrt(  input->w * input-> w + 
                               input->x * input-> x + 
                               input->y * input-> y + 
                               input->z * input-> z) );
    return mag;                                 
}

/************************************************/
void quaternion_normalize(quaternion *input){
    double mag = quaternion_mag( input ); 
    if (mag > 0){
        double oneOverMag = ( 1.0 / mag);
        
        input->w = input->w * oneOverMag;
        input->x = input->w * oneOverMag;
        input->y = input->w * oneOverMag;
        input->z = input->w * oneOverMag;

    } else {
        set(input, 1.0, 0, 0, 0);  
    }    
}

/************************************************/
double dot_product(quaternion a, quaternion b){ 
    return a.x * b.x + a.y * b.y + a.z * b.z + a.z * a.z;   
}

/************************************************/
quaternion conjugate(quaternion q){
    quaternion output;
   
    q.w =  q.w;
    q.x = -q.x;
    q.y = -q.y;
    q.z = -q.z;

    return output;
}        

/************************************************/
quaternion multiply(quaternion a){
    quaternion output;
   
    double w = a.w;
    double x = a.x;
    double y = a.y;
    double z = a.z;

    output.w = w * a.w - x * a.x - y * a.y - z * a.z;
    output.x = w * a.x + x * a.w + z * a.y + y * a.z;
    output.y = w * a.y + y * a.w + x * a.z + z * a.x;
    output.z = w * a.z + z * a.w + y * a.x + x * a.y;
 
    return output;
}

/************************************************/
quaternion quaternion_from_m33(m33 input){
         
    //   m11  m12 m13 
    //   m21  m22 m23 
    //   m31  m32 m33 
            
    double w_sq_min = input.m0 + input.m4 + input.m8;
    double x_sq_min = input.m0 - input.m4 - input.m8;
    double y_sq_min = input.m4 - input.m0 - input.m8;
    double z_sq_min = input.m8 - input.m0 - input.m4;

    quaternion output;
   
    int maxIndex = 0;
    double max = w_sq_min;
    
    if (x_sq_min > max){
        max = x_sq_min;
        maxIndex = 1;
    }
    if (y_sq_min > max){
        max = y_sq_min;
        maxIndex = 2;
    }
    if (z_sq_min > max){
        max = z_sq_min;
        maxIndex = 3;
    }

    max = sqrt (max + 1.0) * 0.5;
    double mult = 0.25 / max;
        
    if (maxIndex == 0){
        output.w = max;
        output.x = (input.m5 - input.m7) * mult;
        output.y = (input.m6 - input.m2) * mult;
        output.z = (input.m1 - input.m3) * mult;
    }
    
    if (maxIndex == 1){
        output.x = max;
        output.w = (input.m5 - input.m7) * mult;
        output.y = (input.m1 + input.m3) * mult;
        output.z = (input.m6 + input.m2) * mult;
    }

    if (maxIndex == 2){
        output.y = max;
        output.w = (input.m6 - input.m2) * mult;
        output.x = (input.m1 + input.m3) * mult;
        output.z = (input.m5 + input.m7) * mult;
    }

    if (maxIndex == 3){
        output.z = max;
        output.w = (input.m1 - input.m3) * mult;
        output.x = (input.m6 + input.m2) * mult;
        output.y = (input.m5 + input.m7) * mult;
    }

    return output;

}


/************************************************/
m33 quaternion_to_m33(quaternion * q ){  
    /*
        hmm - does input q need to be a pointer if we only read? 
        Does it matter?
    */

    m33 mo = identity33();  

    //if (trans_type == 'inertial2obj'):

    mo.m0 = 1.0 - 2.0 * (q->y * q->y + q->z * q->z);
    mo.m1 = 2.0 * (q->x * q->y + q->w * q->z);
    mo.m2 = 2.0 * (q->x * q->z + q->w * q->y);
    mo.m3 = 2.0 * (q->x * q->y - q->w * q->z);
    mo.m4 = 1.0 - 2.0 * (q->x * q->x + q->z * q->z);
    mo.m5 = 2.0 * (q->y * q->z + q->w * q->x);
    mo.m6 = 2.0 * (q->x * q->z + q->w * q->y);
    mo.m7 = 2.0 * (q->y * q->z - q->w * q->x);
    mo.m8 = 1.0 - 2.0 * (q->x * q->x + q->y * q->y);
    return mo; 

    // // elif (trans_type == 'obj2inertial'):
    // mo.m0 = 1.0 - 2.0 * (q->y * q->y + q->z * q->z);
    // mo.m1 = 2.0 * (q->x * q->y - q->w * q->z);
    // mo.m2 = 2.0 * (q->x * q->z + q->w * q->y);
    // mo.m3 = 2.0 * (q->x * q->y + q->w * q->z);
    // mo.m4 = 1.0 - 2.0 * (q->x * q->x + q->z * q->z);
    // mo.m5 = 2.0 * (q->y * q->z - q->w * q->x);
    // mo.m6 = 2.0 * (q->x * q->z + q->w * q->y);
    // mo.m7 = 2.0 * (q->y * q->z + q->w * q->x);
    // mo.m8 = 1.0 - 2.0 * (q->x * q->x + q->y * q->y);
    // return mo; 
}

/************************************************/
 
void quaternion_set_axis( quaternion* input, vec3 axis, double theta){
    // #assert((vectorMag(axis) - 1.0f) < 0.01f);

    double thetaOver2 = theta * .5;
    double sinThetaOver2 = sin(thetaOver2);

    input->w = cos(thetaOver2);
    input->x = axis.x * sinThetaOver2;
    input->y = axis.y * sinThetaOver2;
    input->z = axis.z * sinThetaOver2;

}

/************************************************/
double quaternion_get_rot_angle(quaternion q){
    double thetaOver2 = acos(q.w);
    return thetaOver2 * 2.0;
}

/************************************************/

vec3 quaternion_get_rot_axis(quaternion q){
    double sin_theta_over2Sq = 1.0 - q.w * q.w;
    double one_over_sin_theta = 1.0 / sqrt(sin_theta_over2Sq);
        
    double nx = q.x * one_over_sin_theta;
    double ny = q.y * one_over_sin_theta;
    double nz = q.z * one_over_sin_theta;
       
    return newvec3(nx, ny, nz);
}

/************************************************/
/*
    def quaternion_slerp (self, q0, q1, t):
 
        if (t <= 0):
            return q0
        if (t >= 1):
            return q1

        cosOmega = self.dot_product(q0, q1)
        
        q1w = q1.w
        q1x = q1.x
        q1y = q1.y
        q1z = q1.z
        
        # avoid getting different results
        if (cosOmega < 0):
            q1w = -q1w
            q1x = -q1x
            q1y = -q1y
            q1z = -q1z
            cosOmega -= cosOmega
        
        k0 = 0
        k1 = 0
        # avoid sth over 0 happening
        if cosOmega > 0.999:
            k0 = 1.0 - t
            k1 = t
        else:
        
            sinOmega = math.sqrt(1.0 - cosOmega * cosOmega)
            omega = math.atan2 (sinOmega, cosOmega)
            oneOverSinOmega = 1.0/sinOmega
            
            k0 = math.sin ((1.0- t) * omega) * oneOverSinOmega
            k1 = math.sin (t*omega) * oneOverSinOmega
        
        ####
        result = type(self)()
        
        result.x = k0 * q0.x + k1 * q1x;
        result.y = k0 * q0.y + k1 * q1y;
        result.z = k0 * q0.z + k1 * q1z;
        result.w = k0 * q0.w + k1 * q1w;
        
        return result

*/

/************************************************/


/*
class spherical(object):
    def __init__(self,r=0,t=0,p=0):
        """ r = radius 
            t = theta 
            p = phi  (not needed for polar)
        """
        #self.mu = math_util() 
        self.r=r
        self.t=t
        self.p=p

    def __repr__(self):
        return '(%s, %s, %s)' % (self.r, self.p, self.t)

    def __getitem__(self, index):
        if index==0:
            return self.r
        if index==1:
            return self.t
        if index==2:
            return self.p

    def __setitem__(self, key, item):
        if key==0:
            self.r = item
        if key==1:
            self.t = item
        if key==2:
            self.p = item

    def to_cartesian(self, deg_rad='rad'):
        x = self.r * math.sin(self.p) * math.cos(self.t)
        y = self.r * math.sin(self.p) * math.sin(self.t)
        z = self.r * math.cos(self.p)    
        return (x,y,z)
 

    //  UNTESTED 
    //  from David Gould's book
    //  Complete Maya Programing II 
    //  page 14 
    //  r = length( x y z )
    //  p = tan-1 (length(x y), z)) 
    //  t = tan-1 (y,x)
    
    // def from_cartesian(self, vec3):
    //     if isinstance(vec3,tuple):
    //         vec3 = vec3(vec3[0],vec3[1],vec3[2]) 
    //     r = vec3.length  
    //     p = math.atan( -1*math.sqrt(vec3[0]*vec3[0]+vec3[1]*vec3[1])*vec3[2]  ) 
    //     t = math.atan( -1*(vec3[1]*vec3[0]))
    //     print('### r %s p %s t %s '%(r,p,t))
    //     self.r=r
    //     self.t=t
    //     self.p=p
    //     #return type(self)(r,p,t)


    //UNTESTED
    //polar coordinates are spherical without the phi element 
    //from David Gould's book
    //Complete Maya Programing II 
    //page 13         
    
    // void cartesian_to_polar (vec3){
    //     if isinstance(vec3,tuple):
    //         vec3 = vec3(vec3[0],vec3[1],0) #Z is ignored  
    //     r = math.sqrt(vec3[0]*vec3[0] + vec3[1]*vec3[1])  
    //     t = math.atan( vec3[1]*vec3[0] ) 
    //     print('### cartesian to polar  r %s t %s '%(r,t))
    //     self.r=r
    //     self.t=t
    //     self.p=0
    // }

*/


/************************************************/


/*
class vec4(object):
    """ untested - 
        homogeneous coordinate experiment  
    """

    def __init__(self,x=0,y=0,z=0,w=1):
        self.x=x
        self.y=y
        self.z=z
        self.w=w  
        
        #self.mu = math_util() 

    def __mul__(self, other):
        # https://www.tomdalling.com/blog/modern-opengl/explaining-homogenous-coordinates-and-projective-geometry/
        
        if isinstance(other, float) or isinstance(other, int):
            #return type(self)(self.x*other, self.y*other, self.z*other)
            
            new_x = self.x * other 
            new_y = self.y * other 
            new_z = self.z * other 
            new_w = self.w * other 
            return type(self)( (new_x/new_w), (new_y/new_w), (new_z/new_w), new_w )


    def __repr__(self):
        return '(%s, %s, %s, %s)' % (self.x, self.y, self.z, self.w)

    def __getitem__(self, index):
        if index==0:
            return self.x
        if index==1:
            return self.y
        if index==2:
            return self.z
        if index==3:
            return self.w

    def __setitem__(self, key, item):
        if key==0:
            self.x = item
        if key==1:
            self.y = item
        if key==2:
            self.z = item
        if key==3:
            self.w = item

    def insert(self, iterable):
        """ convert an np.array, tuple or list  to vec3  
            does not check size, so just assume 3 items (x,y,z)
        """
        if isinstance(iterable, vec3):
            self.from_vec3(iterable) 

        if isinstance(iterable, list) or isinstance(iterable, tuple):
            self.x = iterable[0]
            self.y = iterable[1]            
            self.z = iterable[2]

            if len(iterable)==3: 
                self.w = 1  
            if len(iterable)==4:
                self.w = iterable[3]                 

        return self 

    def to_vec3(self):
        """ vec4 to vec3 - 
            divide the weight across the other 3 axis 
        """
        w = self.w
        return vec3( (self.x/w), (self.y/w), (self.z/w) )

    def from_vec3(self, vec3):
        """ vec4 from a vec3, simply add a weight of 1 """
        self.x = vec3.x        
        self.y = vec3.y  
        self.z = vec3.z
        self.w = 1  

#[xyzw]∗⎡⎣⎢⎢⎢m00m10m20m30m01m11m21m31m02m12m22m32m03m13m23m33⎤⎦⎥⎥⎥
#x′=x∗m00+y∗m10+z∗m20+w∗m30y′=x∗m01+y∗m11+z∗m21+w∗m31z′=x∗m02+y∗m12+z∗m22+w∗m32w′=x∗m03+y∗m13+z∗m23+w∗m33



###############################################


    def buildPerspProjMat(self, fov, aspect, znear, zfar):
        """
            UNTESTED 
            
            transformation matrix that changes the W element of each vertex. 
            After the the camera matrix is applied to each vertex, 
            but before the projection matrix is applied, 
            the Z element of each vertex represents the distance away from the camera. 
            the larger Z is, the more the vertex should be scaled down


            http://stackoverflow.com/questions/8633034/basic-render-3d-perspective-projection-onto-2d-screen-with-camera-without-openg

            use homogenous transformations and coordinates. 

            take a point in space and:
                Position it relative to the camera using the model matrix.
                Project it either orthographically or in perspective using the projection matrix.
                Apply the viewport transformation to place it on the screen.




        """

        PI_OVER_360 = 0.00872664625
        #xymax = 100
        xymax = znear * math.tan(fov * PI_OVER_360);

        ymin = -xymax
        xmin = -xymax

        width = xymax - xmin
        height = xymax - ymin

        depth =  zfar - znear
        
        #avoid those pesky divide by zero errors 
        if depth==0:
            depth=.1
        if width==0:
            width=1
        if height==0:
            height=1                        

        q     = -(zfar + znear) / depth
        qn    = -2 * (zfar * znear) / depth

        w = 2 * znear / width
        w = w / aspect
        h = 2 * znear / height

        m = self.identity

        # m[0]  = w; m[4]  = 0; m[8]  = 0 ; m[12] = 0
        # m[1]  = 0; m[5]  = h; m[9]  = 0 ; m[13] = 0
        # m[2]  = 0; m[6]  = 0; m[10] = q ; m[14] = qn 
        # m[3]  = 0; m[7]  = 0; m[11] =-1 ; m[15] = 0

        m[0]=w ; m[1]=0  ; m[2]=0  ; m[3]=0;
        m[4]=0 ; m[5]=h  ; m[6]=0  ; m[7]=0;
        m[8]=0 ; m[9]=0  ; m[10]=q ; m[11]=-1;
        m[12]=0; m[13]=0 ; m[14]=qn; m[15]=0;
        
        #print(m)
        return m

*/


