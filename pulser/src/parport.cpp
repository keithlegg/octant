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

#include "parport.h"
#include "globals.h"
#include "plot.h"







extern bool tog_testport; 




/***************************************/
/***************************************/





/***************************************/
//DEBUG - devise a way to dynamically asign pins with config file 

/*

       Data State: 

       All pins pulled down  == 0x80 - 10000000
       ACK       - pin 10 - 0xc0     - 11000000
       BUSY      - pin 11 - 0x0      - 00000000 (active high)
       PAPER OUT - pin 12 - 0xa0     - 10100000
       SELECT    - pin 13 - 0x08     - 00001000
       LINE FEED - pin 14 - 0x88     - 10001000 (active high)
       ERROR     - pin 15 - 0x90     - 10010000


 void map_pin_to_mask(void)
 { 
 
     unsigned char pin_10_mask = 0b11000000;
     unsigned char pin_12_mask = 0b10100000;
     unsigned char pin_13_mask = 0b00010000;
 }
*/



/***************************************/
unsigned char last_byte;
unsigned char data_read;

int sampa = -99;
int sampb = -99;
int lstep = 0;
int ldir = 0;

void cnc_parport::decode_quadrature(cncglobals* cg, 
                                    int* step,
                                    int* dir,
                                    unsigned char* a_sigmask,
                                    unsigned char* b_sigmask,
                                    bool* stale)
{
    if(ioperm(cg->parport1_addr+1,1,1))
    { 
        fprintf(stderr, "# Couldn't open parallel port \n"), exit(1);
    }    

    //sample the data from parallel port input register
    data_read = inb(cg->parport1_addr+1); 

    //poll the samples - probably not the best way 
    //I know of no way to use interrupts, sadly 
    int newa = ((data_read & *a_sigmask) == *a_sigmask);
    int newb = ((data_read & *b_sigmask) == *b_sigmask);
     
    //detect direction change and determine which changed first, A or B
    if (newa != sampa || newb != sampb) 
    {
        //detect changes on A
        if(newa!=sampa && newb==sampb){
            if(newa==newb){ldir = !newa;}
        //detect changes on B            
        }else if(newa==sampa && newb!=sampb){
            if(newa==newb){ldir = !newa;}
        } 
    }; 
    
    //dont update if nothing to update  
    if(data_read==last_byte)
    {  
        *stale=true;
    }

    //only "decide" the data state when things change 
    if(data_read!=last_byte)
    {  
        sampa = newa;
        sampb = newb;  
        lstep++; 
        
        //only set new data when asked for it
        if(stale)
        {
            *step=lstep;
            *dir=ldir;
            *stale=false;
        }
    }    
 
    last_byte = data_read;
 
} 


/***************************************/
void cnc_parport::test_inputs(cncglobals* cg, unsigned char* data)
{

    if(ioperm(cg->parport1_addr+1,1,1))
    { 
        fprintf(stderr, "# Couldn't open parallel port \n"), exit(1);
        //std::cout << "# Couldn't open parallel port \n";

    }

    unsigned char pin_10_mask = 0b11000000;
    unsigned char pin_12_mask = 0b10100000;
    unsigned char pin_13_mask = 0b00010000;

    unsigned char data_read;
    *data = inb(cg->parport1_addr+1); 
    
    //printf("Data read from parallel port: 0x%x\n", data_read);
    /*
    //X
    if ((data_read & pin_10_mask)==pin_10_mask)
    {
        std::cout << "X limit triggered (p10) \n";  
    };

    //Z
    if ((data_read & pin_12_mask)==pin_12_mask)
    {
        std::cout << "Z limit triggered (p12) \n";  
    };

    //Y 
    if ((data_read & pin_13_mask)==pin_13_mask)
    {
        std::cout << "Y limit triggered (p13) \n";  
    };*/


}



/***************************************/
//turn auxillary device on  (extruder, spindle motor, etc)

