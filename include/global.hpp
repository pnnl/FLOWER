///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
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
#include <cstdint>
// External Includes
// Internal Includes
// Application Includes
#include "Exception.hpp"


// DEFINITIONS

#define APP_NAME "flower"
#define APP_DESC "network packet FLOW analizER (" APP_NAME ")"


#define ERROR_MSG(  p_ecode, p_item, p_context) ::message(p_ecode, p_item, p_context, __CLASS__, __func__, __FILE__, __LINE__, EXIT_SUCCESS)
#define FATAL(  p_ecode, p_item, p_context) ::message(p_ecode, p_item, p_context, __CLASS__, __func__, __FILE__, __LINE__, EXIT_FAILURE)
#define CAUTION(p_ecode, p_item, p_context) ::message(p_ecode, p_item, p_context, __CLASS__, __func__, __FILE__, __LINE__, EXIT_SUCCESS)
#define WARN(   p_ecode, p_item, p_context) ::message(p_ecode, p_item, p_context, __CLASS__, __func__, __FILE__, __LINE__, EXIT_SUCCESS)

//  HACK TODO: This eliminates all DEBUG messages
#define DEBUG(p_a, ...)


// Namespaces
using namespace std;

// Typedefs
#ifdef _MSC_VER
typedef          __int64   int64_t;
typedef unsigned __int64 u_int64_t;
typedef unsigned __int32 u_int32_t;
typedef unsigned __int16 u_int16_t;
typedef unsigned __int8   u_int8_t;
#endif


constexpr bool g_REAL{ false };
constexpr bool g_FAKE{ true };
//extern bool const     g_REAL;
//extern bool const     g_FAKE;
extern bool const     g_OUTPUT2LOG;
extern bool const     g_OUTPUT2CONSOLE;
extern u_int8_t const g_NORMALEXIT;
extern u_int8_t const g_ABNORMALEXIT;

string const & getDataGuideVersion(void) noexcept(true);
string const & getFileOrWireStr(void) noexcept(true);
int unsigned   getCacheForceout(void) noexcept(true);
int unsigned   getCacheTimeout(void) noexcept(true);
int unsigned   getFileOrWireFlag(void) noexcept(true);
int unsigned   getFileFlag(void) noexcept(true);
int unsigned   getWireFlag(void) noexcept(true);
void           setFileOrWireFlag(bool const p_use_device) noexcept(true);
void           setSiteName(string const & p_site_name) noexcept(true);
void           setCacheForceout(int unsigned const p_seconds) noexcept(true);
void           setCacheTimeout(int unsigned const p_seconds) noexcept(true);
string const & getSiteName(void) noexcept(true);
bool const     getOutputLocation(void) noexcept(true);
void           setOutputLocation(bool const p_destination, char * argv0 = NULL) noexcept(true);


void   message(string const & p_ecode,
               string const & p_item,
               string const & p_context,
               string const & p_class,
               string const & p_func,
               string const & p_file,
               int    const   p_line,
               int    const   p_status) noexcept(true);

#endif // GLOBAL_HPP
