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
#define BOOST_TEST_MODULE Flow
#include <boost/date_time/posix_time/posix_time.hpp>
// Internal Includes
// Application Includes
#include "global.hpp"
#include "utility.hpp"
#include "Flow.hpp"

// Namespaces
using namespace boost::posix_time;


struct FlowSuiteFixture
{

  FlowSuiteFixture()
  {
    BOOST_TEST_MESSAGE("FlowSuite setup fixture");
    utility_init();

    return;
  }


  ~FlowSuiteFixture()
  {
    BOOST_TEST_MESSAGE("FlowSuite teardown fixture");
    return;
  }

};


BOOST_FIXTURE_TEST_SUITE(FlowSuite, FlowSuiteFixture)


void checkStats(sharedFlow const p_flow, time_t const p_time, string const p_src_mac_addr, string const p_dst_mac_addr, string const p_duration, int const p_src_packets, int const p_dst_packets, int const p_src_bytes, int const p_dst_bytes)
{
  BOOST_CHECK_EQUAL(p_flow->getVirtualTime(), p_time);
//  BOOST_CHECK_EQUAL(p_flow->physical_struct.packet_count.at(0), p_src_bytes);
//  BOOST_CHECK_EQUAL(p_flow->physical_struct.packet_count.at(1), p_dst_bytes);
//  BOOST_CHECK_EQUAL(p_flow->physical_struct.total_bytes.at(0), p_src_bytes);
//  BOOST_CHECK_EQUAL(p_flow->physical_struct.total_bytes.at(1), p_dst_bytes);
  return;
}


BOOST_AUTO_TEST_CASE(hold_parsed_network_packet_data)
{
  // Pre-condition: NONE
  string duration = "0.000000";

  // Condition: Construct a new flow
  sharedFlow flow = generateFlow(true, generateTimestamp(1), 1);
 
  // Post-condition: the flow has the data right data
  checkStats(flow, generateTimestamp(1), "s1", "d1", duration, 1, 0, sizeof(::g_good_data), 0);

  return;
}


BOOST_AUTO_TEST_CASE(merge_with_another_flow_from_the_same_session)
{
  int unsigned const time_offset = 2;

  // Pre-condition: Construct two flows from the same session
  sharedFlow flow1 = generateFlow(true, generateTimestamp(1), 1);
  sharedFlow flow2 = generateFlow(true, generateTimestamp(time_offset), 1);

// FIX: CRITICAL: We don't test enough fields
//cout << flow1 << endl;
  // Condition: Construct a new flow
  flow1->merge(flow2);
//cout << flow1 << endl;

  // Post-condition: the flow has the data right data
  checkStats(flow1, generateTimestamp(time_offset), "s1", "d1", "1.000000", 2, 0, sizeof(::g_good_data) * 2, 0);
  checkStats(flow2, generateTimestamp(time_offset), "s1", "d1", "0.000000", 1, 0, sizeof(::g_good_data), 0);

  return;
}


BOOST_AUTO_TEST_CASE(provide_string_representation_of_itself)
{
  // Pre-condition: A string representation of a flow from the epoch
  ptime epoch(date(1970,1,1));
  stringstream flowout1("");
  stringstream flowout2("");

  // DEVELOPER NOTE: TODO: 2009/02/25:
  //   Get data from g_good_data in utility.hpp and build the following
  //   strings from those values:
  string flowFromWire = "0,site-name,0.000000,0.000000,8738,1,240.240.240.240,241.241.241.241,,,,,128,,1,,,,1,13107,0030,,+0030,,,,,,,,,,,,,,,,,,,,,,,,C,,";
  string flowFromFile = "1,site-name,0.000000,0.000000,8738,1,240.240.240.240,241.241.241.241,,,,,128,,1,,,,1,13107,0030,,+0030,,,,,,,,,,,,,,,,,,,,,,,,A,,";

  bool wire = true;
  bool file = false;

  // Condition: Construct a new flow as if generated from Wire
  setFileOrWireFlag(wire);
  sharedFlow flow1 = generateFlow(true, epoch, 1, ::g_REAL, ::g_icmp_protocol);
  flowout1 << flow1;

  // Post-condition: the flow has the data right data
  BOOST_CHECK_EQUAL(flowout1.str(), flowFromWire);

  // Condition: Construct a new flow as if generated from a File
  setFileOrWireFlag(file);
  sharedFlow flow2 = generateFlow(true, epoch, 1, ::g_REAL, ::g_icmp_protocol);
  flowout2 << flow2;

  // Post-condition: the flow has the data right data
  BOOST_CHECK_EQUAL(flowout2.str(), flowFromFile);

  return;
}


