///////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2008.  PNNL.  All Rights Reserved. 
//
// THIS FILE INITIALLY CREATED WITH:  
//     TEMPLATE NAME:  lang_cpp_hdr.template 
//     COMMAND NAME:   gensrc 
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifndef FLOWKEY_HPP
#define FLOWKEY_HPP

// System Includes
#include <assert.h>
#include <iostream>
#include <string>
// External Includes
// Internal Includes
// Application Includes
#include "Ip.hpp"
#include "global.hpp"
#include "num2string.hpp"
#include "uchar2string.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "FlowKey"


#define FLOW_KEY_QWORD_COUNT 6


// Namespaces used
using namespace std;


union ip_sd_array
{
  u_int64_t   qword;
  struct
  {
    u_int32_t dst;
    u_int32_t src;
  };

};


typedef struct flow_key_ip_
{
    u_int16_t vlan_id;
    u_int8_t  unused2;
    u_int8_t  protocol;
    u_int32_t unused3;

    union ip_sd_array address[4];
} flow_key_ip_t;


typedef union norm_flow_key_
{

  u_int64_t qwords[FLOW_KEY_QWORD_COUNT];
  u_int32_t dwords[FLOW_KEY_QWORD_COUNT * 2];
  struct
  {
    union
    {
      u_int32_t dword;
      struct
      {
        u_int16_t src_port;
        u_int16_t dst_port;
      };
    };
    u_int32_t unused1;
    flow_key_ip_t ip;
  };

  inline bool operator==(norm_flow_key_ const & p_flow_key) const throw()
  {
    return(
           (this->src_port            == p_flow_key.src_port           ) &&
           (this->dst_port            == p_flow_key.dst_port           ) &&
           (this->ip.vlan_id          == p_flow_key.ip.vlan_id         ) &&
           (this->ip.protocol         == p_flow_key.ip.protocol        ) &&
           (this->ip.address[0].qword == p_flow_key.ip.address[0].qword) &&
           (this->ip.address[1].qword == p_flow_key.ip.address[1].qword) &&
           (this->ip.address[2].qword == p_flow_key.ip.address[2].qword) &&
           (this->ip.address[3].qword == p_flow_key.ip.address[3].qword)
          );
  }

  inline bool const operator<(norm_flow_key_ const & p_flow_key) const throw()
  {
    // DEVELOPER NOTE: The followin is not efficient -> Don't uncomment
    //if (*this == p_flow_key)
    //{
    //  return(false);
    //}

    if (this->src_port < p_flow_key.src_port)
    {
      return true;
    }
    if (this->src_port > p_flow_key.src_port)
    {
      return false;
    }


    if (this->dst_port < p_flow_key.dst_port)
    {
      return true;
    }
    if (this->dst_port > p_flow_key.dst_port)
    {
      return false;
    }


    if (this->ip.protocol < p_flow_key.ip.protocol)
    {
      return true;
    }
    if (this->ip.protocol > p_flow_key.ip.protocol)
    {
      return false;
    }

    if (this->ip.vlan_id < p_flow_key.ip.vlan_id)
    {
      return true;
    }
    if (this->ip.vlan_id > p_flow_key.ip.vlan_id)
    {
      return false;
    }

    for (int unsigned idx = 0; idx < 4; ++idx)
    {
      if (this->ip.address[idx].qword < p_flow_key.ip.address[idx].qword)
      {
        return(true);
      }
      if (this->ip.address[idx].qword > p_flow_key.ip.address[idx].qword)
      {
        return(false);
      }
    }

    return(false);
  }

} norm_flow_key_t;


typedef union frag_flow_key_
{

  u_int64_t qwords[FLOW_KEY_QWORD_COUNT];
  struct
  {
    u_int32_t unused1;
    u_int32_t ident;
  
    flow_key_ip_t ip;
  };
  

  inline bool operator==(frag_flow_key_ const & p_flow_key) const throw()
  {
    return(
           (this->ident               == p_flow_key.ident              ) &&
           (this->ip.vlan_id          == p_flow_key.ip.vlan_id         ) &&
           (this->ip.protocol         == p_flow_key.ip.protocol        ) &&
           (this->ip.address[0].qword == p_flow_key.ip.address[0].qword) &&
           (this->ip.address[1].qword == p_flow_key.ip.address[1].qword) &&
           (this->ip.address[2].qword == p_flow_key.ip.address[2].qword) &&
           (this->ip.address[3].qword == p_flow_key.ip.address[3].qword)
          );
  }

  inline bool const operator<(frag_flow_key_ const & p_flow_key) const throw()
  {
    // DEVELOPER NOTE: The following is not efficient -> Don't uncomment
    //if (*this == p_flow_key)
    //{
    //  return(false);
    //}


    if (this->ident < p_flow_key.ident)
    {
      return true;
    }
    if (this->ident > p_flow_key.ident)
    {
      return false;
    }

    //return(memcmp(&this->qwords[1], &p_flow_key.qwords[1], 40) < 0);

    if (this->ip.protocol < p_flow_key.ip.protocol)
    {
      return true;
    }
    if (this->ip.protocol > p_flow_key.ip.protocol)
    {
      return false;
    }

    if (this->ip.vlan_id < p_flow_key.ip.vlan_id)
    {
      return true;
    }
    if (this->ip.vlan_id > p_flow_key.ip.vlan_id)
    {
      return false;
    }

    for (int unsigned idx = 0; idx < 4; ++idx)
    {
      if (this->ip.address[idx].qword < p_flow_key.ip.address[idx].qword)
      {
        return(true);
      }
      if (ip.address[idx].qword > p_flow_key.ip.address[idx].qword)
      {
        return(false);
      }
    }

    return(false);
  }

} frag_flow_key_t;


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

