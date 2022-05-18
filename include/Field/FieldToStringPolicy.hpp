///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifndef FIELDTOSTRINGPOLICY_HPP
#define FIELDTOSTRINGPOLICY_HPP


// System Includes
#include <algorithm>
#include <iostream>
#include <string>
#include <sys/types.h> 
#include <vector>
// External Includes
#include <Diagnostic.hpp>
// Internal Includes
// Application Includes
#include "date2string.hpp"
#include "global.hpp"
#include "num2string.hpp"
#include "timeUtil.hpp"
#include "uchar2string.hpp"
#include "FieldCommon.hpp"
#include "Data.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "FieldToStringPolicy"


// Namespaces
using namespace std;


#undef  UNUSED
#define UNUSED(a)


// ================================================
//
// ToString Policies
//
// ================================================


template <typename T>
struct ToStringNone
{

  static void toString(T const & p_data, string & p_tmp) noexcept(true)
  {
    UNUSED(p_data);
    p_tmp += ",NONE";
    return;
  }

};


template <typename T>
struct ToStringHexNonzero;

template <typename T>
struct ToStringHexNonzero < Data<T, 1> >
{

  static void toString(Data<T, 1> const & p_data, string & p_tmp, bool const valid) noexcept(true)
  {
    if (! valid || 0 == p_data.at(0))
    {
      p_tmp += ',';
      return;
    }
    p_tmp += ',' + uitoa16(p_data.at(0), 8);
    return;
  }

};


template <typename T>
struct ToStringNonzero;

template <typename T>
struct ToStringNonzero < Data<T, 1> >
{

  static void toString(Data<T, 1> const & p_data, string & p_tmp, bool const valid) noexcept(true)
  {
    if (! valid || 0 == p_data.at(0))
    {
      p_tmp += ',';
      return;
    }
    p_tmp += ',' + uitoa10(p_data.at(0));
    return;
  }

};


template <typename T>
struct ToStringNonzero < Data<T, 2> >
{

  static void toString(Data<T, 2> const & p_data, string & p_tmp, bool const valid_0, bool const valid_1) noexcept(true)
  {
    if (! (valid_0 | valid_1))
    {
      p_tmp += ",,";
      return;
    }
    p_tmp += ',';
    if (valid_0 & (0 != p_data.at(0)))
    {
      p_tmp += uitoa10(p_data.at(0));
    }
    p_tmp += ',';
    if (valid_1 & (0 != p_data.at(1)))
    {
      p_tmp += uitoa10(p_data.at(1));
    }
    return;
  }

};

template <typename T>
struct ToStringPayloadNonzero;

template <typename T>
struct ToStringPayloadNonzero < Data<T, 4> >
{

  static void toString(Data<T, 4> const & p_data, string & p_tmp, bool const valid_0, bool const valid_1) noexcept(true)
  {
    if (! (valid_0 | valid_1))
    {
      p_tmp += ",,";
      return;
    }
    p_tmp += ',';
    if (valid_0 & (0 != p_data.at(0)))
    {
      p_tmp += uitoa10(p_data.at(0));
    }
    p_tmp += ',';
    if (valid_1 & (0 != p_data.at(1)))
    {
      p_tmp += uitoa10(p_data.at(1));
    }
    return;
  }

};



template <typename T>
struct ToStringFlags
{
  static void toString(T const & p_data, string & p_tmp, u_int8_t const valid) noexcept(true)
  {
    p_tmp += ',';
    if (0 == valid || p_data.at(0).is_zero)
    {
      return;
    }

    // DEVELOPER NOTE: This is a complicated routine but is optimized for performance.
    //                 Changes to this function will potentially affect MergeFlags as well.
    
    // This look up table is used to determine the next index in the array that will be read for the last
    // 5 entries if is_wrapped it true.
    // It is faster to build this table on the stack dynamically than to
    // loose locality and fetch it from static global memory
    u_int8_t const lut[32]      = {27,  0,  0,  0,  0,  0,  0,  0,
                                    0,  0,  0,  0,  0,  0,  0,  0,
                                    0,  0,  0,  0,  0,  0,  0,  0,
                                    0,  0,  0, 28, 29, 30, 31, 27};
    u_int8_t const max_position = 26;

    // The flag_direction table is used to determine which character to use to indicate direction
    char const     flag_direction[2][2] = { {'-', '+'}, {'>', '<'} };
    u_int8_t       scale[2]             = {2, 4};
    u_int8_t       current              = p_data.at(0).current;
    u_int8_t       end_of_first_flags   = min(max_position, p_data.at(0).position);
    u_int8_t       idx                  = 0;
    u_int8_t       padding              = scale[p_data.at(0).is_icmp];

    if (! p_data.at(0).is_wrapped)
    {
      // if we have less than 32 packets in the flow, set the index of the last entry
      end_of_first_flags = p_data.at(0).position;
    }
    // Print all of the first flags with +/- as the direction indicators
    for (idx = 0; idx <= end_of_first_flags; ++idx)
    {
      p_tmp += flag_direction[0][(p_data.at(0).direction_bitfield >> idx) & 0x1] + uitoa16(p_data.at(0).value[idx], padding);
    }

    if (p_data.at(0).is_wrapped)
    {
      // If we have wrapped, there are more than 32 packets in the flow, so the last five entries
      // need to use '>' and '<' as the direction indicators.
      for (idx = 0; idx < 5; ++idx)
      {
        // On the first iteration, the "current" index will point to the last written value
        // so the next one will be the oldest value in the last five entries;  So the first thnig to do
        // Is use the look up table to determine which value to print next
        current  = lut[current];
        p_tmp   += flag_direction[1][(p_data.at(0).direction_bitfield >> current) & 0x1] + uitoa16(p_data.at(0).value[current], padding);
      }
    }

    return;
  }
};


