///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


// System Includes
#include <iostream>
#include <sstream>
// External Includes
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
// Internal Includes
// Application Includes
#include "global.hpp"
#include "osUtil.hpp"
#include "uchar2string.hpp"
#include "PacketParser.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "PacketParser"


// Namespaces used
using namespace std;
using namespace boost;
using boost::lexical_cast;
using boost::bad_lexical_cast;

static ByteArray fake_data(NULL,0);

//==============================================================================
// Class PacketParser
//
// API Developer Documentation:   (those who develop/fix this class)
//   [put documentation here for developers that work on this code]
//------------------------------------------------------------------------------
// API User Documentation:  (those who use this class)
//
//   [put a single summary sentence describing this class here]
//
// SYNOPSIS:
//   [put an overview paragraph for this class here]
//
// DESCRIPTION:
//   [put a description section for this class here]
//
// FUTURE FEATURES:
//  None discovered
//
// KNOWN LIMITIATIONS:
//  None discovered
//
// SEE ALSO:
//==============================================================================


// DEVELOPER NOTE: In general Anomalies affect Payload calculations and further
//                 processing of headers. If an anamoly is detected, the algorithm
//                 is to stop and calculate payload from the bottom of the previous
//                 valid header. For example, If there is a malformed Teredo
//                 header, then the payload would be calculated as everything
//                 following the valid UDP header and treat the malformed Teredo
//                 header as payload of the UDP packet.


// ======================================================================
// Constructors
// ======================================================================


PacketParser::PacketParser(
                           FlowAddEvent * const p_flow_add_event,
                           ObjectPool<Flow> &   p_flow_pool,
                           int unsigned const   p_cache_timeout,
                           int unsigned const   p_cache_forceout,
                           bool const           p_skip_ipv4_packets
                          ) noexcept(true) :
  fake_flow(new Flow(::g_FAKE)),
  data(fake_data),
  flow_add_event(p_flow_add_event),
  flow_pool(p_flow_pool),
  cache_timeout(p_cache_timeout),
  cache_forceout(p_cache_forceout),
  eth_bad_bytes_count(0),
  eth_bad_packet_count(0),
  ipv4_bad_bytes_count(0),
  ipv4_bad_packet_count(0),
  ipv4_good_bytes_count(0),
  ipv4_good_packet_count(0),
  ipv6_bad_bytes_count(0),
  ipv6_bad_packet_count(0),
  ipv6_good_bytes_count(0),
  ipv6_good_packet_count(0),
  total_bytes_received(0),
  total_packets_received(0),
  skip_ipv4_packets(p_skip_ipv4_packets),
  vlan_id(0)
{
  DEBUG(TRACE, ENTER);
  DEBUG(TRACE, LEAVE);
  return;
}


bool PacketParser::processIcmp(void) noexcept(true)
{
  static int unsigned const min_icmp_hdr_bytes = 8;

  setIcmp(true);

  if (isEnoughData(getProtoHdrOffset() + min_icmp_hdr_bytes))
  {
    setFlagSum(getData().at2(getProtoHdrOffset()));

    if ((! isIpFrag()) || isFirstIpFrag())
    {
      calcPayload(min_icmp_hdr_bytes);
      return(true);
    }
    return(true);
  }

  CAUTION(IllegalValue, "PacketParser OnAddEvent", "ICMP header length (" + uitoa10(getData().size() - getProtoHdrOffset()) + ") is less than the minumum ICMP header size (" + uitoa10(min_icmp_hdr_bytes) + ")");
  // TotalBytes & Payload are set in previous IPv6 or IPv4 header
  addAnomaly(anomaly_icmp_malformed_hdr);
  return(false);
}


bool PacketParser::processTcpOptions(int unsigned p_tcp_hdr_len, int unsigned p_offset_to_options) noexcept(true)
{
  // DEVELOPER NOTE: Return true if at least one recognized option was parsed, false otherwise
  bool status = false;

  if (p_tcp_hdr_len < p_offset_to_options)
  {
// TODO: 2010-04-19: Change this to EERROR or CAUTION
cout << "ERROR_MSG: bad args for tcp options" << endl;
    return(false);
  }

  int unsigned const opt_length = p_tcp_hdr_len - p_offset_to_options;

  int unsigned current_option_offset = getProtoHdrOffset() + p_offset_to_options;

  int unsigned const max_option_offset = getProtoHdrOffset() + p_offset_to_options + opt_length;

  if (! isEnoughData(max_option_offset))
  {
// TODO: 2010-04-19: Change this to EERROR or CAUTION
cout << "ERROR_MSG: not enough data to parse options" << endl;
    return(false);
  }

  while (current_option_offset < max_option_offset)
  {
    // read 1 byte option type
    char unsigned option = getData().at(current_option_offset);

    // end of option list
    if (0 == option)
    {
      current_option_offset++;
      status = true;
      break;
    }

    // NOP (padding)
    if (1 == option)
    {
      current_option_offset++;
      status = true;
      continue;
    }

    //
    // parse the options with data
    //

    // read and validate the option length
    char unsigned current_option_length = getData().at(current_option_offset+1);

    // TCP option length == 0, that's an anomaly...
    if (0 == current_option_length)
    {
      CAUTION(IllegalValue, "PacketParser processTcpOptions", "TCP option length (0) expected to have a value greater than 0");
      addAnomaly(anomaly_tcp_malformed_hdr);
      status = false;
      break;
    }

    //if (current_option_offset + current_option_length > max_option_offset)
    if (! isEnoughData(current_option_offset + current_option_length))
    {
// TODO: 2010-04-19: Change this to EERROR or CAUTION
cout << "BAD OPTION LENGTH: " << current_option_length << endl;
      status = false;
      break;
    }


/*
    // DEVELOPER NOTE: If other options are used in the future,
    //                 read RFC 1323 and use getFlagSum() to get TCP flags to validate
    //                 that the flags are set correctly for the options being used.
    
    // maximum segment size
    if (2 == option && 4 == current_option_length)
    {
      //cout << "Max segment size" << endl;
      current_option_offset += current_option_length;
      status = true;
      continue;
    }

    // window scaling
    if (3 == option && 3 == current_option_length)
    {
      //cout << "Window scaling" << endl;
      current_option_offset += current_option_length;
      status = true;
      continue;
    }

    // selective acknowledgement permitted
    if (4 == option && 2 == current_option_length)
    {
      //cout << "Selective acknowledgement permitted" << endl;
      current_option_offset += current_option_length;
      status = true;
      continue;
    }

    // selective acknowledgement (SACK)
    if (5 == option)
    {
      //cout << "Selective acknowledgement (SACK)" << endl;
      current_option_offset += current_option_length;
      status = true;
      continue;
    }
*/
    // Timestamp option RFC 1323 (PAWS) 
    if (8 == option && 10 == current_option_length)
    {
      setTcpOptionTsv(getData().at4(current_option_offset+2));
      //setTcpOptionTse(getData().at4(current_option_offset+6));
      //cout << "TSV: " << getTcpOptionTsv() << "   TSE: " << getTcpOptionTse()  << endl;
      current_option_offset += current_option_length;
      status = true;
      continue;
    }
/*
    // Alternate checksum request
    if (14 == option && 3 == current_option_length)
    {
      //cout << "Alternate checksum request" << endl;
      current_option_offset += current_option_length;
      status = true;
      continue;
    }

    // Alternate checksum data
    if (15 == option)
    {
      //cout << "Alternate checksum data" << endl;
      current_option_offset += current_option_length;
      status = true;
      continue;
    }

    // unrecognized option
    //cout << "Unrecognized option: " << option << "length: " << current_option_length << endl;
*/
    current_option_offset += current_option_length;
  }

  // TODO: return parsing result
  return(status);
}


