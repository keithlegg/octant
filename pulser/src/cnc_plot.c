

#include <stdio.h>
#include <unistd.h> //sleep()
#include <sys/io.h> //outb() ioperm()
#include <stdlib.h>
#include <math.h>
#include <iomanip>

#define HEX(x) setw(2) << setfill('0') << hex << (int)( x )

#include <iostream>
#include <algorithm>
#include <vector>

#include "cnc_plot.h"
#include "math_op.h"
#include "point_op.h"


// µs (microsecond) duration between pulses 
// gecko docs say minimun pulse with is 2.5µs per pulse - seems way too fast for me 
int pulse_del = 1000;



#define LPT1 0xc010
//#define LPT1 0x0378



/******************************************/
void cnc_plot::aux_on(unsigned int pin)
{
    if(ioperm(LPT1,1,1))
    { 
        fprintf(stderr, "# Couldn't open parallel port \n"), exit(1);
    
    }

    unsigned char data_read;
    data_read = inb(LPT1);
    data_read = data_read |= (1 << pin);
    outb(data_read, LPT1);            


}



/******************************************/
void cnc_plot::aux_off(unsigned int pin)
{
    if(ioperm(LPT1,1,1))
    { 
        fprintf(stderr, "# Couldn't open parallel port \n"), exit(1);
    
    }

    unsigned char data_read;
    data_read = inb(LPT1);
    data_read = data_read &= ~(1 << pin);
    outb(data_read, LPT1);    

}


/******************************************/
void cnc_plot::gen_pules(vector<int>* pt_pulsetrain, int size, int num)
{
    
    if(num>size)
    {
        cout << "# gen_pules: size arg may not exceed number \n";

        exit(1);
    }

    double div = (double)size/(double)num;
    double gran = div/num;

    int a;  

    //if zero make all zeros, we want the output to be the same size
    if(num==0)
    {
        for(a=0;a<size;a++)
        {
            pt_pulsetrain->push_back(0);            
        }
    }

    //exception for integer 1, put the pulse right in the middle of output 
    if(num==1)
    {
        for(a=0;a<size;a++)
        {

            if( a == size/2)
            {
                pt_pulsetrain->push_back(1);
            }else  
            {
                pt_pulsetrain->push_back(0);
            } 

        }
    }

    // build a pulse waveform, spread it out as evenly as possible over the entirety of the data 
    // I did a true 3D solution (commented out at bottom) but this is way faster and works as far as I can tell
    if(num>1) 
    {
        for(a=0;a<size;a++)
        {
            double chunk = fmod(a,div);
            if( chunk < 1)
            {
                pt_pulsetrain->push_back(1);
            }
            if( chunk > 1)
            {
                pt_pulsetrain->push_back(0);                   
            } 
            if ( chunk==1){
                pt_pulsetrain->push_back(0);                 
            }
        }
    } 

}
      


/******************************************/

void cnc_plot::test_port(void)
{
    if(ioperm(LPT1,1,1))
    { 
        fprintf(stderr, "# Couldn't open parallel port \n"), exit(1);
    
    }

    unsigned char send_byte = 0x00;
    int a=0;int b=0;

    outb(0x00,LPT1); 
    //for(b=0;b<4;b++)
    //{
        send_byte = 0x01;
        for(a<0;a<8;a++)
        {
            outb(send_byte,LPT1);
            usleep(500000); 
                       
            outb(0x00,LPT1); 
            usleep(500000); 
            send_byte = send_byte << 1;
            cout <<"bit "<< a <<" value: "<< HEX(send_byte) <<"\n";

        }
    //}

}




/******************************************/
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

