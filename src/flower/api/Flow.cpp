///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


// DEVELOPER NOTE:
// The timeout provides an assumed end to a flow by flushing the flow from the
// cache when no matching packets have been seen for N seconds in wire time.
// When a cached flow has seen no incoming packets for N seconds (wire time)
// it is flushed downstream to the SummaryCache and SummaryExporter.
//
// The behavior of a cache forceout is similar to the behavior of a timeout,
// where a flow representing a long lasting connection will be flushed out of
// the cache after it has been in existence for M seconds in wire time so that
// the flow is recorded in the data file in a timely manner and the results
// are repeatable with a capture file.


// System Includes
// External Includes
#include <boost/lexical_cast.hpp>
// Internal Includes
// Application Includes
#include "num2string.hpp"
#include "Flow.hpp"
#include "Field/Data.hpp"
#include "Field/FieldToStringPolicy.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Flow"


// Namespaces used
using namespace boost;
using boost::lexical_cast;


//==============================================================================
// Class Flow
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


// ======================================================================
// Constructors
// ======================================================================


Flow::Flow(
           bool const p_fake,
           FlowKey const & p_flow_key,
           u_int32_t const p_first_timestamp,
           int unsigned const p_timeout,
           int unsigned const p_forceout,
           bool const p_flow_direction,
           bool const p_is_tcp,
           bool const p_is_icmp,
           bool const p_is_ip_frag,
           bool const p_is_first_ip_frag,
           bool const p_is_last_ip_frag,
           bool const p_is_ipv4
          ) noexcept(true) :
  flow_key(p_flow_key),
  timeout(p_first_timestamp + p_timeout),
  forceout(p_first_timestamp + p_forceout),
  fake(p_fake),
  flow_direction(p_flow_direction),
  is_tcp(p_is_tcp),
  is_icmp(p_is_icmp),
  is_ip_frag(p_is_ip_frag),
  is_first_ip_frag(p_is_first_ip_frag),
  is_last_ip_frag(p_is_last_ip_frag),
  is_ipv4(p_is_ipv4)
{
  DEBUG(LOTS, ENTER);
  DEBUG(LOTS, LEAVE);
  return;
}


Flow::Flow(Flow const & p_flow) noexcept(true)
{
  DEBUG(LOTS, ENTER);
  
  this->copyFlow(p_flow);

  DEBUG(LOTS, LEAVE);
  return;
}


// ======================================================================
// Operators
// ======================================================================


Flow & Flow::copyFlow(Flow const & p_flow) noexcept(true)
{
  DEBUG(LOTS, ENTER);
  this->timeout                 = p_flow.timeout;
  this->forceout                = p_flow.forceout;
  this->flow_key                = p_flow.flow_key;
  this->fake                    = p_flow.fake;
  this->flow_direction          = p_flow.flow_direction;
  this->is_icmp                 = p_flow.is_icmp;
  this->is_ip_frag              = p_flow.is_ip_frag;
  this->is_first_ip_frag        = p_flow.is_first_ip_frag;
  this->is_last_ip_frag         = p_flow.is_last_ip_frag;
  this->is_ipv4                 = p_flow.is_ipv4;
  // COPY_VISITOR_START
  this->physical_struct         = p_flow.physical_struct;
  // COPY_VISITOR_END
  DEBUG(LOTS, LEAVE);
  return(*this);
}


