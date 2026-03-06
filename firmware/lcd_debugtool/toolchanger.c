
#include <avr/io.h>


#include "common.h"

#include "toolchanger.h"

/*******************************/

/*


   Sowftare only step-dir 

   2 wires NEVER both on at the same time 
   
   Written for the TA8435H on an ancient Amistar pick and place machine.
   This hardware just needs a PWM pulse for CW and a PWM pulse for CCW 
   

   OUTPUTS:
       CW  (clockwise pusle)
       CCW (counter clockwise pusle)       



   //-----------------------------------
   |PORTF | PORTA| 
   |PORTE | PORTC| <---- PORT E CONFLICTS WITH TX/RX
   |-------------| 
   |PORTB |<----- CONFLICTS WITH SPI - BEWARE
   |PORTD |<----- BEWARE
   //-----------------------------------

*/   

volatile extern uint8_t MOTOR_ENABLE;


/*******************************/
/*******************************/
//DEBUG move these to common.c when I figure out the damn linker issue 
//BETTER YET GET RID OF THEM AND FIGURE OUT THE DAMN BUILT IN DELAY FUNCTIONS 

static void delay(uint16_t num)
{
    for(uint16_t c=0;c<num;c++)
    {
        nop();
        nop();
    }
}

static void longdelay(uint16_t num)
{
    for(uint16_t c=0;c<num;c++)
    {
        delay(1000);
    }
}


 
/*******************************/
/*******************************/
 

void set_atc_ports()
{ 
    ATC_CTRL_PORT_DDR = 0xff;
    ATC_CTRL_PORT=0x00;
}


/*******************************/
//inverted! OFF is ON in this world 
void run_atc_test(void)
{
 
    uint8_t x=0;

 

    for(x=0;x<4;x++)
    {
        sbi(ATC_CTRL_PORT, x ); 
        longdelay(10);
        
        cbi(ATC_CTRL_PORT, x ); 
 

    }
    

} 
 
/*******************************/