void cnc_plot::read_limits(vec3* pt_limit_switch_data)
{

    if(ioperm(LPT1+1,1,1))
    { 
        fprintf(stderr, "# Couldn't open parallel port \n"), exit(1);
    
    }

    unsigned char pin_10_mask = 0b11000000;
    unsigned char pin_12_mask = 0b10100000;
    unsigned char pin_13_mask = 0b00010000;

    unsigned char data_read;
    data_read = inb(LPT1+1); 
    
    //printf("Data read from parallel port: 0x%x\n", data_read);

    //X
    if ((data_read & pin_10_mask)==pin_10_mask)
    {
        cout << "X limit triggered (p10) \n";  
        pt_limit_switch_data->x =1;
    };

    //Z
    if ((data_read & pin_12_mask)==pin_12_mask)
    {
        cout << "Z limit triggered (p12) \n";  
        pt_limit_switch_data->z =1;
    };

    //Y 
    if ((data_read & pin_13_mask)==pin_13_mask)
    {
        cout << "Y limit triggered (p13) \n";  
        pt_limit_switch_data->y =1;        
    };


}


/******************************************/




void cnc_plot::load_config_file(char* filepath)
{

    /*  
    printf("#loading setup file %s \n", filepath );
    
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(filepath, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    obj_filepaths.clear();
    float vx,vy,vz,cr,cg,cb = 0;

    // walk the file line by line
    while ((read = getline(&line, &len, fp)) != -1) 
    {

        char cmd_str[256]; // string that verts get copied to 
        
        // walk the line, token by token  
        char* tok_spacs = strtok(line, " ");
        while (tok_spacs) 
        {
            //------------------------------- 
            if ( strcmp( tok_spacs, "tcp_port") == 0)
            {
                strcpy (cmd_str, line);
                char* tok_line = strtok(0, " ");
                int tidx = 0;
                while (tok_line) 
                {
                    if(tidx==0){TCP_PORT = atoi(tok_line);}
                    tidx++;                                        
                    tok_line = strtok(NULL, " \t\n");      

                }
                printf("TCP PORT SET TO %u \n", TCP_PORT );
            }

            //-------------------------------             
            if ( strcmp( tok_spacs, "show_lines") == 0)
            {
                strcpy (cmd_str, line);

                //walk the tokens on the line (a copy of it)
                char* tok_line = strtok(NULL, " \t\n");
                
                int tidx = 0;
                while (tok_line) 
                {
                    char* check_token = strtok(NULL, " \t\n");
                    if(strcmp( tok_line, "true")==0)
                    {
                        draw_lines = true;
                    }else{
                        draw_lines = false;                        
                        printf("# draw lines %s\n", draw_lines ? "true" : "false");
                    }
                                                          
                    tok_line = strtok(NULL, " \t\n");

                }
          
            }

            */


}


/******************************************/
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



*/