bool PacketParser::processTcp(void) noexcept(true)
{
  int unsigned tcp_hdr_len                   =  0;
  int unsigned const min_tcp_hdr_bytes       = 20;
  int unsigned const min_tcp_hdr_size        =  5;
  int unsigned const tcp_hdr_offset_offset   = 12;
  int unsigned const tcp_hdr_flags_offset    = 13;
  int unsigned const tcp_hdr_seq_offset      =  4;
  int unsigned const tcp_hdr_dst_port_offset =  2;

  if (isEnoughData(getProtoHdrOffset() + min_tcp_hdr_bytes))
  {
    setTcp(true);
    setTcpSeq( getData().at4(getProtoHdrOffset() + tcp_hdr_seq_offset));
    setDstPort(getData().at2(getProtoHdrOffset() + tcp_hdr_dst_port_offset));
    setSrcPort(getData().at2(getProtoHdrOffset()));

    tcp_hdr_len = getData().at(getProtoHdrOffset() + tcp_hdr_offset_offset) >> 4;

    if ((! isIpFrag()) || isFirstIpFrag())
    {
      setFlagSum(getData().at(getProtoHdrOffset() + tcp_hdr_flags_offset));
      calcPayload(tcp_hdr_len * 4);
    }

    if (min_tcp_hdr_size == tcp_hdr_len)
    {
      setTcpOpt(0);
      return(true);
    }

    // DEVELOPER NOTE: We don't need to check the maximum (15) because the
    //                 value is only stored in 4 bits in the tcp header so
    //                 it can't be larger than 15
    if (min_tcp_hdr_size  < tcp_hdr_len)
    {
      // convert to byte lengths
      processTcpOptions(tcp_hdr_len<<2, min_tcp_hdr_size<<2);
      setTcpOpt(1);
      return(true);
    }
  }

  CAUTION(IllegalValue, "PacketParser OnAddEvent", "TCP header length (" + uitoa10(tcp_hdr_len) + ") is less than the minumum TCP header size (" + uitoa10(min_tcp_hdr_size) + ")");
  // TotalBytes & Payload are set in previous IPv6 or IPv4 header
  addAnomaly(anomaly_tcp_malformed_hdr);
  return(false);
}


bool PacketParser::processUdp(void) noexcept(true)
{
  int unsigned const min_udp_hdr_bytes = 8;

  if (isEnoughData(getProtoHdrOffset() + min_udp_hdr_bytes))
  {
    setUdp(true);
    if ((! isIpFrag()) || isFirstIpFrag())
    {
      // Not fragmented or first fragment
      setDstPort(getData().at2(getProtoHdrOffset() + 2));
      setSrcPort(getData().at2(getProtoHdrOffset()));

      calcPayload(min_udp_hdr_bytes);
      return(true);
    }
    return(true);
  }

  CAUTION(IllegalValue, "PacketParser OnAddEvent", "UDP header length (" + uitoa10(getData().size() - getProtoHdrOffset()) + ") is less than the minumum UDP header size (" + uitoa10(min_udp_hdr_bytes) + ")");
  // TotalBytes & Payload are set in previous IPv6 or IPv4 header
  addAnomaly(anomaly_udp_malformed_hdr);
  return(false);
}


