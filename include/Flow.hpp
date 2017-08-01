///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifndef FLOW_HPP
#define FLOW_HPP

// System Includes
#include <iostream>
#include <memory>
#include <string>
// External Includes
// Internal Includes
// Application Includes
#include "Field/Data.hpp"
#include "Field/FieldCommon.hpp"
#include "Field/FieldMergePolicy.hpp"
#include "Field/FieldToStringPolicy.hpp"
#include "Field/FieldZeroOutPolicy.hpp"
#include "FlowKey.hpp"
#include "timeUtil.hpp"

#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Flow"


// Namespaces used
using namespace std;
using namespace boost;
using namespace boost::posix_time;


// Data Structure
// PHYSICAL_STRUCT_START
typedef struct PhysicalStruct_t
{
  Data< Elf_t, 1>        elf_flags;         //          72 bytes;   // end   of 2nd cache line (see Flow Member Vars)

  Data< IpAddress_t,  2> ip_address;        // 2 * 16 = 32 bytes;   // start of 3rd line
  Data< IpAddress_t,  2> tunnel_address;    // 2 * 16 = 32 bytes;   // end   of 3rd line

  Data<u_int32_t, 4>     time_duration;     // 4 * 4  = 16 bytes;   // start of 4th line
  Data<u_int64_t, 4>     payload_bytes;     // 4 * 8  = 32 bytes;
  Data<u_int64_t, 2>     total_bytes;       // 2 * 8  = 16 bytes;   // end   of 4th line

  Data<u_int64_t, 2>     packet_count;      // 2 * 8  = 16 bytes;   // start of 5th line
  Data<u_int64_t, 2>     ip_option_count;   // 2 * 8  = 16 bytes;
  Data<u_int64_t, 2>     tcp_option_count;  // 2 * 8  = 16 bytes;
  Data<u_int32_t, 4>     tcp_initial_seq;   // 4 * 4  = 16 bytes;   // end   of 5th line

  Data<u_int32_t, 4>     tcp_last_seq;      // 4 * 4  = 16 bytes;   // start of 6th line
  Data<u_int32_t, 4>     hist_tcp_last_seq; // 4 * 4  = 16 bytes;
  Data<u_int64_t, 2>     tcp_retrans_count; // 2 * 8  = 16 bytes;
  Data<u_int16_t, 2>     ip_port;           // 2 * 2  =  4 bytes;
  Data<u_int16_t, 2>     icmp_flag_sum;     // 2 * 2  =  4 bytes;
  Data<u_int16_t, 2>     tcp_flag_sum;      // 2 * 2  =  4 bytes;
  Data<u_int16_t, 2>     tunnel_port;       // 2 * 2  =  4 bytes;   // end   of 6th line
  
  struct                                    // 1 * 8  =  8 bytes;   // start of 7th line
  {
     u_int64_t  time_duration_0:1;
     u_int64_t  time_duration_1:1;
     u_int64_t  time_duration_2:1;
     u_int64_t  time_duration_3:1;
     u_int64_t  payload_bytes_0:1;
     u_int64_t  payload_bytes_1:1;
     u_int64_t  total_bytes_0:1;
     u_int64_t  total_bytes_1:1;
     u_int64_t  packet_count_0:1;
     u_int64_t  packet_count_1:1;
     u_int64_t  ip_option_count_0:1;
     u_int64_t  ip_option_count_1:1;
     u_int64_t  tcp_option_count_0:1;
     u_int64_t  tcp_option_count_1:1;
     u_int64_t  tcp_initial_seq_0:1;
     u_int64_t  tcp_initial_seq_1:1;
     u_int64_t  tcp_initial_seq_2:1;
     u_int64_t  tcp_initial_seq_3:1;
     u_int64_t  tcp_last_seq_0:1;
     u_int64_t  tcp_last_seq_1:1;
     u_int64_t  tcp_last_seq_2:1;
     u_int64_t  tcp_last_seq_3:1;
     u_int64_t  tcp_retrans_count_0:1;
     u_int64_t  tcp_retrans_count_1:1;
     u_int64_t  ip_port_0:1;
     u_int64_t  ip_port_1:1;
     u_int64_t  icmp_flag_sum_0:1;
     u_int64_t  icmp_flag_sum_1:1;
     u_int64_t  tcp_flag_sum_0:1;
     u_int64_t  tcp_flag_sum_1:1;
     u_int64_t  vlan_id_0:1;
     u_int64_t  ip_protocol_0:1;
     u_int64_t  frag_type_0:1;
     u_int64_t  frag_type_1:1;
     u_int64_t  frag_type_2:1;
     u_int64_t  fragment_0:1;
     u_int64_t  ip_address_0:1;
     u_int64_t  ip_address_1:1;
     u_int64_t  tunnel_depth_0:1;
     u_int64_t  tunnel_address_0:1;
     u_int64_t  tunnel_address_1:1;
     u_int64_t  tunnel_protocol_0:1;
     u_int64_t  tunnel_port_0:1;
     u_int64_t  tunnel_port_1:1;
     u_int64_t  icmp_flags_0:1;
     u_int64_t  tcp_flags_0:1;
     u_int64_t  anomaly_0:1;
  };

  Data<u_int16_t, 1>     vlan_id;           // 1 * 2  =  2 bytes;
  Data<u_int8_t,  1>     ip_protocol;       // 1 * 1  =  1 bytes;
  Data<u_int8_t,  1>     tunnel_protocol;   // 1 * 1  =  1 bytes;
  Data<u_int8_t,  3>     frag_type;         // 3 * 1  =  3 bytes;
  Data<u_int8_t,  1>     fragment;          // 1 * 1  =  1 bytes;    
  Data<u_int32_t, 1>     anomaly;           // 1 * 4  =  4 bytes;
  Data<u_int8_t,  1>     tunnel_depth;      // 1 * 1  =  1 bytes;   // 21 bytes into 7th line
  u_int8_t               pad[3];            // 1 * 3  =  3 bytes;   // 24 bytes into 7th line

} PhysicalStruct;
// PHYSICAL_STRUCT_END


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