void Flow::assign(
                  bool const              p_fake,
                  FlowKey const &         p_flow_key,
                  u_int32_t const         p_first_timestamp,
                  int unsigned const      p_timeout,
                  int unsigned const      p_forceout,
                  bool const              p_flow_direction,
                  bool const              p_is_tcp,
                  bool const              p_is_icmp,
                  bool const              p_is_ip_frag,
                  bool const              p_is_first_ip_frag,
                  bool const              p_is_last_ip_frag,
                  bool const              p_is_ipv4
                 ) noexcept(true)
{
  DEBUG(LOTS, ENTER);
  this->timeout                 = p_first_timestamp + p_timeout;
  this->forceout                = p_first_timestamp + p_forceout;
  this->flow_key                = p_flow_key;
  this->fake                    = p_fake;
  this->flow_direction          = p_flow_direction;
  this->is_tcp                  = p_is_tcp;
  this->is_icmp                 = p_is_icmp;
  this->is_ip_frag              = p_is_ip_frag;
  this->is_first_ip_frag        = p_is_first_ip_frag;
  this->is_last_ip_frag         = p_is_last_ip_frag;
  this->is_ipv4                 = p_is_ipv4;

  DEBUG(LOTS, LEAVE);
  return;
}


bool Flow::compare(std::shared_ptr<Flow> const & p_flow) noexcept(true)
{
  DEBUG(TRACE, ENTER);
  bool           result = false;

// ======================================================================
// DEVELOPER NOTE: Uncomment the following to debug a true comparison.
// ======================================================================
//  if (isDebugLevelSet(INFO))
//  {
//    cout << "Flow0::Compare:"      <<                getVirtualTime()   << ":";
//    cout <<         getForceout()  << ":" <<         getFlowDirection() << ":";
//    cout <<         getFlowKey()   << ":" <<         getFlowKey()       << ":";
//    cout <<         isIpFrag()     << ":" <<         isLastIpFrag()     << ":";
//    cout <<         isFake()       << ":" <<         isIcmp()           << endl;
//    cout << "Flow1::Compare:"      <<        p_flow->getVirtualTime()   << ":";
//    cout << p_flow->getForceout()  << ":" << p_flow->getFlowDirection() << ":";
//    cout << p_flow->getFlowKey()   << ":" << p_flow->getFlowKey()       << ":";
//    cout << p_flow->isIpFrag()     << ":" << p_flow->isLastIpFrag()     << ":";
//    cout << p_flow->isFake()       << ":" << p_flow->isIcmp()           << endl;
//    cout << p_flow->isIpv4()       << ":" << endl;
//  }
// ======================================================================

  if ((getVirtualTime()   == p_flow->getVirtualTime())   &&
      (getTimeout()       == p_flow->getTimeout())       &&
      (getForceout()      == p_flow->getForceout())      &&
      (getFlowDirection() == p_flow->getFlowDirection()) &&
      (getFlowKey()       == p_flow->getFlowKey())       &&
      (isIpFrag()         == p_flow->isIpFrag())         &&
      (isLastIpFrag()     == p_flow->isLastIpFrag())     &&
      (isFake()           == p_flow->isFake())           &&
      (isIcmp()           == p_flow->isIcmp())           &&
      (isIpv4()           == p_flow->isIpv4()))
  {
    if (isFake())
    {
      result = true;
    }
    else
    {
      // DEVELOPER NOTE: Can we compare this->physical_struct to p_flow->physical_struct
      // COMPARE_VISITOR_START
      // COMPARE_VISITOR_END
      result = true;
    }
  }

  DEBUG(TRACE, LEAVE);
  return(result);
}


// ======================================================================
// Public Functions
// ======================================================================


void Flow::fragment_pre_merge(std::shared_ptr<Flow> const & p_flow) noexcept(true)
{
  DEBUG(LOTS, ENTER);

  MergeAdjustTime< Data<u_int32_t, 4> >::merge(this->physical_struct.time_duration, p_flow->physical_struct.time_duration);

  return;
}


