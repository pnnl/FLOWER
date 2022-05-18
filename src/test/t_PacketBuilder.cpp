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
#define BOOST_TEST_MODULE PacketBuilder
// Internal Includes
// Application Includes
#include "utility.hpp"
#include "PacketBuilder.hpp"


// Namespaces
using namespace boost;
  

// Global Variables

const char * g_my_argv = "t_PacketBuilder";
int unsigned g_pkts    = 4;
string       g_pkts_str("4");


struct PacketBuilderSuiteFixture
{
  // Declare any instance variables here that are used in the constructor
  // or destructor.
  // NOTE: These variables will appear as local variables in each test case.
  string             interface;
  string             interface_not_found;
  vector<string>     pcap_files_good;
  vector<string>     pcap_files_missing;
  vector<string>     pcap_files_malformed;
  vector<string>     pcap_files_permissions;
  PacketAddEvent     packet_add_event;
  PacketBuilder *    packet_builder;
  NewPcapDescEvent   new_pcap_desc_event;

  PacketBuilderSuiteFixture()
  {
    BOOST_TEST_MESSAGE("PacketBuilderSuite setup fixture");

    string flower_data_home = getFlowerDataHomeInput();
    string flower_data_nic  = getFlowerDataNic();
    pcap_files_good.push_back(flower_data_home + ::g_pkts_str + "pkts.pcap");
    pcap_files_missing.push_back(flower_data_home + "missing.pcap");
    pcap_files_malformed.push_back(flower_data_home + "malformed.pcap");
    pcap_files_permissions.push_back("/etc/shadow");
    
    interface            = flower_data_nic;
    interface_not_found  = "asdf";

    // sharedPacket packet(new Packet(::g_REAL));
    packet_builder       = new PacketBuilder(&new_pcap_desc_event, &packet_add_event, ::g_pkts, 65535);
    packet_add_event    += new PacketAddEvent::Static("add_event",  &testOnAddEvent);
    new_pcap_desc_event += new NewPcapDescEvent::Static("new_desc", &testOnAddEvent);
    //  TODO:  2017/06/02 - Don't think we need the following line
    //packet_builder->initFile(flower_data_home + "missing.pcap");
    return;
  }


  ~PacketBuilderSuiteFixture()
  {
    BOOST_TEST_MESSAGE("PacketBuilderSuite teardown fixture");

    // Reset the counter for Event Calls between each test case
    howManyEventCalls(::g_add_event_counter);
    delete(packet_builder);
    return;
  }
};


BOOST_FIXTURE_TEST_SUITE(PacketBuilderSuite, PacketBuilderSuiteFixture)


BOOST_AUTO_TEST_CASE(track_total_packets_created)
{
  // DEVELOPER NOTE: Example on how to read argc and argv
  //if (framework::master_test_suite().argc > 1)
  //{
  //  cout << "Argc:   " << framework::master_test_suite().argc    << ":\n";
  //  cout << "Argv[1]:" << framework::master_test_suite().argv[1] << ":\n";
  //}

  // Pre-condition:
  //   pcap files exist
  //   the user is not root
  // Condition:
  //   read the file
  // Post-condition:
  //   the number of packets (N) read should match the N packets in the file

  if (0 != geteuid())
  {
    for (vector<string>::const_iterator itor = pcap_files_good.begin(); itor != pcap_files_good.end(); ++itor)
    {
      packet_builder->initFile(*itor);
      BOOST_CHECK_EQUAL(packet_builder->readFile(), true);
      BOOST_CHECK_EQUAL(packet_builder->getPacketCounter().getItemCount(), ::g_pkts);
    }
  }
  else
  {
    // The user is root so don't do this test but don't fail either
    BOOST_CHECK_EQUAL(true, true);
  }

  return;
}

BOOST_AUTO_TEST_CASE(build_packets_from_pcap_file)
{
  // Pre-condition:
  //   pcap files exist
  //   the user is not root
  // Condition:
  //   read the file
  // Post-condition:
  //   the number of packets (N) read should match the N packets in the file
  //   the onAddEvent should have been called N times

  if (0 != geteuid())
  {
    for (vector<string>::const_iterator itor = pcap_files_good.begin(); itor != pcap_files_good.end(); ++itor)
    {
      BOOST_CHECK_EQUAL(packet_builder->initFile(*itor), true);
      BOOST_CHECK_EQUAL(packet_builder->readFile(), true);
      BOOST_CHECK_EQUAL(packet_builder->getPacketCounter().getItemCount(), ::g_pkts);
      BOOST_CHECK_EQUAL(howManyEventCalls(::g_add_event_counter), ::g_pkts + 1);
    }
  }
  else
  {
    // The user is root so don't do this test but don't fail either
    BOOST_CHECK_EQUAL(true, true);
  }

  return;
}