class Flow
{

public:

  // Getters/Setters

  inline void setFragType(bool const p_more_flag, bool const p_shutdown_flag = false) throw()
  {
    u_int8_t more_flag = this->physical_struct.frag_type.at(0);
    this->physical_struct.frag_type.assign(p_more_flag, more_flag, p_shutdown_flag);
    return;
  }


  inline u_int32_t getVirtualTime(void) const throw()
  {
    return(this->physical_struct.time_duration.at(2));
  }


  inline u_int32_t getTimeout(void) const throw()
  {
    return(this->timeout);
  }


  inline u_int32_t getForceout(void) const throw()
  {
    return(this->forceout);
  }


  inline void setForceout(u_int32_t const p_forceout) throw()
  {
    this->forceout = p_forceout + getCacheForceout();
    return;
  }


  inline FlowKey & getFlowKey(void) throw() // cannot be const
  {
    return(this->flow_key);
  }


  inline norm_flow_key_t & getNormFlowKey(void) throw() // cannot be const
  {
    return(this->flow_key.norm_flow_key);
  }


  inline frag_flow_key_t & getFragFlowKey(void) throw() // cannot be const
  {
    return(this->flow_key.frag_flow_key);
  }


  inline void setNormFlowKey(norm_flow_key_t const & p_norm_flow_key) throw()
  {
    this->flow_key.norm_flow_key = p_norm_flow_key;
    return;
  }


  inline bool isFake(void) const throw()
  {
    return(getFake());
  }


  inline bool isTcp(void) const throw()
  {
    return(this->is_tcp);
  }


  inline bool isIcmp(void) const throw()
  {
    return(this->is_icmp);
  }


  inline bool isIpFrag(void) const throw()
  {
    return(this->is_ip_frag);
  }


  inline bool isFirstIpFrag(void) const throw()
  {
    return(this->is_first_ip_frag);
  }


  inline bool isLastIpFrag(void) const throw()
  {
    return(this->is_last_ip_frag);
  }


  inline bool isIpv4(void) const throw()
  {
    return(this->is_ipv4);
  }


  // Constructors

  Flow(
       bool const         p_is_fake,
       FlowKey const &    p_flow_key         = FlowKey(),
       u_int32_t const    p_first_timestamp  = getEpoch(),
       int unsigned const p_timeout          = getEpoch(),
       int unsigned const p_forceout         = 0,
       bool const         p_flow_direction   = false,
       bool const         p_is_tcp           = false,
       bool const         p_is_icmp          = false,
       bool const         p_is_ip_frag       = false,
       bool const         p_is_first_ip_frag = false,
       bool const         p_is_last_ip_frag  = false,
       bool const         p_is_ipv4          = false
      ) throw();
  Flow(Flow const & p_flow) throw();


  // Destructor

  ~Flow(void) throw()
  {
    return;
  }


  // Operators
  

  // Public Functions

  void      assign(
                    bool const         p_is_fake,
                    FlowKey const &    p_flow_key         = FlowKey(),
                    u_int32_t const    p_first_timestamp  = getEpoch(),
                    int unsigned const p_timeout          = getEpoch(),
                    int unsigned const p_forceout         = 0,
                    bool const         p_flow_direction   = false,
                    bool const         p_is_tcp           = false,
                    bool const         p_is_icmp          = false,
                    bool const         p_is_ip_frag       = false,
                    bool const         p_is_first_ip_frag = false,
                    bool const         p_is_last_ip_frag  = false,
                    bool const         p_is_ipv4          = false
                  ) throw();

