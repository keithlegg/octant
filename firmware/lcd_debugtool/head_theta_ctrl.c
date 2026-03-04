
#include <avr/io.h>


#include "head_theta_ctrl.h"

/*******************************/

/*


   Sowftare only step-dir 

   2 wires NEVER both on at the same time 
   
   Written for the TA8435H on an ancient Amistar pick and place machine.
   This hardware just needs a PWM pulse for CW and a PWM pulse for CCW 
   

   OUTPUTS:
       CW  (clockwise pusle)
       CCW (counter clockwise pusle)       


*/   



/*******************************/



#define sbi(a, b) (a) |= (1 << (b))
#define cbi(a, b) (a) &= ~(1 << (b))
 

#define MOTOR_CTRL_PORT_DDR DDRA
#define MOTOR_CTRL_PORT PORTA



uint8_t MOTOR_ENABLE = 1;
 

//the built in delay functions donmt work for me 
//likely some damn problem with my fuse bits?
 
#define nop() asm volatile( "nop ")

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

/*******************************/

void nopy(void)
{
	for(uint8_t c=0;c<1;c++)
    {nop();
    }
}

/*******************************/
//not exactcly PWM but it works 
void gen_pulses( volatile uint8_t *port, uint8_t pin, 
                 uint16_t num, uint16_t del)
{

    for(uint16_t c=0;c<num;c++)
    {
        sbi(*port, pin );
        //if(usedelay) delay(del);
        nopy();

        cbi(*port, pin );
        //if(usedelay) delay(del); 
        nopy();
    } 
 
}




/*******************************/

//TA8435H datasheet says clock should be bewteen 10-100Khz   
//we want these pulses MUCH slower

void run_steppers(void)
{
    uint16_t freq_adj = 1;

    //one pin needs to be high while the other is pulsing
    sbi(MOTOR_CTRL_PORT, 1 );
    gen_pulses(&MOTOR_CTRL_PORT, 0,
               1000, freq_adj);            
    

    //let things settle, dude        
    longdelay(10);
    
    //------------------------------//
    //NOW RUN THE OTHER WAY   

    //one pin needs to be high while the other is pulsing
    sbi(MOTOR_CTRL_PORT, 0 ); 
    gen_pulses(&MOTOR_CTRL_PORT, 1,
               1000, freq_adj);
    //let things settle, dude
    longdelay(10);

} 
 
/*******************************/



