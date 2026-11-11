/*************************************************************/
/*


    testport - little bit twiddler for a modern-ish PC parallel port 
    

    MIT License

    Copyright (c) 2026 Keith Legg - keithlegg23@gmail.com

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
         
/*************************************************************/


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


/*

    HELP 

    #There are 3 commands that are going to let you know that the Linux kernel has found your Parallel Port.
    sudo lspci -nnv
    sudo cat /proc/ioports | grep parport
    sudo dmesg | grep parport

    #is it listed in devices?
    ls /dev | grep par

    #is the driver loaded?
    lsmod | grep ppdev

    #if you want to add it to /etc/modules 
    #echo "ppdev" | sudo tee -a /etc/modules

    #add user to the lp group  
    sudo usermod -a -G lp $USER

    #get addresses?
    cat /proc/ioports | grep par

    #try writing data to it?
    # echo "Test string" > /dev/parport0
    #try reading data from it 
    # cat /dev/parport0

    #############
    #set the executable to run without root permsission

    sudo setcap cap_sys_rawio+ep testport

    #############

    #more crap to try 


    sudo rmmod parport_pc
    sudo modprobe parport_pc 

    ################
    #try creating /etc/modprobe.d/parport_pc.conf
    #with the following:  

    install parport_pc setpci -d 125b:9100 command=101; /sbin/modprobe --ignore-install parport_pc
    options parport_pc io=0xe010 irq=auto


    #Be sure to replace 125b:9100 with the device ID from the first line of 
    sudo lspci -nnv 
    #and replace 0xe010 with the I/O port from the fourth line.
    #Now when the "paraport_pc" module is loaded it will set up the I/O ports at the same time.


    #to load it 
    sudo modprobe parport_pc

*/

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
void send_byte_allbits(void)
{
    uint a = 0;
    for(a=0;a<256;a++)
    {
        set_byte(a);
        usleep(100000);
    }

}




/***********************/


int main(int argc, char *argv[]) 
{
        
    // flash_byte();
    // flash_ctrl();
    // send_byte_allbits();

    set_byte(atoi(argv[1]));


    return 0;
}

