///////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2008.  PNNL.  All Rights Reserved. 
//
// THIS FILE INITIALLY CREATED WITH:  
//     TEMPLATE NAME:  lang_cpp_class.template 
//     COMMAND NAME:   gensrc 
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifndef DATE2STRING_HPP
#define DATE2STRING_HPP


// System Includes
#include <string>
#include <time.h>
// External Includes
// Internal Includes
// Application Includes


// Namespaces
using namespace std;


string & getDateStringAll(time_t const p_time,                    string & p_result) throw();
string & getDateString(   time_t const p_time, string & p_format, string & p_result) throw();


#endif // DATE2STRING_HPP
