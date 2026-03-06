/*
    created  : Keith Legg Jan 7 - 2016
    modified : Keith Legg Mar 1 - 2026

   //-----------------------------------
   //ORIGINAL ADAFRUIT STUFF:

    3-5V Vin connects to the Arduino 5V pin 
    GND connects to Arduino ground 
    CLK connects to SPI clock. Digital 13. 
        On Mega's, its Digital 52 and on Leonardo/Due its ICSP-3 (See SPI Connections for more details) 
    MOSI connects to SPI MOSI.  Digital 11. 
        On Mega's, its Digital 51 and on Leonardo/Due its ICSP-4 (See SPI Connections for more details) - this is the white wire
    CS connects to our SPI Chip Select pin. 
        We'll be using Digital 10 but you can later change this to any pin  
    RST connects to our TFT reset pin.
        We'll be using Digital 9 but you can later change this pin   
    D/C connects to our SPI data/command select pin. 
        We'll be using Digital 8 but you can later change this pin  

   //-----------------------------------
   //OBSERVATIONS 
      SPI:
         MSB first - Leading edge 
         ~2us per byte speed estimate 

     DC IS LOW FOR CMDS (ST7735_SWRESET, ETC) - FOR "ARGS" IT IS HIGH
     ADAFRUIT CODE CALLS DC  "RS" or "_rs"

   //-----------------------------------
   // HARDWARE WIRING  -PORTB CONFLICTS WITH SPI!!!!
   pin8   | PB0 | D/C
   pin9   | PB1 | NC           - RESET HARDWIRED HIGH 
   pin10  | PB2 | SPI ENABLE   - TFT_CS 
   pin11  | PB3 | SPI MOSI     - WE RUN AS SPI MASTER 
   pin12  | PB4 | NC - MISO    - NOT USED 
   pin13  | PB5 | SPI CLOCK    - 
   +5/+3  |     | RESET 
          |     | NC - CARD_CS 

   //-----------------------------------
   |PORTF | PORTA| 
   |PORTE | PORTC| <---- PORT E CONFLICTS WITH TX/RX
   |-------------| 
   |PORTB |<----- CONFLICTS WITH SPI - BEWARE
   |PORTD |<----- BEWARE
   //-----------------------------------
*/


#include <avr/io.h>
#include <avr/interrupt.h>

// Define software reference clock for delay duration
#define F_CPU 16000000 
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

#include "ST7735.h"

#include "kernel_splatr.h"


#include "common.h"

#include "head_theta_ctrl.h"
#include "toolchanger.h"



 


volatile extern uint8_t MOTOR_ENABLE = 1;





/*******************************/
//yay - finally found a good use of pointers in AVR C
uint8_t and_bits(uint8_t *byte, uint8_t byte_a, uint8_t byte_b){
    *byte = 0xff;
    *byte &= byte_a;
    *byte &= byte_b;
}

/*******************************/
//yay - finally found a good use of pointers in AVR C
uint8_t or_bits(uint8_t *byte, uint8_t byte_a, uint8_t byte_b){
    *byte = 0x00;
    *byte |= byte_a;
    *byte |= byte_b;
}



/*******************************/
/*THESE USE GLOBALS INSTEAD OF RETURNING A VALUE!!!*/
/*
void cvt_16_to_8x2(uint16_t input){
   //break 16 bit into 2 bytes - output goes to global buffer
    if(input<256){
        buf1_lsb = input; 
        buf1_msb = 0; 
    }        
    if(input>=256){
        buf1_lsb = (uint8_t)(input & 0x00FF);
        buf1_msb = (uint8_t)(input >> 8);
    }
}

uint16_t cvt_8x2_to_16(uint8_t msbin, uint8_t lsbin){
   uint8_t dat_8;
   uint16_t dat_16;
   dat_8 = msbin;
   dat_16 = (lsbin<<8);
   return (dat_16 | dat_8);
}
*/


/*******************************/

//pin change LOW ISR on DB25 pin 1 (inverted pin, STROBE)

void setup_isr(void)
{
    //## PD0 == INT0 

    // setup pin change interrupts 
    DDRD &= ~(1 << DDD0);              // Clear the PD0 pin
    PORTD |= (1 << PD3);               // turn on the pull-ups


    // Interrupt 0 Sense Control
    //EICRA |= (1 << ISC01); // trigger on falling edge
    // Interrupt 1 Sense Control
    //EICRA |= (1 << ISC11); // trigger on falling edge
    // External Interrupt Mask Register
    //EIMSK |= (1 << INT0)|(1 << INT1);   // Turns on INT0 and INT1
    
    EIMSK |= (1 << INT0);   // Turns on INT0 

    sei();
}
 



/*******************************/
uint8_t mybyte = 0x00;

int main (void)
{

    DDRA = 0xff; 
    DDRC = 0x00; 
    DDRE = 0x00;
    DDRG = 0xff; //for SPI/TFT
    DDRB = 0xff; //for SPI
    DDRF = 0x00; //DB25 byte in 
  

    SPCR |= (1<<MSTR)|(1<<SPE);  // SPI Master, SPI Enable
    SPSR |= SPI2X;
   
    ST7735_InitR(INITR_BLACKTAB);  
    ST7735_FillScreen(0x00);


    
    setup_isr(); //ISR is PortD pin0


    set_atc_ports();



    while(1)
    {
        //debug_parport(mybyte);
        //_delay_ms(100);  
        
        //run_head_theta();

        run_atc_test();
        
     

    }
    
    


}//main



 
// falling edge pin change ISR  
ISR (INT0_vect)
{
    mybyte = PINF;
}
 





