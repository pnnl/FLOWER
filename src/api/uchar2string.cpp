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
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
// External Includes
// Internal Includes
// Application Includes
#include "num2string.hpp"
#include "uchar2string.hpp"
#include "Exception.hpp"

#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "uchar2string"


// Namespaces
using namespace std;


// Global Variables
string const g_hexdigits("0123456789ABCDEF");
asIpvXPtr_t asIpv4Ptr = asIpv4;
asIpvXPtr_t asIpv6Ptr = asIpv6;


int unsigned asEtherType(ByteArray const & p_array, int unsigned p_offset) noexcept(true)
{
  DEBUG(LOTS, ENTER);

  int unsigned  result = 0;

  // EtherTypes are always two bytes long
  result += ((p_array.at(p_offset) >> 4)    & 255) << 12; // high nibble
  result += ((p_array.at(p_offset) &  0x0F) & 255) <<  8; // low nibble
  ++p_offset;
  result += ((p_array.at(p_offset) >> 4)    & 255) <<  4; // high nibble
  result += ((p_array.at(p_offset) &  0x0F) & 255);       // low nibble

  DEBUG(LOTS, LEAVE);
  return(result);
}


string asIpv4(IpAddress_t const & p_ip_address) noexcept(true)
{
  DEBUG(LOTS, ENTER);
  DEBUG(LOTS, LEAVE);
  return(uitoa10(p_ip_address.at(12), 0) + "." +
         uitoa10(p_ip_address.at(13), 0) + "." +
         uitoa10(p_ip_address.at(14), 0) + "." +
         uitoa10(p_ip_address.at(15), 0));
}


string asIpv6(IpAddress_t const & p_ip_address) noexcept(true)
{
  DEBUG(LOTS, ENTER);

  static u_int8_t g_hexdigits[] = "0123456789abcdef";
  int unsigned                          i_idx  = 0;
  int unsigned                          r_idx  = 0;
  static string                         result("xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx");

  for (int unsigned i = 0; i < 16; ++i)
  {
    result.at(r_idx)    = g_hexdigits[p_ip_address.at(i_idx) >> 4];    // high nibble
    result.at(++r_idx)  = g_hexdigits[p_ip_address.at(i_idx) &  0x0f]; // low nibble
    ++r_idx;
    ++i_idx;

    switch (i_idx)
    {
      case  2:
      case  4:
      case  6:
      case  8:
      case 10:
      case 12:
      case 14:
        ++r_idx;
        break;
    }
  }

  DEBUG(LOTS, LEAVE);
  return(result);
}


//string asIpv4Int(u_int32_t const p_ip_address) noexcept(true)
//{
//  DEBUG(LOTS, ENTER);
//  DEBUG(LOTS, LEAVE);
//  return(uitoa10(p_ip_address & 0xFF, 3) + uitoa10((p_ip_address >> 8) & 0xFF, 3) + uitoa10((p_ip_address >> 16) & 0xFF, 3) + uitoa10(p_ip_address >> 24, 0));
//}


string asIpv4CPP(IpAddress_t const & p_ip_address) noexcept(true)
{
  DEBUG(LOTS, ENTER);
  DEBUG(LOTS, LEAVE);
  // DEVELOPER NOTE: Do not pad the first octet with zeros
  return(uitoa10(p_ip_address.at(12), 0) + uitoa10(p_ip_address.at(13), 3) + uitoa10(p_ip_address.at(14), 3) + uitoa10(p_ip_address.at(15), 3));
}