bool Flow::merge(std::shared_ptr<Flow> const & p_flow) noexcept(true)
{
  DEBUG(LOTS, ENTER);

  bool result = false;

  if ((getNormFlowKey() == p_flow->getNormFlowKey()) || ((isIpFrag() == p_flow->isIpFrag()) && (getFragFlowKey() == p_flow->getFragFlowKey())))
  {
    bool direction = (getFlowDirection() == p_flow->getFlowDirection());

    // MERGE_VISITOR_START
    MergeAdd< Data<u_int32_t, 4> >::merge(     this->physical_struct.time_duration,    p_flow->physical_struct.time_duration,    direction);
    //MergeNone< Data<u_int16_t, 1> >::merge(    this->physical_struct.vlan,             p_flow->physical_struct.vlan,             direction);
    //MergeNone< Data<u_int8_t, 1> >::merge(     this->physical_struct.ip_protocol,      p_flow->physical_struct.ip_protocol,      direction);
    //MergeNone< Data< IpAddress_t, 2 >::merge(  this->physical_struct.ip_address,       p_flow->physical_struct.ip_address,       direction);
    
    // DEVELOPER NOTE: We need to preserve the previous payload bytes before the merge wipes them out
    //                 They are needed for detecting TCP retransmits
    u_int64_t prev_src_payload = this->physical_struct.payload_bytes.at(2);
    u_int64_t prev_dst_payload = this->physical_struct.payload_bytes.at(3);

    MergeAddPayload< Data<u_int64_t, 4> >::merge(     this->physical_struct.payload_bytes,    p_flow->physical_struct.payload_bytes,    direction);
    MergeAdd< Data<u_int64_t,        2> >::merge(     this->physical_struct.total_bytes,      p_flow->physical_struct.total_bytes,      direction);
    MergeAdd< Data<u_int64_t,        2> >::merge(     this->physical_struct.packet_count,     p_flow->physical_struct.packet_count,     direction);
    MergeAdd< Data<u_int64_t,        2> >::merge(     this->physical_struct.ip_option_count,  p_flow->physical_struct.ip_option_count,  direction);
    MergePort< Data<u_int16_t,       2> >::merge(     this->physical_struct.ip_port,          p_flow->physical_struct.ip_port,          direction);

    if (isIcmp())
    {
      // DEVELOPER NOTE: Bitfield members cannot be passed by reference,
      //                 so we make a copy and modify the copy in the merge policy,
      //                 then copy the value back to the bit.
      u_int64_t temp_bool = this->physical_struct.icmp_flag_sum_1;
      MergeBitsum< Data<u_int16_t, 2> >::merge(  this->physical_struct.icmp_flag_sum,    p_flow->physical_struct.icmp_flag_sum,    direction, temp_bool);
      this->physical_struct.icmp_flag_sum_1 = temp_bool;
      MergeFlags< Data<Elf_t, 1> >::merge(       this->physical_struct.elf_flags,        p_flow->physical_struct.elf_flags,        direction, isIcmp());
    }

    if (isTcp())
    {
      // DEVELOPER NOTE: Bitfield members cannot be passed by reference,
      //                 so we make a copy and modify the copy in the merge policy,
      //                 then copy the value back to the bit.
      u_int64_t temp_bool = this->physical_struct.tcp_flag_sum_1;
      MergeBitsum< Data<u_int16_t, 2> >::merge(  this->physical_struct.tcp_flag_sum,     p_flow->physical_struct.tcp_flag_sum,     direction, temp_bool);
      this->physical_struct.tcp_flag_sum_1 = temp_bool;
      MergeFlags< Data<Elf_t, 1> >::merge(       this->physical_struct.elf_flags,        p_flow->physical_struct.elf_flags,        direction, isTcp());
      MergeAdd< Data<u_int64_t, 2> >::merge(     this->physical_struct.tcp_option_count, p_flow->physical_struct.tcp_option_count, direction);



      // DEVELOPER NOTE: Bitfield members cannot be passed by reference,
      //                 so we make a copy and modify the copy in the merge policy,
      //                 then copy the value back to the bit.
      //                 Preserve destination_is_valid field across the call
      temp_bool = this->physical_struct.tcp_initial_seq_1;
      MergeInitseq< Data<u_int32_t, 4> >::merge( this->physical_struct.tcp_initial_seq,  p_flow->physical_struct.tcp_initial_seq,  direction, temp_bool);
      this->physical_struct.tcp_initial_seq_1 = temp_bool;

     
      // DEVELOPER NOTE: MergeTcpRetrans relies on the previous values of the last sequence numbers, so it has to happen before MergeLastseq
      MergeTcpRetrans< Data<u_int64_t, 2> >::merge( this->physical_struct.tcp_retrans_count, 
                                                    direction,
                                                    this->physical_struct.tcp_last_seq,
                                                    p_flow->physical_struct.tcp_last_seq,
                                                    (p_flow->physical_struct.payload_bytes.at(0) > 0),
                                                    prev_src_payload,
                                                    prev_dst_payload,
                                                    this->physical_struct.hist_tcp_last_seq);

      // DEVELOPER NOTE: The historical last sequence is only used in calculating the TCP retransmissions
      //                 It is the sequence number of the last packet to have a payload
      MergeHistoricalSeq< Data<u_int32_t, 4> >::merge( this->physical_struct.hist_tcp_last_seq, p_flow->physical_struct.tcp_last_seq, direction,(p_flow->physical_struct.payload_bytes.at(0) > 0));

      temp_bool = this->physical_struct.tcp_last_seq_1;
      MergeLastseq< Data<u_int32_t, 4> >::merge( this->physical_struct.tcp_last_seq,     p_flow->physical_struct.tcp_last_seq,     direction, temp_bool);
      this->physical_struct.tcp_last_seq_1 = temp_bool;
     
    }

    //MergeNone< Data< u_int8_t, 1 >::merge(  this->physical_struct.tunnel_depth,        p_flow->physical_struct.tunnel_depth,   direction);
    //MergeNone< Data< IpAddress_t, 2 >::merge(  this->physical_struct.tunnel_address,   p_flow->physical_struct.tunnel_address,   direction);
    //MergeNone< Data<u_int8_t, 1> >::merge(     this->physical_struct.tunnel_protocol,  p_flow->physical_struct.tunnel_protocol, direction);
    MergePort< Data<u_int16_t, 2> >::merge(    this->physical_struct.tunnel_port,      p_flow->physical_struct.tunnel_port, direction);
    //MergeNone< Data<u_int8_t, 3> >::merge(     this->physical_struct.frag_type,        p_flow->physical_struct.frag_type, direction);
    MergeOr< Data<u_int8_t, 1> >::merge(       this->physical_struct.fragment,         p_flow->physical_struct.fragment, direction);
    MergeOr< Data<u_int32_t, 1> >::merge(      this->physical_struct.anomaly,          p_flow->physical_struct.anomaly, direction);
    // MERGE_VISITOR_END

    // update the timeout value since a new packet has arrived
    setTimeout(p_flow->getTimeout());
    result = true;
  
    // DEVELOPER NOTE:  We don't do anything with forceout in the merge
    //                  because it was pre-caclulated in the constructor of
    //                  the first flow that arrived in the cache.  Subsequent
    //                  flows have new values for forceout, but they are not
    //                  valid since forceout is an offset from the time of the
    //                  first packet in the flow.
  }
  //else
  //{
  //  CAUTION(IllegalValue, "FlowKey (" + getFlowKeyString() + ") not equal to p_FlowKey (" + p_flow->getFlowKey() + ")", "Flows cannot be merged");
  //}

  DEBUG(LOTS, LEAVE);
  return(result);
}