int unsigned PacketParser::parseTeredoHeader(int unsigned const p_offset) noexcept(true)
{
  //  http://tools.ietf.org/html/rfc4380

  int unsigned const ipv6_hdr_size                    = 40;
  int unsigned const teredo_origin_encapsulation_size = 8;
  int unsigned       offset                           = p_offset;

  //  If this function is being called, we know that p_offset points to a UDP
  //  payload on the teredo port

  //  Make sure there is enough data for an IPv6 header
  if (isEnoughData(offset + ipv6_hdr_size))
  {   
    //            We are in one of these locations
    //               |                |        |
    //              \|/              \|/      \|/
    //  +------+-----+----------------+--------+-------------+
    //  | IPv4 | UDP | Authentication | Origin | IPv6 packet |
    //  +------+-----+----------------+--------+-------------+

    //  Look for the Authentication first
    if ((0 == getData().at(offset)) && (0x01 == getData().at(offset + 1)))
    {
      //   Authentication Encapsulation (optional) always first if both are present
      //  +--------+--------+--------+--------+
      //  |  0x00  | 0x01   | ID-len | AU-len |
      //  +--------+--------+--------+--------+
      //  |  Client identifier (ID-len        |
      //  +-----------------+-----------------+
      //  |  octets)        |  Authentication |
      //  +-----------------+--------+--------+
      //  | value (AU-len octets)    | Nonce  |
      //  +--------------------------+--------+
      //  | value (8 octets)                  |
      //  +--------------------------+--------+
      //  |                          | Conf.  |
      //  +--------------------------+--------+

      //  Calculate the length of the authentication encapsulation
      int unsigned auth_size = 4 + getData().at(offset + 2) + getData().at(offset + 3) + 8 + 1;

      //  Check the data length again
      if (isEnoughData(offset + auth_size))
      {
        offset += auth_size;
      }
      else
      {
        CAUTION(IllegalValue, "PacketParser parseTeredoHeader", "Expected an Authentication Encapsulation " + uitoa10(auth_size) + " bytes long but there is not enough data available (" + uitoa10(getData().size() - offset) + ")");
        // TotalBytes & Payload are set in previous UDP header
        addAnomaly(anomaly_teredo_auth_malformed_hdr);
        return(0);
      }
    }

    //  Make sure we have enough data to parse the Origin header or IPv6 Header
    if (isEnoughData(offset + ipv6_hdr_size))
    {
      // Look for the location
      if (0x0000 == getData().at2(offset))
      {
        // Origin Indication Encapsulation (optional) always second if both are present
        //+--------+--------+-----------------+
        //|  0x00  | 0x00   | Origin port #   |
        //+--------+--------+-----------------+
        //|  Origin IPv4 address              |
        //+-----------------------------------+

        offset += teredo_origin_encapsulation_size;
      }

      // Do we still have enough data to hold an IPv6 header?
      if (isEnoughData(offset + ipv6_hdr_size))
      {
        int unsigned const ipv6 = 6;
        int unsigned version    = getData().at(offset) >> 4;
        // Make sure it's IPv6
        if (ipv6 == version)
        {
          // Simple Encapsulation
          //+------+-----+-------------+
          //| IPv4 | UDP | IPv6 packet |
          //+------+-----+-------------+
 
          //return(processIpv6Hdr(offset));
          return(offset);
        }
   
        CAUTION(IllegalValue, "PacketParser parseTeredoHeader", "Expected IPv6 Header with version 6 but found value = " + uitoa10(version));
        // TotalBytes & Payload are set in previous UDP header
        addAnomaly(anomaly_teredo_missing_ipv6_hdr);
        return(0);
      }
    }
  }

  CAUTION(IllegalValue, "PacketParser parseTeredoHeader", "Expected IPv6 Header but there is not enough data for a valid header (" + uitoa10(getData().size() - offset) + ")");
  // TotalBytes & Payload are set in previous UDP header
  addAnomaly(anomaly_teredo_malformed_hdr);
  return(0);
}


void PacketParser::setTunnelVars(void) noexcept(true)
{
  incrTunnelDepth();

  if (isTunnel())
  {
    // DEVELOPER NOTE: We only care about the outermost tunnel
    return;
  }

  setTunnel(true);

  this->tunnel_src_ip_address = this->src_ip_address;
  this->tunnel_dst_ip_address = this->dst_ip_address;
  setTunnelIpProtocol(getIpProtocol());
  setTunnelSrcPort(getSrcPort());
  setTunnelDstPort(getDstPort());

  // DEVELOPER NOTE: Need to reset all the variables so that the
  //                 tunnel data and conversation data don't get
  //                 accidentally mixed.
  resetVars();

  return;
}


void PacketParser::processIpv4Frag(int unsigned const p_ip_hdr_offset) noexcept(true)
{
  static int unsigned const ip_hdr_ident_offset = 4;
  static int unsigned const ip_hdr_frag_offset  = 6;

  // DEVELOPER NOTE: This method is only called from processIpv4Hdr which
  //                 has already checked to see if we have enough data to
  //                 safely read. If that ever changes you should uncomment
  //                 the lines below to check to make sure you have enough
  //                 data in the buffer to read before calling getData().at(...)
  //static int unsigned const min_data_len        = 8;  // ip_hdr_frag_offset + 2 bytes
    
  //if (isEnoughData(p_ip_hdr_offset + min_data_len))
  //{
    int unsigned flags       =   (getData().at( p_ip_hdr_offset + ip_hdr_frag_offset) >> 5);
    int unsigned frag_offset = (((getData().at2(p_ip_hdr_offset + ip_hdr_frag_offset)) & 0x1FFF) << 3);

    setIpFrag((1 == (flags & 0x1)) || (0 != frag_offset));
    if (isIpFrag())
    {
      setIpIdent(getData().at2(p_ip_hdr_offset + ip_hdr_ident_offset));
      setFirstIpFrag(isIpFrag() && ((1 == (flags & 0x1)) && (0 == frag_offset)));
      setLastIpFrag( isIpFrag() && ((0 == (flags & 0x1)) && (0 != frag_offset)));
    }
  //}
   
  return;
}


bool PacketParser::processIpv4Hdr(int unsigned const p_ip_hdr_offset) noexcept(true)
{
  DEBUG(TRACE, ENTER);

  u_int32_t const min_internet_hdr_len = 5;                        // That is five 4-byte lines
  u_int32_t const min_ip_hdr_bytes     = min_internet_hdr_len * 4; // 20 Bytes

  if (isEnoughData(p_ip_hdr_offset + min_ip_hdr_bytes))
  {
    u_int32_t ip_hdr_len  = (getData().at(p_ip_hdr_offset) & 0x0F);
    if (min_internet_hdr_len > ip_hdr_len)
    {
      // No need to continue because there is no more data
      string message = "packet bytes=" + uitoa10(getData().size()) + " (" + uitoa10(p_ip_hdr_offset + min_ip_hdr_bytes) + " bytes required to parse successfully)";
      EERROR(e_error_pp_short_v4_packet, BadData, "bad IPv4 packet header", message.c_str());
      dropIpv4Packet();
      return(false);
    }

    //  Promote the flow to IPv4
    setIpVersion(4);
    setDstIpAddress(p_ip_hdr_offset);
    setSrcIpAddress(p_ip_hdr_offset);

    u_int32_t const ip_protocol_offset           = 9;
    setIpProtocol(getData().at(p_ip_hdr_offset + ip_protocol_offset));

    u_int32_t const ip_protocol_total_len_offset = 2;
    setTotalBytes(p_ip_hdr_offset + getData().at2(p_ip_hdr_offset + ip_protocol_total_len_offset), __LINE__);
    setProtoHdrOffset(p_ip_hdr_offset + (ip_hdr_len * 4));
    calcPayload(0);

    processIpv4Frag(p_ip_hdr_offset);

    // DEVELOPER NOTE: Question? Do we have enough data to report in a Flow?
    //                 2009-11-12: Current answer: Yes but with anomaly

    if (processTunnel(getProtoHdrOffset()))
    {
      // DEVELOPER NOTE: No need to process other information if this
      //                 is a tunnel so check for a tunnel first and
      //                 use recursion to set the other information if
      //                 neccessary.
      return(true);
    }

    // Only process IPv4 packets if processIpv4Packets is true or if we are
    // in a Tunneled Flow.
    //       IPv4 only    - no output
    //       IPv4 in IPv4 - no output
    //       IPv6 in IPv4 - output
    //       IPv6 only    - output
    if (skipIpv4Packets() && (! isTunnel()))
    {
      dropIpv4Packet();
      return(false);
    }

    // DEVELOPER NOTE: Reference: Internet Protocol, DARPA Internet Program
    //                 Protocol Specification, September 1981. On page 31, read
    //                 the "Options" section for more info about how the internet
    //                 header length determines the end of the option field. An
    //                 example of a packet with options set is given on page 38,
    //                 example 3, figure 9.
    if (min_internet_hdr_len == ip_hdr_len)
    {
      setIpOpt(0);
    }
    else if (min_internet_hdr_len < ip_hdr_len)
    {
      setIpOpt(1);
    }
        
    calcPayload(0);
    processProtocolLayer4();
    keepIpv4Packet();

    DEBUG(TRACE, LEAVE);
    return(true);
  }

  if (isTunnel())
  {
    CAUTION(IllegalValue, "PacketParser OnAddEvent", "There is not enough data to hold the minimim IPv4 header");
    // TotalBytes & Payload calculated in previous IPv4 or IPv6 or other header
    addAnomaly(anomaly_ipv4_malformed_hdr);
    return(false);
  }

  dropIpv4Packet();
  string message = "packet bytes=" + uitoa10(getData().size()) + " (" + uitoa10(p_ip_hdr_offset + min_ip_hdr_bytes) + " bytes required to parse successfully)";
  EERROR(e_error_pp_short_v4_packet, BadData, "bad IPv4 packet dropped", message.c_str());
  DEBUG(TRACE, LEAVE);
  return(false);
}


