#ifndef _THETA_CTRL_
#define _THETA_CTRL_

#include <avr/io.h>
#include <util/delay.h>


#include "hio_util.h"



/*******************************/


//each direction gets its own pin 
//assume same port 
#define MOTOR_PULSE_PIN_CW 3   //ard D25
#define MOTOR_PULSE_PIN_CCW 5  //ard D27

#define MOTOR_ENCODER_INPUT 1  //ard D22



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


void setup_ports(void);


uint8_t read_encoder(void);
uint8_t test_encoder(void);

void shutdown_stepper(void);

void home_stepper_ccw(void);
void home_stepper_cw(void);

void run_stepper_cw(uint16_t num_pulses);
void run_stepper_ccw(uint16_t num_pulses);







#endif