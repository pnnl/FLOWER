///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifndef PACKETPARSER_HPP
#define PACKETPARSER_HPP


// System Includes
#include <string>
// External Includes
// Internal Includes
// Application Includes
#include "EventTypes.hpp"
#include "ObjectPool.hpp"
#include "Packet.hpp"
#include "Stats.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "PacketParser"


// Namespaces used


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


class PacketParser
{

public:

  // Getters/Setters
  

  inline bool isEnoughData(int unsigned const p_check) const throw()
  {
    return(getData().size() >= p_check);
  }


  inline u_int64_t getTotalBytesReceived(void) const throw()
  {
    return(this->total_bytes_received);
  }


  inline u_int64_t getTotalPacketsReceived(void) const throw()
  {
    return(this->total_packets_received);
  }


  inline u_int64_t getTotalDroppedPacketCount(void) const throw()
  {
    return(getBadEthPacketCount() + getBadIpv4PacketCount() + getBadIpv6PacketCount());
  }


  inline u_int64_t getTotalDroppedBytes(void) const throw()
  {
    return(getBadEthByteCount() + getBadIpv4ByteCount() + getBadIpv6ByteCount());
  }


  inline u_int64_t getBadEthPacketCount(void) const throw()
  {
    return(this->eth_bad_packet_count);
  }


  inline u_int64_t getBadEthByteCount(void) const throw()
  {
    return(this->eth_bad_bytes_count);
  }


  inline u_int64_t getBadIpv4PacketCount(void) const throw()
  {
    return(this->ipv4_bad_packet_count);
  }


  inline u_int64_t getBadIpv4ByteCount(void) const throw()
  {
    return(this->ipv4_bad_bytes_count);
  }


  inline u_int64_t getBadIpv6PacketCount(void) const throw()
  {
    return(this->ipv6_bad_packet_count);
  }


  inline u_int64_t getBadIpv6ByteCount(void) const throw()
  {
    return(this->ipv6_bad_bytes_count);
  }


  inline u_int64_t getGoodIpv4PacketCount(void) const throw()
  {
    return(this->ipv4_good_packet_count);
  }


  inline u_int64_t getGoodIpv4ByteCount(void) const throw()
  {
    return(this->ipv4_good_bytes_count);
  }


  inline u_int64_t getGoodIpv6PacketCount(void) const throw()
  {
    return(this->ipv6_good_packet_count);
  }


  inline u_int64_t getGoodIpv6ByteCount(void) const throw()
  {
    return(this->ipv6_good_bytes_count);
  }


  // Constructors

  PacketParser(
               FlowAddEvent * const p_flow_add_event,
               ObjectPool<Flow> &   p_flow_pool,
               int unsigned const   p_cache_timeout,
               int unsigned const   p_cache_forceout,
               bool         const   p_use_vlan,
               bool const           p_skip_ipv4_packets
              ) throw();


  // Destructor

  ~PacketParser(void) throw()
  {
    return;
  }


  // Public Functions

  void   onAddEvent(sharedPacket const p_packet) throw();
  string onMetricsEvent(int unsigned const p_level) throw();
  void   onShutdownSystemEvent(int unsigned const p_level) throw();


private:

  // Constructors

  PacketParser(void) throw();
  PacketParser(PacketParser const & p_packet_parser) throw();


  // Operators

  PacketParser &  operator=(PacketParser const & p_packet_parser) throw();
  bool           operator==(PacketParser const & p_packet_parser) const throw();
  bool           operator!=(PacketParser const & p_packet_parser) const throw();


  // Variables

  typedef enum
  {
    e_icmp_v4 =  1,
    e_tcp     =  6,
    e_udp     = 17,
    e_icmp_v6 = 58
  } layer4_protocol;


