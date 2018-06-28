///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifndef UCHAR2STRING_HPP
#define UCHAR2STRING_HPP


// System Includes
#include <string>
// External Includes
// Internal Includes
// Application Includes
#include "FixedArray.hpp"
#include "Ip.hpp"


// Namespaces
using namespace std;


int unsigned asEtherType(ByteArray const & p_array, int unsigned p_offset) throw(); 
string       asIpv4(IpAddress_t const & p_ip_address) throw();
string       asIpv4CPP(IpAddress_t const & p_ip_address) throw();
//string       asIpv4Int(u_int32_t const p_ip_address) throw();
string       asIpv6(IpAddress_t const & p_ip_address) throw();
string       asIpv6CPP(IpAddress_t const & p_ip_address) throw();
string       asMac(ByteArray const & p_array, int unsigned p_offset) throw();
string       asIcmpFlag(short unsigned const p_icmp_flag) throw();
string       asTcpFlag(short unsigned const p_tcp_flag) throw();
void         hexdumper(ostream &o, u_int8_t * const p_array, int unsigned const p_length);

// Function Pointers
typedef string  (*asIpvXPtr_t)(IpAddress_t const & p_ip_address);
extern  asIpvXPtr_t asIpv4Ptr;
extern  asIpvXPtr_t asIpv6Ptr;

#endif // UCHAR2STRING_HPP
