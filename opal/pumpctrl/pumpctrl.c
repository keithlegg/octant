/*************************************************************/
/*

    pumpctrl - take liquids in one place and get them to another.

    
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
         
/*************************************************************/


#include <stdio.h>

#include <unistd.h> //sleep()

#include <sys/io.h> //outb() ioperm()


#include "pumpctrl.h"




//#define useport 0x0378
//#define useport 0xc010

#define useport 0xb010

#define HIGH 255
#define LOW 0



#include <stdio.h>
#include <stdlib.h>



/***********************/


void run_pump(int delay)
{
 

    if(ioperm(useport,1,1))
    { 
        fprintf(stderr, "Couldn't open parallel port"), exit(1);
    }



    //first read the state of the register (so we dont mangle the wrong bits) 
    unsigned char data_read;
    data_read = inb(useport);

    //twiddle the pins accordingly     
    data_read = data_read |= byte;
    outb(byte, useport); 
    usleep(delay); 

    // //twiddle the pins accordingly     
    // data_read = data_read |= byte;
    // outb(byte, useport); 
    // usleep(delay); 
 

}


/***********************/

void set_byte(uint byte)
{
    if(ioperm(useport,1,1))
    { 
        fprintf(stderr, "Couldn't open parallel port"), exit(1);
    }

    outb(byte,useport);  
}



/***********************/


int main(int argc, char *argv[]) 
{


    //set_byte(atoi(argv[1]));
    run_pump(300000);


    return 0;
}

