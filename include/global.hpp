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


#ifndef GLOBAL_HPP
#define GLOBAL_HPP


// System Includes
#include <string>
// External Includes
// Internal Includes
// Application Includes
#include "Exception.hpp"


// DEFINITIONS

#define APP_NAME "flower"
#define APP_DESC "network packet FLOW analizER (" APP_NAME ")"


//#define FATAL(p_ecode, p_item, p_context) message(p_ecode, p_item, p_context, __CLASS__, __func__, __FILE__, __LINE__, EXIT_FAILURE)
//#define WARN(p_ecode,  p_item, p_context) message(p_ecode, p_item, p_context, __CLASS__, __func__, __FILE__, __LINE__, EXIT_SUCCESS)


// Namespaces
using namespace std;

// Typedefs
#ifdef _MSC_VER
typedef          __int64   int64_t;
typedef unsigned __int64 u_int64_t;
typedef unsigned __int32 u_int32_t;
typedef unsigned __int16 u_int16_t;
typedef unsigned __int8  u_int8_t;
#endif


extern bool const     g_REAL;
extern bool const     g_FAKE;
extern bool const     g_OUTPUT2LOG;
extern bool const     g_OUTPUT2CONSOLE;
extern u_int8_t const g_NORMALEXIT;
extern u_int8_t const g_ABNORMALEXIT;

string const & getDataGuideVersion(void) throw();
string const & getFileOrWireStr(void) throw();
int unsigned   getCacheForceout(void) throw();
int unsigned   getCacheTimeout(void) throw();
int unsigned   getFileOrWireFlag(void) throw();
int unsigned   getFileFlag(void) throw();
int unsigned   getWireFlag(void) throw();
void           setFileOrWireFlag(bool const p_use_device) throw();
void           setSiteName(string const & p_site_name) throw();
void           setCacheForceout(int unsigned const p_seconds) throw();
void           setCacheTimeout(int unsigned const p_seconds) throw();
string const & getSiteName(void) throw();
bool const     getOutputLocation(void) throw();
void           setOutputLocation(bool const p_destination, char * argv0 = NULL) throw();


void   message(string const & p_ecode,
               string const & p_item,
               string const & p_context,
               string const & p_class,
               string const & p_func,
               string const & p_file,
               int    const   p_line,
               int    const   p_status) throw();

#endif // GLOBAL_HPP