void PacketParser::processIpv6FragHdr(int unsigned const p_offset) noexcept(true)
{
  // IPv6 Fragment header
  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //|  Next Header  |   Reserved    |      Fragment Offset    |Res|M|
  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //|                         Identification                        |
  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //
  // M flag  ==>  1 = more fragments; 0 = last fragment.

  // DEVELOPER NOTE: This method is only called from processIpv4Hdr which
  //                 has already checked to see if we have enough data to
  //                 safely read. If that ever changes you should uncomment
  //                 the lines below to check to make sure you have enough
  //                 data in the buffer to read before calling getData().at(...)
  //static int unsigned const min_data_len        = 8;

  //if (isEnoughData(p_ip_hdr_offset + min_data_len))
  //{
    setIpFrag(true);
    u_int16_t frag_info = getData().at2(p_offset + 2);
    //  If the fragment offset  is zero, this is the first fragment
    if (0 == (frag_info >> 3))
    {
      setFirstIpFrag(true);
    }
    else
    {
      // Check the M flag
      if (0 == (frag_info & 0x1))
      {
        setLastIpFrag(true);
      }
    }
  
    setIpIdent(getData().at4(p_offset + 4));
  //}

  return;
}


bool PacketParser::processIpv6ExtensionHdrs(int unsigned & p_extension_hdr_offset, u_int8_t & p_next_hdr_type) noexcept(true)
{
  // DEVELOPER NOTE: see http://tools.ietf.org/html/rfc2460 ==> IPv6 in general
  //                     http://tools.ietf.org/html/rfc4302 ==> Auth Headers
  //                     http://tools.ietf.org/html/rfc4303 ==> ESP
  //                     1 qword = 8 bytes (64 bits)

  u_int8_t const ipv6_hopopt  =  0; // HOPOPT (Hop-by-Hop Opt) Len = qword count not including the first qword
  u_int8_t const ipv6_routing = 43; // Routing                 Len = qword count not including the first qword
  u_int8_t const ipv6_frag    = 44; // Fragment                Len = 1 qword
  //u_int8_t const ipv6_esp     = 50; // ESP (Encap Sec Payload) Len = 0 (data is encrypted, so stop processsing headers)
  u_int8_t const ipv6_auth    = 51; // AH (Auth Header)        Len = dword count not includeng the first 2 dwords
  u_int8_t const ipv6_dest_op = 60; // Destination Options     Len = qword count not including the first qword
  u_int8_t const ipv6_no_next = 59; // No Next Header          Len = 1 qword

  int unsigned next_hdr_type  =  0;
  int unsigned next_hdr_len   =  0;
  bool hop_opt_found          = false;
  bool dest_opt1_found        = false;
  bool dest_opt2_found        = false;
  bool routing_opt_found      = false;
  bool frag_opt_found         = false;
  bool auth_opt_found         = false;
  int unsigned offset         = p_extension_hdr_offset;
  u_int8_t current_hdr_type   = p_next_hdr_type;
  bool seen_first_hdr         = false;

  while (true)
  {
    //  Be sure to check length each time
    //  IPv6 requires that extension headers be 8 bytes minimum
    if (isEnoughData(offset + 8))
    {
      if (ipv6_hopopt == current_hdr_type)
      {
        setIpOpt(1);

        if (seen_first_hdr)
        {
          // DEVELOPER NOTE: HOPOPT should only exist as the first option
          addAnomaly(anomaly_ipv6_out_of_order_hop_opt);
        }

        if (hop_opt_found)
        {
          // DEVELOPER NOTE: There should only be one hop opt header
          addAnomaly(anomaly_ipv6_mult_hop_hdr);
        }

        // DEVELOPER NOTE: 2009/11/24 - We changed the way Payload and
        //                              TotalBytes work so we don't need
        //                              to process Jumbograms anymore.
        next_hdr_type    = getData().at(offset);
        next_hdr_len     = getData().at(offset + 1);
        offset          += (next_hdr_len + 1) * 8;
        current_hdr_type = next_hdr_type;
        seen_first_hdr   = true;
        hop_opt_found    = true;
        continue;
      }

      if (ipv6_dest_op == current_hdr_type)
      {
        setIpOpt(1);

        if (dest_opt2_found)
        {
          // DEVELOPER NOTE: There should be at most two destination option headers
          addAnomaly(anomaly_ipv6_mult_dest_hdr);
        }

        if (dest_opt1_found)
        {
          dest_opt2_found = true;
        }

        next_hdr_type    = getData().at(offset);
        next_hdr_len     = getData().at(offset + 1);
        offset          += (next_hdr_len + 1) * 8;
        current_hdr_type = next_hdr_type;
        seen_first_hdr   = true;
        dest_opt1_found  = true;
        continue;
      }

      if (ipv6_routing == current_hdr_type)
      {
        setIpOpt(1);

        if (routing_opt_found)
        {
          // DEVELOPER NOTE: There should only be one routing header
          addAnomaly(anomaly_ipv6_mult_routing_hdr);
        }

        next_hdr_type     = getData().at(offset);
        next_hdr_len      = getData().at(offset + 1);
        offset           += (next_hdr_len + 1) * 8;
        current_hdr_type  = next_hdr_type;
        seen_first_hdr    = true;
        routing_opt_found = true;
        continue;
      }

      if (ipv6_frag == current_hdr_type)
      {
        setIpOpt(1);

        if (frag_opt_found)
        {
          // DEVELOPER NOTE: There should only be one frag header
          addAnomaly(anomaly_ipv6_mult_frag_hdr);
          next_hdr_type    = getData().at(offset);
          next_hdr_len     = getData().at(offset + 1);
          offset          += 8;
          current_hdr_type = next_hdr_type;
          seen_first_hdr   = true;
          continue;
        }
        processIpv6FragHdr(offset);
        if (! isFirstIpFrag())
        {
          p_extension_hdr_offset = offset + 8;
          p_next_hdr_type        = current_hdr_type;
          // DEVELOPER NOTE: We have to quit parsing because we are not the
          //                 first fragment so we have to assume that the bytes
          //                 following this fragment extension header are data
          return(true);
        }
        next_hdr_type    = getData().at(offset);
        next_hdr_len     = getData().at(offset + 1);
        offset          += 8;
        current_hdr_type = next_hdr_type;
        seen_first_hdr   = true;
        frag_opt_found   = true;
        continue;
      }

      if (ipv6_auth == current_hdr_type)
      {
        setIpOpt(1);

        if (auth_opt_found)
        {
          // DEVELOPER NOTE: There should only be one auth header
          addAnomaly(anomaly_ipv6_mult_auth_hdr);
        }

        // DEVELOPER NOTE: For some reason the Authentication header length
        //                 is in multiples of 4 bytes instead of 8 like the
        //                 other IPv6 header extension lengths.
        next_hdr_type    = getData().at(offset);
        next_hdr_len     = getData().at(offset + 1);
        offset          += (next_hdr_len + 2) * 4;
        current_hdr_type = next_hdr_type;
        seen_first_hdr   = true;
        auth_opt_found   = true;
        continue;
      }

      //  No matching Ext Header so update arguments by reference and return
      p_extension_hdr_offset = offset;
      p_next_hdr_type        = current_hdr_type;
      return(true);
    }

    if (ipv6_no_next == current_hdr_type)
    {
      // DEVELOPER NOTE: This is specifically to handle things like Teredo
      //                 keep alive packets that send IPv6 headers with
      //                 no payload and have a next header set to no next
      //                 header (59)
      p_extension_hdr_offset = offset;
      p_next_hdr_type        = current_hdr_type;
      return(true);
    }

    return(true);
  }

  return(true);
}

   
bool PacketParser::processGreHdr(int unsigned const p_gre_hdr_offset) noexcept(true)
{
  //  Generic Routing Encapsulation http://tools.ietf.org/html/rfc2890
  //
  //   0                   1                   2                   3
  //   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //  |C| |K|S| Reserved0       | Ver |         Protocol Type         |
  //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //  |      Checksum (optional)      |       Reserved1 (Optional)    |
  //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //  |                         Key (optional)                        |
  //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //  |                 Sequence Number (Optional)                    |
  //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //

  //  PPTP version of GRE http://tools.ietf.org/html/rfc2637#section-4.1
  //
  //   0                   1                   2                   3
  //   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //  |C|R|K|S|s|Recur|A| Flags | Ver |         Protocol Type         |
  //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //  |    Key (HW) Payload Length    |       Key (LW) Call ID        |
  //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //  |                  Sequence Number (Optional)                   |
  //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //  |               Acknowledgment Number (Optional)                |
  //  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

  // The first line of the header in bytes
  int unsigned offset          = p_gre_hdr_offset + 4;
  int unsigned protocol_type   = 0;
  int unsigned const ipv4_type = 0x0800;
  int unsigned const ipv6_type = 0x86DD;

  if (isEnoughData(offset))
  {
    u_int16_t bits     = getData().at2(p_gre_hdr_offset);
    protocol_type      = getData().at2(p_gre_hdr_offset + 2);

    u_int16_t version  =  bits & 0x07;
    u_int16_t checksum = (bits & 0x8000) >> 15;
    u_int16_t key      = (bits & 0x2000) >> 13;
    u_int16_t sequence = (bits & 0x1000) >> 12;
    u_int16_t acknum   = (bits & 0x0080) >>  7;

    if (0 == version)
    {
      offset += checksum * 4;
      offset += key      * 4;
      offset += sequence * 4;
    }

    if (1 == version)
    {
      offset += key      * 4;
      offset += sequence * 4;
      offset += acknum   * 4;
    }
  }

  if (isEnoughData(offset))
  {
    if (ipv6_type == protocol_type)
    {
      setTunnelVars();
      return(processIpv6Hdr(offset));
    }

    if (ipv4_type == protocol_type)
    {
      setTunnelVars();
      return(processIpv4Hdr(offset));
    }

    addAnomaly(anomaly_gre_non_tunnel);
    return(false);  // DEVELOPER NOTE: We are not in a tunnel
  }
  
  addAnomaly(anomaly_gre_malformed_hdr);
  return(false);
}