void cnc_parport::aux_on(cncglobals* cg, unsigned int pin)
{
    if(ioperm(cg->parport1_addr,1,1))
    { 
        fprintf(stderr, "# Couldn't open parallel port \n"), exit(1);
    
    }

    //read the byte to get the current state, 
    //then twiddle the pins accordingly 
    unsigned char data_read;
    data_read = inb(cg->parport1_addr);
    data_read = data_read |= (1 << pin);
    outb(data_read, cg->parport1_addr);            
}



/***************************************/
//turn auxillary device off  (extruder, spindle motor, etc)

void cnc_parport::aux_off(cncglobals* cg, unsigned int pin)
{
    if(ioperm(cg->parport1_addr,1,1))
    { 
        fprintf(stderr, "# Couldn't open parallel port \n"), exit(1);
    
    }

    //read the byte to get the current state, 
    //then twiddle the pins accordingly 
    unsigned char data_read;
    data_read = inb(cg->parport1_addr);
    data_read = data_read &= ~(1 << pin);
    outb(data_read, cg->parport1_addr);    

}





/***************************************/
/*
    test that port is working 
*/

void cnc_parport::test_port_output(cncglobals* cg)
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

       DEBUG - need to wire up pin assignments from configuration file

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
    
    DEBUG - need to add a file configurable pin assignment 


    OLD DB25 PINOUT (using the CNC4PC/LinuxCNC board as my "default")

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



    DEBUG progress is not tested yet 

*/

void cnc_parport::send_pulses(float* pt_progress, cncglobals* cg, cnc_plot* pt_plot )
{
    //unsigned char send_byte = 0x00;
    unsigned int send_byte = 0;

    bool debug         = false;
    bool enable_send   = true; 
    bool enable_limits = false; 

    if (debug)
    {
        std::cout << "# we have pulses! count: " << pt_plot->pulsetrain.size() << "\n";
        std::cout << "# parport address is :   " << cg->parport1_addr          << "\n";
        std::cout << "# delay us           :   " << cg->pp1_pulse_dly_us       << "\n";
    }

    if(enable_send==1)
    {
        if(ioperm(cg->parport1_addr,1,1))
        { 
            fprintf(stderr, "# Couldn't open parallel port \n"), exit(1);
        }
        if(debug)
        {
            std::cout << "# transmitting pulses to LPT port \n";
        }
    }

    //**************************//
    Vector3 dirpulses = pt_plot->pulsetrain.at(0);
    
    Vector3 limit_switches = Vector3(0,0,0);


    if(enable_send==0)
    {
        std::cout <<"# debug - send_pulses dir "<< dirpulses.x<<" " << dirpulses.y<<" " << dirpulses.z <<"\n";
    }

    if(enable_send==1)
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
    for(x=1;x<pt_plot->pulsetrain.size();x++)
    {

        //update the progress so we can display it in the GUI 
        *pt_progress = (int) pt_plot->pulsetrain.size()/x;


        //if(enable_send==0)
        //{
            std::cout << pt_plot->pulsetrain.at(x).x<<" " 
                      << pt_plot->pulsetrain.at(x).y<<" " 
                      << pt_plot->pulsetrain.at(x).z <<"\n";
        //}

        if(enable_send==1)
        {
            
            // watch the limit switches - if triggered, switch off motors NOW!
            if(enable_limits)
            {
                read_limits(cg, &limit_switches);
            }

            //if limits are good (or disabled) - run it 
            if(limit_switches.x==1 || limit_switches.y==1 || limit_switches.z==1){
                std::cout << "machine has crashed. Condolences. pulsing aborted. ";
            }
            else
            {
                //X channel 
                if(pt_plot->pulsetrain.at(x).x==1){
                    send_byte = send_byte |= (1 << 0);
                    outb(send_byte, cg->parport1_addr); 
                }else{
                    send_byte = send_byte &= ~ (1 << 0);
                    outb(send_byte, cg->parport1_addr);  
                }
                    
                //Y channel
                if(pt_plot->pulsetrain.at(x).y==1){
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
                if(pt_plot->pulsetrain.at(x).z==1){
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

    if(enable_send ==1 && debug)
    {
        std::cout << "finished transmitting pulses.\n";
    }


}




/***************************************/



