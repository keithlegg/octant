#ifndef CNC_PARPORT_H    
#define CNC_PARPORT_H


#include <vector>

#include "globals.h"
#include "plot.h"



#define HEX(x) std::setw(2) << std::setfill('0') << std::hex << (int)( x )


//https://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 


//https://stackoverflow.com/questions/11070183/how-do-i-print-a-hexadecimal-number-with-leading-0-to-have-width-2-using-sprintf
// sprintf("%02X",1)    # ->  "01"
// sprintf("%02X",10)   # ->  "0A"
// sprintf("%02X",16)   # ->  "10"
// sprintf("%02X",255)  # ->  "FF"

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

    void test_port_output(cncglobals*);

    void aux_on(cncglobals*, unsigned int);
    void aux_off(cncglobals*, unsigned int);

    void test_inputs(cncglobals* cg, 
                     unsigned char* data);

    void decode_quadrature(cncglobals* cg, 
                            int* step,
                            int* dir,
                            unsigned char* a_sigmask,
                            unsigned char* b_sigmask,
                            bool* stale);


    void send_pulses(float* pt_progress, 
                     cncglobals* cg, 
                     cnc_plot* pt_plot);

    void read_limits(cncglobals*, Vector3*);

};



#endif

