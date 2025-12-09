/*************************************************************/
/*
    cnc_io.cpp 
    
    interface to a parallel port (and hopefully someday a Mesa5i25 card)


    MIT License

    Copyright (c) 2025 Keith Legg - keithlegg23@gmail.com

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
         

/*****************************************************************************/

#include <unistd.h> //sleep()
#include <sys/io.h> //outb() ioperm()

#include <math.h>
#include <iomanip>

#include <iostream>
#include <algorithm>
#include <vector>


//#include "math_op.h"
//#include "point_op.h"


#include "cnc_globals.h"
#include "cnc_plot.h"
#include "cnc_parport.h"



// µs (microsecond) duration between pulses 
// gecko docs say minimun pulse with is 2.5µs per pulse - seems way too fast for me 
//int pulse_del = 1000;




/***************************************/
void cnc_parport::aux_on(cncglobals* cg, unsigned int pin)
{
    if(ioperm(cg->parport1_addr,1,1))
    { 
        fprintf(stderr, "# Couldn't open parallel port \n"), exit(1);
    
    }

    unsigned char data_read;
    data_read = inb(cg->parport1_addr);
    data_read = data_read |= (1 << pin);
    outb(data_read, cg->parport1_addr);            


}



/***************************************/
void cnc_parport::aux_off(cncglobals* cg, unsigned int pin)
{
    if(ioperm(cg->parport1_addr,1,1))
    { 
        fprintf(stderr, "# Couldn't open parallel port \n"), exit(1);
    
    }

    unsigned char data_read;
    data_read = inb(cg->parport1_addr);
    data_read = data_read &= ~(1 << pin);
    outb(data_read, cg->parport1_addr);    

}



/***************************************/

void cnc_parport::test_port(cncglobals* cg)
{
    if(ioperm(cg->parport1_addr,1,1))
    { 
        fprintf(stderr, "# Couldn't open parallel port \n"), exit(1);
    
    }

    // unsigned char send_byte = 0x00;
    unsigned int send_byte = 0;

    int a=0;int b=0;

    outb(0x00,cg->parport1_addr); 
    //for(b=0;b<4;b++)
    //{
        send_byte = 0x01;
        for(a<0;a<8;a++)
        {
            outb(send_byte,cg->parport1_addr);
            usleep(500000); 
                       
            outb(0x00,cg->parport1_addr); 
            usleep(500000); 
            send_byte = send_byte << 1;
            std::cout <<"bit "<< a <<" value: "<< HEX(send_byte) <<"\n";

        }
    //}

}




/***************************************/
/*
       Read the parallel port inputs to make sure we didnt crash the machine 

       Data State: 

       All pins pulled down  == 0x80 - 10000000
       ACK       - pin 10 - 0xc0     - 11000000
       BUSY      - pin 11 - 0x0      - 00000000 (active high)
       PAPER OUT - pin 12 - 0xa0     - 10100000
       SELECT    - pin 13 - 0x08     - 00001000
       LINE FEED - pin 14 - 0x88     - 10001000 (active high)
       ERROR     - pin 15 - 0x90     - 10010000

                            
*/

void cnc_parport::read_limits(cncglobals* cg, Vector3* pt_limit_switch_data)
{

    if(ioperm(cg->parport1_addr+1,1,1))
    { 
        fprintf(stderr, "# Couldn't open parallel port \n"), exit(1);
    
    }

    unsigned char pin_10_mask = 0b11000000;
    unsigned char pin_12_mask = 0b10100000;
    unsigned char pin_13_mask = 0b00010000;

    unsigned char data_read;
    data_read = inb(cg->parport1_addr+1); 
    
    //printf("Data read from parallel port: 0x%x\n", data_read);

    //X
    if ((data_read & pin_10_mask)==pin_10_mask)
    {
        std::cout << "X limit triggered (p10) \n";  
        pt_limit_switch_data->x =1;
    };

    //Z
    if ((data_read & pin_12_mask)==pin_12_mask)
    {
        std::cout << "Z limit triggered (p12) \n";  
        pt_limit_switch_data->z =1;
    };

    //Y 
    if ((data_read & pin_13_mask)==pin_13_mask)
    {
        std::cout << "Y limit triggered (p13) \n";  
        pt_limit_switch_data->y =1;        
    };


}

/***************************************/
/*
    take the output of calc_3d_pulses() and send the signals to the parallel port 
    
    The first elemtent of the array denotes direction pulses
    

    DB25 PINOUT (using the CNC4PC/LinuxCNC board as my "default")

    db25 pin #2 - X pulse  -  address 0x01  -  bitshift (1<<0) 
    db25 pin #3 - X dir    -  address 0x02  -  bitshift (1<<1)
    db25 pin #4 - Y pulse  -  address 0x04  -  bitshift (1<<2)
    db25 pin #5 - Y dir    -  address 0x08  -  bitshift (1<<3)
    db25 pin #6 - Z pulse  -  address 0x10  -  bitshift (1<<4)
    db25 pin #7 - Z dir    -  address 0x20  -  bitshift (1<<5)


    #optional pulses
    db25 pin #8 - INV Y pulse   -  address 0x40  -  bitshift (1<<6)


    Args:

        pt_progress    - update a float from 0-1 on how far we are in the pulsetrain
        cncglobals     - pointer to globals 
        pt_pulsetrain  - pointer to the data to send (XYZ array of 1s and 0s with the first element indicating direction)


*/

