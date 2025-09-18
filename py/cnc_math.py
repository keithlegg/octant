import math
import os

"""

simplest possible CNC control written in pure pyhton. 

code is derived from gnelscript.math_ops library - Keith Legg 2018-2025

"""


DEG_TO_RAD = 0.0174532925 # degree = radian * (180 / PI) # PI = 3.14159265
RAD_TO_DEG = 57.29577951  # radian = degree * (PI/180)


class math_util(object):    
    """ general math library - may contain some of the same functions 
        as the vector and matrix objects, but those will be implemented 
        to operate relative to themselves, these will work more generally

        for example self.dot_vec3() will be vec3.dot , etc 
    """

    ##-------------------------------------------##

    # # iterate through rows of X
    # for i in range(len(X)):
    #    # iterate through columns of Y
    #    for j in range(len(Y[0])):
    #        # iterate through rows of Y
    #        for k in range(len(Y)):
    #            result[i][j] += X[i][k] * Y[k][j]

    ##-------------------------------------------##

    # def matmult(a,b):
    #     zip_b = zip(*b)
    #     # uncomment next line if python 3 : 
    #     # zip_b = list(zip_b)
    #     return [[sum(ele_a*ele_b for ele_a, ele_b in zip(row_a, col_b)) 
    #              for col_b in zip_b] for row_a in a]

    ##-------------------------------------------##
    def calc_line_length(self, x1, y1, x2, y2):
        """ distance between two points 
            DEBUG -- make work in 2d and 3d!
            DEBUG -- merge with -  self.calc_line_length(pt1[0], pt1[1], pt2[0], pt2[1] )

        """
        return math.sqrt( ((x1-x2)**2)+ ((y1-y2)**2) )

    ##-------------------------------------------##        
    def dot_vec3 (self, v1, v2):
         """ scalar - mag but not direction """
         return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2] 

    ##-------------------------------------------##
    def cross_vec3 (self, v1, v2):
        return (v1[1]*v2[2]-v1[2]*v2[1], v1[2]*v2[0]-v1[0]*v2[2], v1[0]*v2[1]-v1[1]*v2[0])

    ##-------------------------------------------##
    def dtr (self, deg ):
        #print('########## DTR ########')
        #print(deg, type(deg) )

        if type(deg)==int: 
            return float(deg) * DEG_TO_RAD

        if type(deg)==float: 
            return deg * DEG_TO_RAD
       
        if type(deg)==tuple:
            out = []
            for e in deg: 
                out.append(e * DEG_TO_RAD)
            return out 


        raise ValueError('dtr: unknown type ')

    ##-------------------------------------------##
    def rtd (self, rad ):
        if type(rad)==int: 
            return float(rad) * RAD_TO_DEG

        if type(rad)==float:         
            return rad * RAD_TO_DEG

        if type(rad)==tuple:
            out = []
            for e in rad: 
                out.append(e * RAD_TO_DEG)
            return out 


        raise ValueError('rtd: unknown type ')

    ##-------------------------------------------##
    def dtr_vec3(self, invec):
        return ( self.dtr( invec[0] ),
                 self.dtr( invec[1] ),
                 self.dtr( invec[2] ),
               )
    ##-------------------------------------------##
    def rtd_vec3(self, invec):
        return ( self.rtd( invec[0] ),
                 self.rtd( invec[1] ),
                 self.rtd( invec[2] ),
               )
    ##-------------------------------------------##
    def vect_add(self, v1, v2):
        return [v1[0]+v2[0], v1[0]+v2[1], v1[2]+v2[2]]
    ##-------------------------------------------##
    def mult_scalar (self, scalar, v):
        return [v[0]*scalar, v[1]*scalar, v[2]*scalar ]
    ##-------------------------------------------##
    def mult_m33_vec3(self, m, v):
        """ multiplies a 3X3 matrix by a 3D vector - returns a vector tuple 
            NUMPY DOT is the same as multiplying 
        """
        
        outx = m[0] * v[0] + m[3] * v[1] + m[6] * v[2] 
        outy = m[1] * v[0] + m[4] * v[1] + m[7] * v[2] 
        outz = m[2] * v[0] + m[5] * v[1] + m[8] * v[2] 
          
        return  (outx, outy, outz)
    ##-------------------------------------------##
    def mult_m44_vec3(self, m, v):
        """ multiplies a 4X4 matrix by a 3D vector - returns a vector tuple """
        
        outx = m[0]*v[0] + m[4]*v[1] + m[8]  * v[2]+m[12]
        outy = m[1]*v[0] + m[5]*v[1] + m[9]  * v[2]+m[13]
        outz = m[2]*v[0] + m[6]*v[1] + m[10] * v[2]+m[14]
          
        return  (outx, outy, outz)
    ##-------------------------------------------##
    def mult_mat33(self, m, n):
        """ multiply two 3X3 matricies together """
        return [
                m[0]*n[0] + m[1]*n[3] + m[2]*n[6],
                m[0]*n[1] + m[1]*n[4] + m[2]*n[7],
                m[0]*n[2] + m[1]*n[5] + m[2]*n[8],
                m[3]*n[0] + m[4]*n[3] + m[5]*n[6],
                m[3]*n[1] + m[4]*n[4] + m[5]*n[7],
                m[3]*n[2] + m[4]*n[5] + m[5]*n[8],
                m[6]*n[0] + m[7]*n[3] + m[8]*n[6],
                m[6]*n[1] + m[7]*n[4] + m[8]*n[7],
                m[6]*n[2] + m[7]*n[5] + m[8]*n[8]   
               ]        
    ##-------------------------------------------##
    def mult_mat44(self, m, n):
        """multiply two 4X4 matricies together """
        return [
                m[0]*n[0]  + m[1]*n[4]  + m[2]*n[8]   + m[3]*n[12],
                m[0]*n[1]  + m[1]*n[5]  + m[2]*n[9]   + m[3]*n[13],
                m[0]*n[2]  + m[1]*n[6]  + m[2]*n[10]  + m[3]*n[14],
                m[0]*n[3]  + m[1]*n[7]  + m[2]*n[11]  + m[3]*n[15],
                m[4]*n[0]  + m[5]*n[4]  + m[6]*n[8]   + m[7]*n[12],
                m[4]*n[1]  + m[5]*n[5]  + m[6]*n[9]   + m[7]*n[13],
                m[4]*n[2]  + m[5]*n[6]  + m[6]*n[10]  + m[7]*n[14],
                m[4]*n[3]  + m[5]*n[7]  + m[6]*n[11]  + m[7]*n[15],
                m[8]*n[0]  + m[9]*n[4]  + m[10]*n[8]  + m[11]*n[12],
                m[8]*n[1]  + m[9]*n[5]  + m[10]*n[9]  + m[11]*n[13],
                m[8]*n[2]  + m[9]*n[6]  + m[10]*n[10] + m[11]*n[14],
                m[8]*n[3]  + m[9]*n[7]  + m[10]*n[11] + m[11]*n[15],
                m[12]*n[0] + m[13]*n[4] + m[14]*n[8]  + m[15]*n[12],
                m[12]*n[1] + m[13]*n[5] + m[14]*n[9]  + m[15]*n[13],
                m[12]*n[2] + m[13]*n[6] + m[14]*n[10] + m[15]*n[14],
                m[12]*n[3] + m[13]*n[7] + m[14]*n[11] + m[15]*n[15]
               ]

    ##-------------------------------------------##
    def normalize_vec3(self, in_vec):
        try:
           invLength = 1.0/math.sqrt(in_vec[0]*in_vec[0] + in_vec[1]*in_vec[1]+ in_vec[2]*in_vec[2])
           return (in_vec[0] *invLength, in_vec[1] * invLength,  in_vec[2] * invLength)
        except:
           print('normalize_vec3: divide by zero error.') 
           return [0,0,1]

    ##-------------------------------------------##
    def mul_square_matrices(self, m1, m2):
        
        """
        # From the brilliant Bryant Ewert's script 
        matrix_playground.mel 

        matrix $cMatrix[4][4] = << 0, 0, 0, 0;  0, 0, 0, 0;  0, 0, 0, 0;  0, 0, 0, 0 >>;
        for ( $i = 0; $i < 4; $i++ )
        {
            for ( $j = 0; $j < 4; $j++ )
            {
                for ( $k = 0; $k < 4; $k++ )
                {
                    $cMatrix[$i][$j] = $cMatrix[$i][$j] + ( $aMatrix[$i][$k] * $bMatrix[$k][$j] );
                }
            }
        }
        return $cMatrix;
        """
        pass

    ##-------------------------------------------##
    def matrix_adjoint(self, m44, size):
        
        """
        # From the brilliant Bryant Ewert's script 
        matrix_playground.mel 

        # Returns a matrix which is the Adjoint of $aMatrix.
        # The $size of the input matrix must be specified (3 or 4).

        """
        cMatrix = matrix44()
        detSize = size - 1 

        if size > 2:
            """
            # Cofactor of top-left 3×3 matrix
            $cMatrix[0][0] =  determinant( matrixCollapseRowColumn( $aMatrix, $size, 1, 1 ), $detSize );
            $cMatrix[0][1] = -determinant( matrixCollapseRowColumn( $aMatrix, $size, 1, 2 ), $detSize );
            $cMatrix[0][2] =  determinant( matrixCollapseRowColumn( $aMatrix, $size, 1, 3 ), $detSize );

            $cMatrix[1][0] = -determinant( matrixCollapseRowColumn( $aMatrix, $size, 2, 1 ), $detSize );
            $cMatrix[1][1] =  determinant( matrixCollapseRowColumn( $aMatrix, $size, 2, 2 ), $detSize );
            $cMatrix[1][2] = -determinant( matrixCollapseRowColumn( $aMatrix, $size, 2, 3 ), $detSize );

            $cMatrix[2][0] =  determinant( matrixCollapseRowColumn( $aMatrix, $size, 3, 1 ), $detSize );
            $cMatrix[2][1] = -determinant( matrixCollapseRowColumn( $aMatrix, $size, 3, 2 ), $detSize );
            $cMatrix[2][2] =  determinant( matrixCollapseRowColumn( $aMatrix, $size, 3, 3 ), $detSize );
            """
            pass

        if size > 3:
            """
            # Cofactor of 4th column
            $cMatrix[0][3] = -determinant( matrixCollapseRowColumn( $aMatrix, $size, 1, 4 ), $detSize );

            $cMatrix[1][3] =  determinant( matrixCollapseRowColumn( $aMatrix, $size, 2, 4 ), $detSize );

            $cMatrix[2][3] = -determinant( matrixCollapseRowColumn( $aMatrix, $size, 3, 4 ), $detSize );

            # Cofactor of 4th row
            $cMatrix[3][0] = -determinant( matrixCollapseRowColumn( $aMatrix, $size, 4, 1 ), $detSize );
            $cMatrix[3][1] =  determinant( matrixCollapseRowColumn( $aMatrix, $size, 4, 2 ), $detSize );
            $cMatrix[3][2] = -determinant( matrixCollapseRowColumn( $aMatrix, $size, 4, 3 ), $detSize );
            $cMatrix[3][3] =  determinant( matrixCollapseRowColumn( $aMatrix, $size, 4, 4 ), $detSize );
            """ 
            pass 

        # Adjoint is TRANSPOSE of matrix containing cofactors
        #cMatrix = transpose( $cMatrix, $size );
        return cMatrix

    ##-------------------------------------------##

    def matrix_invert(self, aMatrix, size):
        
        """
        # From the brilliant Bryant Ewert's script 
        matrix_playground.mel 

        # Returns a matrix which is the Inverse of $aMatrix.
        # The $size of the input matrix must be specified (3 or 4).

        """

        iMatrix  = matrix44()
    
        #determinant = determinant( aMatrix, size )
    
        if  determinant != 0.0: 
            #iMatrix = ( 1 / determinant( aMatrix, size ) ) * adjoint( aMatrix, size )
            pass

        return iMatrix;

    ##-------------------------------------------##
    def matrix_rotate(self, aMatrix, size):
        
        """
        # From the brilliant Bryant Ewert's script 
        matrix_playground.mel 

        # Applies a Rotation Transformation to the specified Matrix ("A", "B" or "C").
        # The rotation value and unit is derived from the current UI settings.
        # Note: The rotation matrices used may seem Transposed to those typically
        #    documented, but they are correct for this implementation within Maya,
        #    specifically in regard to the Acquire and Apply functions (above).

        """

    
        sAxis = ( "X", "Y", "Z" )
        kRADIAN = 57.295779513082320876798154814105;
        aMatrix = matrix44()
        rMatrix = matrix44() 
   
        """ 
        if ( $unit == 1 )       // must convert to degrees
            $rotate = $fRotate / $kRADIAN;
            
        float $cos = `cos $rotate`;
        float $sin = `sin $rotate`;
            
        switch ( $axis )
        {
            case 1:     // X axis
                $rMatrix = << 1, 0, 0, 0;  0, $cos, $sin, 0;  0, -$sin, $cos, 0;  0, 0, 0, 1 >>;
                break;
                
            case 2:     // Y axis
                $rMatrix = << $cos, 0, -$sin, 0;  0, 1, 0, 0;  $sin, 0, $cos, 0;  0, 0, 0, 1 >>;
                break;
                
            case 3:     // Z axis
                $rMatrix = << $cos, $sin, 0, 0;  -$sin, $cos, 0, 0;  0, 0, 1, 0;  0, 0, 0, 1 >>;
                break;
        }
        
        $aMatrix = getMatrix( $which );
        $rMatrix = multiplyMatrix( $aMatrix, $rMatrix );
        
        populateMatrix( "C", $rMatrix, ( "Rotate Matrix A " + $fRotate + ( $unit == 1 ? " deg" : " rad" ) + " on " + $sAxis[$axis-1] ) );
        """