BOOST_AUTO_TEST_CASE(handle_missing_pcap_file)
{
  // Pre-condition:
  //   pcap file does not exist
  //   the user is not root
  // Condition:
  //   try to read the file
  // Post-condition:
  //   the number of packets read should be 0

  if (0 != geteuid())
  {
    for (vector<string>::const_iterator itor = pcap_files_missing.begin(); itor != pcap_files_missing.end(); ++itor)
    {
      cout << "DEVELOPER NOTE: There should be an missing pcap file ERROR message that follows" << endl;
      BOOST_CHECK_EQUAL(packet_builder->initFile(*itor), false);
      BOOST_CHECK_EQUAL(packet_builder->getPacketCounter().getItemCount(), (int unsigned)0);
    }
  }
  else
  {
    // The user is root so don't do this test but don't fail either
    BOOST_CHECK_EQUAL(true, true);
  }

  return;
}


BOOST_AUTO_TEST_CASE(handle_malformed_pcap_file)
{
  // Pre-condition:
  //   pcap file is malformed (bad data)
  //   the user is not root
  // Condition:
  //   try to read the file
  // Post-condition:
  //   the number of packets read should be 0

  if (0 != geteuid())
  {
    for (vector<string>::const_iterator itor = pcap_files_malformed.begin(); itor != pcap_files_malformed.end(); ++itor)
    {
      cout << "DEVELOPER NOTE: There should be an malformed pcap file ERROR message that follows" << endl;
      BOOST_CHECK_EQUAL(packet_builder->initFile(*itor), false);
      BOOST_CHECK_EQUAL(packet_builder->getPacketCounter().getItemCount(), (int unsigned)0);
    }
  }
  else
  {
    // The user is root so don't do this test but don't fail either
    BOOST_CHECK_EQUAL(true, true);
  }

  return;
}


BOOST_AUTO_TEST_CASE(handle_permission_denied_on_pcap_file)
{
  // Pre-condition:
  //   pcap file exists
  //   the user is not root
  //   the user does not have permissions to read the file
  // Condition:
  //   try to read the file
  // Post-condition:
  //   the number of packets read should be 0

  if (0 != geteuid())
  {
    for (vector<string>::const_iterator itor = pcap_files_permissions.begin(); itor != pcap_files_permissions.end(); ++itor)
    {
      cout << "DEVELOPER NOTE: There should be an permission denied on pcap file ERROR message that follows" << endl;
      BOOST_CHECK_EQUAL(packet_builder->initFile(*itor), false);
      BOOST_CHECK_EQUAL(packet_builder->getPacketCounter().getItemCount(), (int unsigned)0);
    }
  }
  else
  {
    // The user is root so don't do this test but don't fail either
    BOOST_CHECK_EQUAL(true, true);
  }

  return;
}


BOOST_AUTO_TEST_CASE(handle_permission_denied_on_interface)
{
  // Pre-condition:
  //   the network interface exists
  //   the user is not root
  // Condition:
  //   try to read the network interface
  // Post-condition:
  //   an Permission Denied message should be caught

  if (0 != geteuid())
  {
    cout << "DEVELOPER NOTE: There should be an permission denied on interface (";
    cout << interface << ") ERROR message that follows" << endl;
    BOOST_CHECK_EQUAL(packet_builder->initDevice(interface), false);
  }
  else
  {
    // The user is root so don't do this test but don't fail either
    BOOST_CHECK_EQUAL(true, true);
  }

  return;
}


// DEVELOPER NOTE: 2009/02/19: This was turned off because we do not
//                             implement the onIdleSystemEvent method yet
//BOOST_AUTO_TEST_CASE(build_a_fake_packet)
//{
//  // Pre-condition:
//  //   the user is not root
//  // Condition:
//  //   call the onIdleSystemEvent
//  // Post-condition:
//  //   a Not Found message should be caught
//
//  if (0 != geteuid())
//  {
//    packet_builder->onIdleSystemEvent(120);
//    BOOST_CHECK_EQUAL(packet_builder->getPacketCounter().getItemCount(), (int unsigned)0);
//    BOOST_CHECK_EQUAL(wasEventCalled(::g_add_event_called), true);
//  }
//  else
//  {
//    // The user is root so don't do this test but don't fail either
//    BOOST_CHECK_EQUAL(true, true);
//  }
//
//  return;
//}


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
    cout << "DEVELOPER NOTE: There should be an missing network interface ERROR message that follows" << endl;
    BOOST_CHECK_EQUAL(packet_builder->initDevice(interface_not_found), false);
  }
  else
  {
    // The user is NOT root so don't do this test but don't fail either
    BOOST_CHECK_EQUAL(true, true);
  }

  return;
}


BOOST_AUTO_TEST_CASE(build_packets_from_network_interface)
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
    try
    {
      packet_builder->initDevice(interface);
      packet_builder->readDevice();
      BOOST_CHECK_EQUAL(howManyEventCalls(::g_add_event_counter), ::g_pkts + 1);
    }
    catch(string & e)
    {
      BOOST_CHECK_EQUAL(e == NotFound, interface_not_found == "asdf");
    }
  }
  else
  {
    // The user is NOT root so don't do this test but don't fail either
    BOOST_CHECK_EQUAL(true, true);
  }

  return;
}


BOOST_AUTO_TEST_SUITE_END()