  Flow &    copyFlow(Flow const & p_flow) throw();
  bool      compare(std::shared_ptr<Flow> const & p_flow) throw();
  string    createFlowString(void) const throw();
  void      forceFake(u_int32_t const p_first_timestamp) throw();
  void      fragment_pre_merge(std::shared_ptr<Flow> const & p_flow) throw();
  bool      merge(std::shared_ptr<Flow> const & p_flow) throw();
  ostream & printFlow(ostream& os) const throw();
  void      zero(void) throw();


  // ASSIGN_PROTOTYPE_START
  void assignFields
  (
    u_int32_t const                p_packet_sec            = getEpoch(),
    u_int32_t const                p_packet_usec           = getEpoch(),
    u_int16_t const                p_vlan_id               = 0,
    u_int8_t const                 p_ip_protocol           = 0,
    IpAddress_t const &            p_src_ip_address        = IpAddress_t(),
    IpAddress_t const &            p_dst_ip_address        = IpAddress_t(),
    u_int64_t const                p_payload               = 0,
    u_int64_t const                p_total_bytes           = 0,
    u_int64_t const                p_packet_count          = 0,
    u_int64_t const                p_ip_opt                = 0,
    u_int16_t const                p_src_port              = 0,
    u_int16_t const                p_dst_port              = 0,
    u_int64_t const                p_tcp_opt               = 0,
    bool      const                p_is_tcp                = false,
    u_int16_t const                p_flag_sum              = 0,
    bool      const                p_is_icmp               = false,
    u_int32_t const                p_tcp_seq               = 0,
    u_int32_t const                p_tcp_opt_tsv           = 0,
    bool      const                p_is_tunnel             = false,
    u_int8_t const                 p_tunnel_depth          = 0,
    IpAddress_t const &            p_tunnel_src_ip_address = IpAddress_t(),
    IpAddress_t const &            p_tunnel_dst_ip_address = IpAddress_t(),
    u_int8_t const                 p_tunnel_ip_protocol    = 0,
    u_int16_t const                p_tunnel_src_port       = 0,
    u_int16_t const                p_tunnel_dst_port       = 0,
    bool const                     p_fragment              = 0,
    u_int32_t const                p_anomaly               = 0
  ) throw();
  // ASSIGN_PROTOTYPE_END

private:

  // Getters/Setters

  inline bool getFake(void) const throw()
  {
    return(this->fake);
  }


  inline bool getFlowDirection(void) const throw()
  {
    return(this->flow_direction);
  }


  inline void setTimeout(u_int32_t const p_timeout) throw()
  {
    this->timeout = p_timeout;
    return;
  }


  inline void setVirtualTime(u_int32_t const p_time) throw()
  {
    this->physical_struct.time_duration.assign(p_time, 0, p_time, 0);
    return;
  }


  // Constructors

  Flow(void) throw();

  // Operators
  Flow &    operator=(Flow const & p_flow) throw();

friend
  ostream & operator<<(ostream & p_os, Flow const * p_flow) throw();
friend
  ostream & operator<<(ostream & p_os, Flow const & p_flow) throw();

  bool      operator<(Flow const & p_flow) const throw();
  bool      operator>(Flow const & p_flow) const throw();


 // Variables
public:
//private: // TODO: change back to private after optimizing data layout
  FlowKey        flow_key;                   // 48 bytes
  u_int32_t      timeout;                    //  4 bytes
  u_int32_t      forceout;                   //  4 bytes
  PhysicalStruct physical_struct;            


  struct                                     //  2 bytes
  {
    u_int16_t   fake:1;                       
    u_int16_t   flow_direction:1;             
    u_int16_t   is_tcp:1;                     
    u_int16_t   is_icmp:1;                    
    u_int16_t   is_ip_frag:1;                 
    u_int16_t   is_first_ip_frag:1;            
    u_int16_t   is_last_ip_frag:1;            
    u_int16_t   is_ipv4:1;            
  };                                         // end of 7th line - 38 bytes  416 bytes total because of compiler

  // DEVELOPER NOTE: Optimization trial and error with padding
  //   Adding padding to the end of the struct actually made the overall code run
  //   slower. Add
  // u_int8_t   final_pad[6];                // end of 7th line - 32 bytes  416 bytes total
  // u_int8_t   final_pad[32];               // end of 7th line             448 bytes total
  // u_int8_t   final_pad[96];               // end of 8th line             512 bytes total
  // 
  // final_pad[0]                     450-497k pkt per sec
  // final_pad[6]                     446-488k pkt per sec
  // final_pad[32]                    424-468k pkt per sec
  // final_pad[96]                    448-496k pkt per sec

};

#undef  GET

#endif // FLOW_HPP