string asIpv6CPP(IpAddress_t const & p_ip_address) noexcept(true)
{
  DEBUG(LOTS, ENTER);

  static u_int8_t g_hexdigits[] = "0123456789ABCDEF";
  int unsigned                          i_idx  = 0;
  int unsigned                          r_idx  = 0;
  static string                         result("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

  for (int unsigned i = 0; i < 16; ++i)
  {
    result.at(r_idx)    = g_hexdigits[p_ip_address.at(i_idx) >> 4];    // high nibble
    result.at(++r_idx)  = g_hexdigits[p_ip_address.at(i_idx) &  0x0F]; // low nibble
    ++r_idx;
    ++i_idx;
  }

  DEBUG(LOTS, LEAVE);
  return(result);
}


string asMac(ByteArray & p_array, int unsigned p_offset) noexcept(true)
{
  DEBUG(LOTS, ENTER);

  int unsigned                          idx  = 0;
  string                                result("xx:xx:xx:xx:xx:xx");

  // DEVELOPER NOTE: I loop up to 5 instead of 8 because on Windows, the final
  //                 ++itor within the loop causes a failure when i is 7.  So, I
  //                 perform the last iteration of the loop afterwards.
  for (int unsigned i = 0; i < 5; ++i) // MAC addresses are always six bytes long
  {
    result.at(idx)    = ::g_hexdigits[(p_array.at(p_offset) >> 4)];      // high nibble
    result.at(++idx)  = ::g_hexdigits[(p_array.at(p_offset) &  0x0F)];   // low nibble
    idx              += 2;
    ++p_offset;
  }
  result.at(idx)      = ::g_hexdigits[(p_array.at(p_offset) >> 4)];      // high nibble
  result.at(++idx)    = ::g_hexdigits[(p_array.at(p_offset) &  0x0F)];   // low nibble

  DEBUG(LOTS, LEAVE);
  return(result);
}


string asIcmpFlag(u_int16_t const p_flag) noexcept(true)
{
  DEBUG(LOTS, ENTER);

  string result("xxxx");
  result.at(0) = ::g_hexdigits[(p_flag >> 12)];
  result.at(1) = ::g_hexdigits[(p_flag >>  8) & 0x0F];
  result.at(2) = ::g_hexdigits[(p_flag >>  4) & 0x0F];
  result.at(3) = ::g_hexdigits[(p_flag &  0x0F)];

  DEBUG(LOTS, LEAVE);
  return(result);
}


string asTcpFlag(u_int16_t const p_flag) noexcept(true)
{
  DEBUG(LOTS, ENTER);

  string result("xx");
  result.at(0) = ::g_hexdigits[(p_flag >> 4)];
  result.at(1) = ::g_hexdigits[(p_flag &  0x0F)];

  DEBUG(LOTS, LEAVE);
  return(result);
}


void hexdumper(ostream &o, u_int8_t * const p_array, int unsigned const p_length)
{
  // DEVELOPER NOTE:
  //  Adapted from: http://www.cpax.org.uk/prg/windows/structstor.cpp
  int unsigned       size   = p_length;
  u_int8_t *         base   = p_array;
  int unsigned const indent = 4;
  string             literal("");
  string const       hexdigits("0123456789ABCDEF");
  int unsigned       idx;

  for (idx = 0; idx < size; ++idx)
  {
    if (0 == idx % 16)
    {
      if (0 != idx)
      {
        o << "  " << literal;
        literal = "";
        o << "\n";
      }
      for (int unsigned j = 0; j < indent; ++j)
      {
        o << ' ';
      }
    }

    if (isprint(base[idx]))
    {
      literal += base[idx];
    }
    else
    {
      literal += '.';
    }
    if (0 == idx % 16)
    {
      // print offset
      o << hexdigits[(idx & 0xF00) >> 8];
      o << hexdigits[(idx & 0x0F0) >> 4];
      o << hexdigits[(idx & 0x00F)     ];
      o << "  ";
    }

    // print hex data
    o << hexdigits[(base[idx] & 0xF0) >> 4];
    o << hexdigits[(base[idx] & 0x0F)];
    o << ' ';
  }

  idx %= 16;
  while (idx % 16 != 0)
  {
    o << "   ";
    idx++;
  }

  o << "  " << literal;
}
