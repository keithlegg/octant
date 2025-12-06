/*****************************************************************************/
/* 
    parse_cmds.cpp
    


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

#include <sstream>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <unistd.h>  

#include "parse_cmds.h"
#include "gl_setup.h"

#include "obj_model.h"
#include "octant.h"


int cursor = 0;


/***************************************/
/***************************************/


void parse_cmd_text(std::string *buffer)
{
    //std::vector<std::string>  tokenized = tokenizer(buffer, *" ");
    //if(tokenized.size()>0)

    // Vector of string to save tokens
    vector <std::string> tokens;
    
    // stringstream class check1
    std::stringstream check1(*buffer);
    
    std::string intermediate;
    
    // Tokenizing w.r.t. space ' '
    while(getline(check1, intermediate, ' '))
    {
        tokens.push_back(intermediate);
    }
    
    std::string first;
    std::string second;

    // Printing the token vector
    for(int i = 0; i < tokens.size(); i++)
    {
        if(i==0){ 
            first = tokens[i];
        }

        if(i==1){ 
            second = tokens[i];
        }

    }
    
    std::cout << "first:" << first << " second:" << second << '\n';

    //std::cout << " hello parser \n" << *buffer <<"\n";

}



/***************************************/
void parse_cmds(std::string *buffer, unsigned char *pt_key )
{
    int i = static_cast<int>(*pt_key);
    
    std::cout << " key val " << i << std::endl;
    


    //-----
    //backspace key
    if(i==8)
    {
        if (buffer->size()>0 && cursor>0)
        {
            cursor--;
            buffer->erase(cursor, buffer->size());
        }
    }

    //-----
    //enter key
    if(i==13)
    {
        //todo - keep a log of commands as txt file ??
        
        //std::cout << "command to parse \n" << *buffer << "\n";

        parse_cmd_text(buffer);

        buffer->clear();
        //std::cout << "ENTER PRESSED\n";        
    }    

    //-----
    //esc key
    if(i==27)
    {
        key_cb(i); 

    }  

    //-----
    //ignore backspace here
    if(i!=8)
    {   
        cursor = buffer->size(); 
        buffer->push_back(*pt_key);
    }

};




