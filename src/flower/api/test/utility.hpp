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


#ifndef UTILITY_HPP
#define UTILITY_HPP

// System Includes
#include <iostream>
#include <locale>
#include <map>
#include <pcap.h>
#include <string>
#ifndef _MSC_VER
#include <syslog.h>
#endif
// External Includes
#include <boost/test/included/unit_test.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <Diagnostic.hpp>
// Internal Includes
// Application Includes
#include "global.hpp"
#include "uchar2string.hpp"
#include "EventTypes.hpp"
#include "Exception.hpp"
#include "Flow.hpp"
#include "PacketParser.hpp"
#include "FixedArray.hpp"


const int unsigned g_icmp_protocol =  0x1;
const int unsigned  g_tcp_protocol =  0x6;
const int unsigned  g_udp_protocol = 0x11;


// Namespaces
using namespace boost;


Diagnostic    g_diag(framework::master_test_suite().argc, framework::master_test_suite().argv, Exception::getErrorCodes());

int unsigned  g_timeout             = 120;

int unsigned  g_message_id          = 1;
int unsigned  g_time_step           = 0;
ptime         g_timestamp(second_clock::local_time());

bool          g_add_event_called    = false;
int unsigned  g_add_event_counter   = 0;
bool          g_remove_event_called = false;

int unsigned  g_zero                = 0;

sharedFlow    g_flow;
sharedPacket  g_packet;
pcap_t *      g_pcap_desc;

char unsigned g_good_data[] = {
                                0x11, 0x22, 0x33, 0x44, 0x55, 0x66,  // Destination MAC
                                0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,  // Source MAC
                                0x08, 0x00,                          // EtherType
                                0x45, 0x00, 0x00, 0x54, 0x00, 0x00,
                                0x40, 0x00, 0x40, 0x01, 0xD4, 0x4C,
                                0xF0, 0xF0, 0xF0, 0xF0, 0xF1, 0xF1,
                                0xF1, 0xF1, 0xF2, 0xFB, 0xFF, 0xFF
                              };

char unsigned g_incomplete_data[] = {
                                      0xBA, 0xBA, 0xBA, 0xBA, 0xBA
                                    };

// 240 240 240 240, 241 241 241 241

ByteArray             g_good_packet_data(g_good_data, 38);
vector<char unsigned> g_good_src_ip(16, 0);
vector<char unsigned> g_good_dst_ip(16, 0);

ObjectPool<Flow> *    g_flow_pool = NULL;


void resetTimestamp(void)
{
  ::g_timestamp = time_from_string("2001-09-09 01:46:40.000");
  ::g_time_step = 0;
  return;
}


string getEnsipDataHome ()
{
  string ensip_data_home;
  // This has to be char * because sudo will run as root and root
  // may not have ENSIP_HOME set. The HOME variable will be set to
  // the user running the command.
  char * env_ensip_home   = getenv("ENSIP_HOME");
  if (env_ensip_home)
  {
    ensip_data_home       = env_ensip_home;
  }
  else
  {
    ensip_data_home      += getenv("HOME");
    ensip_data_home      += "/dev/ensip";
  }
  ensip_data_home        += "/data/flower/";

  return(ensip_data_home);
}


string getEnsipDataHomeInput ()
{
  return(getEnsipDataHome() + "input/");
}


string getEnsipDataHomeOutput ()
{
  return(getEnsipDataHome() + "output/");
}


void utility_init()
{
  resetTimestamp();

  ::g_good_dst_ip.assign(16, 0xCC);
  ::g_good_src_ip.assign(16, 0xCC);

  // Load the good packet vector with the good data
  //for (int unsigned idx = 0; idx < sizeof(::g_good_data); ++idx)
  //{
  //  ::g_good_packet_data.push_back(::g_good_data[idx]);
  //}
  for (int unsigned idx = 0; idx < 10; ++idx)
  {
    ::g_good_dst_ip[idx] = 0x00;
    ::g_good_src_ip[idx] = 0x00;
  }
  int unsigned idx    = 26;
  ::g_good_src_ip[10] = 0xFF;
  ::g_good_src_ip[11] = 0xFF;
  ::g_good_src_ip[12] = ::g_good_data[idx++];
  ::g_good_src_ip[13] = ::g_good_data[idx++];
  ::g_good_src_ip[14] = ::g_good_data[idx++];
  ::g_good_src_ip[15] = ::g_good_data[idx++];
  ::g_good_dst_ip[10] = 0xFF;
  ::g_good_dst_ip[11] = 0xFF;
  ::g_good_dst_ip[12] = ::g_good_data[idx++];
  ::g_good_dst_ip[13] = ::g_good_data[idx++];
  ::g_good_dst_ip[14] = ::g_good_data[idx++];
  ::g_good_dst_ip[15] = ::g_good_data[idx++];

  return;
}


void testAddEvent()
{
  BOOST_TEST_MESSAGE("in testOnAddEvent()");
  ::g_add_event_called = true;
  ++::g_add_event_counter;
  return;
}


void testOnAddEvent(pcap_t * p_new_pcap_desc)
{
  ::g_pcap_desc = p_new_pcap_desc;
  testAddEvent();
  return;
}


void testOnAddEvent(sharedPacket p_packet)
{
  ::g_packet = p_packet;
  testAddEvent();
  return;
}