bool PacketParser::processTunnel(int unsigned const p_offset) noexcept(true)
{
  u_int8_t const six_in_six = 41;
  if (six_in_six == getIpProtocol())
  {
    setTunnelVars();
    return(processIpv6Hdr(p_offset));
  }

  u_int8_t const ip_in_ip   =  4;
  if (ip_in_ip == getIpProtocol())
  {
    setTunnelVars();
    return(processIpv4Hdr(p_offset));
  }

  // DEVELOPER NOTE: GRE Tunnel
  u_int8_t const ip_in_gre  = 47;
  if (ip_in_gre == getIpProtocol())
  {
    // DEVELOPER NOTE: We have to setTunnelVars in processGreHdr
    return(processGreHdr(p_offset));
  }

  u_int8_t const udp  = 17;
  if (udp == getIpProtocol())
  {
    int unsigned const min_udp_hdr_bytes = 8;

    if (isEnoughData(p_offset + min_udp_hdr_bytes))
    {
      if ((! isIpFrag()) || isFirstIpFrag())
      {
        // Not fragmented or first fragment
        u_int16_t destination_port  = getData().at2(p_offset + 2);
        u_int16_t source_port       = getData().at2(p_offset);
        u_int16_t const teredo_port = 3544;
        u_int16_t const vxlan_port  = 4789;
  
        setSrcPort(source_port);
        setDstPort(destination_port);
        if (teredo_port == source_port || teredo_port == destination_port)
        {
          //setTunnelVars();
          //calcPayload(min_udp_hdr_bytes);
          //return(parseTeredoHeader(p_offset + min_udp_hdr_bytes));
          int unsigned new_offset = parseTeredoHeader(p_offset + min_udp_hdr_bytes);
          if (new_offset > 0)
          {
            setTunnelVars();
            calcPayload(min_udp_hdr_bytes);
            return(processIpv6Hdr(new_offset));
          }
        }

        if (vxlan_port == destination_port)
        {
          // DEVELOPER NOTE: We probably have a VXLAN tunnel like AWS
          // processEthernetLayer2()???
          // int unsigned new_offset = parseVxLanHeader(p_offset + min_udp_hdr_bytes);
          // if (new_offset > 0)
          // {
          //   setTunnelVars();
          //   calcPayload(min_udp_hdr_bytes);
          // }
        }
      }
    }
  }

  return(false);
}