void cnc_plot::send_pulses(vector<vec3>* pt_pulsetrain)
{
    unsigned char send_byte = 0x00;
    int send_it = 1; 

    cout << "# we have pulses! count: " << pt_pulsetrain->size() << "\n";

    if(send_it==1)
    {
        if(ioperm(LPT1,1,1))
        { 
            fprintf(stderr, "# Couldn't open parallel port \n"), exit(1);
        }
        cout << "# transmitting pulses to LPT port \n";
    }

    //**************************//
    vec3 dirpulses = pt_pulsetrain->at(0);
    
    vec3 limit_switches;


    if(send_it==0)
    {
        cout <<"# debug - direction "<< dirpulses.x<<" " << dirpulses.y<<" " << dirpulses.z <<"\n";
    }

    if(send_it==1)
    {   
        //x direction high 
        if (dirpulses.x>1){
            //outb(0x02, LPT1);
            send_byte = send_byte |= (1 << 1);
            outb(send_byte, LPT1);            
        }else{
             //outb(0x00, LPT1);  
            send_byte = send_byte &= ~(1 << 1);
            outb(send_byte, LPT1);               
        }

        /*****/

        //y direction high 
        if (dirpulses.y>1){
            send_byte = send_byte |= (1 << 3);
            outb(send_byte, LPT1);  
            
            //!! THIS IS EXTRA FOR A GANTRY MACHINE INVERTED Z AXIS
            //send_byte = send_byte &= ~(1 << 5);   
            //outb(send_byte, LPT1); 

        }else{
            //y direction low         
            send_byte = send_byte &= ~(1 << 3);
            outb(send_byte, LPT1);   

            //!! THIS IS EXTRA FOR A GANTRY MACHINE INVERTED Z AXIS
            //send_byte = send_byte |= (1 << 5);
            //outb(send_byte, LPT1);                          

        }


        /*****/
        //THIS IS A STANDARD 3D SETUP USING Z AXIS

        //z direction high 
        if (dirpulses.z>1){
            send_byte = send_byte |= (1 << 5);
            outb(send_byte, LPT1);               
        }else{
            send_byte = send_byte &= ~(1 << 5);   
            outb(send_byte, LPT1);                   
        }
        
        
        




    }


    //**************************//
    int x=0;

    //the first element is reserved for direction data 
    //we intentionally skip it starting at index 1 
    for(x=1;x<pt_pulsetrain->size();x++)
    {

        if(send_it==0)
        {
            cout<< pt_pulsetrain->at(x).x<<" " << pt_pulsetrain->at(x).y<<" " << pt_pulsetrain->at(x).z <<"\n";
        }

        if(send_it==1)
        {
            
            // watch the limit switches - if triggered, switch off motors NOW!
            (*this).read_limits(&limit_switches);
            
            if(limit_switches.x==1 || limit_switches.y==1 || limit_switches.z==1){
                cout << "machine has crashed. Condolences. pulsing aborted. ";
            }
            else
            {
                //X channel 
                if(pt_pulsetrain->at(x).x==1){
                    send_byte = send_byte |= (1 << 0);
                    outb(send_byte, LPT1); 
                }else{
                    send_byte = send_byte &= ~ (1 << 0);
                    outb(send_byte, LPT1);  
                }
                    
                //Y channel
                if(pt_pulsetrain->at(x).y==1){
                    send_byte = send_byte |= (1 << 2);
                    outb(send_byte, LPT1);    

                    //!! THIS IS ALSO RUNNING Z AXIS (INVERTED DIR) FOR A GANTRY 
                    //send_byte = send_byte |= (1 << 4);
                    //outb(send_byte, LPT1);    

                }else{
                    send_byte = send_byte &= ~(1 << 2);
                    outb(send_byte, LPT1);           

                    //!! THIS IS ALSO RUNNING Z AXIS (INVERTED DIR) FOR A GANTRY 
                    //send_byte = send_byte &= ~(1 << 4);
                    //outb(send_byte, LPT1);                    
                }

                  
                //standard Z channel
                if(pt_pulsetrain->at(x).z==1){
                    send_byte = send_byte |= (1 << 4);
                    outb(send_byte, LPT1);   
                }else{
                    send_byte = send_byte &= ~(1 << 4);
                    outb(send_byte, LPT1);                 
                }
            

            }




            usleep(pulse_del); 
        }

    }

    if(send_it==1)
    {
        cout << "finished transmitting pulses.\n";
    }


}



/******************************************/

