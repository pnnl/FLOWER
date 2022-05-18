///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


// System Includes
#include <iomanip>
#include <iostream>
// External Includes
// Internal Includes
// Application Includes
#include "CONSTANTS.hpp"
#include "global.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "global"


// Namespaces
using namespace std;


//bool const     g_REAL                = false;  // NOTE: So flow.isFake() returns false
//bool const     g_FAKE                = true;   // NOTE: So flow.isFake() returns true

bool           g_FLR06               = true;   // NOTE: So isFlr06() returns true
bool const     g_OUTPUT2LOG          = false;  // NOTE: So getConsoleOrLog() returns true
bool const     g_OUTPUT2CONSOLE      = true;   // NOTE: So getConsoleOrLog() returns false

u_int8_t const g_NORMALEXIT          = 0;
u_int8_t const g_ABNORMALEXIT        = 1;

static string  g_DATA_GUIDE_VERSION  = "flr" DATA_GUIDE_VERSION;

static string       site_name        = "site-name";

static int unsigned wire_or_file_num =  0 ;    // NOTE:  0  = Wire,  1  = File;
static string       wire_or_file_str = "0";    // NOTE: "0" = Wire, "1" = File;
static int unsigned cache_forceout   = 900;
static int unsigned cache_timeout    = 120;
static bool         console_or_log   = true;   // NOTE: true = Console, false = log


// Getters/Setters

bool const getFlr06(void) noexcept(true)
{
  return(::g_FLR06);
}


string const & getDataGuideVersion(void) noexcept(true)
{
  return(::g_DATA_GUIDE_VERSION);
}


void setDataGuideVersion(string const & p_version) noexcept(true)
{
  ::g_DATA_GUIDE_VERSION = "flr" + p_version;
  if (p_version.compare("06") != 0)
  {
    ::g_FLR06 = false;
  }
}


string const & getFileOrWireStr(void) noexcept(true)
{
  return(wire_or_file_str);
}


int unsigned getFileOrWireFlag(void) noexcept(true)
{
  return(wire_or_file_num);
}


void setFileOrWireFlag(bool const p_use_device) noexcept(true)
{
  if (p_use_device)
  {
    wire_or_file_num =  0;
    wire_or_file_str = "0";
  }
  else
  {
    wire_or_file_num =  1;
    wire_or_file_str = "1";
  }
  return;
}


string const & getSiteName(void) noexcept(true)
{
  return(site_name);
}


void setSiteName(string const & p_site_name) noexcept(true)
{
  site_name = p_site_name;
  return;
}


int unsigned getCacheForceout(void) noexcept(true)
{
  return(cache_forceout);
}


void setCacheForceout(int unsigned const p_seconds) noexcept(true)
{
  cache_forceout = p_seconds;
  return;
}


int unsigned getCacheTimeout(void) noexcept(true)
{
  return(cache_timeout);
}


void setCacheTimeout(int unsigned const p_seconds) noexcept(true)
{
  cache_timeout = p_seconds;
  return;
}


bool const getOutputLocation(void) noexcept(true)
{
  return(console_or_log);
}


void setOutputLocation(bool const p_destination, char * argv0) noexcept(true)
{
  console_or_log = p_destination;
  if (NULL != argv0)
  {
    // HACK
    //SYSLOG(argv0);
  }
  return;
}


void message(string const & p_ecode,
             string const & p_item,
             string const & p_context,
             string const & p_class,
             string const & p_func,
             string const & p_file,
             int    const   p_line,
             int    const   p_status) noexcept(true)
{
  u_int16_t pad = 14;

  cerr << "\nFATAL ERROR_MSG:\n";
  cerr << setw(pad) << "File:  "     << p_file        << "(line:" << p_line << ")\n";
  cerr << setw(pad) << "Class:  "    << p_class       << "\n";
  cerr << setw(pad) << "Function:  " << p_func        << "\n";
  cerr << setw(pad) << "Message:  "  << "Error Code=" << p_ecode  << "\n";
  cerr << setw(pad) << " "           << p_item        << "\n";
  cerr << setw(pad) << " "           << p_context     << "\n"     << endl;

  if (EXIT_FAILURE == p_status)
  {
    exit(EXIT_FAILURE);
  }

  return;
}
