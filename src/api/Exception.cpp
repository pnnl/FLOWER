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
// External Includes
// Internal Includes
// Application Includes
#include "num2string.hpp"
#include "Exception.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Exception"


// Namespaces


static map<error_enum, int unsigned> error_map;


void updateErrorCount(error_enum p_error) noexcept(true)
{
  map<error_enum, int unsigned>::iterator itor = error_map.find(p_error);
  if (itor != error_map.end())
  {
    ++(*itor).second;
    return;
  }
  error_map.insert(make_pair(p_error, 1));
  return;
}


string getErrorRecord(void) noexcept(true)
{
  map<error_enum, int unsigned>::iterator itor = error_map.begin();
  map<error_enum, int unsigned>::iterator etor = error_map.end();
  string result = "";
  for (itor = itor; itor != etor; ++itor)
  {
    result += "," + uitoa10((*itor).first, 0) + ":" + uitoa10((*itor).second, 0);
  }
  return(result);
}


map<string, string> * Exception::getErrorCodes(void) noexcept(true)
{
  DEBUG(TRACE, ENTER);
  static map<string, string> * error_codes = new map<string, string>();
  if (error_codes->empty())
  {
    error_codes->insert(make_pair(AlphaNumeric, "String must be alpha numeric (a-zA-Z0-9_)"));
    error_codes->insert(make_pair(BadOption, "Problem with command line or configuration file option"));
    error_codes->insert(make_pair(FakeData, "Fake Data"));
    error_codes->insert(make_pair(FileIO, "File IO error"));
    error_codes->insert(make_pair(FloatingPoint, "Floating point error"));
    error_codes->insert(make_pair(IllegalValue, "Illegal value"));
    error_codes->insert(make_pair(NotFound, "Not found"));
    error_codes->insert(make_pair(PermissionDenied, "Permission denied"));
    error_codes->insert(make_pair(RangeError, "Out of range error"));
    error_codes->insert(make_pair(SegFault, "Segmentation fault"));
    error_codes->insert(make_pair(Shutdown, "Shutting down " APP_NAME " due to"));
    error_codes->insert(make_pair(Signal, "Signal encountered"));
    error_codes->insert(make_pair(TSNH, "This should not happen"));
    error_codes->insert(make_pair(Unsupported, "Unsupported"));
    error_codes->insert(make_pair(UnknownException, "Unknown error code"));
  }
  DEBUG(TRACE, LEAVE);
  return(error_codes);
}