  // DEVELOPER NOTE: In general, these anomalies are used when the data in a
  //                 packet or flow CAN be kept but should be flagged.
  //                 Order them by assumed frequency of occurance.
  //
  typedef enum
  {
    anomaly_tcp_malformed_hdr         = 0x00000001,
    anomaly_udp_malformed_hdr         = 0x00000002,
    anomaly_teredo_missing_ipv6_hdr   = 0x00000004,
    anomaly_teredo_malformed_hdr      = 0x00000008,
    anomaly_teredo_auth_malformed_hdr = 0x00000010,
    anomaly_icmp_malformed_hdr        = 0x00000020,
    anomaly_gre_non_tunnel            = 0x00000040,
    anomaly_gre_malformed_hdr         = 0x00000080,
    anomaly_ipv4_malformed_hdr        = 0x00000100,
    anomaly_ipv6_malformed_hdr        = 0x00000200,
    anomaly_ipv6_size_mismatch        = 0x00000400,
    anomaly_ipv6_out_of_order_hop_opt = 0x00000800,
    anomaly_ipv6_mult_frag_hdr        = 0x00001000,
    anomaly_total_bytes_mismatch      = 0x00002000,
    anomaly_payload_bytes_mismatch    = 0x00004000,
    anomaly_ipv6_mult_hop_hdr         = 0x00008000,
    anomaly_ipv6_mult_dest_hdr        = 0x00010000,
    anomaly_ipv6_mult_routing_hdr     = 0x00020000,
    anomaly_ipv6_mult_auth_hdr        = 0x00040000,
    //
    // DEVELOPER NOTE: Add new ones here
    //
    anomaly_ipv6_malformed_jumbogram  = 0x80000000
  } Anomaly;

  typedef enum
  {
    e_drop_v4 =  4,
    e_drop_v6 =  6
  } drop_type;

  sharedFlow         fake_flow; 
  ByteArray          data;
  FlowAddEvent *     flow_add_event;
  ObjectPool<Flow> & flow_pool;
  int unsigned       cache_timeout;
  int unsigned       cache_forceout;
  u_int64_t          eth_bad_bytes_count;              // total bytes found in bad packets
  u_int64_t          eth_bad_packet_count;             // total bad packets dropped
  u_int64_t          ipv4_bad_bytes_count;             // total bytes found in bad packets
  u_int64_t          ipv4_bad_packet_count;            // total bad packets dropped
  u_int64_t          ipv4_good_bytes_count;            // total bytes found in good packets
  u_int64_t          ipv4_good_packet_count;           // total good packets
  u_int64_t          ipv6_bad_bytes_count;             // total bytes found in bad packets
  u_int64_t          ipv6_bad_packet_count;            // total bad packets dropped
  u_int64_t          ipv6_good_bytes_count;            // total bytes found in good packets
  u_int64_t          ipv6_good_packet_count;           // total good packets
  u_int64_t          total_bytes_received;
  u_int64_t          total_packets_received;

  bool               use_vlan;
  bool               skip_ipv4_packets;

  bool               is_vlan;
  bool               is_icmp;
  bool               is_tcp;
  bool               is_udp;
  bool               is_ip_frag;
  bool               is_last_ip_frag;
  bool               is_first_ip_frag;
  bool               is_src_first;
  int unsigned       ip_ident;
  int unsigned       ether_hdr_offset;
  int unsigned       ether_type;
  int unsigned       vlan_id;
  int unsigned       ip_version;
  int unsigned       ip_hdr_offset;
  int unsigned       ip_protocol;
  int unsigned       ip_opt;
  int unsigned       proto_hdr_offset;
  int unsigned       total_bytes;
  int unsigned       tcp_opt;
  int unsigned       dst_port;
  int unsigned       src_port;
  short unsigned     flag_sum;
  int unsigned       payload;
  int unsigned       tcp_seq;
  IpAddress_t        dst_ip_address;
  IpAddress_t        src_ip_address;
  bool               is_tunnel;
  IpAddress_t        tunnel_dst_ip_address;
  IpAddress_t        tunnel_src_ip_address;
  int unsigned       tunnel_ip_protocol;
  int unsigned       tunnel_dst_port;
  int unsigned       tunnel_src_port;
  int unsigned       tunnel_depth;
  int                anomaly;

  u_int32_t          tcp_option_tsv; // time stamp value
  u_int32_t          tcp_option_tse; // time stamp echo

  // Private Functions

  void resetTunnelVars(void) throw();
  void initVars(void) throw();
  void resetVars(void) throw();

  bool processTeredo(void) throw();

  bool processTcpOptions(int unsigned p_tcp_hdr_len, int unsigned p_options_offset) throw();

  bool processIcmp(void) throw();
  bool processTcp(void) throw();
  bool processUdp(void) throw();
  bool processOther(void) throw();

  bool processGreHdr(int unsigned const p_gre_hdr_offset) throw();
  bool processTunnel(int unsigned const p_offset) throw();

  bool processIpv4Hdr(int unsigned const p_ip_hdr_offset) throw();
  void processIpv4Frag(int unsigned const p_ip_hdr_offset) throw();
  bool processIpv6ExtensionHdrs(int unsigned & p_extension_hdr_offset, u_int8_t & p_next_hdr_type) throw();
  void processIpv6FragHdr(int unsigned const p_offset) throw();
  bool processIpv6Hdr(int unsigned const p_ip_hdr_offset) throw();