string Flow::createFlowString(void) const noexcept(true)
{
  DEBUG(LOTS, ENTER);
  static string flow_str(512, ' ');

  if (! isFake())
  {
    flow_str = getFileOrWireStr() + ',' + getSiteName();
  }
  else
  {
    flow_str = "2," + getSiteName();
  }
  // TOSTRING_VISITOR_START
  ToString< Data<u_int32_t, 4> >::toString(             this->physical_struct.time_duration,    flow_str, this->physical_struct.time_duration_0, this->physical_struct.time_duration_1, this->physical_struct.time_duration_2, this->physical_struct.time_duration_3);
  ToStringNonzero< Data<u_int16_t, 1> >::toString(      this->physical_struct.vlan_id,          flow_str, this->physical_struct.vlan_id_0);
  ToString< Data<u_int8_t, 1> >::toString(              this->physical_struct.ip_protocol,      flow_str, this->physical_struct.ip_protocol_0);
  ToStringIp< Data< IpAddress_t, 2> >::toString(        this->physical_struct.ip_address,       flow_str, this->physical_struct.ip_address_0, this->physical_struct.ip_address_1);
  ToStringPayloadNonzero< Data<u_int64_t, 4> >::toString(this->physical_struct.payload_bytes,    flow_str, this->physical_struct.payload_bytes_0, this->physical_struct.payload_bytes_1);
  ToStringNonzero< Data<u_int64_t, 2> >::toString(      this->physical_struct.total_bytes,      flow_str, this->physical_struct.total_bytes_0, this->physical_struct.total_bytes_1);
  ToStringNonzero< Data<u_int64_t, 2> >::toString(      this->physical_struct.packet_count,     flow_str, this->physical_struct.packet_count_0, this->physical_struct.packet_count_1);
  ToStringNonzero< Data<u_int64_t, 2> >::toString(      this->physical_struct.ip_option_count,  flow_str, this->physical_struct.ip_option_count_0, this->physical_struct.ip_option_count_1);
  ToStringNonzero< Data<u_int16_t, 2> >::toString(      this->physical_struct.ip_port,          flow_str, this->physical_struct.ip_port_0, this->physical_struct.ip_port_1);
  ToStringIcmpflags< Data<u_int16_t, 2> >::toString(    this->physical_struct.icmp_flag_sum,    flow_str, this->physical_struct.icmp_flag_sum_0, this->physical_struct.icmp_flag_sum_1);
  ToStringFlags< Data<Elf_t, 1> >::toString(            this->physical_struct.elf_flags,        flow_str, this->physical_struct.icmp_flags_0);
  ToStringNonzero< Data<u_int64_t, 2> >::toString(      this->physical_struct.tcp_option_count, flow_str, this->physical_struct.tcp_option_count_0, this->physical_struct.tcp_option_count_1);
  ToStringTcpflags< Data<u_int16_t, 2> >::toString(     this->physical_struct.tcp_flag_sum,     flow_str, this->physical_struct.tcp_flag_sum_0, this->physical_struct.tcp_flag_sum_1);
  ToStringFlags< Data<Elf_t, 1> >::toString(            this->physical_struct.elf_flags,        flow_str, this->physical_struct.tcp_flags_0);
  ToStringSeq< Data<u_int32_t, 4> >::toString(          this->physical_struct.tcp_initial_seq,  flow_str, this->physical_struct.tcp_initial_seq_0, this->physical_struct.tcp_initial_seq_1);
  ToStringSeq< Data<u_int32_t, 4> >::toString(          this->physical_struct.tcp_last_seq,     flow_str, this->physical_struct.tcp_last_seq_0, this->physical_struct.tcp_last_seq_1);
  ToStringNonzero< Data<u_int64_t, 2> >::toString(      this->physical_struct.tcp_retrans_count,flow_str, this->physical_struct.tcp_retrans_count_0, this->physical_struct.tcp_retrans_count_1);
  ToString< Data<u_int8_t, 1> >::toString(              this->physical_struct.tunnel_depth,     flow_str, this->physical_struct.tunnel_depth_0);
  ToStringIp< Data< IpAddress_t, 2> >::toString(        this->physical_struct.tunnel_address,   flow_str, this->physical_struct.tunnel_address_0, this->physical_struct.tunnel_address_1);
  ToString< Data<u_int8_t, 1> >::toString(              this->physical_struct.tunnel_protocol,  flow_str, this->physical_struct.tunnel_protocol_0);
  ToStringNonzero< Data<u_int16_t, 2> >::toString(      this->physical_struct.tunnel_port,      flow_str, this->physical_struct.tunnel_port_0, this->physical_struct.tunnel_port_1);
  ToStringFragType< Data<u_int8_t, 3> >::toString(      this->physical_struct.frag_type,        flow_str, this->physical_struct.frag_type_0, this->physical_struct.frag_type_1, this->physical_struct.frag_type_2);
  ToStringNonzero< Data<u_int8_t, 1> >::toString(       this->physical_struct.fragment,         flow_str, this->physical_struct.fragment_0);
  ToStringHexNonzero< Data<u_int32_t, 1> >::toString(   this->physical_struct.anomaly,          flow_str, this->physical_struct.anomaly_0);
  // TOSTRING_VISITOR_END

  DEBUG(LOTS, LEAVE);
  return(flow_str);
}


