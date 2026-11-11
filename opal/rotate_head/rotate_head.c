/*************************************************************/
/*

    rotate_head - adjust the theta property of an Amistar 5600 pick and place head X4
    

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


//#define LPT1 0x0378
//#define LPT1 0xc010

#define LPT1 0xb010

#define HIGH 255
#define LOW 0



#include <stdio.h>
#include <stdlib.h>

//#include <unistd.h>
//#include <asm/io.h>

/***********************/

void flash_byte(void)
{
    if(ioperm(LPT1,1,1))
    { 
        fprintf(stderr, "Couldn't open parallel port"), exit(1);
    }

    outb(255,LPT1);  //set all pins hi
    sleep(1); 
    outb(0,LPT1);    //set all pins lo
    sleep(1); 

    outb(255,LPT1);  //set all pins hi
    sleep(1); 
    outb(0,LPT1);    //set all pins lo  
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
//+1 is input , +2 is control lines output  

void flash_ctrl(void)
{

    if(ioperm(LPT1+2,1,1))
    { 
        fprintf(stderr, "Couldn't open parallel port"), exit(1);
    }

    outb(0xff,LPT1+2);  //set all pins hi
    sleep(1); 
    outb(0,LPT1+2);    //set all pins lo
    sleep(1); 

    outb(0xff,LPT1+2);  //set all pins hi
    sleep(1); 
    outb(0,LPT1+2);    //set all pins lo  
}





/***********************/


int main(int argc, char *argv[]) 
{
    set_byte(atoi(argv[1]));
    return 0;
}