void cnc_plot::calc_3d_pulses(vector<vec3>* pt_pulsetrain,
                              vec3 fr_pt, 
                              vec3 to_pt,
                              int numdivs)
{

            bool debug = false;

            pointgen PG;

            //set the pulses per linear unit (spatial unit divions) - X,Y,Z unit prescaler 
            //for now use one number for all 3 - we will add the others in later
            // int pp_lux      = 10;
            // int pp_luy      = 10;
            // int pp_luz      = 10;
            int pp_lux      = numdivs;
            int pp_luy      = numdivs;
            int pp_luz      = numdivs;


            //make some storage for the data to work in 
            vector<vec3> x_pts;
            vector<vec3> y_pts;
            vector<vec3> z_pts;
            vector<vec3> samples;

            //make some pointers to those data.
            //(people who say THOSE data are technically correct, but they are pedantic dillholes) 
            vector<vec3>* pt_xpts    = &x_pts;
            vector<vec3>* pt_ypts    = &y_pts;
            vector<vec3>* pt_zpts    = &z_pts;
            vector<vec3>* pt_samples = &samples;


            //set up variables to do vector-y stuff
            vec3 between   = sub(fr_pt, to_pt);
            double mag     = length(between);
            
            //double gran    = 0;  //granularity 
            //double thresh  = 0;  //threshold 
            //cout << mag <<"\n";
            
            int xp=0;int yp=0;int zp=0;

            //calculate the absolute change for each axis  
            double delta_x = fr_pt.x-to_pt.x;
            double delta_y = fr_pt.y-to_pt.y;
            double delta_z = fr_pt.z-to_pt.z;

            //calc the direction of the vector 
            if (to_pt.x>fr_pt.x){
                xp=2;
            }else{
                xp=0; 
            }
            //calc the direction of the vector 
            if (to_pt.y>fr_pt.y){
                yp=2;
            }else{
                yp=0; 
            }
            //calc the direction of the vector 
            if (to_pt.z>fr_pt.z){
                zp=2;
            }else{
                zp=0; 
            }
            //first element of pulse train stores the direction 
            pt_pulsetrain->push_back(newvec3(xp,yp,zp));

            //use the amount of change times the spatial divions to get the pulses 
            //DEBUG - we may want to use the mag of the 3d vector in here                  
            int num_pul_x = pp_lux*abs(delta_x);
            int num_pul_y = pp_luy*abs(delta_y);
            int num_pul_z = pp_luz*abs(delta_z); 

            if (debug)
                cout << "# num pulses " << num_pul_x <<" "<<num_pul_y<<" "<<num_pul_z <<"\n";

            // get the absolute highest number of pulses (on any axis) to calculate 
            int tmp[] = {num_pul_x, num_pul_y, num_pul_z};
            //cout << "before: "<<tmp[0] << " "<< tmp[1] <<" "<< tmp[2] <<"\n";
            std::sort(std::begin(tmp), std::end(tmp)  );
            //cout << "after: "<<tmp[0] << " "<< tmp[1] <<" "<< tmp[2] <<"\n";
            int most = tmp[2];
              

            ////////////////////////////////////              
            if (debug)
            {            
                cout << "# most   " << most << " "<< numdivs << " " <<"\n";  
                cout << "# numpts " << num_pul_x <<" " << num_pul_y <<" " << num_pul_z <<"\n"; 
                cout << "#####\n";
            }

            cnc_plot plot;

            vector<int> calcpt_x;
            vector<int> calcpt_y;
            vector<int> calcpt_z;
                       
            // just calc the pulses using a ratio of length to divs. 
            // I experimented with a true 3D method below but this seems to work fine (?)                       
            plot.gen_pules(&calcpt_x, most, num_pul_x);  
            plot.gen_pules(&calcpt_y, most, num_pul_y);  
            plot.gen_pules(&calcpt_z, most, num_pul_z);  

            int a=0;
            for(a=0;a<most;a++)
            {
                pt_pulsetrain->push_back(newvec3(calcpt_x.at(a), calcpt_y.at(a), calcpt_z.at(a)));
                pt_pulsetrain->push_back(newvec3(0,0,0));
            }
            

} 


/******************************************/
/******************************************/

