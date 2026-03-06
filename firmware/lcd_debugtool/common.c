
#include <avr/io.h>

#include "common.h"



//the built in delay functions donmt work for me 
//likely some damn problem with my fuse bits?

//linker not seeing these and right now I dont care - just duplicate them 

/*
void delay(uint16_t num)
{
	for(uint16_t c=0;c<num;c++)
    {
        nop();
        nop();
    }
}

void longdelay(uint16_t num)
{
	for(uint16_t c=0;c<num;c++)
    {
        delay(1000);
    }
}
*/
