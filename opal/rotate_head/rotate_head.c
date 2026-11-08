
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

