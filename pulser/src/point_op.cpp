/*****************************************************************************/
/* 
    point_op.cpp

    class point_op:
        Deal exclusively with 3D point geometry 
        Point Generators == generate patterns of points.  
        create 2D/3D coordinates in all manner of fun ways. 
    

    class polygon_ops: (3D Object Supereclass)
        Deal with 3D point geometry in the form old polygons, triangles and quads mostly



    
    MIT License

    Copyright (c) 2026 Keith Legg - keithlegg23@gmail.com

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.


*/
         

/*****************************************************************************/


#include <iostream>
using namespace std; 

#include "math.h"

#include "point_op.h"
#include "Vectors.h"

#include <cmath>
#include <vector>

//#include "math_op.h"
#include "point_op.h"

#include <iostream>



#define DEG_TO_RAD 0.0174532925
#define RAD_TO_DEG 57.29577951


float deg_to_rad ( float deg){
   return deg * DEG_TO_RAD;
}

float rad_to_deg ( float rad){
   return rad * RAD_TO_DEG;
}


/***************************************/

//test of inheritance 
void  polygon_ops::hello(void)
{
    cout << "hello from point opszzz !! \n";
}



/***************************************/
/*
   Overridden function in obj_model 
   this is empty
*/
void polygon_ops::reset(void){;}


/***************************************/
int polygon_ops::numpts(void){
    // NOT USED YET 
    std::cout << "numpts NOT DONE\n";
    return 0;
}




/***************************************/



/***************************************/



/***************************************/



/***************************************/

/*
    3d lerp function ?? 
    
    https://keithmaggio.wordpress.com/2011/02/15/math-magician-lerp-slerp-and-nlerp/

    https://stackoverflow.com/questions/18755251/linear-interpolation-of-three-3d-points-in-3d-space

    // You can express P4 coordinates in the P1P2P3 vector basis.

    x4 = x1 + A * (x2 - x1) + B * (x3 - x1)
    y4 = y1 + A * (y2 - y1) + B * (y3 - y1)

    // This is easy-to-solve linear equation system. You have to find A and B coefficients, then use them to calculate z-coordinate
    z4 = z1 + A * (z2 - z1) + B * (z3 - z1)
*/


//DEBUG - this seems to be running backwards
//0 is end of line instead of start, etc 

void point_ops::lerp_along( Vector3* output,
                            Vector3 fpos, 
                            Vector3 spos, 
                            float dist )
{
 
    Vector3 npos = Vector3(0,0,0);

    //DEBUG  &&dist<1.0  is an attempt to stop the "popping bug"
    //X
    if(fpos.x-spos.x!=0 &&dist<1.0 )
    {
        npos.x =  spos.x+(((fpos.x-spos.x))*(-dist+1));
    }else if(dist<1.0)
    {
        npos.x =  spos.x+(((fpos.x-spos.x))*(dist+1));        
    }

    //Y
    if (fpos.y-spos.y!=0&&dist<1.0)
    { 
        npos.y =  spos.y+(((fpos.y-spos.y))*(-dist+1));         
    }else if(dist<1.0)
    {
        npos.y =  spos.y+(((fpos.y-spos.y))*(dist+1));        
    }

    //Z
    if (fpos.z-spos.z!=0&&dist<1.0)
    { 
        npos.z =  spos.z+(((fpos.z-spos.z))*(-dist+1));        
    }else if(dist<1.0)
    {

        npos.z =  spos.z+(((fpos.z-spos.z))*(dist+1));                     
    }

    //I really have no idea what im doing. 
    //This is a hack to stop the weird math bug when it shoots to zero
    if(npos.x==0 && npos.y == 0 && npos.x==0)
    {
        //std:cout << "ALL ZEROS!!!!! \n";
    }else{
        *output = npos;        
    }


};


