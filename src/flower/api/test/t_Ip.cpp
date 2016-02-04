///////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2008.  .  All Rights Reserved. 
//
// THIS FILE INITIALLY CREATED WITH:  
//     TEMPLATE NAME:  lang_cpp_test.template 
//     COMMAND NAME:   gensrc 
//
// ND file: $Id$ 
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
#include "Ip.hpp"


struct IpTestSuiteFixture
{
  
  IpTestSuiteFixture()
  {
    BOOST_TEST_MESSAGE("IpTestSuite setup fixture");
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

  // Condition:
  BOOST_CHECK_EQUAL(true, true);

  return;
}


BOOST_AUTO_TEST_SUITE_END()