  bool processVlan(void) throw();
  void processMpls(void) throw();

  int unsigned parseTeredoHeader(int unsigned const p_offset) throw();

  bool dropEthernetHardwareHeaders(void) throw();
  void skipEthernetHardwareHeaders(void) throw();

  bool processEthernetLayer2(void) throw();
  bool processIpLayer3(void) throw();
  bool processProtocolLayer4(void) throw();

  void createFlow(sharedPacket const & p_packet) throw();

  inline void incrTotalBytesReceived(u_int64_t const p_bytes) throw()
  {
    this->total_bytes_received += p_bytes;
    return;
  }

  inline void incrTotalPacketsReceived(void) throw()
  {
    this->total_packets_received += 1;
    return;
  }

  void         setTunnelVars(void) throw();

  inline void dropEthPacket(void) throw()
  {
    DEBUG(LOTS, ENTER);
    this->eth_bad_bytes_count  += getData().size();
    this->eth_bad_packet_count += 1;
    DEBUG(LOTS, LEAVE);
    return;
  }


  inline void dropIpv4Packet(void) throw()
  {
    DEBUG(LOTS, ENTER);
    this->ipv4_bad_bytes_count  += getData().size();
    this->ipv4_bad_packet_count += 1;
    DEBUG(LOTS, LEAVE);
    return;
  }


  inline void dropIpv6Packet(void) throw()
  {
    DEBUG(LOTS, ENTER);
    this->ipv6_bad_bytes_count  += getData().size();
    this->ipv6_bad_packet_count += 1;
    DEBUG(LOTS, LEAVE);
    return;
  }


  inline void keepIpv4Packet(void) throw()
  {
    DEBUG(LOTS, ENTER);
    this->ipv4_good_bytes_count  += getData().size();
    this->ipv4_good_packet_count += 1;
    DEBUG(LOTS, LEAVE);
    return;
  }


  inline void keepIpv6Packet(void) throw()
  {
    DEBUG(LOTS, ENTER);
    this->ipv6_good_bytes_count  += getData().size();
    this->ipv6_good_packet_count += 1;
    DEBUG(LOTS, LEAVE);
    return;
  }


  // Private Getters/Setters

  inline int unsigned getCacheForceout(void) const throw()
  {
    return(this->cache_forceout);
  }


  inline int unsigned getCacheTimeout(void) const throw()
  {
    return(this->cache_timeout);
  }


  inline FlowAddEvent * getFlowAddEvent(void) const throw()
  {
    return(this->flow_add_event);
  }


  inline ByteArray const & getData(void) const throw()
  {
    return(this->data);
  }


  inline void setData(ByteArray const & p_data) throw()
  {
    this->data = p_data;
    return;
  }


  inline bool isIslEncapsulation(void) const throw()
  {
    // DEVELOPER NOTE: Assume the calling function has checked to
    //                 make sure there is enough data for getData().at
    //                 to work.
    return(
            (0x01 == getData().at(0) || 0x03 == getData().at(0)) &&
             0x00 == getData().at(1) &&
             0x0c == getData().at(2) &&
             0x00 == getData().at(3) &&
             0x00 == getData().at(4)
          );
  }


  inline bool isSpanningTreeProtocol(void) const throw()
  {
    // DEVELOPER NOTE: Assume the calling function has checked to
    //                 make sure there is enough data for getData().at
    //                 to work.
    return(
             0x01 == getData().at(0) &&
             0x80 == getData().at(1) &&
             0xc2 == getData().at(2) &&
             0x00 == getData().at(3) &&
             0x00 == getData().at(4) &&
             0x00 == getData().at(5)
          );
  }


  inline bool isPvstProtocol(void) const throw()
  {
    // DEVELOPER NOTE: Assume the calling function has checked to
    //                 make sure there is enough data for getData().at
    //                 to work.
    return(
             0x01 == getData().at(0) &&
             0x00 == getData().at(1) &&
             0x0c == getData().at(2) &&
             0xcc == getData().at(3) &&
             0xcc == getData().at(4) &&
             0xcd == getData().at(5)
          );
  }


  inline bool isIcmp(void) const throw()
  {
    return(this->is_icmp);
  }


  inline void setIcmp(bool const p_icmp) throw()
  {
    this->is_icmp = p_icmp;
    return;
  }


  inline bool isTcp(void) const throw()
  {
    return(this->is_tcp);
  }


