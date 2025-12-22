#ifndef CONFUSION36D_H    
#define CONFUSION36D_H



#include "cnc_globals.h"
#include "gl_setup.h"




void timer_init(void);
//void timer_reset(void)



void key_cb(unsigned int key);

void run_send_pulses(cncglobals*, float, float, float,
                                  float, float, float, int);  


void start_gui(int *argc, char** argv);






#endif

