///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
// _____________________________________________________________________________
//
// PURPOSE OF TEST: 
//  [put a single sentence stating the purpost of this file 
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


// System Includes
#include <arpa/inet.h>
// External Includes
#define BOOST_TEST_MODULE IpTest
// Internal Includes
// Application Includes
#include "utility.hpp"
#include "uchar2string.hpp"
#include "Ip.hpp"


struct IpTestSuiteFixture
{
  IpAddress_t ipv4_address;
  IpAddress_t ipv6_address;

  
  IpTestSuiteFixture()
  {
    BOOST_TEST_MESSAGE("IpTestSuite setup fixture");
    utility_init();
    char unsigned ipv6_data[] = {
                                  0x20, 0x01, //  2001
                                  0x00, 0x00, //  0000
                                  0x41, 0x36, //  4136
                                  0xe3, 0x78, //  e378
                                  0x80, 0x00, //  8000
                                  0x63, 0xbf, //  63BF
                                  0x3f, 0xff, //  3fff
                                  0xfd, 0xd2  //  fdd2
                                };

    for (int unsigned idx = 0; idx < 12; ++idx)
    {
      ipv4_address.at(idx) = 0x00;
    }
    ipv4_address.at(12) = 10;
    ipv4_address.at(13) = 109;
    ipv4_address.at(14) = 19;
    ipv4_address.at(15) = 9;

    for (int unsigned idx = 0; idx < 16; ++idx)
    {
      ipv6_address.at(idx) = ipv6_data[idx];
    }

    return;
  }


  ~IpTestSuiteFixture()
  {
    BOOST_TEST_MESSAGE("IpTestSuite teardown fixture");
    return;
  }

};


BOOST_FIXTURE_TEST_SUITE(IpTestSuite, IpTestSuiteFixture)


BOOST_AUTO_TEST_CASE(compare_RFC_ip_addresses)
{
  // Pre-condition: 
  // Condition: 
  // Post-condition:
  asIpvXPtr_t asIpv4Ptr = asIpv4;
  asIpvXPtr_t asIpv6Ptr = asIpv6;
  string      ipv4_str("10.109.19.9");
  string      ipv6_str("2001:0000:4136:e378:8000:63bf:3fff:fdd2");

  string out_ipv4_addr = asIpv4Ptr(ipv4_address);
  string out_ipv6_addr = asIpv6Ptr(ipv6_address);

  // Condition:
  cout << ipv4_str << " = " << out_ipv4_addr << endl;
  cout << ipv6_str << " = " << out_ipv6_addr << endl;
  BOOST_CHECK_EQUAL(ipv4_str, out_ipv4_addr);
  BOOST_CHECK_EQUAL(ipv6_str, out_ipv6_addr);

  return;
}


BOOST_AUTO_TEST_CASE(compare_CPP_ip_addresses)
{
  // Pre-condition:
  // Condition:
  // Post-condition:
  asIpvXPtr_t asIpv4Ptr = asIpv4CPP;
  asIpvXPtr_t asIpv6Ptr = asIpv6CPP;
  string      ipv4_str("10109019009");
  string      ipv6_str("200100004136E378800063BF3FFFFDD2");

  string out_ipv4_addr = asIpv4Ptr(ipv4_address);
  string out_ipv6_addr = asIpv6Ptr(ipv6_address);

  // Condition:
  cout << ipv4_str << " = " << out_ipv4_addr << endl;
  cout << ipv6_str << " = " << out_ipv6_addr << endl;
  BOOST_CHECK_EQUAL(ipv4_str, out_ipv4_addr);
  BOOST_CHECK_EQUAL(ipv6_str, out_ipv6_addr);

  return;
}


BOOST_AUTO_TEST_CASE(validate_RFC_ip_addresses)
{
  // Pre-condition: 
  // Condition: 
  // Post-condition:
  // NOTE: From URL: https://beej.us/guide/bgnet/html/multi/inet_ntopman.html
  struct      sockaddr_in  sa4;
  struct      sockaddr_in6 sa6;
  char        str4[INET_ADDRSTRLEN];
  char        str6[INET6_ADDRSTRLEN];

  asIpvXPtr_t asIpv4Ptr = asIpv4;
  asIpvXPtr_t asIpv6Ptr = asIpv6;
  strncpy(str4, asIpv4Ptr(ipv4_address).c_str(), sizeof(str4) - 1);
  strncpy(str6, asIpv6Ptr(ipv6_address).c_str(), sizeof(str6) - 1);

  int         ipv4_check = inet_pton(AF_INET,  str4, &(sa4.sin_addr));
  int         ipv6_check = inet_pton(AF_INET6, str6, &(sa6.sin6_addr));

  // Condition:
  BOOST_CHECK_EQUAL(1, ipv4_check);
  BOOST_CHECK_EQUAL(1, ipv6_check);

  return;
}


BOOST_AUTO_TEST_SUITE_END()
