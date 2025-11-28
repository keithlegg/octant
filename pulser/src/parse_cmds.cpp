
#include "parse_cmds.h"
#include "gl_setup.h"


#include <sstream>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <unistd.h>     



/***************************************/



void parse_cmds(std::string *buffer, unsigned char *pt_key )
{

    std::string parsed;
    std::string word;

    std::istringstream iss(*buffer);
    if (getline(iss, parsed, ' '))
    {
         // do some processing.
    };

    // std::stringstream ss(*buffer);
    // int number_int;
    // double number_double;
    // // Extracting words separated by spaces
    // ss >> word; // word will be "apple"
    // ss >> word; // word will be "banana"

    buffer->push_back(*pt_key);

    //buffer->push_back(*"HELLO KITTY\n");
    
    std::cout << *buffer << "\n";

};




/*
    std::stringstream ss(buffer);
    std::string word;
    int number_int;
    double number_double;

    // Extracting words separated by spaces
    ss >> word; // word will be "apple"
    ss >> word; // word will be "banana"

    // Extracting numbers
    ss >> number_int;    // number_int will be 123
    ss >> number_double; // number_double will be 4.5

    std::cout << "Word 1: " << word << std::endl;
    std::cout << "Integer: " << number_int << std::endl;
    std::cout << "Double: " << number_double << std::endl;
*/


/*
std::string input_txt(void){

    std::string sentence;
    std::cout << "Enter a sentence please: "; std::cout.flush();

    std::getline(std::cin,sentence);
    std::istringstream iss(sentence);

    std::vector<std::string> words;
    std::string word;
    while(iss >> word) {
        words.push_back(word);
    } 

    for(std::vector<std::string>::const_iterator it = words.begin();
        it != words.end();
        ++it) {
        std::cout << *it << ' ';
    }
    std::cout << std::endl; return 0;
    
    return sentence;
}
*/
