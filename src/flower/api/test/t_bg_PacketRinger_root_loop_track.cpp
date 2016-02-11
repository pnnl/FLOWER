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
#define BOOST_TEST_MODULE PacketRinger
#include <boost/scoped_ptr.hpp>
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
  string             interface;
  int unsigned       num_packets_to_capture;
  OutputHelper *     output_helper;
  PacketAddEvent     packet_add_event;
  PacketRinger *     packet_ringer;

  PacketRingerSuiteFixture()
  {
    BOOST_TEST_MESSAGE("PacketRingerSuite setup fixture");

    //cout << "ARGS:" << boost::unit_test::framework::master_test_suite().argc << endl;
    if (boost::unit_test::framework::master_test_suite().argc == 3)
    {
      interface              = lexical_cast<string>(boost::unit_test::framework::master_test_suite().argv[1]);
      num_packets_to_capture = lexical_cast<int unsigned>(boost::unit_test::framework::master_test_suite().argv[2]);
    }
    else
    {
      cout << "\nERROR: Must supply arguments" << endl;
      cout << "  interface"                    << endl;
      cout << "  num_packets_to_capture"       << endl;
      exit(1);
    }

    int unsigned cache_timeout = 1000000;
    bool buffer_packets        = false;
      
    string file_ext   = "dat";
    string out_dir    = getEnsipDataHomeOutput();
    string site_name  = "pnnl_dev";
    output_helper     = new OutputHelper(out_dir, file_ext, 500, site_name, getDataGuideVersion(), ".");
    packet_ringer     = new PacketRinger(*output_helper, &packet_add_event, cache_timeout, num_packets_to_capture, buffer_packets, num_packets_to_capture);
    packet_add_event += new PacketAddEvent::Static("add_event",  &testOnAddEvent);

    return;
  }


  ~PacketRingerSuiteFixture()
  {
    BOOST_TEST_MESSAGE("PacketRingerSuite teardown fixture");

    // Reset the counter for Event Calls between each test case
    howManyEventCalls(::g_add_event_counter);
    packet_ringer->onShutdownSystemEvent(0); //   NORMAL
    delete(packet_ringer);
    delete(output_helper);

    return;
  }
};


BOOST_FIXTURE_TEST_SUITE(PacketRingerSuite, PacketRingerSuiteFixture)

BOOST_AUTO_TEST_CASE(track_total_packets_created_including_fake_packets)
{
  // Pre-condition:
  //   pcap files exist
  //   the user is not root
  // Condition:
  //   read the file
  // Post-condition:
  //   the number of packets (N) read should match the N packets in the file

  // Only run by root
  if (0 == geteuid())
  {
    BOOST_CHECK_EQUAL(packet_ringer->initDevice(interface), true);
    packet_ringer->readDevice();
    packet_ringer->finish();                 //   NORMAL
    BOOST_CHECK_EQUAL(packet_ringer->getPacketCounter().getItemCount(), num_packets_to_capture);
    BOOST_CHECK_EQUAL(packet_ringer->getHeartbeatCount(), 0);
    BOOST_CHECK_EQUAL(howManyEventCalls(::g_add_event_counter), packet_ringer->getHeartbeatCount() + num_packets_to_capture);
  }
  else
  {
    // The user is NOT root so don't do this test
    BOOST_CHECK_EQUAL(true, false);
  }

  return;
}

BOOST_AUTO_TEST_SUITE_END()