BOOST_AUTO_TEST_CASE(copy_flow)
{
  // Pre-condition: A string representation of a flow from the epoch
  ptime epoch(date(1970,1,1));

  // Condition: Construct a new flow and copy it
  sharedFlow flow1 = generateFlow(true, epoch, 1);
  sharedFlow flow2(flow1);

  // Post-condition: the flow has the data right data
  BOOST_CHECK_EQUAL(flow1->compare(flow2), true);
  
  return;
}


BOOST_AUTO_TEST_CASE(zero_icmp_flow)
{
  bool wire = true;
  bool file = false;

  // Condition: Construct a new flow as if generated from File
  setFileOrWireFlag(file);

  // Pre-condition: Three Flows
  sharedFlow flow1 = generateFlow(true, 1, 1, ::g_REAL, ::g_icmp_protocol);
  sharedFlow flow2 = generateFlow(true, 2, 1, ::g_REAL, ::g_icmp_protocol);
  sharedFlow flow3 = generateFlow(true, 3, 1, ::g_REAL, ::g_icmp_protocol);
  sharedFlow flow4 = generateFlow(true, 4, 1, ::g_REAL, ::g_icmp_protocol);
  sharedFlow flow5 = generateFlow(true, 5, 1, ::g_REAL, ::g_icmp_protocol);
  sharedFlow flow6 = generateFlow(true, 6, 1, ::g_REAL, ::g_icmp_protocol);

  string     part1 = "1,site-name,";
  string     part2 = ".000000,0.000000,8738,1,240.240.240.240,241.241.241.241,,,,,,,,,,,1,13107,0000,,,,,,,,,,,,,,,,,,,,,,,,,,";
  string     part3 = ",,";

  BOOST_CHECK_EQUAL(flow1->createFlowString(), "1,site-name,1.000000,0.000000,8738,1,240.240.240.240,241.241.241.241,,,,,128,,1,,,,1,13107,0030,,+0030,,,,,,,,,,,,,,,,,,,,,,,,A,,");

  // Condition: 
  flow1->merge(flow2);
  BOOST_CHECK_EQUAL(flow1->createFlowString(), "1,site-name,2.000000,1.000000,8738,1,240.240.240.240,241.241.241.241,,,,,256,,2,,,,1,13107,0030,,+0030+0030,,,,,,,,,,,,,,,,,,,,,,,,A,,");
  flow1->setFragType(true);
  BOOST_CHECK_EQUAL(flow1->createFlowString(), "1,site-name,2.000000,1.000000,8738,1,240.240.240.240,241.241.241.241,,,,,256,,2,,,,1,13107,0030,,+0030+0030,,,,,,,,,,,,,,,,,,,,,,,,F,,");
  flow1->zero();
  BOOST_CHECK_EQUAL(flow1->createFlowString(), part1 + "2" + part2 + "F" + part3);

  flow1->merge(flow3);
  flow1->merge(flow4);
  flow1->setFragType(true);
  flow1->zero();
  BOOST_CHECK_EQUAL(flow1->createFlowString(), part1 + "4" + part2 + "N" + part3);

  flow1->merge(flow5);
  flow1->merge(flow6);
  flow1->setFragType(true);
  flow1->zero();
  BOOST_CHECK_EQUAL(flow1->createFlowString(), part1 + "6" + part2 + "N" + part3);

  flow1->setFragType(false);
  BOOST_CHECK_EQUAL(flow1->createFlowString(), part1 + "6" + part2 + "L" + part3);

  return;
}


