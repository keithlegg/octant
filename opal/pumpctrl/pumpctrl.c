
#include <stdio.h>

#include <unistd.h> //sleep()

#include <sys/io.h> //outb() ioperm()


#include "pumpctrl.h"




//#define LPT1 0x0378
//#define LPT1 0xc010

#define LPT1 0xb010

#define HIGH 255
#define LOW 0



#include <stdio.h>
#include <stdlib.h>



/***********************/


void run_pump(int delay)
{
    if(ioperm(LPT1,1,1))
    { 
        fprintf(stderr, "Couldn't open parallel port"), exit(1);
    }

    // although this lets you "PWM" lets just use one pulse so we dont burn up the solenoids
    int numpulses = 1;

    for(int a=0;a<numpulses;a++)
    {
        outb(255,LPT1);  //set all pins hi
        usleep(delay); 
        outb(0,LPT1);    //set all pins lo
        usleep(delay); 
    }

}


/***********************/

void set_byte(uint byte)
{
    if(ioperm(LPT1,1,1))
    { 
        fprintf(stderr, "Couldn't open parallel port"), exit(1);
    }

    outb(byte,LPT1);  
}



/***********************/


int main(int argc, char *argv[]) 
{


    //set_byte(atoi(argv[1]));
    run_pump(300000);


    return 0;
}

