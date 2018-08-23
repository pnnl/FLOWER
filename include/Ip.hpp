///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
//
// CODING CONVENTIONS:
//    * Class names are CamelCase with first word upper case
//    * Functions are camelCase with first word lower case
//    * Function parameters are lower case with _ and have p_ prefix
//    * Member variables always use 'this' pointer
///////////////////////////////////////////////////////////////////////////////


#ifndef IP_HPP
#define IP_HPP


#include "global.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Ip"

#include "global.hpp"

// Namespaces used
using namespace std;


union IpAddress_t
{
public:
  u_int64_t qwords[2];
  u_int32_t dwords[4];
  u_int8_t  bytes[16];


  void clear(void) noexcept(true)
  {
    qwords[0] = 0;
    qwords[1] = 0;
    dwords[2] = 0xFFFF0000;
    //dwords[3] = 0;
    //bytes[12] = 127;
    //bytes[15] = 1;
    return;
  }


  // Constructors

  IpAddress_t(void) noexcept(true)
  {
    clear();
    return;
  }

  IpAddress_t(u_int32_t const p_ip_address) noexcept(true)
  {
    clear();
    this->bytes[12] = p_ip_address  >> 24;
    this->bytes[13] = (p_ip_address >> 16)& 0xFF;
    this->bytes[14] = (p_ip_address >> 8) & 0xFF;
    this->bytes[15] = p_ip_address        & 0xFF;
    return;
  }


  IpAddress_t(IpAddress_t const & p_ip_address) noexcept(true)
  {
    *this = p_ip_address;
    return;
  }


  // Functions

  inline u_int8_t & at(int unsigned const idx) noexcept(true)
  {
    assert(idx < sizeof(bytes));
    return bytes[idx];
  }


  inline u_int8_t const at(int unsigned const idx) const noexcept(true)
  {
    assert(idx < sizeof(bytes));
    return bytes[idx];
  }


  inline bool isIpv4(void) const noexcept(true)
  {
    return( (0 == qwords[0]) && (0xFFFF0000 == dwords[2]));
  }


  // Operators

  IpAddress_t & operator=(IpAddress_t const & p_ip_address) noexcept(true)
  {
    qwords[0] = p_ip_address.qwords[0];
    qwords[1] = p_ip_address.qwords[1];
    return(*this);
  }


  inline bool operator>(IpAddress_t const & p_ip_address) noexcept(true)
  {
    for (int unsigned idx = 0; idx < sizeof(bytes); ++idx)
    {
      if (bytes[idx] > p_ip_address.bytes[idx])
      {
        return(true);
      }

      if (bytes[idx] < p_ip_address.bytes[idx])
      {
        return(false);
      }
    }

    return(false);
  }


  inline bool operator==(IpAddress_t const & p_ip_address) const noexcept(true)
  {
    return(this->qwords[0] == p_ip_address.qwords[0] && this->qwords[1] == p_ip_address.qwords[1]);
  }


  inline bool operator!=(IpAddress_t const & p_ip_address) const noexcept(true)
  {
    return( !(*this == p_ip_address) );
  }

};

#endif // IP_HPP