void cnc_parport::send_pulses(float* pt_progress, cncglobals* cg, vector<Vector3>* pt_pulsetrain)
{
    //unsigned char send_byte = 0x00;
    unsigned int send_byte = 0;

    int send_it = 1; 

    std::cout << "# we have pulses! count: " << pt_pulsetrain->size() << "\n";

    if(send_it==1)
    {
        if(ioperm(cg->parport1_addr,1,1))
        { 
            fprintf(stderr, "# Couldn't open parallel port \n"), exit(1);
        }
        std::cout << "# transmitting pulses to LPT port \n";
    }

    //**************************//
    Vector3 dirpulses = pt_pulsetrain->at(0);
    
    Vector3 limit_switches;


    if(send_it==0)
    {
        std::cout <<"# debug - direction "<< dirpulses.x<<" " << dirpulses.y<<" " << dirpulses.z <<"\n";
    }

    if(send_it==1)
    {   
        //x direction high 
        if (dirpulses.x>1){
            //outb(0x02, cg->parport1_addr);
            send_byte = send_byte |= (1 << 1);
            outb(send_byte, cg->parport1_addr);            
        }else{
             //outb(0x00, cg->parport1_addr);  
            send_byte = send_byte &= ~(1 << 1);
            outb(send_byte, cg->parport1_addr);               
        }

        /*****/

        //y direction high 
        if (dirpulses.y>1){
            send_byte = send_byte |= (1 << 3);
            outb(send_byte, cg->parport1_addr);  
            
            //!! THIS IS EXTRA FOR A GANTRY MACHINE INVERTED Z AXIS
            //send_byte = send_byte &= ~(1 << 5);   
            //outb(send_byte, cg->parport1_addr); 

        }else{
            //y direction low         
            send_byte = send_byte &= ~(1 << 3);
            outb(send_byte, cg->parport1_addr);   

            //!! THIS IS EXTRA FOR A GANTRY MACHINE INVERTED Z AXIS
            //send_byte = send_byte |= (1 << 5);
            //outb(send_byte, cg->parport1_addr);                          

        }


        /*****/
        //THIS IS A STANDARD 3D SETUP USING Z AXIS

        //z direction high 
        if (dirpulses.z>1){
            send_byte = send_byte |= (1 << 5);
            outb(send_byte, cg->parport1_addr);               
        }else{
            send_byte = send_byte &= ~(1 << 5);   
            outb(send_byte, cg->parport1_addr);                   
        }

    }//end send pulses 


    //**************************//
    int x=0;
    

    //the first element is reserved for direction data 
    //we intentionally skip it starting at index 1 
    for(x=1;x<pt_pulsetrain->size();x++)
    {

        //update the progress so we can display it in the GUI 
        *pt_progress = (int) pt_pulsetrain->size()/x;


        if(send_it==0)
        {
            std::cout<< pt_pulsetrain->at(x).x<<" " << pt_pulsetrain->at(x).y<<" " << pt_pulsetrain->at(x).z <<"\n";
        }

        if(send_it==1)
        {
            
            // watch the limit switches - if triggered, switch off motors NOW!
            (*this).read_limits(cg, &limit_switches);
            
            if(limit_switches.x==1 || limit_switches.y==1 || limit_switches.z==1){
                std::cout << "machine has crashed. Condolences. pulsing aborted. ";
            }
            else
            {
                //X channel 
                if(pt_pulsetrain->at(x).x==1){
                    send_byte = send_byte |= (1 << 0);
                    outb(send_byte, cg->parport1_addr); 
                }else{
                    send_byte = send_byte &= ~ (1 << 0);
                    outb(send_byte, cg->parport1_addr);  
                }
                    
                //Y channel
                if(pt_pulsetrain->at(x).y==1){
                    send_byte = send_byte |= (1 << 2);
                    outb(send_byte, cg->parport1_addr);    

                    //!! THIS IS ALSO RUNNING Z AXIS (INVERTED DIR) FOR A GANTRY 
                    //send_byte = send_byte |= (1 << 4);
                    //outb(send_byte, cg->parport1_addr);    

                }else{
                    send_byte = send_byte &= ~(1 << 2);
                    outb(send_byte, cg->parport1_addr);           

                    //!! THIS IS ALSO RUNNING Z AXIS (INVERTED DIR) FOR A GANTRY 
                    //send_byte = send_byte &= ~(1 << 4);
                    //outb(send_byte, cg->parport1_addr);                    
                }

                  
                //standard Z channel
                if(pt_pulsetrain->at(x).z==1){
                    send_byte = send_byte |= (1 << 4);
                    outb(send_byte, cg->parport1_addr);   
                }else{
                    send_byte = send_byte &= ~(1 << 4);
                    outb(send_byte, cg->parport1_addr);                 
                }
            

            }

            usleep(cg->pp1_pulse_dly_us); 
        }

    }

    if(send_it==1)
    {
        std::cout << "finished transmitting pulses.\n";
    }


}




/***************************************/



