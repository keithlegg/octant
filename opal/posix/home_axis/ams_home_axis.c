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
// to compile                 :  gcc -O parport.c -o parport
// after compiling, set suid  :  chmod +s parport   
// copy to /usr/sbin/         :  sudo cp parport /usr/sbin/



//HELP 

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



int main(void) 
{
    if(ioperm(LPT1,1,1))
    { 
        fprintf(stderr, "Couldn't open parallel port"), exit(1);
    }

    // if (ioperm(LPT1,1,1) == -1) //set LPT permissions
    // {
    //     printf("cant open port!\n");
    //     return -1;
    // } 

    outb(255,LPT1);  //set all pins hi
    sleep(1); 
    outb(0,LPT1);    //set all pins lo
    sleep(1); 

    outb(255,LPT1);  //set all pins hi
    sleep(1); 
    outb(0,LPT1);    //set all pins lo

    return 0;
}

