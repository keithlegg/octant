// #include <stdlib.h>
// #include <stdio.h>

#include <chrono>
#include <iostream>


#include "timer.h"



double trav_speed ; //linear unit per sec 


void timer::reset_sim(void)
{
    sim_time_us=0;
}

void timer::start()
{
    stopped = 0; // reset stop flag
    running = true;
    gettimeofday(&startCount, NULL);

}




void timer::stop()
{
    stopped = 1; // set timer stopped flag
    running = false;

    gettimeofday(&endCount, NULL);

}




double timer::getElapsedTimeInMicroSec()
{

    if(!stopped)
        gettimeofday(&endCount, NULL);

    startTimeInMicroSec = (startCount.tv_sec * 1000000.0) + startCount.tv_usec;
    endTimeInMicroSec = (endCount.tv_sec * 1000000.0) + endCount.tv_usec;

    //keith added this for a resetable "time" var 
    sim_time_us = sim_time_us+10000;
 

    return endTimeInMicroSec - startTimeInMicroSec;
}

 
double timer::get_elapsed_simtime_ms()
{
    this->getElapsedTimeInMicroSec();
    return this->sim_time_us * 0.001;
}
double timer::get_elapsed_simtime_sec()
{
    this->getElapsedTimeInMicroSec();
    return this->sim_time_us * 0.000001;
}



double timer::getElapsedTimeInMilliSec()
{
    return this->getElapsedTimeInMicroSec() * 0.001;
}
double timer::getElapsedTimeInSec()
{
    return this->getElapsedTimeInMicroSec() * 0.000001;
}



double timer::get_elapsed_simtime()
{
    return this->get_elapsed_simtime_sec();
}

double timer::getElapsedTime()
{
    return this->getElapsedTimeInSec();
}