class vec3(object):    
    def __init__(self,x=0,y=0,z=0):
        #DEBUG source of bugs here there be 
        #this is sloppy - check the first item and assume we are initializing with a tuple xyz 
        if type(x) is list or type(x) is tuple:
            if len(x)==2:
                self.x=x[0];self.y=x[1];self.z=0
            if len(x)==3:                
                self.x=x[0];self.y=x[1];self.z=x[2]  
        else:    
            self.x=x;self.y=y;self.z=z  
            self.mu = math_util() 

    def __repr__(self):
        return '(%s, %s, %s)' % (self.x, self.y, self.z)

    def __abs__(self):
        return type(self)(abs(self.x), abs(self.y), abs(self.z))

    def __add__(self, other):
        if isinstance(other, float) or isinstance(other, int):
            return type(self)(self.x+other, self.y+other, self.z+other)
        if isinstance(other, vec3):                    
            return type(self)(self.x+other.x, self.y+other.y, self.z+other.z)
        if isinstance(other, tuple) or isinstance(other, list):
            return type(self)(self.x+other[0], self.y+other[1], self.z+other[2])  

    def __neg__(self):
        return type(self)(-(self.x), -(self.y), -(self.z))

    def __sub__(self, other):
        if isinstance(other, float) or isinstance(other, int):
            return type(self)(self.x-other, self.y-other, self.z-other)
        if isinstance(other, vec3):                    
            return type(self)(self.x-other.x, self.y-other.y, self.z-other.z)
        if isinstance(other, tuple) or isinstance(other, list):
            return type(self)(self.x-other[0], self.y-other[1], self.z-other[2])  

    def __mul__(self, other):
        if isinstance(other, float) or isinstance(other, int):
            return type(self)(self.x*other, self.y*other, self.z*other)
        if isinstance(other, vec3):
            return type(self)(self.x*other.x, self.y*other.y, self.z*other.z)
        if isinstance(other, tuple) or isinstance(other, list):
            return type(self)(self.x*other[0], self.y*other[1], self.z*other[2])          

    def __truediv__(self, other):
        if isinstance(other, vec3):                    
            self.x = other.x/other.length()
            self.y = other.y/other.length()
            self.z = other.y/other.length()
            return type(self)(self.x, self.y, self.z)
        if isinstance(other, float) or isinstance(other, int):
            return type(self)(self.x/other, self.y/other, self.z/other)

    def __float__(self):
        return type(self)(float(self.x), float(self.y), float(self.z) )

    def __getitem__(self, index):
        if index==0:
            return self.x
        if index==1:
            return self.y
        if index==2:
            return self.z
        # not sure the "proper" way to exit from a loop     
        raise(IndexError)   

    def __setitem__(self, key, item):
        if key==0:
            self.x = item
        if key==1:
            self.y = item
        if key==2:
            self.z = item

    #def sqrt(self):
    #    numpy has this - not sure how to implement
    #    return math.sqrt(self.x*self.x + self.y*self.y + self.z*self.z )


    @property
    def aspt(self):
        return (self.x, self.y, self.z)


    #def is_parallel(self, vec):

    def is_orthogonal(self, vec):
        """UNTESTED
        #https://www.learndatasci.com/glossary/orthogonal-and-orthonormal-vectors/
        """

        if self.dot(vec)==0:
            return True 
        else:
            return False     
 
    ##----------------------------------------
    def project_pt(self, offset):
        """ UNTESTED? 
            project a point along a vector 
        """
        
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
    
    ##----------------------------------------
    def lookat(self, target):
        #if you want to point one vector at another all you need to do is subtract! 
        aim = vec3()
        return vec3(target) - self 

    ##----------------------------------------
    def between(self, pt2):
        """ given 2 points in 3D, create a 3D vector (lookat) 
            representing the offset between them 

            doesnt get much easier than this, just subtract 
             
            usage: 
                v1 = vec3(0,0,0)
                v2 = vec3(1,1,1)
                print(v2.between(v1) )

        """
   
        #if isinstance(pt1, tuple):
        #    pt1 = vec3( pt1[0], pt1[1], pt1[2]) 


        if isinstance(pt2, tuple):
            pt2 = vec3( pt2[0], pt2[1], pt2[2])

        return pt2 - self

    ##----------------------------------------
    """
    def dist_between(self, other):
        #   DEBUG NOT DONE  
        #   Calculate the angle between the two vectors
        
        print("P1 ", self)
        print("P2 ", other)

        sub = self-other

        print( np.dot(self, other-self) )
        print( self.dot(sub) )

        #angle = math.acos(self.dot(sub) )
        #angle = np.arccos(np.dot(self, other-self) / (np.linalg.norm(self) * np.linalg.norm(other-self)))

        #print("NormP1 ", np.linalg.norm(self))
        #print("NormP2 ", np.linalg.norm(other))
        
        #return np.degrees(angle)
        return None
    """
      
    ##------------------------------------- 
    def angle_between(self, other):
        """ 
            result is in radians
            derived from law of cosines 
            range from 0 (colinear) to PI radians (180 deg)  

            FAILS WHEN 0 or 180 ?
        """
        if type(other) is tuple:
            other=vec3(other)

        try:
            #DEBUG this may be better - WHY NO NORMALIZE?? I am lost 
            #angle = np.arccos(np.dot(point1, point2-point1) / (np.linalg.norm(point1) * np.linalg.norm(point2-point1)))

            o = math.acos( self.dot(other) / (self.length*other.length) ) 
            return o 
        except:
            return None


    ##----------------------------------------
    def insert(self, iterable):
        """ convert an np.array, tuple or list  to vec3  
            does not check size, so just assume 3 items (x,y,z)
        """

        if isinstance(iterable, list) or isinstance(iterable, tuple):
            self.x = iterable[0]
            self.y = iterable[1]            
            self.z = iterable[2]

        return self 

    ##----------------------------------------
    def copy(self, vtype=None):
        if vtype == None:
            return type(self)(self.x,self.y,self.z)

        if vtype == 'tuple':
            return ( (self.x,self.y,self.z) )

    
    ##----------------------------------------
    @property
    def length(self):
        """ output - scalar representing the length of this vector """
        return math.sqrt(self.x*self.x + self.y*self.y + self.z*self.z )

    def dot (self, v2):
         """ input  - vector
             output - scalar  
         """
         return self.x*v2[0] + self.y*v2[1] + self.z*v2[2]

    def cross (self, other):
        """ input  - vector
            output - vector 
        """
        x = self.y*other.z - self.z*other.y
        y = self.z*other.x - self.x*other.z
        z = self.x*other.y - self.y*other.x
        return type(self)(x,y,z)

    @property
    def dtr(self):
        """ degree to radian """
        return ( self.mu.drt( self.x ),
                 self.mu.dtr( self.y ),
                 self.mu.dtr( self.z )                 
               )

    @property
    def rtd(self):
        """ radian to degree """
        return ( self.mu.rtd( self.x ),
                 self.mu.rtd( self.y ),
                 self.mu.rtd( self.z )                 
               )  
    @property
    def normal(self) :
        """ normal of this vector """
        try:
           invLength = 1.0/math.sqrt(self.x*self.x + self.y*self.y + self.z*self.z)
           return type(self)(self.x *invLength, self.y * invLength,  self.z * invLength)
        except:
           print('normal vec3: divide by zero error.') 
           return [0,0,1]

    ##------------------------------------- 
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
    
    ##------------------------------------- 
    def ray_tri_intersect(self, orig, dir, v0, v1, v2):
        """ taken from C code at 
            https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution.html 

            orig - location of ray source 
            dir  - vector of ray direction 
            v0   - tri pt1
            v1   - tri pt2 
            v2   - tri pt3

        """

        t = 0 


        #DEBUG  a but hacky 
        if type(dir) is not vec3:
            dir = vec3(dir)
        if type(orig) is not vec3:
            orig = vec3(orig)


        # compute the plane's normal
        v0v1 = vec3()
        v0v2 = vec3()
        
        v0v1 = v1-v0
        v0v2 = v2-v0
        
        # no need to normalize
        N =vec3() 
        N = v0v1.cross(v0v2) 

        #// Step 1: check if the ray and plane are parallel.
        n_dot_ray_dir = N.dot(dir)

        # check almost 0
        #if abs(n_dot_ray_dir) < kEpsilon: 
        if n_dot_ray_dir <.001:
            # they are parallel, so they don't intersect! 
            return False 

        # compute (d) parameter using equation 2
        d = -N.dot(v0)
    
        # compute (t) - distance from the ray origin to the intersection point
        t = -(N.dot(orig) + d) / n_dot_ray_dir
        
        # check if the triangle is behind the ray
        # If is greater than 0, the triangle is "visible" to that ray
        if t < 0:
            return False 
     
        # compute the intersection point  
        P = vec3() 
        P = orig+(dir*t)
        
        #print("## distance %s point "%t, P)
        ##-----

        # inside-outside test - vector perpendicular to triangle's plane
        C = vec3() 
     
        # edge 0
        edge0 = vec3()
        edge0 = v1 - v0 
        vp0 =vec3()
        vp0 = P - v0
        C = edge0.cross(vp0)
        if N.dot(C) < 0:
            # P is on the right side
            return False; 
     
        # edge 1
        edge1 = vec3()
        edge1 = v2 - v1 
        vp1 = vec3() 
        vp1 = P - v1
        C = edge1.cross(vp1)
        if N.dot(C) < 0:  
            # P is on the right side
            return False; 
     
        # edge 2
        edge2 = vec3()
        edge2 = v0 - v2 
        vp2 = vec3()        
        vp2 = P - v2
        C = edge2.cross(vp2)
        if N.dot(C) < 0:
            # P is on the right side;
            return False; 
        
        # this ray hits the triangle
        return P
  

