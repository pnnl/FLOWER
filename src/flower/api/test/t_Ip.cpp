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
// External Includes
#define BOOST_TEST_MODULE IpTest
// Internal Includes
// Application Includes
#include "utility.hpp"
#include "uchar2string.hpp"
#include "Ip.hpp"


struct IpTestSuiteFixture
{
  asIpvXPtr_t asIpv4Ptr = asIpv4CPP;
  asIpvXPtr_t asIpv6Ptr = asIpv6CPP;

  
  IpTestSuiteFixture()
  {
    BOOST_TEST_MESSAGE("IpTestSuite setup fixture");
    utility_init();
    return;
  }


  ~IpTestSuiteFixture()
  {
    BOOST_TEST_MESSAGE("IpTestSuite teardown fixture");
    return;
  }

};


BOOST_FIXTURE_TEST_SUITE(IpTestSuite, IpTestSuiteFixture)


BOOST_AUTO_TEST_CASE(compare_ipv4_addresses)
{
  // Pre-condition: 
  // Condition: 
  // Post-condition:
  IpAddress_t ipv4_address;
  IpAddress_t ipv6_address;
  string      ipv4_str("10.109.19.9");
  string      ipv6_str("2001:0000:4136:e378:8000:63bf:3fff:fdd2");
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

  string out_ipv4_addr = asIpv4Ptr(ipv4_address);
  string out_ipv6_addr = asIpv6Ptr(ipv6_address);

  // Condition:
  cout << ipv4_str << " = " << out_ipv4_addr << endl;
  cout << ipv6_str << " = " << out_ipv6_addr << endl;
  BOOST_CHECK_EQUAL(ipv4_str, out_ipv4_addr);
  BOOST_CHECK_EQUAL(ipv6_str, out_ipv6_addr);

  return;
}


BOOST_AUTO_TEST_CASE(validate_ipv4_addresses)
{
  // Pre-condition: 
  // Condition: 
  // Post-condition:

  // Condition:
  BOOST_CHECK_EQUAL(true, true);

  return;
}


BOOST_AUTO_TEST_SUITE_END()
