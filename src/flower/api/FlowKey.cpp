///////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2008.  PNNL.  All Rights Reserved. 
//
// THIS FILE INITIALLY CREATED WITH:  
//     TEMPLATE NAME:  lang_cpp_class.template 
//     COMMAND NAME:   gensrc 
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
// Internal Includes
// Application Includes
#include "FlowKey.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "FlowKey"


// Namespaces used


//==============================================================================
// Class FlowKey
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


// ======================================================================
// Operators
// ======================================================================


void FlowKey::buildFlowKeys(
                            bool        const p_src_first,
                            u_int16_t   const p_src_port,
                            u_int16_t   const p_dst_port,
                            u_int16_t   const p_ident,
                            u_int16_t   const p_vlan_id,
                            u_int8_t    const p_protocol,
                            IpAddress_t const &  p_src_ip,
                            IpAddress_t const &  p_dst_ip
                           ) throw()
{

  this->norm_flow_key.qwords[1]   = 0;
  this->norm_flow_key.src_port    = p_src_port;
  this->norm_flow_key.dst_port    = p_dst_port;
  this->frag_flow_key.ident       = p_ident;
  this->norm_flow_key.ip.vlan_id  = p_vlan_id;
  this->norm_flow_key.ip.protocol = p_protocol;

  IpAddress_t const * two = &p_src_ip;
  IpAddress_t const * one = &p_dst_ip;

  if (p_src_first)
  {
    one = &p_src_ip;
    two = &p_dst_ip;

    this->norm_flow_key.src_port   = p_dst_port;
    this->norm_flow_key.dst_port   = p_src_port;
  }

  this->norm_flow_key.ip.address[0].dst = one->dwords[3];
  this->norm_flow_key.ip.address[0].src = two->dwords[3];

  this->norm_flow_key.ip.address[1].dst = one->dwords[2];
  this->norm_flow_key.ip.address[1].src = two->dwords[2];

  this->norm_flow_key.ip.address[2].dst = one->dwords[1];
  this->norm_flow_key.ip.address[2].src = two->dwords[1];

  this->norm_flow_key.ip.address[3].dst = one->dwords[0];
  this->norm_flow_key.ip.address[3].src = two->dwords[0];

  return;
}


// -------------------------------------
// Friends
// -------------------------------------


ostream & operator<<(ostream & p_os, FlowKey const * p_flow_key) throw()
{
  //DEBUG(LOTS, ENTER);
  //DEBUG(LOTS, LEAVE);
  return(p_os << p_flow_key->createFlowKeyString());
}


ostream & operator<<(ostream & p_os, FlowKey const & p_flow_key) throw()
{
  //DEBUG(LOTS, ENTER);
  //DEBUG(LOTS, LEAVE);
  return(p_os << p_flow_key.createFlowKeyString());
}