  inline void setTcp(bool const p_tcp) throw()
  {
    this->is_tcp = p_tcp;
    return;
  }

  inline bool isUdp(void) const throw()
  {
    return(this->is_udp);
  }


  inline void setUdp(bool const p_udp) throw()
  {
    this->is_udp = p_udp;
    return;
  }


  inline bool isLastIpFrag(void) const throw()
  {
    return(this->is_last_ip_frag);
  }


  inline void setLastIpFrag(bool const p_last_ip_frag) throw()
  {
    this->is_last_ip_frag = p_last_ip_frag;
    return;
  }


  inline bool isFirstIpFrag(void) const throw()
  {
    return(this->is_first_ip_frag);
  }


  inline void setFirstIpFrag(bool const p_first_ip_frag) throw()
  {
    this->is_first_ip_frag = p_first_ip_frag;
    return;
  }


  inline bool isIpFrag(void) const throw()
  {
    return(this->is_ip_frag);
  }


  inline void setIpFrag(bool const p_ip_frag) throw()
  {
    this->is_ip_frag = p_ip_frag;
    return;
  }


  inline bool isSrcFirst(void) const throw()
  {
    return(this->is_src_first);
  }


  inline void setSrcFirst(bool const p_src_first) throw()
  {
    this->is_src_first = p_src_first;
    return;
  }


  inline bool isTunnel(void) const throw()
  {
    return(this->is_tunnel);
  }


  inline void setTunnel(bool const p_tunnel) throw()
  {
    this->is_tunnel = p_tunnel;
    return;
  }


  inline u_int16_t getIpIdent(void) const throw()
  {
    return(this->ip_ident);
  }


  inline void setIpIdent(u_int16_t const p_ip_ident) throw()
  {
    this->ip_ident = p_ip_ident;
    return;
  }


  inline int unsigned getEtherTypeOffset(void) const throw()
  {
    return(this->ether_hdr_offset);
  }


  inline void incrEtherTypeOffset(int unsigned const p_offset) throw()
  {
    setEtherTypeOffset(getEtherTypeOffset() + p_offset);
    return;
  }


  inline void setEtherTypeOffset(int unsigned const p_ether_hdr_offset) throw()
  {
    this->ether_hdr_offset = p_ether_hdr_offset;
    return;
  }


  inline int unsigned getEtherType(void) const throw()
  {
    return(this->ether_type);
  }


  inline bool updateEtherType(void) throw()
  {
    if (isEnoughData(getEtherTypeOffset() + 2))
    {
      setEtherType(getData().at2(getEtherTypeOffset()));
      incrFirstSeenIpHdrOffset(2);
      return(true);
    }
    return(false);
  }


  inline void setEtherType(int unsigned const p_ether_type) throw()
  {
    this->ether_type = p_ether_type;
    return;
  }


  inline u_int16_t getVlanId(void) const throw()
  {
    return(this->vlan_id);
  }


  inline bool updateVlanId(void) throw()
  {
    const int unsigned ether_hdr_vlan_offset_byte = 2;

    if (isEnoughData(getEtherTypeOffset() + 6))
    {
      setVlanId(getData().at2(getEtherTypeOffset() + ether_hdr_vlan_offset_byte) & 0x0FFF);
      setIsVlan(true);
      incrEtherTypeOffset(4);
      return(true);
    }

    return(false);
  }


  inline void setVlanId(u_int16_t const p_vlan_id) throw()
  {
    if (useVlan())
    {
      this->vlan_id = p_vlan_id;
    }
    return;
  }


  inline void setIsVlan(bool const p_is_vlan) throw()
  {
    this->is_vlan = p_is_vlan;
    return;
  }


  inline int unsigned getIpVersion(void) const throw()
  {
    return(this->ip_version);
  }


  inline void setIpVersion(int unsigned const p_ip_version) throw()
  {
    this->ip_version = p_ip_version;
    return;
  }


  inline bool useVlan(void) const throw()
  {
    return(this->use_vlan);
  }


  inline bool skipIpv4Packets(void) const throw()
  {
    return(this->skip_ipv4_packets);
  }


  inline bool isVlan(void) const throw()
  {
    if (useVlan())
    {
      return(this->is_vlan);
    }
    else
    {
      return(false);
    }
  }


  inline bool isIpv4(void) const throw()
  {
    return(4 == getIpVersion());
  }


  inline bool isIpv6(void) const throw()
  {
    return(6 == getIpVersion());
  }