void Flow::forceFake(u_int32_t const p_first_timestamp) noexcept(true)
{
  assign(::g_FAKE);
  assignFields();
  setVirtualTime(p_first_timestamp);
  return;
}


void Flow::zero(void) noexcept(true)
{
  DEBUG(TRACE, ENTER);
  // ZEROOUT_VISITOR_START
  ZeroOut< Data<u_int32_t, 4> >::zeroOut(this->physical_struct.time_duration);
  //ZeroOutNone< Data<u_int16_t, 1> >::zeroOut(this->physical_struct.vlan_id);
  //ZeroOutNone< Data<u_int8_t, 1> >::zeroOut(this->physical_struct.ip_protocol);
  //ZeroOutNone< Data< IpAddress_t, 2> >::zeroOut(this->physical_struct.ip_address);
  ZeroOut< Data<u_int64_t, 4> >::zeroOut(this->physical_struct.payload_bytes);
  ZeroOut< Data<u_int64_t, 2> >::zeroOut(this->physical_struct.total_bytes);
  ZeroOut< Data<u_int64_t, 2> >::zeroOut(this->physical_struct.packet_count);
  ZeroOut< Data<u_int64_t, 2> >::zeroOut(this->physical_struct.ip_option_count);
  //ZeroOutNone< Data<u_int16_t, 2> >::zeroOut(this->physical_struct.ip_port);
  
  if (isIcmp())
  {
    ZeroOut< Data<u_int16_t, 2> >::zeroOut(this->physical_struct.icmp_flag_sum);
  }

  ZeroOut< Data<Elf_t, 1> >::zeroOut(this->physical_struct.elf_flags);

  if (isTcp())
  {
    ZeroOut< Data<u_int64_t, 2> >::zeroOut(this->physical_struct.tcp_option_count);
    ZeroOut< Data<u_int16_t, 2> >::zeroOut(this->physical_struct.tcp_flag_sum);
    ZeroOutSeq< Data<u_int32_t, 4> >::zeroOut(this->physical_struct.tcp_initial_seq);
    ZeroOutSeq< Data<u_int32_t, 4> >::zeroOut(this->physical_struct.tcp_last_seq);
    this->physical_struct.tcp_initial_seq_0 = false;
    this->physical_struct.tcp_initial_seq_1 = false;
    this->physical_struct.tcp_last_seq_0    = false;
    this->physical_struct.tcp_last_seq_1    = false;
    ZeroOutSeq< Data<u_int32_t, 4> >::zeroOut(this->physical_struct.hist_tcp_last_seq);
    ZeroOut< Data<u_int64_t, 2> >::zeroOut(this->physical_struct.tcp_retrans_count);
  }

  //ZeroOutNone< Data< u_int8_t, 1> >::zeroOut(this->physical_struct.tunnel_depth);
  //ZeroOutNone< Data< IpAddress_t, 2> >::zeroOut(this->physical_struct.tunnel_address);
  //ZeroOutNone< Data<u_int8_t, 1> >::zeroOut(this->physical_struct.tunnel_protocol);
  //ZeroOutNone< Data<u_int16_t, 2> >::zeroOut(this->physical_struct.tunnel_port);
  //ZeroOutNone< Data<u_int8_t, 3> >::zeroOut(this->physical_struct.frag_type);
  ZeroOut< Data<u_int8_t, 1> >::zeroOut(this->physical_struct.fragment);
  ZeroOut< Data<u_int32_t, 1> >::zeroOut(this->physical_struct.anomaly);
  // ZEROOUT_VISITOR_END

  
  DEBUG(TRACE, LEAVE);
  return;
}


