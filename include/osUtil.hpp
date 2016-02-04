///////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2008.  PNNL.  All Rights Reserved.
//
// THIS FILE INITIALLY CREATED WITH:
//     TEMPLATE NAME:  lang_cpp_hdr.template
//     COMMAND NAME:   gensrc
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

string const & getPathSeparator(void) throw();
bool   const   isPriviledgedUser(void) throw();
void           output(string const & p_message) throw();
void           outputSeparator(void) throw();
bool   const   isLinux(void) throw();
bool   const   checkNetworkInterface(string const & p_device) throw();

#endif // OSUTIL_HPP
