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
#define BOOST_TEST_MODULE PacketRinger
// Internal Includes
// Application Includes
#include "utility.hpp"
#include "Exception.hpp"
#include "ExceptionHandler.hpp"
#include "PacketRinger.hpp"


// Namespaces
using namespace boost;
  

// Global Variables

SignalTranslator<SigTerm>                g_objSigTermTranslator;


struct PacketRingerSuiteFixture
{
  // Declare any instance variables here that are used in the constructor
  // or destructor.
  // NOTE: These variables will appear as local variables in each test case.
  string             interface_not_found;
  OutputHelper *     output_helper;
  PacketAddEvent     packet_add_event;
  PacketRinger *     packet_ringer;

  PacketRingerSuiteFixture()
  {
    BOOST_TEST_MESSAGE("PacketRingerSuite setup fixture");

    interface_not_found  = "asdf";

    string file_ext   = "dat";
    string out_dir    = "/tmp";
    string site_name  = "pnnl_dev";
    output_helper     = new OutputHelper(out_dir, file_ext, 500, site_name, getDataGuideVersion(), ".");
    packet_ringer     = new PacketRinger(*output_helper, &packet_add_event, 1, 1, 1, 1);

    return;
  }


  ~PacketRingerSuiteFixture()
  {
    BOOST_TEST_MESSAGE("PacketRingerSuite teardown fixture");

    // Reset the counter for Event Calls between each test case
    delete(packet_ringer);
    delete(output_helper);

    return;
  }
};


BOOST_FIXTURE_TEST_SUITE(PacketRingerSuite, PacketRingerSuiteFixture)

BOOST_AUTO_TEST_CASE(handle_missing_network_interface)
{
  // Pre-condition:
  //   the network interface does not exists
  //   the user is not root
  // Condition:
  //   try to read the network interface
  // Post-condition:
  //   a Not Found message should be caught

  if (0 == geteuid())
  {
    cout << "DEVELOPER NOTE: There should be a missing network interface ERROR message that follows" << endl;
sharedPacket fakePacket = packet_ringer->getFakePacket();
cout << fakePacket << endl;
    BOOST_CHECK_EQUAL(packet_ringer->initDevice(interface_not_found), false);
  }
  else
  {
    // The user is NOT root so don't do this test
    BOOST_CHECK_EQUAL(true, false);
  }

  return;
}

BOOST_AUTO_TEST_SUITE_END()