bool PacketParser::processIpv6Hdr(int unsigned const p_ip_hdr_offset) noexcept(true)
{
  //  http://tools.ietf.org/html/rfc2460

  DEBUG(TRACE, ENTER);

  int unsigned const ipv6_hdr_size       = 40;
  int unsigned const ipv6_nxt_hdr_offset = 6;
  int unsigned offset                    = p_ip_hdr_offset + ipv6_hdr_size;       // the end of the IPv6 header
  int unsigned next_hdr_type_offset      = p_ip_hdr_offset + ipv6_nxt_hdr_offset;

  //  Make sure there is enough remaining data in the packet to parse
  if (isEnoughData(offset))
  {
    //  Promote the flow to IPv6
    setIpVersion(6);
    setDstIpAddress(p_ip_hdr_offset);
    setSrcIpAddress(p_ip_hdr_offset);

    u_int32_t const ip_protocol_payload_len_offset = 4;
    u_int16_t payload_len = getData().at2(p_ip_hdr_offset + ip_protocol_payload_len_offset);

    setProtoHdrOffset(offset);

    calcPayload(0);
    setTotalBytes(offset + payload_len, __LINE__);

    // DEVELOPER NOTE: payload_len should only be 0 if this is an IPv6 Jumbogram
    //                 which should never have Ethernet trailers so getData().size()
    //                 should always be correct.
    if (0 == payload_len)
    {
      setTotalBytes(getData().size(), __LINE__);
    }

    //  Get the next header from the IPv6 header
    u_int8_t next_hdr_type = getData().at(next_hdr_type_offset);
    setIpProtocol(next_hdr_type);

    processIpv6ExtensionHdrs(offset, next_hdr_type);

    setIpProtocol(next_hdr_type);
    setProtoHdrOffset(offset);

    if (processTunnel(getProtoHdrOffset()))
    {
      // DEVELOPER NOTE: No need to process other information if this
      //                 is a tunnel so check for a tunnel first and
      //                 use recursion to set the other information if
      //                 neccessary.
      return(true);
    }

    // DEVELOPER NOTE: We are not in a tunnel so keep processing

    calcPayload(0);
    
    processProtocolLayer4();

    keepIpv6Packet();
    DEBUG(TRACE, LEAVE);
    return(true);
  }

  dropIpv6Packet();
  string message = "packet bytes=" + uitoa10(getData().size()) + " (" + uitoa10(p_ip_hdr_offset + 40) + " bytes required to parse successfully)";
  EERROR(e_error_pp_short_v6_packet, BadData, "bad packet dropped", message.c_str());
  DEBUG(TRACE, LEAVE);
  return(false);
}


void PacketParser::createFlow(sharedPacket const & p_packet) noexcept(true)
{
  DEBUG(TRACE, ENTER);

  sharedFlow flow;

  FlowKey    flow_key;
  setSrcFirst(getSrcIpAddress() > getDstIpAddress());
  flow_key.buildFlowKeys(isSrcFirst(), getSrcPort(), getDstPort(), getIpIdent(), getVlanId(), getIpProtocol(), getSrcIpAddress(), getDstIpAddress());

//cout << "FlowKey:" << flow_key << ":" << endl;

  if (this->flow_pool.acquire(flow))
  {
    static int unsigned const           packet_count = 1;

    flow->assignFields(p_packet->getHeaderSeconds(),
                       p_packet->getHeaderMicroSeconds(),
                       getVlanId(),
                       getIpProtocol(),
                       getSrcIpAddress(),
                       getDstIpAddress(),
                       getPayload(),
                       getTotalBytes(),
                       packet_count,
                       getIpOpt(),
                       getSrcPort(),
                       getDstPort(),
                       getTcpOpt(),
                       isTcp(),
                       getFlagSum(),
                       isIcmp(),
                       getTcpSeq(),
                       getTcpOptionTsv(),
                       isTunnel(),
                       getTunnelDepth(),
                       getTunnelSrcIpAddress(),
                       getTunnelDstIpAddress(),
                       getTunnelIpProtocol(),
                       getTunnelSrcPort(),
                       getTunnelDstPort(),
                       isIpFrag(),
                       getAnomaly()
                      );

    flow->assign(::g_REAL, flow_key,
                 p_packet->getHeaderSeconds(),
                 getCacheTimeout(),
                 getCacheForceout(),
                 isSrcFirst(),
                 isTcp(),
                 isIcmp(),
                 isIpFrag(),
                 isFirstIpFrag(),
                 isLastIpFrag(),
                 isIpv4());

    getFlowAddEvent()->call(flow);
  }
  else
  {
    EERROR(e_error_pp_object_pool_empty, TSNH, "Get Flow from Object Pool", "Could not get a Flow from Object Pool");
    // DEVELOPER NOTE: Must update the time in the fake flow to advance the
    //                 virtual time in the FlowCache and downstream objects
    fake_flow->forceFake(p_packet->getHeaderSeconds());
    getFlowAddEvent()->call(this->fake_flow);
    if (isIpv4())
    {
      dropIpv4Packet();
      return;
    }
    //else    // Commented out for optimization
    //{
      dropIpv6Packet();
      //return;
    //}
  }

  DEBUG(TRACE, LEAVE);
  return;
}