union FlowKey
{
  // Variables

  u_int64_t qwords[FLOW_KEY_QWORD_COUNT];
  norm_flow_key_t norm_flow_key;
  frag_flow_key_t frag_flow_key;

  // Default Constructor

  FlowKey(void)
  {
    for(int unsigned i = 0; i < FLOW_KEY_QWORD_COUNT; ++i)
    {
      qwords[i] = 0;
    }

    return;
  };


  // Copy Constructor

  FlowKey(FlowKey const & p_flow_key) throw()
  {
    *this = p_flow_key;
    return;
  }

 
  // Destructor

  ~FlowKey(void)
  {
    return;
  }


  // Operators

  FlowKey & operator=(FlowKey const & p_flow_key) throw()
  {
    for(int unsigned i = 0; i < FLOW_KEY_QWORD_COUNT; ++i)
    {
      this->qwords[i] = p_flow_key.qwords[i];
    }

    return(*this);
  }


  inline u_int64_t const & at(int unsigned const idx) const throw()
  {
    assert(idx < FLOW_KEY_QWORD_COUNT);
    return qwords[idx];
  }

  inline u_int64_t & at(int unsigned const idx) throw()
  {
    assert(idx < FLOW_KEY_QWORD_COUNT);
    return qwords[idx];
  }


  inline bool operator==(FlowKey const & p_flow_key) const throw()
  {
    u_int8_t equal = 1;

    for(int unsigned i = 0; (i < FLOW_KEY_QWORD_COUNT) & equal; ++i)
    {
      equal &= (this->qwords[i] == p_flow_key.qwords[i]);
    }

    return(1 == equal);
  }


  inline bool operator!=(FlowKey const & p_flow_key) const throw()
  {
    return(! (*this == p_flow_key));
  }


  //static string createFlowKeyString(FlowKey const & p_flow_key) throw()
  inline string createFlowKeyString() const throw()
  {
    IpAddress_t src;
    IpAddress_t dst;
    src.dwords[0] = this->norm_flow_key.ip.address[3].src;
    src.dwords[1] = this->norm_flow_key.ip.address[2].src;
    src.dwords[2] = this->norm_flow_key.ip.address[1].src;
    src.dwords[3] = this->norm_flow_key.ip.address[0].src;
    dst.dwords[0] = this->norm_flow_key.ip.address[3].dst;
    dst.dwords[1] = this->norm_flow_key.ip.address[2].dst;
    dst.dwords[2] = this->norm_flow_key.ip.address[1].dst;
    dst.dwords[3] = this->norm_flow_key.ip.address[0].dst;
    string out = "";
    out += ":SP:"  + uitoa10(this->norm_flow_key.src_port);
    out += ":DP:"  + uitoa10(this->norm_flow_key.dst_port);
    out += ":VD:"  + uitoa10(this->norm_flow_key.ip.vlan_id);
    out += ":ID:"  + uitoa10(this->frag_flow_key.ident);
    out += ":PR:"  + uitoa10(this->frag_flow_key.ip.protocol);
    out += ":SIP:" + asIpv6(src);
    out += ":DIP:" + asIpv6(dst);
    return(out);
  }


  // Functions

  void buildFlowKeys(
                      bool        const    p_src_first = false,
                      u_int16_t   const    p_src_port  = 0,
                      u_int16_t   const    p_dst_port  = 0,
                      u_int16_t   const    p_ident     = 0,
                      u_int16_t   const    p_vlan_id   = 0,
                      u_int8_t    const    p_protocol  = 0,
                      IpAddress_t const &  p_src_ip    = IpAddress_t(),
                      IpAddress_t const &  p_dst_ip    = IpAddress_t()
                    ) throw();


friend
  ostream & operator<<(ostream & p_os, union FlowKey const * p_flow_key) throw();
friend
  ostream & operator<<(ostream & p_os, union FlowKey const & p_flow_key) throw();

};

#endif // FLOWKEY_HPP
