/*************************************************************/
/*
    cnc_io.cpp 
    
    general IO utilities. Deal with all electrical signals going in or out.
    The specific interfaces (Mesa, Parallel) get thier own files. 





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


#include "globals.h"
#include "plot.h"
#include "io.h"








// this file intentionally left blank. Error 404 
// this was the home of parallel port IO but we need to think ahead to when there are more than 1 interface



