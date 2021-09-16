/**
 * @file    cerror.h
 * @brief   print pretty perrors
 * @details this header provides a function
 *          and a macro which wrap the perror()
 *          c function to add useful information,
 *          i.e. __FILE__ and __LINE__
 */

#pragma once
#ifndef CERROR_H
#define CERROR_H

#include <string>

/**
 * @brief print error to stdout
 */
inline void cerror(int line, const char* file, const char* mssg)
{
    std::string s {"Error, file \e[1m"+ std::string(file) + "\e[0m, line \e[1m~" + std::to_string(line-1) + "\e[0m, \e[1m" + std::string(mssg) + "\e[0m"};
    perror( s.c_str() );
}

/**
 * @brief print error to stdout and exit program
 */
inline void cerror_exit(int line, const char* file, const char* mssg)
{
    std::string s {"Error, file \e[1m"+ std::string(file) + "\e[0m, line \e[1m~" + std::to_string(line-1) + "\e[0m, \e[1m" + std::string(mssg) + "\e[0m"};
    perror( s.c_str() );
    exit(EXIT_FAILURE);
}

#endif