void testOnAddEvent(sharedFlow p_flow)
{
  if (! p_flow->isFake())
  {
    ::g_flow = p_flow;
  }
  testAddEvent();
  return;
}


void testOnRemoveEvent(int unsigned p_id)
{
  stringstream ss("");
  ss << p_id;
  ::g_remove_event_called = true;
  string message = "in testOnRemoveEvent(): id = " + ss.str();
  BOOST_TEST_MESSAGE(message);
  return;
}


int unsigned howManyEventCalls(int unsigned & p_event_counter)
{
  int unsigned result = p_event_counter;
  p_event_counter     = 0;
  return(result);
}


bool wasEventCalled(bool & p_event_called)
{
  // If the Event was called, reset the global variable and indicate
  // that the event was called.
  bool reset = false;
  if (p_event_called == true)
  {
    p_event_called = false;
    reset          = true;
  }
  return(reset);
}


sharedFlow generateFlow(bool const p_alloc, ptime const p_packet_time, int unsigned const p_key = 0, bool const p_flow_type = ::g_REAL, int unsigned const p_protocol = ::g_udp_protocol)
{
  int unsigned                 p_vlan_id(0x2222);
  IpAddress_t                  p_src_ip_address; 
  IpAddress_t                  p_dst_ip_address; 
  IpAddress_t                  p_fix_ip_address;
  int unsigned                 p_ip_protocol(p_protocol);
  int unsigned                 p_ip_ident(0);
  int unsigned                 p_payload(0);
  int unsigned                 p_total_bytes(128);
  int unsigned                 p_ip_opt(0);
  int unsigned                 p_src_port(p_key);
  int unsigned                 p_dst_port(0x3333);
  int unsigned                 p_tcp_opt(0);
  int unsigned                 p_fragment(0);
  bool                         p_is_icmp(p_protocol == ::g_icmp_protocol ? 1 : 0);
  bool                         p_is_tcp(p_protocol  ==  ::g_tcp_protocol ? 1 : 0);
  bool                         p_is_ip_fragment(0);
  bool                         p_is_first_ip_frag(0);
  bool                         p_is_last_ip_frag(0);
  bool                         p_is_ipv4(1);
  u_int16_t                    p_flag_sum((p_protocol == ::g_icmp_protocol || p_protocol  ==  ::g_tcp_protocol) ? 48 : 0);
  int unsigned                 p_tcp_seq(p_protocol  ==  ::g_tcp_protocol ? 888888888 : 0);
  int unsigned                 p_fix_num(9999);
  FlowKey                   flow_key;

  // Load the good packet vector with the good data
  for (int unsigned idx = 0; idx < 16; ++idx)
  {
    p_src_ip_address.at(idx) = ::g_good_src_ip[idx];
    p_dst_ip_address.at(idx) = ::g_good_dst_ip[idx];
  }

  bool                         src_first = (p_src_ip_address > p_dst_ip_address);
  time_duration                diff      = p_packet_time - getEpochUTC();

  flow_key.buildFlowKeys(src_first, (short unsigned)p_src_port, (short unsigned)p_dst_port, (short unsigned)p_ip_ident, (short unsigned)p_vlan_id, (char unsigned)p_ip_protocol, p_src_ip_address, p_dst_ip_address);

  sharedFlow flow;
  if (p_alloc)
  {
    flow = sharedFlow(new Flow(::g_REAL, flow_key));
  }
  else
  {
    BOOST_CHECK_EQUAL(::g_flow_pool->acquire(flow), true);
  }

  flow->assignFields(diff.total_seconds(),
                     getEpoch(),
                     p_vlan_id,
                     p_ip_protocol,
                     p_src_ip_address,
                     p_dst_ip_address,
                     p_payload,
                     p_total_bytes,
                     1,
                     p_ip_opt,
                     p_src_port,
                     p_dst_port,
                     p_tcp_opt,
                     p_is_tcp,
                     p_flag_sum,
                     p_is_icmp,
                     p_tcp_seq,
                     0,
                     false,
                     0,
                     p_fix_ip_address,
                     p_fix_ip_address,
                     p_fix_num,
                     0,
                     0,
                     p_fragment,
                     0
                    );

  flow->assign(p_flow_type, flow_key, diff.total_seconds(), getCacheTimeout(), getCacheForceout(), src_first, p_is_tcp, p_is_icmp, p_is_ip_fragment, p_is_first_ip_frag, p_is_last_ip_frag, p_is_ipv4);

  return(flow);
}


sharedFlow generateFlow(bool const p_alloc, time_t const p_packet_time, int unsigned const p_key = 0, bool const p_flow_type = ::g_REAL, int unsigned const p_protocol = ::g_udp_protocol)
{
  ptime p_time = from_time_t(p_packet_time);
  return(generateFlow(p_alloc, p_time, p_key, p_flow_type, p_protocol));
}


int unsigned generateMessageId()
{
  // Just a generic unique message id generator
  return(++::g_message_id);
}


time_t generateTimestamp(int unsigned const p_idx = 0)
{
  ptime now;
  if ( p_idx == 0 )
  {
    // Just a generic unique ptime generator
    now = ::g_timestamp.operator+(seconds(++::g_time_step));
  }
  else
  {
    // Just a specific non-unique ptime getter
    now = ::g_timestamp.operator+(seconds(p_idx));
  }
  time_duration diff = now - getEpochUTC();
  return(diff.total_seconds());
}


#endif
