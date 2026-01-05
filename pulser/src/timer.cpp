/*****************************************************************************/
/* 
    timer.cpp

    

    modified - 2026 Keith Legg - keithlegg23@gmail.com


    ORIGINAL AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
    Copyright (C) 2005 Song Ho Ahn


    MIT License

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



#include <chrono>
#include <iostream>

#include "globals.h"
#include "timer.h"

double trav_speed ; //linear unit per sec 

extern cncglobals cg;


/***************************************/
//step/reset gets called once every "tick" between 0-1 
void timer::reset_sim(void)
{
    gettimeofday(&startCount, NULL);
    start_simtime = (startCount.tv_sec * 1000000.0) + startCount.tv_usec;

    //std::cout << "timer -> time was reset " << sim_time_us <<"\n";
    sim_time_us = 0;
}

/***************************************/
void timer::start(void)
{
    tm_stopped = 0; // reset stop flag
    tm_running = true;
    gettimeofday(&startCount, NULL);

}


/***************************************/
//void timer::pause()

/***************************************/
void timer::stop(void)
{
    tm_stopped = 1; // set timer tm_stopped flag
    tm_running = false;

    gettimeofday(&endCount, NULL);

}


/***************************************/

/*

DUBIOUS EXAMPLE OF A TIMER USING CHRONO

int main() {
    auto start = std::chrono::steady_clock::now();
    
    //do soemthing here to time 

    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Elapsed time: " << elapsed.count() << " ms\n";
}
*/

/***************************************/ 
double timer::getElapsedTimeInMicroSec(void)
{
    if(!tm_stopped)
        gettimeofday(&endCount, NULL);
    startTimeInMicroSec = (startCount.tv_sec * 1000000.0) + startCount.tv_usec;
    endTimeInMicroSec   = (endCount.tv_sec   * 1000000.0) + endCount.tv_usec;
    return endTimeInMicroSec - startTimeInMicroSec;;
}


/***************************************/ 
/*
  note this uses the same variables (startCount,endCount) as getElapsedTimeInMicroSec()
  just dont run them concurrently and you should be good  
*/

double timer::get_elapsed_simtime_us(void)
{
    if(!tm_stopped)
        gettimeofday(&endCount, NULL);

    start_simtime = (startCount.tv_sec * 1000000.0) + startCount.tv_usec;
    end_simtime   = (endCount.tv_sec   * 1000000.0) + endCount.tv_usec;
 
    double elapsed_us = end_simtime - start_simtime;

    sim_time_us = elapsed_us;

    return elapsed_us;
}



/***************************************/
double timer::getElapsedTimeInMilliSec(void)
{
    return this->getElapsedTimeInMicroSec() * 0.001;
}

/***************************************/
double timer::getElapsedTimeInSec(void)
{
    return this->getElapsedTimeInMicroSec() * 0.000001;
}
/***************************************/
double timer::getElapsedTime(void)
{
    return this->getElapsedTimeInSec();
}

/***************************************/ 
/***************************************/ 

/***************************************/ 
double timer::get_elapsed_simtime_ms(void)
{
    this->get_elapsed_simtime_us();
    return this->sim_time_us * 0.001;
}

/***************************************/
double timer::get_elapsed_simtime_sec(void)
{
    this->get_elapsed_simtime_us();
    return this->sim_time_us * 0.000001;
}

/***************************************/
double timer::get_elapsed_simtime_fakeunits(void)
{
    
    // cncglobals cg;

    this->get_elapsed_simtime_us();
    return this->sim_time_us * 0.00001;
}

/***************************************/
double timer::get_elapsed_simtime(void)
{
    
    //return this->get_elapsed_simtime_sec();
    
    return this->get_elapsed_simtime_fakeunits();

}