template <typename T>
struct ToStringIcmpflags
{
  static void toString(T const & p_data, string & p_tmp, bool const valid_0, bool const valid_1) noexcept(true)
  {
    p_tmp += ',';
    if (valid_0)
    {
      p_tmp += asIcmpFlag(p_data.at(0));
    }
    p_tmp += ',';
    if (valid_1)
    {
      p_tmp += asIcmpFlag(p_data.at(1));
    }
    return;
  }
};


template <typename T>
struct ToStringTcpflags
{
  static void toString(T const & p_data, string & p_tmp, bool const valid_0, bool const valid_1) noexcept(true)
  {
    p_tmp += ',';
    if (valid_0)
    {
      p_tmp += asTcpFlag(p_data.at(0));
    }
    p_tmp += ',';
    if (valid_1)
    {
      p_tmp += asTcpFlag(p_data.at(1));
    }
    return;
  }
};


template <typename T>
struct ToStringTbd;


template <typename T>
struct ToString;

template <>
struct ToString < Data<u_int64_t, 1> >
{

  static void toString(Data<u_int64_t, 1> const & p_data, string & p_tmp, bool const valid) noexcept(true)
  {
    if (! valid)
    {
      p_tmp += ',';
      return;
    }
    p_tmp += ',' + uitoa10(p_data.at(0));
    return;
  }

};


template <>
struct ToString < Data<u_int8_t, 1> >
{

  static void toString(Data<u_int8_t, 1> const & p_data, string & p_tmp, bool const valid) noexcept(true)
  {
    if (! valid)
    {
      p_tmp += ',';
      return;
    }
    p_tmp += ',' + uitoa10(p_data.at(0));
    return;
  }

};


template <>
struct ToString < Data<u_int32_t, 4> >
{

  static void toString(Data<u_int32_t, 4> const & p_data, string & p_tmp, bool const valid_0, bool const valid_1, bool const valid_2, bool const valid_3) noexcept(true)
  {
    // DEVELOPER NOTE:
    //   first  - start time - seconds
    //   second - start time - micro seconds
    //   third  - end time   - seconds
    //   fourth - end time   - micro seconds
    u_int64_t usec_diff = ((p_data.at(2) * 1000000) + p_data.at(3)) - ((p_data.at(0) * 1000000) + p_data.at(1));
    u_int64_t seconds   = usec_diff / 1000000;
    u_int64_t frac_secs = usec_diff % 1000000;

    // DEVELOPER NOTE:
    //   Format is seconds.micro_seconds,duration_seconds.duration_microseconds
    p_tmp += ',' + uitoa10(p_data.at(2)) + '.' + uitoa10(p_data.at(3), 6) + ',' + uitoa10(seconds) + '.' + uitoa10(frac_secs, 6);

    return;
  }

};


template <typename T>
struct ToStringSeq;

template <>
struct ToStringSeq < Data<u_int32_t, 4> >
{

  // DEVELOPER NOTE: This is for the First/Last Seen Src/Dst Tcp Sequence Number

  static void toString(Data<u_int32_t, 4> const & p_data, string & p_tmp, bool const valid_0, bool const valid_1, bool const print_timestamp) noexcept(true)
  {
    p_tmp += ',';
    if (print_timestamp)
    {
      if (valid_0 & (0 != p_data.at(0)))
      {
        p_tmp += uitoa10(p_data.at(0));
      }
      p_tmp += ',';
    }
    if (valid_0 & (0 != p_data.at(1)))
    {
      p_tmp += uitoa10(p_data.at(1));
    }
    p_tmp += ',';
    if (print_timestamp)
    {
      if (valid_1 & (0 != p_data.at(2)))
      {
        p_tmp += uitoa10(p_data.at(2));
      }
      p_tmp += ',';
    }
    if (valid_1 & (0 != p_data.at(3)))
    {
      p_tmp += uitoa10(p_data.at(3));
    }
    return;
  }

};


template <typename T>
struct ToStringIp;

template <>
struct ToStringIp < Data< IpAddress_t, 2 > >
{

  static void toString(Data< IpAddress_t, 2 > const & p_data, string & p_tmp, bool const valid_0, bool const valid_1) noexcept(true)
  {
    if (! (valid_0 & valid_1))
    {
      p_tmp += ",,,,";
      return;
    }
    
    if (p_data.at(0).isIpv4())
    {
      // We have an IPv4 Address
      p_tmp += ',' + asIpv4Ptr(p_data.at(0)) + ',' + asIpv4Ptr(p_data.at(1)) + ",,";
      return;
    }

    // We have an IPv6 Address
    p_tmp += ",,," + asIpv6Ptr(p_data.at(0)) + ',' + asIpv6Ptr(p_data.at(1));
    return;
  }

};


template <typename T>
struct ToStringFragType;

template <>
struct ToStringFragType < Data<u_int8_t, 3> >
{

  static void toString(Data<u_int8_t, 3> const & p_data, string & p_tmp, bool const valid_0, bool const valid_1, bool const valid_2) noexcept(true)
  {
    // We always print the FragType even though we pass in the valid_*

    static string lut[2][8] = {{"C", "A", "L", "A", "F", "A", "N", "A"}, {"A", "A", "L", "A", "F", "A", "N", "A"}};
    int position = (p_data.at(0) << 2) + (p_data.at(1) << 1) + p_data.at(2);
    p_tmp += ',' + lut[getFileOrWireFlag()][position];

    return;
  }

};


#undef  UNUSED

#endif // FIELDTOSTRINGPOLICY_HPP
