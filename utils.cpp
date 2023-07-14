//This file contains the implementations of utility functions used across the code
//
//


#include <string>
#include <iostream>
#include <sstream>
#include <limits>
#include "utilities.h"

//Helper function for distance calculation
int utl::manhattan_dist(int x1, int y1, int x2, int y2)
{
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

//Helper function to strip whitespaces
void utl::strip_whitespaces(std::string& to_strip)
{
    std::stringstream string_holder{};
    std::string temp{}, new_string{};
    std::size_t leading_nonspace{ to_strip.find_first_not_of(" ") };
    std::size_t ending_nonspace{ to_strip.find_last_not_of(" ") };
    to_strip = to_strip.substr(leading_nonspace, ending_nonspace - leading_nonspace + 1);

    string_holder << to_strip;
    while (!string_holder.eof()) {
        string_holder >> temp;
        new_string += temp;
    }
    to_strip = new_string;
}


//Utility functions for display
void utl::enter_continue()
{
    std::cout << "Press ENTER to start... " << std::flush;
    std::cin.ignore(std::numeric_limits <std::streamsize> ::max(), '\n');
}

void utl::clear_screen()
{
    //Check if on Windows
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN64) ||defined(WIN64)|| defined(_WIN64) && !defined(__CYGWIN__)
    std::system("CLS");
    //Assume UNIX otherwise
#else
    std::system("clear");
#endif
}