class matrix33(object):
    """ 3X3 matrix from pure python 

        the patern "return type(self) is nice to return copies of itself,
        but beware that this structure is not compatible for passing mutable types.
        Only primitive types work, in this case, 9 floats  
    """
    def __init__(self, a=1,b=0,c=0,
                       d=0,e=1,f=0,
                       g=0,h=0,i=1):
        self.m = [a,b,c,d,e,f,g,h,i]
        self.mu = math_util()

    def __getitem__(self, index):
        return self.m[index]

    def __setitem__(self, key, item):
        self.m[key] = item

    """
    def __repr__(self):
        return '(%s, %s, %s, %s, %s, %s, %s, %s, %s )'%(
                self.m[0], self.m[1], self.m[2],  self.m[3],  self.m[4],  
                self.m[5],  self.m[6],  self.m[7], self.m[8] )
    """

    def __repr__(self):
        return '%s, %s, %s,\n %s, %s, %s,\n %s, %s, %s'%(
                self.m[0], self.m[1], self.m[2],  self.m[3],  self.m[4],  
                self.m[5],  self.m[6],  self.m[7], self.m[8] )

    def test_index(self):
        """ fill matrix with incrementing numbers to see indices 
            for testing transpose and other things 
        """
        tmp = type(self)()
        for i in range(9):
            tmp.m[i] = i
        return tmp

    @property
    def identity(self):
        """ using the "standard" 9 element, Row major, 3X3 rotation matrix """
        return type(self)()

    @property
    def determinant(self):
        """
            https://www.mathsisfun.com/algebra/matrix-determinant.html

                  a b c  |  0 1 2  |        
             A =  d e f  |  3 4 5  |       
                  g h i  |  6 7 8  |     
           
            |A| = a(ei − fh) − b(di − fg) + c(dh − eg)
            |A| = 0(48 − 57) − 1(38 − 56) + 2(37 − 46)

        """  
        a = self.copy() 
        o = a[0]* ((a[4]*a[8])-(a[5]*a[7])) - a[1]*((a[3]*a[8])-(a[5]*a[6])) +  a[2]*((a[3]*a[7])-(a[4]*a[6]))
        return o

    def serialize(self, inarray):
        """ serialize this array into a list 
            if you want it as a numpy array use self.copy(mtype='numpy')
        """

        out = []
        for i in self.m:
            out.append(i)
        return out


    def from_np(self, np):
        """ convert a numpy matrix to a matrix33 object
        """

        return type(self)(
            np[0][0] , np[0][1] , np[0][2] ,
            np[1][0] , np[1][1] , np[1][2] ,
            np[2][0] , np[2][1] , np[2][2]
        )


    def insert(self, iterable):
        """ load the first 9 things we find into this matrix 
            accepts numpy.ndarray, list, and tuple 
        """
       
        if isinstance(iterable, matrix33):
            self.m = iterable.m
        
        #serialized simple array
        if isinstance(iterable, list) or isinstance(iterable, tuple):
            for idx,i in enumerate(iterable):
                if idx <= 9:
                     self.m[idx] = iterable[idx] 

    def copy(self, mtype=None):
        """ create a copy of this matrix - can be same type or a numpy.ndarray """
        if not mtype:
            return type(self)(
                self.m[0] , self.m[1] , self.m[2] ,
                self.m[3] , self.m[4] , self.m[5] ,
                self.m[6] , self.m[7] , self.m[8]
            )

    @property
    def transpose(self):
        """
        m33 transpose

        # standard indicies  |  #transposed indicies
        1  2  3              |   1 4 7
        4  5  6              |   2 5 8
        7  8  9              |   3 6 9 
       
        """

        return type(self)(
            self.m[0], self.m[3], self.m[6],
            self.m[1], self.m[4], self.m[7],
            self.m[2], self.m[5], self.m[8] 
        )

    @property
    def inverse(self):
        """
            NOT DONE 
            multiply a matrix by its inverse and we end up with the Identity Matrix.

            https://www.wikihow.com/Find-the-Inverse-of-a-3x3-Matrix
            
            https://www.mathsisfun.com/algebra/matrix-inverse.html
        a = self.copy()
        o = self.identity 

        det = a.determinant
        
        if det == 0:
            # determinant is 0, matrix has no inverse
            return None 
        else:     
            print( '### ### ' , a )

            b = a.transpose

            print ("TRANSPOSE IS  ", a.transpose )
       

            return type(self)(
                o.m[0], o.m[1], o.m[2],
                o.m[3], o.m[4], o.m[5],
                o.m[6], o.m[7], o.m[8] 
            )
        """
        pass

    def __add__(self, other):
        return type(self)(
            self.m[0]+n[0], self.m[1]+n[1], self.m[2]+n[2],
            self.m[3]+n[3], self.m[4]+n[4], self.m[5]+n[5],
            self.m[6]+n[6], self.m[7]+n[7], self.m[8]+n[8]     
        )
    
    def __sub__(self, other):
        return type(self)(
            self.m[0]-n[0], self.m[1]-n[1], self.m[2]-n[2],
            self.m[3]-n[3], self.m[4]-n[4], self.m[5]-n[5],
            self.m[6]-n[6], self.m[7]-n[7], self.m[8]-n[8]     
        )

    # def __truediv__(self, other):
    #     # matrices don't divide! there is no concept of dividing by a matrix.
    #     # multiply by an inverse, which achieves the same thing.

    def __mul__(self, n):
        """ multiply two 3X3 matricies together 

            the order that you mutliply will call a different object!!!
            make sure you do "this * other", NOT "other * this"

            DONT FIX THE ROUNDING ERROR HERE - KEEP THE PRECISION!
            
        """

        if isinstance(n, vec3):
            outx = self.m[0]*n.x + self.m[3]*n.y + self.m[6]*n.z 
            outy = self.m[1]*n.x + self.m[4]*n.y + self.m[7]*n.z 
            outz = self.m[2]*n.x + self.m[5]*n.y + self.m[8]*n.z 
            return  (outx, outy, outz)

        if isinstance(n, tuple) or isinstance(n, list) or isinstance(n, np.ndarray):
            outx = self.m[0]*n[0] + self.m[3]*n[1] + self.m[6]*n[2] 
            outy = self.m[1]*n[0] + self.m[4]*n[1] + self.m[7]*n[2] 
            outz = self.m[2]*n[0] + self.m[5]*n[1] + self.m[8]*n[2] 
            return  (outx, outy, outz)

        if type(n) == type(self):
            return type(self)(
                    self.m[0]*n[0] + self.m[1]*n[3] + self.m[2]*n[6],
                    self.m[0]*n[1] + self.m[1]*n[4] + self.m[2]*n[7],
                    self.m[0]*n[2] + self.m[1]*n[5] + self.m[2]*n[8],
                    self.m[3]*n[0] + self.m[4]*n[3] + self.m[5]*n[6],
                    self.m[3]*n[1] + self.m[4]*n[4] + self.m[5]*n[7],
                    self.m[3]*n[2] + self.m[4]*n[5] + self.m[5]*n[8],
                    self.m[6]*n[0] + self.m[7]*n[3] + self.m[8]*n[6],
                    self.m[6]*n[1] + self.m[7]*n[4] + self.m[8]*n[7],
                    self.m[6]*n[2] + self.m[7]*n[5] + self.m[8]*n[8]   
                   )

    def batch_mult_pts(self, pts):
        """ iterate a list of points and multiply them by this matrix """

        tmp_buffer = []
        out = None
        for pvec in pts:  
            tmp_buffer.append( self * pvec )
        return tmp_buffer

    def from_euler(self, rotation):
        """
            derived from the rotate_pts_3d function 
            input in degrees 
            
        """

        xrot = rotation[0]
        yrot = rotation[1]
        zrot = rotation[2]

        dtr = self.mu.dtr

        # build rotationY (see diagram above) 
        y_matrix =  self.identity
        y_matrix[0]  =  math.cos(dtr( yrot ))
        y_matrix[2]  = -math.sin(dtr( yrot ))
        y_matrix[6]  =  math.sin(dtr( yrot ))
        y_matrix[8]  =  math.cos(dtr( yrot ))

        ####                
        # build rotationZ (see diagram above) 
        z_matrix    =  self.identity
        z_matrix[0] =  math.cos(dtr( zrot ))
        z_matrix[1] =  math.sin(dtr( zrot ))
        z_matrix[3] = -math.sin(dtr( zrot ))
        z_matrix[4] =  math.cos(dtr( zrot ))
        tmp_matr =  y_matrix * z_matrix 

        ####
        # build rotationX (see diagram above) 
        x_matrix =  self.identity
        x_matrix[4]  =   math.cos(dtr( xrot )) 
        x_matrix[5]  =   math.sin(dtr( xrot )) 
        x_matrix[7]  =  -math.sin(dtr( xrot ))
        x_matrix[8]  =   math.cos(dtr( xrot ))
        rotation_33 = x_matrix * tmp_matr 
 
        self.insert(rotation_33)

    def rotate_pts_3d(self, points, xrot, yrot, zrot):
        """
          The "standard" 9 element, Row major, 3X3 rotation matrix used by Maya
             

           ⎡0  1  2⎤      xx xy xz 
           ⎢3  4  5⎥      yx yy yz 
           ⎣6  7  8⎦      zx zy zz 
           ------------------------------
                Rotate Y matrix     
           ⎡cos(y)  0      -sin(y) ⎤ 
           ⎢0       1       0      ⎥ 
           ⎣sin(y)  0       cos(y) ⎦ 
           ------------------------------
                Rotate Z  matrix 
           ⎡  cos(z)  sin(z)  0    ⎤ 
           ⎢ -sin(z)  cos(z)  0    ⎥
           ⎣  0       0       1    ⎦
           ------------------------------
                Rotate X matrix  
           ⎡ 1       0         0   ⎤    
           ⎢ 0    cos(x)   sin(x)  ⎥  
           ⎣ 0   -sin(x)   cos(x)  ⎦  
           ------------------------------            
 
        """
        dtr = self.mu.dtr

        # build rotationY (see diagram above) 
        y_matrix =  self.identity
        y_matrix[0]  =  math.cos(dtr( yrot ))
        y_matrix[2]  = -math.sin(dtr( yrot ))
        y_matrix[6]  =  math.sin(dtr( yrot ))
        y_matrix[8]  =  math.cos(dtr( yrot ))

        ####                
        # build rotationZ (see diagram above) 
        z_matrix    =  self.identity
        z_matrix[0] =  math.cos(dtr( zrot ))
        z_matrix[1] =  math.sin(dtr( zrot ))
        z_matrix[3] = -math.sin(dtr( zrot ))
        z_matrix[4] =  math.cos(dtr( zrot ))
        tmp_matr =  y_matrix * z_matrix 

        ####
        # build rotationX (see diagram above) 
        x_matrix =  self.identity
        x_matrix[4]  =   math.cos(dtr( xrot )) 
        x_matrix[5]  =   math.sin(dtr( xrot )) 
        x_matrix[7]  =  -math.sin(dtr( xrot ))
        x_matrix[8]  =   math.cos(dtr( xrot ))
        rotation_33 = x_matrix * tmp_matr 

        ############ 
        return rotation_33.batch_mult_pts(points)










