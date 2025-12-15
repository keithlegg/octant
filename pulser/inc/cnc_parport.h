#ifndef CNC_PARPORT_H    
#define CNC_PARPORT_H


#include <vector>
using std::vector; //point_op did not make me do this!!?? WHY?

#include "cnc_globals.h"
#include "cnc_plot.h"



#define HEX(x) std::setw(2) << std::setfill('0') << std::hex << (int)( x )



/***************************************/
//define the way that pins get assigned in the parports 
class cnc_bitmasks
{

    public:
         cnc_bitmasks(){};
        ~cnc_bitmasks(){};

};


/***************************************/

//handle the IO to the parports 
class cnc_parport
{
    public:
         cnc_parport(){};
        ~cnc_parport(){};

    void test_port(cncglobals*);

    void aux_on(cncglobals*, unsigned int);
    void aux_off(cncglobals*, unsigned int);

    // /void send_pulses(cncglobals*, vector<Vector3>*);
    void send_pulses(float* pt_progress, cncglobals* cg, cnc_plot* pt_plot);

    void read_limits(cncglobals*, Vector3*);

};



#endif