  inline int unsigned getFirstSeenIpHdrOffset(void) const throw()
  {
    return(this->ip_hdr_offset);
  }


  inline void incrFirstSeenIpHdrOffset(int unsigned const p_offset) throw()
  {
    setFirstSeenIpHdrOffset(getEtherTypeOffset() + p_offset);
    return;
  }


  inline void setFirstSeenIpHdrOffset(int unsigned const p_ip_hdr_offset) throw()
  {
    this->ip_hdr_offset = p_ip_hdr_offset;
    return;
  }


  inline u_int8_t getIpProtocol(void) const throw()
  {
    return(this->ip_protocol);
  }


  inline void setIpProtocol(u_int8_t const p_ip_protocol) throw()
  {
    this->ip_protocol = p_ip_protocol;
    return;
  }


  inline u_int8_t getTunnelIpProtocol(void) const throw()
  {
    return(this->tunnel_ip_protocol);
  }


  inline void setTunnelIpProtocol(u_int8_t const p_ip_protocol) throw()
  {
    this->tunnel_ip_protocol = p_ip_protocol;
    return;
  }


  inline int unsigned getIpOpt(void) const throw()
  {
    return(this->ip_opt);
  }


  inline void setIpOpt(int unsigned const p_ip_opt) throw()
  {
    this->ip_opt = p_ip_opt;
    return;
  }


  inline int unsigned getProtoHdrOffset(void) const throw()
  {
    return(this->proto_hdr_offset);
  }


  inline void setProtoHdrOffset(int unsigned const p_proto_hdr_offset) throw()
  {
    this->proto_hdr_offset = p_proto_hdr_offset;
    return;
  }


  inline int unsigned getTotalBytes(void) const throw()
  {
    return(this->total_bytes);
  }


  inline void setTotalBytes(int unsigned const p_total_bytes) throw()
  {
    if (p_total_bytes <= getData().size())
    {
      this->total_bytes = p_total_bytes;
      return;
    }
    this->total_bytes = getData().size();
    CAUTION(RangeError, "Adjust TotalBytes", "Expected total bytes (" + uitoa10(p_total_bytes) + ") to be less than or equal to bytes on the wire (" + uitoa10(getData().size()) + ")");
    addAnomaly(anomaly_total_bytes_mismatch);
    return;
  }


  inline int unsigned getTcpOpt(void) const throw()
  {
    return(this->tcp_opt);
  }


  inline void setTcpOpt(int unsigned const p_tcp_opt) throw()
  {
    this->tcp_opt = p_tcp_opt;
    return;
  }


  inline u_int16_t getDstPort(void) const throw()
  {
    return(this->dst_port);
  }


  inline void setDstPort(u_int16_t const p_dst_port) throw()
  {
    this->dst_port = p_dst_port;
    return;
  }


  inline u_int16_t getSrcPort(void) const throw()
  {
    return(this->src_port);
  }


  inline void setSrcPort(u_int16_t const p_src_port) throw()
  {
    this->src_port = p_src_port;
    return;
  }


  inline u_int16_t getTunnelDstPort(void) const throw()
  {
    return(this->tunnel_dst_port);
  }


  inline void setTunnelDstPort(u_int16_t const p_dst_port) throw()
  {
    this->tunnel_dst_port = p_dst_port;
    return;
  }


  inline u_int16_t getTunnelSrcPort(void) const throw()
  {
    return(this->tunnel_src_port);
  }


  inline void setTunnelSrcPort(u_int16_t const p_src_port) throw()
  {
    this->tunnel_src_port = p_src_port;
    return;
  }


  inline short unsigned getFlagSum(void) const throw()
  {
    return(this->flag_sum);
  }


  inline void setFlagSum(short unsigned const p_flag_sum) throw()
  {
    this->flag_sum = p_flag_sum;
    return;
  }


  inline int unsigned getPayload(void) const throw()
  {
    return(this->payload);
  }


  inline void calcPayload(int unsigned const p_hdr_size) throw()
  {
    int unsigned bytes = getProtoHdrOffset() + p_hdr_size;

    if (getTotalBytes() >= bytes)
    {
      setPayload(getTotalBytes() - bytes);
      return;
    }

    this->payload = 0;
    CAUTION(RangeError, "Adjust Payload", "Expected payload (" + uitoa10(bytes) + ") to be less than or equal to bytes on the wire (" + uitoa10(getTotalBytes()) + ")");
    addAnomaly(anomaly_payload_bytes_mismatch);

    return;
  }  