// ASSIGN_METHOD_START
void Flow::assignFields(
                        u_int32_t const     p_packet_sec,
                        u_int32_t const     p_packet_usec,
                        u_int16_t const     p_vlan_id,
                        u_int8_t const      p_ip_protocol,
                        IpAddress_t const & p_src_ip_address,
                        IpAddress_t const & p_dst_ip_address,
                        u_int64_t const     p_payload,
                        u_int64_t const     p_total_bytes,
                        u_int64_t const     p_packet_count,
                        u_int64_t const     p_ip_opt,
                        u_int16_t const     p_src_port,
                        u_int16_t const     p_dst_port,
                        u_int64_t const     p_tcp_opt,
                        bool      const     p_is_tcp,
                        u_int16_t const     p_flag_sum,
                        bool      const     p_is_icmp,
                        u_int32_t const     p_tcp_seq,
                        u_int32_t const     p_tcp_opt_tsv,
                        bool      const     p_is_tunnel,
                        u_int8_t  const     p_tunnel_depth,
                        IpAddress_t const & p_tunnel_src_ip_address,
                        IpAddress_t const & p_tunnel_dst_ip_address,
                        u_int8_t const      p_tunnel_ip_protocol,
                        u_int16_t const     p_tunnel_src_port,
                        u_int16_t const     p_tunnel_dst_port,
                        bool const          p_fragment,
                        u_int32_t const     p_anomaly
                       ) noexcept(true)
{

  // ASSIGN_VISITOR_START
  this->physical_struct.time_duration.assign(p_packet_sec, p_packet_usec, p_packet_sec, p_packet_usec);
  this->physical_struct.payload_bytes.assign(p_payload, 0, p_payload, 0);
  this->physical_struct.total_bytes.assign(p_total_bytes, 0);
  this->physical_struct.packet_count.assign(1, 0);
  this->physical_struct.ip_option_count.assign(p_ip_opt, 0);
  this->physical_struct.tcp_option_count.assign(p_tcp_opt, 0);
  this->physical_struct.tcp_initial_seq.assign(p_tcp_opt_tsv, p_tcp_seq, 0, 0);
  this->physical_struct.tcp_last_seq.assign(p_tcp_opt_tsv, p_tcp_seq, 0, 0);
  this->physical_struct.hist_tcp_last_seq.assign(0,0,0,0);
  this->physical_struct.tcp_retrans_count.assign(0, 0);
  this->physical_struct.ip_port.assign(p_src_port, p_dst_port);
  this->physical_struct.icmp_flag_sum.assign(p_flag_sum, 0);
  this->physical_struct.tcp_flag_sum.assign(p_flag_sum, 0);
  this->physical_struct.tunnel_port.assign(p_tunnel_src_port, p_tunnel_dst_port);
  this->physical_struct.vlan_id.assign(p_vlan_id);
  this->physical_struct.ip_protocol.assign(p_ip_protocol);
  this->physical_struct.frag_type.assign(0, 0, 0);
  this->physical_struct.fragment.assign(p_fragment);
  this->physical_struct.ip_address.assign(p_src_ip_address, p_dst_ip_address);
  this->physical_struct.tunnel_depth.assign(p_tunnel_depth);
  this->physical_struct.tunnel_address.assign(p_tunnel_src_ip_address, p_tunnel_dst_ip_address);
  this->physical_struct.tunnel_protocol.assign(p_tunnel_ip_protocol);
  this->physical_struct.tunnel_port.assign(p_tunnel_src_port, p_tunnel_dst_port);
  this->physical_struct.elf_flags.assign(Elf_t(p_flag_sum, p_is_icmp));
  this->physical_struct.anomaly.assign(p_anomaly);
  //
  // bools
  //
  this->physical_struct.time_duration_0    = true;
  this->physical_struct.time_duration_1    = true;
  this->physical_struct.time_duration_2    = true;
  this->physical_struct.time_duration_3    = true;
  this->physical_struct.payload_bytes_0    = true;
  this->physical_struct.payload_bytes_1    = true;
  this->physical_struct.total_bytes_0      = true;
  this->physical_struct.total_bytes_1      = true;
  this->physical_struct.packet_count_0     = true;
  this->physical_struct.packet_count_1     = true;
  this->physical_struct.ip_option_count_0  = true;
  this->physical_struct.ip_option_count_1  = true;
  this->physical_struct.tcp_option_count_0 = p_is_tcp;
  this->physical_struct.tcp_option_count_1 = p_is_tcp;
  this->physical_struct.tcp_initial_seq_0  = p_is_tcp;  // Src timestamp and src seq num
  this->physical_struct.tcp_initial_seq_1  = false;     // Dst timestamp and dst seq num
  this->physical_struct.tcp_last_seq_0     = p_is_tcp;  // Src timestamp and src seq num
  this->physical_struct.tcp_last_seq_1     = false;     // Dst timestamp and dst seq num
  this->physical_struct.tcp_retrans_count_0= p_is_tcp;
  this->physical_struct.tcp_retrans_count_1= p_is_tcp;
  this->physical_struct.ip_port_0          = true;
  this->physical_struct.ip_port_1          = true;
  this->physical_struct.icmp_flag_sum_0    = p_is_icmp;
  this->physical_struct.icmp_flag_sum_1    = false;
  this->physical_struct.tcp_flag_sum_0     = p_is_tcp;
  this->physical_struct.tcp_flag_sum_1     = false;
  this->physical_struct.vlan_id_0          = true;
  this->physical_struct.ip_protocol_0      = true;
  this->physical_struct.frag_type_0        = false;
  this->physical_struct.frag_type_1        = false;
  this->physical_struct.frag_type_2        = false;
  this->physical_struct.fragment_0         = true;
  this->physical_struct.ip_address_0       = true;
  this->physical_struct.ip_address_1       = true;
  this->physical_struct.tunnel_depth_0     = p_is_tunnel;
  this->physical_struct.tunnel_address_0   = p_is_tunnel;
  this->physical_struct.tunnel_address_1   = p_is_tunnel;
  this->physical_struct.tunnel_protocol_0  = p_is_tunnel;
  this->physical_struct.tunnel_port_0      = p_is_tunnel;
  this->physical_struct.tunnel_port_1      = p_is_tunnel;
  this->physical_struct.icmp_flags_0       = p_is_icmp;
  this->physical_struct.tcp_flags_0        = p_is_tcp;
  this->physical_struct.anomaly_0          = true;
  // ASSIGN_VISITOR_END
  return;
}
// ASSIGN_METHOD_END


// -------------------------------------
// Friends
// -------------------------------------


ostream & operator<<(ostream & p_os, Flow const * p_flow) noexcept(true)
{
  //DEBUG(LOTS, ENTER);
  //DEBUG(LOTS, LEAVE);
  return(p_os << p_flow->createFlowString());
}


ostream & operator<<(ostream & p_os, Flow const & p_flow) noexcept(true)
{
  //DEBUG(LOTS, ENTER);
  //DEBUG(LOTS, LEAVE);
  return(p_os << p_flow.createFlowString());
}