bool PacketParser::dropEthernetHardwareHeaders(void) noexcept(true)
{
  if (isSpanningTreeProtocol() || isPvstProtocol())
  {
    // DEVELOPER NOTE: Silently drop Span Tree or Pvst Packets
    dropEthPacket();
    return(true);
  }
  return(false);
}


void PacketParser::skipEthernetHardwareHeaders(void) noexcept(true)
{
  // DEVELOPER NOTE: From http://www.cisco.com/en/US/tech/tk389/tk689/technologies_tech_note09186a0080094665.shtml#topic1
  if (isIslEncapsulation())
  {
    // DEVELOPER NOTE: There is a VLAN Id in the ISL but we do not extract it.
    incrEtherTypeOffset(26);
  }
  return;
}


bool PacketParser::processVlan(void) noexcept(true)
{
  int unsigned const vlan_type = 0x8100;

  while (vlan_type == getEtherType())
  {
    // DEVELOPER NOTE:  Only the last VLAN tag will be retrieved.
    if (updateVlanId() && updateEtherType())
    {
      continue;
    }

    // DEVELOPER NOTE: Should only get here if the packet is bad
    EERROR(e_error_pp_short_vlan_packet, RangeError, "PacketParser", "Not enough data to parse VLAN in Ethernet Frame. Size of packet is less than " + uitoa10(2 + getEtherTypeOffset()) + " (size = " + uitoa10(getData().size()) + ")");
    dropEthPacket();
    return(false);
  }

  return(true);
}


void PacketParser::processMpls(void) noexcept(true)
{
  int unsigned const mlps_type = 0x8847; // Multi Protocol Layered Switching
  int unsigned const ipv4_type = 0x0800;

  if (mlps_type == getEtherType())
  {
    setFirstSeenIpHdrOffset(getFirstSeenIpHdrOffset() + 4);
    // DEVELOPER NOTE: We have no idea what is next so assume IPv4 because
    //                 the caller will determine the actual IP version from
    //                 the IP header
    setEtherType(ipv4_type);
  }

  return;
}


bool PacketParser::processEthernetLayer2(void) noexcept(true)
{
  skipEthernetHardwareHeaders();

  if (dropEthernetHardwareHeaders())
  {
    return(false);
  }

  if (! updateEtherType())
  {
    // DEVELOPER NOTE: Not enough data to parse in Ethernet frame
    dropEthPacket();
    EERROR(e_error_pp_short_eth_packet, RangeError, "PacketParser", "Not enough data to parse Ethernet Frame. Size of packet is less than " + uitoa10(2 + getEtherTypeOffset()) + " (size = " + uitoa10(getData().size()) + ")");
    return(false);
  }

  if (! processVlan())
  {
    return(false);
  }

  processMpls();

  // DEVELOPER NOTE: Refer to the following for ether type values
  //                 http://www.iana.org/assignments/ethernet-numbers

  int unsigned const ipv4_type = 0x0800;
  int unsigned const ipv6_type = 0x86DD;

  if ((ipv4_type == getEtherType()) || (ipv6_type == getEtherType()))
  {
    if (isEnoughData(getFirstSeenIpHdrOffset()))
    {
      setIpVersion(getData().at(getFirstSeenIpHdrOffset()) >> 4);
      return(true);
    }
  }

  // DEVELOPER NOTE: Unsupported ethernet type (At least not IPv4 or IPv6)
  dropEthPacket();
  return(false);
}


bool PacketParser::processIpLayer3(void) noexcept(true)
{
  // DEVELOPER NOTE: processIpv* is responsible for dropping bad packets
  //                 and reporting an error when the packet cannot be parsed.
  if (isIpv4())
  {
    return(processIpv4Hdr(getFirstSeenIpHdrOffset()));
  }

  // DEVELOPER NOTE: The only way to get into processIpLayer3 is to find an
  //                 IPv4 or IPv6 packet so we can assume that we have IPv6
  //                 here.
  //if (isIpv6())
  //{
  //}
  return(processIpv6Hdr(getFirstSeenIpHdrOffset()));

  // DEVELOPER NOTE: In the future (your present) if you ever had to add
  //                 another protocol then you may have to drop a bad Ethernet
  //                 packet and return false.
  //dropEthPacket();
  //return(false);
}


bool PacketParser::processOther(void) noexcept(true)
{
  return(true);
}


bool PacketParser::processProtocolLayer4(void) noexcept(true)
{
  // DEVELOPER NOTE: The following code is faster than a switch/case statement

  u_int8_t protocol = getIpProtocol();

  if (e_tcp == protocol)
  {
    return(processTcp());
  }

  if (e_udp == protocol)
  {
    return(processUdp());
  }

  if ((e_icmp_v4 == protocol) || (e_icmp_v6 == protocol))
  {
    return(processIcmp());
  }

  // DEFAULT case
  return(true);    //return(processOther());
}


void PacketParser::resetTunnelVars(void) noexcept(true)
{
  setTunnelDepth(0);
  setTunnelIpProtocol(0);
  this->tunnel_src_ip_address.clear();
  this->tunnel_dst_ip_address.clear();
  setTunnelDstPort(0);
  setTunnelSrcPort(0);
  setTunnel(false);

  return;
}


void PacketParser::initVars(void) noexcept(true)
{
  DEBUG(TRACE, ENTER);

  // Layer 2
  setVlanId(0);
  setEtherTypeOffset(12);
  setEtherType(0);
  setIsVlan(false);

  // Layer 3
  setFirstSeenIpHdrOffset(0);
  
  // Layer 4
  setProtoHdrOffset(0);

  // Other
  setAnomaly(0);
  setSrcFirst(false);

  resetVars();
  resetTunnelVars();

  DEBUG(TRACE, LEAVE);
  return;
}