/***************************************/
void point_ops::locate_pt_along3d(std::vector<Vector3>* output,
                                 Vector3 fpos, 
                                 Vector3 spos, 
                                 int num)
{
    
    int n = 0;

    for (n=0;n<num;n++)
    {
        Vector3 npos;

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


/***************************************/
void point_ops::cubic_bezier(std::vector<Vector3>* output, 
                            std::vector<Vector3>* colors,
                            int* numvecs,
                            int draw_steps, 
                            Vector3 start, 
                            Vector3 ctrl1, 
                            Vector3 ctrl2,
                            Vector3 end
                        )
{
       
        float x,y,z,t,tt,ttt,u,uu,uuu = 0;
        float i;
 
        Vector3 color = Vector3(1.0,0.0,0.0);

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

            Vector3 out;
            out.x = x; 
            out.y = y;
            out.z = z;

            output->push_back( out );
            colors->push_back( color );
            *numvecs = *numvecs+1;

        }


        //return out;
}



/***************************************/

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

int point_ops::get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, 
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


/***************************************/
void point_ops::calc_square ( std::vector<Vector3> *out_coords, 
                              Vector3* origin, uint axis, float size) 
{

    Vector3 tmp = Vector3(0, 0, 0);

    if(axis==0)
    {
        tmp.set(0, origin->x-size, origin->y+size); //#tl
        out_coords->push_back( tmp ); 
        
        tmp.set(0, origin->x+size, origin->y+size); //#tr
        out_coords->push_back( tmp ); 
        
        tmp.set(0, origin->x+size, origin->y-size); //#br
        out_coords->push_back( tmp ); 
        
        tmp.set(0, origin->x-size, origin->y-size); //#bl        
        out_coords->push_back( tmp ); 
        
        //make periodic - append first point back  
        tmp.set(0, origin->x-size, origin->y+size);         
        out_coords->push_back( tmp ); 

    }

    if(axis==1)
    {
        tmp.set(origin->x-size, 0, origin->y+size); //#tl
        out_coords->push_back( tmp ); 
        
        tmp.set(origin->x+size, 0, origin->y+size); //#tr
        out_coords->push_back( tmp ); 
        
        tmp.set(origin->x+size, 0, origin->y-size); //#br
        out_coords->push_back( tmp ); 
        
        tmp.set(origin->x-size, 0, origin->y-size); //#bl        
        out_coords->push_back( tmp ); 
        
        //make periodic - append first point back  
        tmp.set(origin->x-size, 0, origin->y+size);         
        out_coords->push_back( tmp ); 

    }


    if(axis==2)
    {
        tmp.set(origin->x-size, origin->y+size, 0); //#tl
        out_coords->push_back( tmp ); 
        
        tmp.set(origin->x+size, origin->y+size, 0); //#tr
        out_coords->push_back( tmp ); 
        
        tmp.set(origin->x+size, origin->y-size, 0); //#br
        out_coords->push_back( tmp ); 
        
        tmp.set(origin->x-size, origin->y-size, 0); //#bl        
        out_coords->push_back( tmp ); 
        
        //make periodic - append first point back  
        tmp.set(origin->x-size, origin->y+size, 0);         
        out_coords->push_back( tmp ); 

    }

    //else:
    //    out.append( ( -size,  size)  )
    //    out.append( (  size,  size)  )
    //    out.append( (  size, -size)  )
    //    out.append( ( -size, -size)  )
 
    

}


/***************************************/
//DEBUG - FIX THIS 

void point_ops::calc_circle ( std::vector<Vector3> *out_coords, 
                              uint numdiv, uint axis, 
                              float x_orig, float y_orig, float dia )
{

    //check for null pointer 
    if( out_coords != 0) 
    {

        //float rot_offset = 0;
        

        uint divamt = (uint)(360/numdiv);

        float px = 0;
        float py = 0;

        //for (uint i = 0; i<=360; i++)
        for (uint i = 0; i<=360; i=(i+divamt) )
        {  
           // this is - orig vars not initializing properly 
           // px = x_orig + sin(deg_to_rad(i))*dia ;
           // py = y_orig + cos(deg_to_rad(i))*dia ;
           
           px = sin(deg_to_rad(i))*dia ;
           py = cos(deg_to_rad(i))*dia ;

           Vector3 tmp = Vector3(0, 0, 0);

           if(axis==0){ tmp.set(0,  px , py); }
           if(axis==1){ tmp.set(px, 0  , py); }
           if(axis==2){ tmp.set(px, py , 0 ); }                      
        
           //std::cout << "adding pt  " << i << "\n";
           out_coords->push_back( tmp );
        }  

    }//check null pointer 


}

/***************************************/

void point_ops::calc_line( pt2d *out_coords, int *pt1, int *pt2, int *num)
{

    int x1 = pt1[0];
    int y1 = pt1[1];
    int const x2 = pt2[0];
    int const y2 = pt2[1];

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



/***************************************/

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


/***************************************/


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

/***************************************/

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


/***************************************/
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

/***************************************/
// Z axis only centroid 
float polygon_ops::triangle_mean_z(Vector3 p1, Vector3 p2, Vector3 p3)
{
    float z1 = p1.z;
    float z2 = p2.z;
    float z3 = p3.z;
    return (z1+z2+z3)/3;
}



/***************************************/
//DEBUG NOT FULLY TESTED  
/*
    WIP test of python code mapping to C++, inheritance, etc 
    equivalent of python -> polygon_ops.calc_3d_bbox()
    
    this sets the intenal attribs (bb_min_x, bb_max_x ), etc   
*/

void polygon_ops::bbox3d(void)
{
    
    bool debug = false;
    
    //std::cout << "hello bbox3d ! "<< num_pts <<"\n";

    // grab a point from data to initialize
    bb_min_x = points[0].x;
    bb_max_x = points[0].x;
    bb_min_y = points[0].y;
    bb_max_y = points[0].y;
    bb_min_z = points[0].z;
    bb_max_z = points[0].z;


    for(uint pi=0; pi<num_pts;pi++)
    {
        //std::cout << "POINTZ " << points[pi].x << "\n";
        if (points[pi].x<bb_min_x) { bb_min_x=points[pi].x; }
        if (points[pi].x>bb_max_x) { bb_max_x=points[pi].x; }
        
        if (points[pi].y<bb_min_y) { bb_min_y=points[pi].y; }                    
        if (points[pi].y>bb_max_y) { bb_max_y=points[pi].y; }

        if (points[pi].z<bb_min_z) { bb_min_z=points[pi].z; }
        if (points[pi].z>bb_max_z) { bb_max_z=points[pi].z; }        

    }            
 

    if (debug)
    {
        std::cout << bb_min_x << " " << bb_max_x << "\n";
        std::cout << bb_min_y << " " << bb_max_y << "\n";
        std::cout << bb_min_z << " " << bb_max_z << "\n";    
    }
   
}

/***************************************/
//DEBUG NOT DONE 
//generate and return 8 3d points from an objects bbox 
void polygon_ops::bbox3d_pts(void)
{
    //DEBUG - NAME OF THESE ARE SHITE  
    t_maxx = Vector3( bb_max_x, bb_min_y, bb_max_z); 
    t_maxy = Vector3( bb_min_x, bb_max_y, bb_max_z); 
    t_minx = Vector3( bb_min_x, bb_min_y, bb_max_z); 
    t_miny = Vector3( bb_max_x, bb_max_y, bb_max_z); 

    b_maxx = Vector3( bb_max_x, bb_min_y, bb_min_z); 
    b_maxy = Vector3( bb_min_x, bb_max_y, bb_min_z); 
    b_minx = Vector3( bb_min_x, bb_min_y, bb_min_z); 
    b_miny = Vector3( bb_max_x, bb_max_y, bb_min_z); 


    //std::cout << t_maxx.x << " " << t_maxx.y << " "<< t_maxx.z << "\n";

}


/***************************************/
/*
    XYZ centroid of all loaded geom 
    we need bbox3d() to get centroid 
    assume bbox is set prior to this 
*/

Vector3 polygon_ops::centroid(void)
{

    float xv = 0;
    float yv = 0; 
    float zv = 0;  

    for (uint i=0;i<num_pts;i++){
        Vector3 tp = points[i];
        xv=xv+tp.x;
        yv=yv+tp.y;
        zv=zv+tp.z;        
    }

    //// average them 
    float ox = xv/num_pts;
    float oy = yv/num_pts;
    float oz = zv/num_pts;

    return Vector3(ox, oy, oz);

}

/***************************************/
// XYZ centroid of a triangle 
Vector3 polygon_ops::centroid(Vector3 p1, Vector3 p2, Vector3 p3)
{
    Vector3 out;
    out.x = (p1.x + p2.x + p3.x)/3;
    out.y = (p1.y + p2.y + p3.y)/3;
    out.z = (p1.z + p2.z + p3.z)/3; 
    return out;
}



