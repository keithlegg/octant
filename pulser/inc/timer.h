#ifndef TIMER_H    
#define TIMER_H


#include <sys/time.h>

void timer_test(void);




// time_t         tv_sec      seconds
// suseconds_t    tv_usec     microseconds
// The <sys/time.h> header defines the itimerval structure that includes at least the following members:
// struct timeval it_interval timer interval
// struct timeval it_value    current value


/*
   FROM HERE 

   http://www.songho.ca/misc/timer/timer.html
 
*/


class timer
{
    public:
        
        timer()
        {


            startCount.tv_sec = startCount.tv_usec = 0;
            endCount.tv_sec = endCount.tv_usec = 0;

            startTimeInMicroSec = 0;
            endTimeInMicroSec   = 0;
            
            start_simtime = 0;
            end_simtime   = 0;    

            sim_time_us = 0;

            tm_running = false;
            tm_stopped = 0;//yes this is a contradiction to running = false, roill with it 

        }

        ~timer(){};

        void   start();                             // start timer
        void   stop();                              // stop the timer

        double getElapsedTime(void);                    // get elapsed time in second
        double getElapsedTimeInSec(void);               // get elapsed time in second (same as getElapsedTime)
        double getElapsedTimeInMilliSec(void);          // get elapsed time in milli-second
        double getElapsedTimeInMicroSec(void);          // get elapsed time in micro-second

        double get_elapsed_simtime_us(void);            // get elapsed (sim)time in micro-second
        double get_elapsed_simtime_ms(void);
        double get_elapsed_simtime_sec(void);
        double get_elapsed_simtime(void);
        double get_elapsed_simtime_fakeunits(void);     //user adjustable time increment 

        double cvt_num_to_ratio; //convenience to convert num us to a decimal ratio 

        //keith added these
        void reset_sim(void);

        bool   tm_running;   // public property 
        double sim_time_us; //us == microsecond 
        //double time_offset = 0;

    protected:


    private:
        double startTimeInMicroSec;   // starting time in micro-second
        double endTimeInMicroSec;     // ending time in micro-second

        double start_simtime;         // starting time in micro-second
        double end_simtime;           // ending time in micro-second

        int    tm_stopped;               // stop flag 

        timeval startCount;                         
        timeval endCount;                            



};






#endif
