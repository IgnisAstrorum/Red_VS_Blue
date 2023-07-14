//This file contains the declarations of a few utility functions used across the code
//
//

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iostream>
#include <sstream>

namespace utl {
	int manhattan_dist(int x1, int y1, int x2, int y2);

	//Strip whitespaces from a string
	void strip_whitespaces(std::string& to_strip);

	//Utility functions for display
	void enter_continue();
	void clear_screen();
}

#endif