class matrix44(object):
    """ 4X4 matrix from pure python 
        limited support to interface to numpy arrays 

        the patern "return type(self) is nice to return copies of itself,
        but beware that this structure is not compatible for passing mutable types.
        Only primitive types work, in this case floats  


       -------------------------------------------------------------------------
       standard affine transformation matrix.

       ⎡m00  m01 m02 0⎤
       ⎢m10  m11 m12 0⎥
       ⎢m20  m21 m22 0⎥
       ⎣Tx   Ty  Tz  1⎦
       -------------------------------------------------------------------------

    """    

    def __init__(self, a=1,b=0,c=0,d=0,
                       e=0,f=1,g=0,h=0,
                       i=0,j=0,k=1,l=0,
                       m=0,n=0,o=0,p=1):

        self.mu = math_util()
        self.m = [a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p]


    def from_np(self, np, order='cm'):
        """ convert a numpy matrix to a matrix44 object
            
            order can be column major or row major  

        """
        # return type(self)(
        #     np[0][0] , np[1][0] , np[2][0] ,
        #     np[0][1] , np[1][1] , np[2][1] ,
        #     np[0][2] , np[1][2] , np[2][2]
        # )
        

        if order=='rm':
            return type(self)(
                np[0][0], np[1][0], np[2][0], np[3][0],
                np[0][1], np[1][1], np[2][1], np[3][1],
                np[0][2], np[1][2], np[2][2], np[3][2],
                np[0][3], np[1][3], np[2][3], np[3][3]
            )

        if order=='cm':
            return type(self)(
                np[0][0], np[0][1], np[0][2], np[0][3],
                np[1][0], np[1][1], np[1][2], np[1][3],
                np[2][0], np[2][1], np[2][2], np[2][3],
                np[3][0], np[3][1], np[3][2], np[3][3]
            )

    def load_file(self, filename, transpose=False):
       if os.path.lexists(filename):
            f = open( filename,"r", encoding='utf-8')
            ct = 0
            for lin in f.readlines():
                c = lin.replace('\n','').split(' ')
                if ct==0:
                    self.m[0]=float(c[0])
                    self.m[1]=float(c[1])
                    self.m[2]=float(c[2])
                    self.m[3]=float(c[3])                                                        
                if ct==1:
                    self.m[4]=float(c[0])
                    self.m[5]=float(c[1])
                    self.m[6]=float(c[2])
                    self.m[7]=float(c[3])  
                if ct==2:
                    self.m[8]=float(c[0])
                    self.m[9]=float(c[1])
                    self.m[10]=float(c[2])
                    self.m[11]=float(c[3])  
                if ct==3:
                    self.m[12]=float(c[0])
                    self.m[13]=float(c[1])
                    self.m[14]=float(c[2])
                    self.m[15]=float(c[3])  

                ct += 1

    def __getitem__(self, index):
        return self.m[index]

    def __setitem__(self, key, item):
        self.m[key] = item

    def __repr__(self):
        return '(%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s )'%(
                self.m[0], self.m[1], self.m[2],  self.m[3],  self.m[4],  self.m[5],  self.m[6],  self.m[7], 
                self.m[8], self.m[9], self.m[10], self.m[11], self.m[12], self.m[13], self.m[14], self.m[15])

    def __add__(self, n):
        return type(self)(
                self.m[0] +n[0] , self.m[1]+n[1]  , self.m[2]+n[2]  , self.m[3]+n[3]  , 
                self.m[4] +n[4] , self.m[5]+n[5]  , self.m[6]+n[6]  , self.m[7]+n[7]  ,
                self.m[8] +n[8] , self.m[9]+n[9]  , self.m[10]+n[10], self.m[11]+n[11],
                self.m[12]+n[12], self.m[13]+n[13], self.m[14]+n[14], self.m[15]+n[15]
               )
       
    def __sub__(self, n):
        return type(self)(
                self.m[0] -n[0] , self.m[1]-n[1]  , self.m[2]-n[2]  , self.m[3]-n[3]  , 
                self.m[4] -n[4] , self.m[5]-n[5]  , self.m[6]-n[6]  , self.m[7]-n[7]  ,
                self.m[8] -n[8] , self.m[9]-n[9]  , self.m[10]-n[10], self.m[11]-n[11],
                self.m[12]-n[12], self.m[13]-n[13], self.m[14]-n[14], self.m[15]-n[15]
               )

    def __mul__(self, n):
        """multiply this 4X4 by another 4X4 matrix or a vector3, vector4 """
        
        ## if isinstance(n, vec4):
        ##     #untested
        ##     #https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic
        ##     #    -projection-matrix/building-basic-perspective-projection-matrix
        ##     # row major * vec4 
        ##     # outx = n.x*self.m[0]   + n.y*self.m[1]  +  n.z*self.m[2]   +  n.w *self.m[3] 
        ##     # outy = n.x*self.m[4]   + n.y*self.m[5]  +  n.z*self.m[6]   +  n.w *self.m[7] 
        ##     # outz = n.x*self.m[8]   + n.y*self.m[9]  +  n.z*self.m[10]  +  n.w *self.m[11]                         
        ##     # outw = n.x*self.m[12]  + n.y*self.m[13] +  n.z*self.m[14]  +  n.w *self.m[15] 
        ##     # column major * vec4 
        ##     outx = n.x*self.m[0]  + n.y*self.m[4]  +  n.z*self.m[8]   +  n.w *self.m[12] 
        ##     outy = n.x*self.m[1]  + n.y*self.m[5]  +  n.z*self.m[9]   +  n.w *self.m[13] 
        ##     outz = n.x*self.m[2]  + n.y*self.m[6]  +  n.z*self.m[10]  +  n.w *self.m[14]                         
        ##     outw = n.x*self.m[3]  + n.y*self.m[7]  +  n.z*self.m[11]  +  n.w *self.m[15] 
        ##     return  (outx, outy, outz, outw)

   
        if isinstance(n, vec3): # or isinstance(n, np.ndarray):
            # column major -                      why add the last 12,13,14 ? (affine?)            
            # outx = self.m[0] * n.x + self.m[4] * n.y + self.m[8]  * n.z     + self.m[12]
            # outy = self.m[1] * n.x + self.m[5] * n.y + self.m[9]  * n.z     + self.m[13]
            # outz = self.m[2] * n.x + self.m[6] * n.y + self.m[10] * n.z     + self.m[14]

            # column major  , without elements 12,13,14 
            outx = self.m[0] * n.x + self.m[4] * n.y + self.m[8]  * n.z 
            outy = self.m[1] * n.x + self.m[5] * n.y + self.m[9]  * n.z 
            outz = self.m[2] * n.x + self.m[6] * n.y + self.m[10] * n.z 
            return  (outx, outy, outz)


        if isinstance(n, tuple) or isinstance(n, list):
            
            # what is the purspose of adding 12,13,14 ?
            outx = self.m[0] * n[0] + self.m[4] * n[1] + self.m[8]  * n[2]     + self.m[12]
            outy = self.m[1] * n[0] + self.m[5] * n[1] + self.m[9]  * n[2]     + self.m[13]
            outz = self.m[2] * n[0] + self.m[6] * n[1] + self.m[10] * n[2]     + self.m[14]

            # column major, same as first, without 12,13,14               
            #outx = self.m[0] * n[0] + self.m[4] * n[1] + self.m[8]  * n[2] 
            #outy = self.m[1] * n[0] + self.m[5] * n[1] + self.m[9]  * n[2] 
            #outz = self.m[2] * n[0] + self.m[6] * n[1] + self.m[10] * n[2] 

            # row major - same as first, without 12,13,14    
            # outx = self.m[0] * n[0] + self.m[1] * n[1] + self.m[2]  * n[2] 
            # outy = self.m[4] * n[0] + self.m[5] * n[1] + self.m[6]  * n[2] 
            # outz = self.m[8] * n[0] + self.m[9] * n[1] + self.m[10] * n[2] 
            return  (outx, outy, outz)


        if type(n) == type(self):
            return type(self)(
                    self.m[0]*n[0]  + self.m[1]*n[4]  + self.m[2]*n[8]   + self.m[3]*n[12] ,
                    self.m[0]*n[1]  + self.m[1]*n[5]  + self.m[2]*n[9]   + self.m[3]*n[13] ,
                    self.m[0]*n[2]  + self.m[1]*n[6]  + self.m[2]*n[10]  + self.m[3]*n[14] ,
                    self.m[0]*n[3]  + self.m[1]*n[7]  + self.m[2]*n[11]  + self.m[3]*n[15] ,
                    self.m[4]*n[0]  + self.m[5]*n[4]  + self.m[6]*n[8]   + self.m[7]*n[12] ,
                    self.m[4]*n[1]  + self.m[5]*n[5]  + self.m[6]*n[9]   + self.m[7]*n[13] ,
                    self.m[4]*n[2]  + self.m[5]*n[6]  + self.m[6]*n[10]  + self.m[7]*n[14] ,
                    self.m[4]*n[3]  + self.m[5]*n[7]  + self.m[6]*n[11]  + self.m[7]*n[15] ,
                    self.m[8]*n[0]  + self.m[9]*n[4]  + self.m[10]*n[8]  + self.m[11]*n[12],
                    self.m[8]*n[1]  + self.m[9]*n[5]  + self.m[10]*n[9]  + self.m[11]*n[13],
                    self.m[8]*n[2]  + self.m[9]*n[6]  + self.m[10]*n[10] + self.m[11]*n[14],
                    self.m[8]*n[3]  + self.m[9]*n[7]  + self.m[10]*n[11] + self.m[11]*n[15],
                    self.m[12]*n[0] + self.m[13]*n[4] + self.m[14]*n[8]  + self.m[15]*n[12],
                    self.m[12]*n[1] + self.m[13]*n[5] + self.m[14]*n[9]  + self.m[15]*n[13],
                    self.m[12]*n[2] + self.m[13]*n[6] + self.m[14]*n[10] + self.m[15]*n[14],
                    self.m[12]*n[3] + self.m[13]*n[7] + self.m[14]*n[11] + self.m[15]*n[15]
                   )

    @property
    def as_np(self):
        return np.array((
            [self.m[0] , self.m[1] , self.m[2] , self.m[3]  ],
            [self.m[4] , self.m[5] , self.m[6] , self.m[7]  ],
            [self.m[8] , self.m[9] , self.m[10], self.m[11] ],
            [self.m[12], self.m[13], self.m[14], self.m[15] ]
        ))   

    @property
    def identity(self):
        """ using the "standard" 16 element, Row major, 4X4 rotation matrix """
        #return  [ 1,0,0,0 ,0,1,0,0 ,0,0,1,0 ,0,0,0,1]
        return type(self)()

    def np_inverse(self, mtype='numpy'):
        """ untested """
        if NUMPY_IS_LOADED:
            a = self.copy(mtype='numpy')
            b = inv(a)
            if mtype=='numpy':
                return b 
            if mtype=='m44':    
                c = matrix44()
                c.insert(b)
                return c

    #@property
    def test_index(self):
        """ fill matrix with incrementing number to see indices """
        tmp = type(self)()
        for i in range(16):
            tmp[i] = i
        return tmp

    def serialize(self, inarray):
        """ serialize this array into a list """

        out = []
        for i in self.m:
            out.append(i)
        return out

    def insert(self, iterable):
        """ UNTESTED - load the first 16 things we find into this matrix """

        if isinstance(iterable, matrix44):
            self.m = iterable.m

        #serialized simple array
        if isinstance(iterable, list) or isinstance(iterable, tuple):
            for idx,i in enumerate(iterable):
                if idx <= 16:
                     self.m[idx] = iterable[idx] 

        if NUMPY_IS_LOADED:
             #numpy ND array 
            if isinstance(iterable, np.ndarray):
                out = [];idx=0
                for row in iterable:
                    for col in row:
                        self.m[idx] = col
                        idx+=1

    #@property
    def copy(self, mtype=None):
        """ UNTESTED """
        if mtype == None:        
            return type(self)(
                self.m[0] , self.m[1] , self.m[2] , self.m[3] ,
                self.m[4] , self.m[5] , self.m[6] , self.m[7] ,
                self.m[8] , self.m[9] , self.m[10], self.m[11],
                self.m[12], self.m[13], self.m[14], self.m[15]
            )

        if NUMPY_IS_LOADED:
            if mtype == 'numpy':
                return np.array((
                    [self.m[0] , self.m[1] , self.m[2] , self.m[3]  ],
                    [self.m[4] , self.m[5] , self.m[6] , self.m[7]  ],
                    [self.m[8] , self.m[9] , self.m[10], self.m[11] ],
                    [self.m[12], self.m[13], self.m[14], self.m[15] ]
                ))               

    @property
    def transpose(self):
        """ m44 transpose """

        return type(self)(
            self.m[0], self.m[4], self.m[8] , self.m[12],
            self.m[1], self.m[5], self.m[9] , self.m[13],
            self.m[2], self.m[6], self.m[10], self.m[14],
            self.m[3], self.m[7], self.m[11], self.m[15]
        )

    @property
    def determinant(self):
        """
            Laplace expansion method 

            https://www.mathsisfun.com/algebra/matrix-determinant.html

            plus  a times the determinant of the matrix that is not in a's row or column,
            minus b times the determinant of the matrix that is not in b's row or column,
            plus  c times the determinant of the matrix that is not in c's row or column,
            minus d times the determinant of the matrix that is not in d's row or column,

                       |               | A*        -     B*      +        C*    -         D*|
            a  b  c  d | 0   1  2   3  | a -->     |  <--b-->    |     <--c-->  |      <--d |  
            e  f  g  h | 4   5  6   7  | |  f g h  |  e  |  g  h |  e  f  |  h  | e  f  g | |
            i  j  k  l | 8   9  10  11 |    j k l  |  i     k  l |  i  j     l  | i  j  k   |
            m  n  o  p | 12  13 14  15 |    n o p  |  m     o  p |  m  n     p  | m  n  o   | 

        """  


        def det33(a):
            #same as matrix33 determinant method 
            o = a[0]* ((a[4]*a[8])-(a[5]*a[7])) - a[1]*((a[3]*a[8])-(a[5]*a[6])) +  a[2]*((a[3]*a[7])-(a[4]*a[6]))
            return o

        a = self.copy() 
        
        a33 = [a[5],a[6],a[7],a[9],a[10],a[11],a[13],a[14],a[15] ]
        b33 = [a[4],a[6],a[7],a[8],a[10],a[11],a[12],a[14],a[15] ]
        c33 = [a[4],a[5],a[7],a[8],a[9] ,a[11],a[12],a[13],a[15] ]
        d33 = [a[4],a[5],a[6],a[8],a[9] ,a[10],a[12],a[13],a[14] ]
        o = (a[0] * det33(a33)) - (a[1]*det33(b33)) + (a[2]*det33(c33)) - (a[3]*det33(d33))
        return o

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

    def from_euler(self, rotation ):
        """
            derived from the rotate_pts_3d function 
            go read that doc for explanation  
        """
        
        xrot = rotation[0]
        yrot = rotation[1]
        zrot = rotation[2]


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
 
        self.insert(rotation_44)

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


    def align_vectors(self, a, b, return_angle=False):
        """
        From - trimesh.geometry.align_vectors 

        Find the rotation matrix that transforms one 3D vector
        to another.

        Parameters
        ------------
        a : (3,) float
          Unit vector
        b : (3,) float
          Unit vector
        return_angle : bool
          Return the angle between vectors or not

        Returns
        -------------
        matrix : (4, 4) float
          Homogeneous transform to rotate from `a` to `b`
        angle : float
          If `return_angle` angle in radians between `a` and `b`

        """
        if type(a)==vec3: 
            a=a.as_np 
        if type(b)==vec3: 
            b=b.as_np 

        a = np.array(a, dtype=np.float64)
        b = np.array(b, dtype=np.float64)
        if a.shape != (3,) or b.shape != (3,):
            raise ValueError("vectors must be (3,)!")

        # find the Singular Value Decomposition of the two vectors
        au = np.linalg.svd(a.reshape((-1, 1)))[0]
        bu = np.linalg.svd(b.reshape((-1, 1)))[0]

        if np.linalg.det(au) < 0:
            au[:, -1] *= -1.0
        if np.linalg.det(bu) < 0:
            bu[:, -1] *= -1.0

        # put rotation into homogeneous transformation
        matrix = np.eye(4)
        matrix[:3, :3] = bu.dot(au.T)

        if return_angle:
            # projection of a onto b
            # first row of SVD result is normalized source vector
            dot = np.dot(au[0], bu[0])
            # clip to avoid floating point error
            angle = np.arccos(np.clip(dot, -1.0, 1.0))
            if dot < -1e-5:
                angle += np.pi
            return matrix, angle

        return matrix

