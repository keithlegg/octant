
#include <avr/io.h>
#include <util/delay.h>


#include "hio_util.h"


/*******************************/

/*
   
   control a stepper on amistar pick and place machine 

   each direction gets a pin (NOT both at the same time)

   use a single input for the optical encoder to mark the zero position 


   INPUTS:
       optical encoder - 5V (up to 24 on this machine)

   OUTPUTS:
       pulse 


   HARDWARE PINOUT:



*/   



/*******************************/



#define sbi(a, b) (a) |= (1 << (b))
#define cbi(a, b) (a) &= ~(1 << (b))


//built in led is PORTB7
#define TEST_LED_DDR DDRB
#define TEST_LED_PORT PORTB

//encoder is on same port as outputs 
#define MOTOR_ENCODER_IN_DDR DDRA
#define MOTOR_ENCODER_IN_PORT PINA

#define MOTOR_PULSE_DDR DDRA
#define MOTOR_PULSE_PORT PORTA

//------------


//each direction gets its own pin 
//assume same port 
#define MOTOR_PULSE_PIN_CW 3   //ard D25
#define MOTOR_PULSE_PIN_CCW 5  //ard D27

#define MOTOR_ENCODER_INPUT 1  //ard D22




const uint16_t delval = 200; 


/*******************************/
void setup_ports(void)
{
    TEST_LED_DDR    = 0xff;

    // Sets 0,1 as inputs  
    MOTOR_PULSE_DDR = 0xfc;

    //MOTOR_ENCODER_IN_DDR &= ~(1 << MOTOR_ENCODER_IN_DDR); 
}


/*******************************/
uint8_t read_encoder(void)
{
    uint8_t out = 0;

    // !(PINA & (1 << BTN_BIT)

    if (MOTOR_ENCODER_IN_PORT & (1 << 1)) 
    {
        out = 1;
    }else{
        out = 0;        
    } 

    return out;

}


/*******************************/
uint8_t test_encoder(void)
{
    uint8_t val = read_encoder();

    if(val==1)
    {
        TEST_LED_PORT = 0xff;
    }else{
        TEST_LED_PORT = 0x00;
    }

}

/*******************************/
void shutdown_stepper(void)
{
    cbi(MOTOR_PULSE_PORT, MOTOR_PULSE_PIN_CW );
    cbi(MOTOR_PULSE_PORT, MOTOR_PULSE_PIN_CCW );    
}


/*******************************/
void home_stepper_ccw(void)
{
    uint8_t enc_val = read_encoder();
     
    // make sure to never turn both on at same time
    sbi(MOTOR_PULSE_PORT, MOTOR_PULSE_PIN_CW );

    while(enc_val==1)
    {
        enc_val = read_encoder();

        sbi(MOTOR_PULSE_PORT, MOTOR_PULSE_PIN_CCW );
        _delay_us(delval);

        cbi(MOTOR_PULSE_PORT, MOTOR_PULSE_PIN_CCW );
        _delay_us(delval);
    }

}

/*******************************/
void home_stepper_cw(void)
{
    uint8_t enc_val = read_encoder();
     
    // make sure to never turn both on at same time
    sbi(MOTOR_PULSE_PORT, MOTOR_PULSE_PIN_CCW );

    while(enc_val==1)
    {
        enc_val = read_encoder();

        sbi(MOTOR_PULSE_PORT, MOTOR_PULSE_PIN_CW );
        _delay_us(delval);

        cbi(MOTOR_PULSE_PORT, MOTOR_PULSE_PIN_CW );
        _delay_us(delval);
    }

}


/*******************************/
void run_stepper_cw(uint16_t num_pulses)
{
    //uint8_t enc_val = 0;

    // make sure to never turn both on at same time
    sbi(MOTOR_PULSE_PORT, MOTOR_PULSE_PIN_CCW );

    for(uint16_t x = 0;x<num_pulses;x++)
    {
        //enc_val = read_encoder();

        sbi(MOTOR_PULSE_PORT, MOTOR_PULSE_PIN_CW );
        _delay_us(delval);

        cbi(MOTOR_PULSE_PORT, MOTOR_PULSE_PIN_CW );
        _delay_us(delval);
    }
}

/*******************************/
void run_stepper_ccw(uint16_t num_pulses)
{
    // make sure to never turn both on at same time
    sbi(MOTOR_PULSE_PORT, MOTOR_PULSE_PIN_CW );

    for(uint16_t x = 0;x<num_pulses;x++)
    {
        sbi(MOTOR_PULSE_PORT, MOTOR_PULSE_PIN_CCW );
        _delay_us(delval);

        cbi(MOTOR_PULSE_PORT, MOTOR_PULSE_PIN_CCW );
        _delay_us(delval);
    }
}



