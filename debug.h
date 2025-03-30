#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include <iostream>
#include <direct.h>  // For _getcwd on Windows
#include <limits>

// Function to print the current working directory.
inline void printWorkingDirectory() {
    char buffer[_MAX_PATH];
    if (_getcwd(buffer, _MAX_PATH)) {
        std::cout << "Current working directory: " << buffer << std::endl;
    } else {
        std::cerr << "Error: Unable to get current working directory." << std::endl;
    }
}

#endif // DEBUG_H_INCLUDED