BOOST_AUTO_TEST_CASE(zero_tcp_flow)
{
  bool wire = true;
  bool file = false;

  // Condition: Construct a new flow as if generated from File
  setFileOrWireFlag(file);

  // Pre-condition: Three Flows
  sharedFlow flow1 = generateFlow(true, 1, 1, ::g_REAL, ::g_tcp_protocol);
  sharedFlow flow2 = generateFlow(true, 2, 1, ::g_REAL, ::g_tcp_protocol);
  sharedFlow flow3 = generateFlow(true, 3, 1, ::g_REAL, ::g_tcp_protocol);
  sharedFlow flow4 = generateFlow(true, 4, 1, ::g_REAL, ::g_tcp_protocol);
  sharedFlow flow5 = generateFlow(true, 5, 1, ::g_REAL, ::g_tcp_protocol);
  sharedFlow flow6 = generateFlow(true, 6, 1, ::g_REAL, ::g_tcp_protocol);

  string     part1 = "1,site-name,";
  string     part2 = ".000000,0.000000,8738,6,240.240.240.240,241.241.241.241,,,,,,,,,,,1,13107,,,,,,00,,,,,,,,,,,,,,,,,,,,,";
  string     part3 = ",,";

  BOOST_CHECK_EQUAL(flow1->createFlowString(), "1,site-name,1.000000,0.000000,8738,6,240.240.240.240,241.241.241.241,,,,,128,,1,,,,1,13107,,,,,,30,,+30,,888888888,,,,888888888,,,,,,,,,,,,,A,,");

  // Condition:
  flow1->merge(flow2);
  BOOST_CHECK_EQUAL(flow1->createFlowString(), "1,site-name,2.000000,1.000000,8738,6,240.240.240.240,241.241.241.241,,,,,256,,2,,,,1,13107,,,,,,30,,+30+30,,888888888,,,,888888888,,,,,,,,,,,,,A,,");
  flow1->setFragType(true);
  BOOST_CHECK_EQUAL(flow1->createFlowString(), "1,site-name,2.000000,1.000000,8738,6,240.240.240.240,241.241.241.241,,,,,256,,2,,,,1,13107,,,,,,30,,+30+30,,888888888,,,,888888888,,,,,,,,,,,,,F,,");
  flow1->zero();
  BOOST_CHECK_EQUAL(flow1->createFlowString(), part1 + "2" + part2 + "F" + part3);

  flow1->merge(flow3);
  flow1->merge(flow4);
  flow1->setFragType(true);
  flow1->zero();
  BOOST_CHECK_EQUAL(flow1->createFlowString(), part1 + "4" + part2 + "N" + part3);

  flow1->merge(flow5);
  flow1->merge(flow6);
  flow1->setFragType(true);
  flow1->zero();
  BOOST_CHECK_EQUAL(flow1->createFlowString(), part1 + "6" + part2 + "N" + part3);

  flow1->setFragType(false);
  BOOST_CHECK_EQUAL(flow1->createFlowString(), part1 + "6" + part2 + "L" + part3);

  return;
}

BOOST_AUTO_TEST_CASE(size_of_flow)
{
  bool wire = true;
  bool file = false;

  // Condition: Construct a new flow as if generated from File
  setFileOrWireFlag(file);

  // Pre-condition: Three Flows
  sharedFlow flow1 = generateFlow(true, 1, 1, ::g_REAL, ::g_tcp_protocol);
  sharedFlow flow2 = generateFlow(true, 2, 1, ::g_REAL, ::g_tcp_protocol);
  size_t     flow1size = sizeof(*flow1.get());
  size_t     flow2size = sizeof(*flow2.get());

  BOOST_CHECK_EQUAL(flow1size, 0);
  BOOST_CHECK_EQUAL(flow2size, 0);

  // Condition:
  flow1->merge(flow2);
  flow1size = sizeof(*flow1.get());
  BOOST_CHECK_EQUAL(flow1size, 0);

  flow1->zero();
  flow1size = sizeof(*flow1.get());
  BOOST_CHECK_EQUAL(flow1size, 0);

  return;
}

BOOST_AUTO_TEST_SUITE_END()
