#ifndef CNC_PLOT_H    
#define CNC_PLOT_H

//#include <iostream>
//#include <math.h>

#include <vector>
using std::vector; //point_op did not make me do this!!?? WHY?

#include "math_op.h"


class cnc_plot
{
    public:
        cnc_plot(){};
        ~cnc_plot(){};

    void test_port(void);

    void aux_on(unsigned int);
    void aux_off(unsigned int);

    void gen_pules(vector<int>*, int, int);

    void gen_3d_pules(vector<vec3>*, int, int);

    void calc_3d_pulses(vector<vec3>*,
                        vec3, 
                        vec3,
                        int);


    void load_config_file(char*);

    void send_pulses(vector<vec3>*);
    void read_limits(vec3*);

};



#endif

