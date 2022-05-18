///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifndef NUM2STRING_HPP
#define NUM2STRING_HPP


// System Includes
#include <string>
// External Includes
// Internal Includes
// Application Includes
#include "global.hpp"


// Namespaces
using namespace std;


// Global Variables

static const double g_pow10[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};


string itoa10(int64_t    const p_value, u_int64_t const p_pad_length = 0) noexcept(true);
string uitoa10(u_int64_t const p_value, u_int64_t const p_pad_length = 0) noexcept(true);
string uitoa16(u_int32_t const p_value, u_int32_t const p_pad_length)     noexcept(true);
string dtoa(double       const p_value, u_int64_t const p_prec       = 2) noexcept(true);
string ftoa(float        const p_value, u_int64_t const p_prec       = 2) noexcept(true);


#endif // NUM2STRING_HPP
