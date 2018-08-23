///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
//
// CODING CONVENTIONS:
//    * Class names are CamelCase with first word upper case
//    * Functions are camelCase with first word lower case
//    * Function parameters are lower case with _ and have p_ prefix
//    * Member variables always use 'this' pointer
///////////////////////////////////////////////////////////////////////////////


#ifndef OSUTIL_HPP
#define OSUTIL_HPP


// System Includes
#include <string>
#ifndef _MSC_VER
#include <syslog.h>
#endif
// External Includes
// Internal Includes
// Application Includes


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "osUtil"


// Namespaces
using namespace std;


// Getters/Setters

string const & getPathSeparator(void) noexcept(true);
bool   const   isPriviledgedUser(void) noexcept(true);
void           output(string const & p_message) noexcept(true);
void           outputSeparator(void) noexcept(true);
bool   const   isLinux(void) noexcept(true);
bool   const   checkNetworkInterface(string const & p_device) noexcept(true);

#endif // OSUTIL_HPP
