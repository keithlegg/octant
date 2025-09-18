#ifndef MATHOP_H    
#define MATHOP_H


//degree = radian * (180 / PI) # PI = 3.14159265
#define DEG_TO_RAD 0.0174532925 
//radian = degree * (PI/180)
#define RAD_TO_DEG 57.29577951  


//2 dimensional vector 
struct vec2 {
    double x;
    double y;
};

//3 dimensional vector 
struct vec3 {
    double x;
    double y;
    double z;    
};

//2X2 matrix 
struct m22 {
    double m0; double m1;
    double m2; double m3;
};

//3X3 matrix 
struct m33 {
    double m0; double m1; double m2;    
    double m3; double m4; double m5;
    double m6; double m7; double m8;    
};

//4X4 matrix 
struct m44 {
    double m0 ; double m1 ; double m2 ; double m3;
    double m4 ; double m5 ; double m6 ; double m7;
    double m8 ; double m9 ; double m10; double m11;
    double m12; double m13; double m14; double m15; 
};

//Quaternion (vector 4) 
struct quaternion {
    double x;
    double y;
    double z;    
    double w; 
};


double dtr ( double deg);
double rtd ( double rad);

double dotProduct ( vec2, vec2);
double dotProduct ( vec3, vec3);

double fcalc_distance(int pt1[2], int pt2[2]);
double fcalc_distance(double pt1[2], double pt2[2]);
double length( vec2);
double length( vec3);


double calc_theta_vert ( double start_x, double start_y, double end_x, double end_y);
double angle_between( vec2 v_one, vec2 v_two );


//object instance generators
vec2 newvec2( double x, double y );
vec2 newvec2( int   x, int   y );

vec2 normalize( vec2 input );
vec2 scale_vec( vec2 input, double amount );
vec2 mult_vec_scalar( vec2 input, double amount );

vec2 line2vect(int start_x, int start_y, int end_x, int end_y);
vec2 line2vect(double start_x, double start_y, double end_x, double end_y);
vec2 vmul_2d ( vec2 v1, vec2 v2 );

vec3 add( vec3 v1, vec3 v2 );
vec3 add( vec3 v1, vec3 v2 );

vec3 sub (vec3 v1, vec3 v2 );

vec3 mult( vec3 v1, vec3 v2 );
vec3 mult( m33 m, vec3 v );
vec3 mult( m44 m, vec3 v );
vec3 mult( double scalar, vec3 v );

vec3 div (vec3 v2); //BROKEN?
vec3 div (vec3 v1, double sclr);

vec3 cross(vec3 v1, vec3 v2);

/////////

vec3 newvec3( double x, double y, double z );
vec3 newvec3( int   x, int   y, int   z );

vec3 normalize( vec3 input );

//untested - not done 
vec3 dtr_vec3(vec3 invec);
vec3 rtd_vec3(vec3 invec);       




void print_vec2(vec2 input);
void print_vec3(vec3 input);
void print_matrix(m33 input);
void print_matrix(m44 input);


m33 new_m33( double m0 , double m1 , double m2,  
             double m3 , double m4 , double m5, 
             double m6 , double m7 , double m8
    );

m44 new_m44( double m0 , double m1 , double m2 , double m3, 
             double m4 , double m5 , double m6 , double m7,
             double m8 , double m9 , double m10, double m11,
             double m12, double m13, double m14, double m15
    );


m33 identity33(void);
m44 identity44(void);


m33 matrix_add(m33 other);
m44 matrix_add(m44 other);

m33 matrix_sub(m33 other);
m44 matrix_sub(m44 other);

m33 mult_mat33(m33 m, m33 n);
m44 mult_mat44(m44 m, m44 n);

m33 copy_matrix( m33 input );
m44 copy_matrix( m44 input );

m33 test_indices33(void);
m44 test_indices44(void);

double determinant(m33 input);
double determinant(m44 input);

void transpose(m33 *input);
void transpose(m44 *input);


m33 m33_from_euler(double xrot, double yrot, double zrot);
m44 m44_from_euler(double xrot, double yrot, double zrot);


quaternion new_quaternion( double x, double y, double z, double w );
quaternion quaternion_identity();

void set( quaternion *input, double x, double y, double z, double w);
// void set( vec2 *input, double x, double y);
// void set( vec3 *input, double x, double y, double z);
// void set( m33 *input,  );
// void set( m44 *input,  );

void print_quaternion(quaternion input);

void quaternion_rotx(quaternion *input, double theta);
void quaternion_roty(quaternion *input, double theta);
void quaternion_rotz(quaternion *input, double theta);


void quaternion_fr_euler(quaternion *input, double h, double p, double b);
double quaternion_mag(quaternion *input);
void quaternion_normalize(quaternion *input);


double dot_product(quaternion q);
quaternion conjugate(quaternion q);
quaternion multiply(quaternion a);
quaternion quaternion_from_m33(m33 input);

m33 quaternion_to_m33(quaternion *q);
void quaternion_set_axis( quaternion *input, vec3 axis, double theta);

double quaternion_get_rot_angle(quaternion q);
vec3 quaternion_get_rot_axis(quaternion q);


#endif

