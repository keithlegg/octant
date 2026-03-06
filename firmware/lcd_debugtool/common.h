
#ifndef _COMMON_TOOLS_
#define _COMMON_TOOLS_


#define nop() asm volatile( "nop ")

#define sbi(a, b) (a) |= (1 << (b))
#define cbi(a, b) (a) &= ~(1 << (b))
 

//beware PORTE has TX/RX in it 
#define ATC_CTRL_PORT_DDR DDRE
#define ATC_CTRL_PORT PORTE

#include <avr/io.h>


//-----------------------

#define HEAD_1_CTRL_PORT_DDR DDRA
#define HEAD_1_CTRL_PORT PORTA

/*
#define HEAD_1_CTRL_PORT_DDR 
#define HEAD_1_CTRL_PORT 

#define HEAD_1_CTRL_PORT_DDR 
#define HEAD_1_CTRL_PORT 

#define HEAD_1_CTRL_PORT_DDR 
#define HEAD_1_CTRL_PORT 
*/

//-----------------------




// void delay(uint16_t num);
// void longdelay(uint16_t num);



#endif