void PacketParser::resetVars(void) noexcept(true)
{
  DEBUG(TRACE, ENTER);

  // Layer 3
  setIpProtocol(0);
  this->src_ip_address.clear();
  this->dst_ip_address.clear();
  setPayload(0);
  //setTotalBytes(0);
  // NOTE: Packet Count is always 1
  setIpOpt(0);

  // Layer 4
  setDstPort(0);
  setSrcPort(0);
  setFlagSum(0);
  //this->flags.assign(1, "");
  setTcpOpt(0);
  setTcpSeq(0);
  setTcpOptionTsv(0);
  // NOTE: Frag type       is set when flushed from Cache
  // NOTE: Reverse Session is set when flushed from Cache

  // Control Variables that determine parsing rules
  setIcmp(false);
  setTcp(false);
  setUdp(false);

  setIpFrag(false);
  setLastIpFrag(false);
  setFirstIpFrag(false);
  setIpIdent(0); 
  setIpVersion(0);

  DEBUG(TRACE, LEAVE);
  return;
}


void PacketParser::onAddEvent(sharedPacket const p_packet) noexcept(true)
{
  DEBUG(TRACE, ENTER);

  const int unsigned min_valid_packet_size = 34;

  if (! p_packet->isFake())
  {
    setData(p_packet->getData());
    incrTotalPacketsReceived();
    incrTotalBytesReceived(getData().size());

    // DEVELOPER NOTE: At this point, since we are simply converting the bytes
    //                 to hex strings there is no such thing as 'bad data',
    //                 only 'not enough data'. We need a minimum of 14 bytes
    //                 for a minimum Ethernet header and another 20 bytes for
    //                 a minimum IPv4 header for a total of 34 bytes minimum
    if (isEnoughData(min_valid_packet_size))
    {
      initVars();
      setTotalBytes(getData().size(), __LINE__);

      // DEVELOPER NOTE: processEthernetLayer2 and processIpLayer3 are
      //                 responsible to drop the packet and report an
      //                 error if there is a problem with the packet and
      //                 we cannot create a flow from that packet.
      if (processEthernetLayer2())
      {
        if (processIpLayer3())
        {
          createFlow(p_packet);
        }
      }
      return;
    }
    dropEthPacket();
    EERROR(e_error_pp_short_eth_packet, RangeError, "PacketParser", "Size of packet is less than " + uitoa10(min_valid_packet_size) + " (size = " + uitoa10(p_packet->getData().size()) + ")");
    return;
  }

#ifndef NDEBUG
  CAUTION(FakeData, "PacketParser", "Creating FAKE FLOW");
#endif
  fake_flow->forceFake(p_packet->getHeaderSeconds());
  getFlowAddEvent()->call(this->fake_flow);

  DEBUG(TRACE, LEAVE);
  return;
}


string PacketParser::onMetricsEvent(int unsigned const p_level) noexcept(true)
{
  u_int64_t total_bytes_calc   = this->eth_bad_bytes_count  + this->ipv4_bad_bytes_count  + this->ipv6_bad_bytes_count  + this->ipv4_good_bytes_count  + this->ipv6_good_bytes_count;
  u_int64_t total_packets_calc = this->eth_bad_packet_count + this->ipv4_bad_packet_count + this->ipv6_bad_packet_count + this->ipv4_good_packet_count + this->ipv6_good_packet_count;

  if (total_bytes_calc != this->total_bytes_received)
  {
    ERROR_MSG(TSNH, "Total bytes received != Total bytes calculated",  (uitoa10(this->total_bytes_received) + " != " + uitoa10(total_bytes_calc)).c_str());
  }
  if (total_packets_calc != this->total_packets_received)
  {
    ERROR_MSG(TSNH, "Total packets received != Total packets calculated",  (uitoa10(this->total_packets_received) + " != " + uitoa10(total_packets_calc)).c_str());
  }

  output("      Bytes   received:   " + uitoa10(this->total_bytes_received));
  output("      Packets received:   " + uitoa10(this->total_packets_received));
  output("      Bad  Eth Bytes:     " + uitoa10(this->eth_bad_bytes_count));
  output("      Bad  Eth Pkts:      " + uitoa10(this->eth_bad_packet_count));
  output("      Bad  Ipv4 Bytes:    " + uitoa10(this->ipv4_bad_bytes_count));
  output("      Bad  Ipv4 Pkts:     " + uitoa10(this->ipv4_bad_packet_count));
  output("      Bad  Ipv6 Bytes:    " + uitoa10(this->ipv6_bad_bytes_count));
  output("      Bad  Ipv6 Pkts:     " + uitoa10(this->ipv6_bad_packet_count));
  output("      Good Ipv4 Bytes:    " + uitoa10(this->ipv4_good_bytes_count));
  output("      Good Ipv4 Pkts:     " + uitoa10(this->ipv4_good_packet_count));
  output("      Good Ipv6 Bytes:    " + uitoa10(this->ipv6_good_bytes_count));
  output("      Good Ipv6 Pkts:     " + uitoa10(this->ipv6_good_packet_count));

  string result = "PP";
  result += ":tbr#"   + uitoa10(this->total_bytes_received);   this->total_bytes_received   = 0;
  result += ":tpr#"   + uitoa10(this->total_packets_received); this->total_packets_received = 0;
  result += ":ebbc#"  + uitoa10(this->eth_bad_bytes_count);    this->eth_bad_bytes_count    = 0;
  result += ":ebpc#"  + uitoa10(this->eth_bad_packet_count);   this->eth_bad_packet_count   = 0;
  result += ":v4bbc#" + uitoa10(this->ipv4_bad_bytes_count);   this->ipv4_bad_bytes_count   = 0;
  result += ":v4bpc#" + uitoa10(this->ipv4_bad_packet_count);  this->ipv4_bad_packet_count  = 0;
  result += ":v6bbc#" + uitoa10(this->ipv6_bad_bytes_count);   this->ipv6_bad_bytes_count   = 0;
  result += ":v6bpc#" + uitoa10(this->ipv6_bad_packet_count);  this->ipv6_bad_packet_count  = 0;
  result += ":v4gbc#" + uitoa10(this->ipv4_good_bytes_count);  this->ipv4_good_bytes_count  = 0;
  result += ":v4gpc#" + uitoa10(this->ipv4_good_packet_count); this->ipv4_good_packet_count = 0;
  result += ":v6gbc#" + uitoa10(this->ipv6_good_bytes_count);  this->ipv6_good_bytes_count  = 0;
  result += ":v6gpc#" + uitoa10(this->ipv6_good_packet_count); this->ipv6_good_packet_count = 0;
  return(result);
}


void PacketParser::onShutdownSystemEvent(int unsigned const p_level) noexcept(true)
{
  DEBUG(TRACE, ENTER);
  DEBUG(TRACE, LEAVE);
  return;
}
