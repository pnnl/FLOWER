///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP


// System Includes
#include <map>
#include <string>
// External Includes
#include "Diagnostic.hpp"
// Internal Includes
// Application Includes


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Exception"


// Namespaces
using namespace std;


typedef enum
{
  // DEVELOPER NOTE: In general, these errors are used when the data in a
  //                 packet or flow CANNOT be kept but should be reported
  //  
  // DEVELOPER NOTE: 
  //  Use number scheme (i.e. flowCache has an uppercase C which is 12 in base 26)
  //  flowCache        = 12 followed by a number
  //  packetBuilder    = 11 followed by a number
  //  summmaryExporter = 14 followed by a number
  //  Flow             = 15 followed by a number
  //  packetParser     = 25 followed by a number
  //  packetRinger     = 27 followed by a number
  e_error_fc_cant_merge_frag_flow  = 121,
  e_error_fc_cant_merge_norm_flow  = 122,
  e_error_fc_cant_add_frag_flow    = 123,
  e_error_fc_cant_add_norm_flow    = 124,
  e_error_fc_frag_cache_full       = 125,
  e_error_fc_norm_cache_full       = 126,
  e_error_pp_short_eth_packet      = 251,
  e_error_pp_short_ip_packet       = 252,
  e_error_pp_short_v4_packet       = 253,
  e_error_pp_short_v6_packet       = 254,
  e_error_pp_object_pool_empty     = 255,
  e_error_pp_short_vlan_packet     = 256
} error_enum;


#define EERROR(a, b, c, d) updateErrorCount(a); ERROR_MSG(b, c, d)


void updateErrorCount(error_enum p_error) noexcept(true);
string getErrorRecord(void) noexcept(true);


//
//  DEVELOPER NOTE:
//    Renamed Unknown to UnknownException due to a name collision
//    in the Winioctl.h _MEDIA_TYPE enum
//
static const string AlphaNumeric("AlphaNumeric");
static const string BadOption("BadOption");
static const string BadData("BadData");
static const string FakeData("FakeData");
static const string FileIO("FileIO");
static const string FloatingPoint("FloatingPoint");
static const string IllegalValue("IllegalValue");
static const string NotFound("NotFound");
static const string PermissionDenied("PermissionDenied");
static const string RangeError("RangeError");
static const string SegFault("SegFault");
static const string Shutdown("ShutDown");
static const string Signal("Signal");
static const string TSNH("TSNH");
static const string Unsupported("Unsupported");
static const string UnknownException("Unknown Exception");


class Exception
{

public:

  // Getters/Setters

  static map<string, string> * getErrorCodes(void) noexcept(true);


private:

  // Constructors

  Exception(void);
  Exception(Exception const & p_exception);

  // Operators

  Exception & operator=(Exception const & p_exception);


  // Destructor
  
  ~Exception(void) noexcept(true)
  {
    return;
  }

};

#endif // EXCEPTION_HPP
