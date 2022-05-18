///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute.
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
#define BOOST_TEST_MODULE Packet
// Internal Includes
// Application Includes
#include "utility.hpp"
#include "Packet.hpp"


struct PacketSuiteFixture
{
  
  PacketSuiteFixture()
  {
    BOOST_TEST_MESSAGE("PacketSuite setup fixture");
    utility_init();
    return;
  }


  ~PacketSuiteFixture()
  {
    BOOST_TEST_MESSAGE("PacketSuite teardown fixture");
    return;
  }

};


BOOST_FIXTURE_TEST_SUITE(PacketSuite, PacketSuiteFixture)


BOOST_AUTO_TEST_CASE(hold_raw_network_packet_data)
{
  // Pre-condition: Good packet data
  // Condition: Construct a Packet
  // Post-condition:
  //   the packet has the same data      we put in it
  //   the packet has the same timestamp we put in it
  //   the packet has the same size      we put in it

  // Condition:
  sharedPacket packet(new Packet(::g_REAL, ::g_good_packet_data, 0, ::g_good_packet_data.size(), generateTimestamp(1), 0));
  // Post-condition:
  BOOST_CHECK_EQUAL(::g_good_packet_data == packet->getData(), true);
  BOOST_CHECK_EQUAL(generateTimestamp(1), packet->getHeaderSeconds());
  BOOST_CHECK_EQUAL(::g_good_packet_data.size(), packet->getLength());

  return;
}


BOOST_AUTO_TEST_SUITE_END()