/*

void cnc_plot::calc_3d_pulses(vector<vec3>* pt_pulsetrain,
                              vec3 fr_pt, 
                              vec3 to_pt,
                              int numdivs)
{

            bool debug = false;

            pointgen PG;

            //set the pulses per linear unit (spatial unit divions) - X,Y,Z unit prescaler 
            //for now use one number for all 3 - we will add the others in later
            // int pp_lux      = 10;
            // int pp_luy      = 10;
            // int pp_luz      = 10;
            int pp_lux      = numdivs;
            int pp_luy      = numdivs;
            int pp_luz      = numdivs;


            //make some storage for the data to work in 
            vector<vec3> x_pts;
            vector<vec3> y_pts;
            vector<vec3> z_pts;
            vector<vec3> samples;

            //make some pointers to those data.
            //(people who say THOSE data are technically correct, but they are pedantic dillholes) 
            vector<vec3>* pt_xpts    = &x_pts;
            vector<vec3>* pt_ypts    = &y_pts;
            vector<vec3>* pt_zpts    = &z_pts;
            vector<vec3>* pt_samples = &samples;


            //set up variables to do vector-y stuff
            vec3 between   = sub(fr_pt, to_pt);
            double mag     = length(between);
            double gran    = 0;  //granularity 
            double thresh  = 0;  //threshold 
            //cout << mag <<"\n";
            
            int xp=0;int yp=0;int zp=0;

            //calculate the absolute change for each axis  
            double delta_x = fr_pt.x-to_pt.x;
            double delta_y = fr_pt.y-to_pt.y;
            double delta_z = fr_pt.z-to_pt.z;

            //calc the direction of the vector 
            if (to_pt.x>fr_pt.x){
                xp=2;
            }else{
                xp=0; 
            }
            //calc the direction of the vector 
            if (to_pt.y>fr_pt.y){
                yp=2;
            }else{
                yp=0; 
            }
            //calc the direction of the vector 
            if (to_pt.z>fr_pt.z){
                zp=2;
            }else{
                zp=0; 
            }
            //first element of pulse train stores the direction 
            pt_pulsetrain->push_back(newvec3(xp,yp,zp));

            
            //not totally sure this is right 
            int num_pul_x = (mag*pp_lux)*abs(delta_x);
            int num_pul_y = (mag*pp_luy)*abs(delta_y);
            int num_pul_z = (mag*pp_luz)*abs(delta_z);            

            if (debug)
                cout << "# num pulses " << num_pul_x <<" "<<num_pul_y<<" "<<num_pul_z <<"\n";

            // get the absolute highest number of pulses (on any axis) to calculate 
            int tmp[] = {num_pul_x, num_pul_y, num_pul_z};
            //cout << "before: "<<tmp[0] << " "<< tmp[1] <<" "<< tmp[2] <<"\n";
            std::sort(std::begin(tmp), std::end(tmp)  );
            //cout << "after: "<<tmp[0] << " "<< tmp[1] <<" "<< tmp[2] <<"\n";
            int most = tmp[2];
                                 
            // get the smallest division to use for sampling (granularity) 
            if (mag!=0 && most !=0)
            {
                gran = (mag/most);
            }else{
                gran = 0;
            }


            // calculate a series of points along vector for each axis 
            if(num_pul_x!=0)
            {
                PG.locate_pt_along3d(pt_xpts, to_pt, fr_pt, num_pul_x);
            }
            if(num_pul_y!=0) 
            {
                PG.locate_pt_along3d(pt_ypts, to_pt, fr_pt, num_pul_y);
            }
            if(num_pul_z!=0)
            {            
                PG.locate_pt_along3d(pt_zpts, to_pt, fr_pt, num_pul_z);
            }
      
            //# build a sampleset of all points along the vector - then iterate and match each axis to those points
            //# converting the spatial points into a pulse train 
            if (most!=0 && gran!=0)
            {
                //take the smallest possible sample.. and cut it in half for good measure
                thresh = gran/2;
                
                PG.locate_pt_along3d(pt_samples, to_pt, fr_pt, most);
                
                //DEBUG this "works" but does not distribute the pulses properly due to the 3 loops in a loop
                //the pulses should be evenly spaced across the entirety, and this makes clumps of pulses 
                int a=0;int i=0;
                for(a=0;a<samples.size();a++)
                {
 
                    xp=0;
                    yp=0;
                    zp=0;

                    vec3 spt = samples[a]; 

                    //X 
                    for (i=0;i<x_pts.size();i++)
                    {
                        vec3 xpt = x_pts[i];
                        vec3 ss = sub(xpt,spt);
                        if( length(ss)<thresh)
                        {
                            xp=1;
                            //break;
                        }
                    }

                    //Y
                    for (i=0;i<y_pts.size();i++)
                    {
                        vec3 ypt = y_pts[i];
                        vec3 ss = sub(ypt,spt);
                        if( length(ss)<thresh)
                        {
                            yp=1;
                            //break;
                        }
                    }


                    //Z
                    for (i=0;i<z_pts.size();i++)
                    {
                        vec3 zpt = z_pts[i];
                        vec3 ss = sub(zpt,spt);
                        if( length(ss)<thresh)
                        {
                            zp=1;
                            //break;
                        }
                    }

                    pt_pulsetrain->push_back(newvec3(xp,yp,zp));
                    pt_pulsetrain->push_back(newvec3(0,0,0));
                }
             

           }


}

*/