  inline void setPayload(int unsigned const p_payload) throw()
  {
    this->payload = p_payload;
    return;
  }


  inline int unsigned getTcpSeq(void) const throw()
  {
    return(this->tcp_seq);
  }


  inline void setTcpSeq(int unsigned const p_tcp_seq) throw()
  {
    this->tcp_seq = p_tcp_seq;
    return;
  }


  inline IpAddress_t & getDstIpAddress(void) throw()
  {
    return(this->dst_ip_address);
  }


  inline IpAddress_t & getSrcIpAddress(void) throw()
  {
    return(this->src_ip_address);
  }


  inline IpAddress_t & getTunnelDstIpAddress(void) throw()
  {
    return(this->tunnel_dst_ip_address);
  }


  inline IpAddress_t & getTunnelSrcIpAddress(void) throw()
  {
    return(this->tunnel_src_ip_address);
  }


  void setIpAddress(IpAddress_t & p_address, int unsigned const p_ipv4_addr_offset, int unsigned const p_ipv6_addr_offset_start, int unsigned const p_ipv6_addr_offset_end) throw()
  {
    // DEVELOPER NOTE: Assume the calling function chain has checked to
    //                 make sure there is enough data for getData().at
    //                 to work.
    DEBUG(TRACE, ENTER);
    if (isIpv4())
    {
      // DEVELOPER NOTE: Wipe out any previous address that might be a tunnel
      for (int unsigned idx = 0; idx < 12; ++idx)
      {
        p_address.at(idx) = 0;
      }
      // DEVELOPER NOTE: We are embedding a 4 byte IPv4 address in
      //                 in a 16 byte IPv6 address so we need to start
      //                 at the 12th byte and go to the 16th byte.
      for (int unsigned idx = 12; idx < 16; ++idx)
      {
        p_address.at(idx) = getData().at(p_ipv4_addr_offset + idx);
      }
      // DEVELOPER NOTE: We need to put 0xFFFF at the beginning of the
      //                 IPv4 address so that it can be recognized as
      //                 an IPv4 mapped address in IPv6 notation.
      p_address.at(10)    = 0xFF;
      p_address.at(11)    = 0xFF;
    }
    else
    {
      for (int unsigned idx = 0; idx < 16; ++idx)
      {
        p_address.at(idx) = getData().at(p_ipv6_addr_offset_start + idx);
      }
    }
    DEBUG(TRACE, LEAVE);
    return;
  }


  void setDstIpAddress(int unsigned const p_ip_hdr_offset) throw()
  {
    DEBUG(TRACE, ENTER);
    setIpAddress(getDstIpAddress(), p_ip_hdr_offset + 4, p_ip_hdr_offset + 24, p_ip_hdr_offset + 40);
    DEBUG(TRACE, LEAVE);
    return;
  }


  void setSrcIpAddress(int unsigned const p_ip_hdr_offset) throw()
  {
    DEBUG(TRACE, ENTER);
    setIpAddress(getSrcIpAddress(), p_ip_hdr_offset + 0, p_ip_hdr_offset +  8, p_ip_hdr_offset + 24);
    DEBUG(TRACE, LEAVE);
    return;
  }

  inline void incrTunnelDepth(void) throw()
  {
    ++(this->tunnel_depth);
    return;
  }


  inline void setTunnelDepth(u_int8_t const p_depth) throw()
  {
    this->tunnel_depth = p_depth;
    return;
  }


  inline u_int8_t getTunnelDepth(void) const throw()
  {
    return(this->tunnel_depth);
  }


  inline void addAnomaly(Anomaly p_anomaly) throw()
  {
    this->anomaly |= p_anomaly;
    return;
  }


  inline u_int32_t getAnomaly(void) const throw()
  {
    return(this->anomaly);
  }


  inline void setAnomaly(u_int32_t const p_anomaly) throw()
  {
    this->anomaly = p_anomaly;
    return;
  }

  inline u_int32_t getTcpOptionTsv(void) throw()
  {
    return(this->tcp_option_tsv);
  }

  inline u_int32_t getTcpOptionTse(void) throw()
  {
    return(this->tcp_option_tse);
  }

  inline void setTcpOptionTsv(u_int32_t const p_timestamp_value) throw()
  {
    this->tcp_option_tsv = p_timestamp_value;
    return;
  }

  inline void setTcpOptionTse(u_int32_t const p_timestamp_echo) throw()
  {
    this->tcp_option_tse = p_timestamp_echo;
    return;
  }


};

#endif // PACKETPARSER_HPP
