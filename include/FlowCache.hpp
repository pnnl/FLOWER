///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifndef FLOWCACHE_HPP
#define FLOWCACHE_HPP


// System Includes
#include <algorithm>  // for:  copy
#include <iostream>
#include <iterator>
#include <string>
// External Includes
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
// Internal Includes
// Application Includes
#include "EventTypes.hpp"
#include "ExceptionHandler.hpp"
#include "Flow.hpp"
#include "ObjectPool.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "FlowCache"


// Namespaces used
using namespace std;
using namespace boost;
using namespace boost::multi_index;
using boost::multi_index_container;


//==============================================================================
// Class FlowCache
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


class FlowCache
{

private:

  // Typedefs

  struct NormFlowKey  {};
  struct Timeout      {};
  struct FragFlowKey  {};

  // DEVELOPER NOTE: The Timeout index must be non_unique
  //                 because we cannot guarantee that libpcap will return
  //                 unique timestamps or not.

  // DEVELOPER NOTE: The frag_flow_multi_index_container is used to assemble
  //                 fragmented Fragmented packets into a Fragmented Flow with
  //                 the same source port and destination port. From there,
  //                 the assembled flow can be added to or merged with
  //                 existing flows in the norm_flow_cache.

  typedef multi_index_container
  <
    sharedFlow,
    indexed_by
    <
      ordered_unique<     tag<FragFlowKey>,  BOOST_MULTI_INDEX_MEM_FUN      (Flow, frag_flow_key_t &, getFragFlowKey), less< frag_flow_key_t > >,
      ordered_non_unique< tag<Timeout>,      BOOST_MULTI_INDEX_CONST_MEM_FUN(Flow, u_int32_t,         getTimeout),     greater<u_int32_t> >
    >
  > frag_flow_multi_index_container;


  typedef multi_index_container
  <
    sharedFlow,
    indexed_by
    <
      ordered_unique<     tag<NormFlowKey>,  BOOST_MULTI_INDEX_MEM_FUN      (Flow, norm_flow_key_t &, getNormFlowKey), less< norm_flow_key_t > >,
      ordered_non_unique< tag<Timeout>,      BOOST_MULTI_INDEX_CONST_MEM_FUN(Flow, u_int32_t,         getTimeout),     greater<u_int32_t> >
    >
  > norm_flow_multi_index_container;


  // Getters/Setters

  inline u_int32_t getVirtualTime(void) const throw()
  {
    return(this->virtual_time);
  }


  inline void setVirtualTime(u_int32_t const p_virtual_time) throw()
  {
    this->virtual_time = p_virtual_time;
    return;
  }


  inline int unsigned getMaxFragFlowCacheSize(void) const throw()
  {
    return(this->max_frag_flowcache_size);
  }


  inline int unsigned getMaxNormFlowCacheSize(void) const throw()
  {
    return(this->max_norm_flowcache_size);
  }


  inline frag_flow_multi_index_container & getFragFlowCache(void) throw() // Cannot be const
  {
    return(this->frag_flow_cache);
  }


  inline norm_flow_multi_index_container & getNormFlowCache(void) throw() // Cannot be const
  {
    return(this->norm_flow_cache);
  }


  inline int unsigned getNormFlowCacheSize(void) const throw()
  {
    return(this->norm_flow_cache.size());
  }


  inline int unsigned getFragFlowCacheSize(void) const throw()
  {
    return(this->frag_flow_cache.size());
  }


  inline SummaryAddEvent * getSummaryAddEvent(void) const throw()
  {
    return(this->summary_add_event);
  }


  inline void setSummaryAddEvent(SummaryAddEvent * const p_summary_add_event) throw()
  {
    this->summary_add_event = p_summary_add_event;
  }


  // Constructors

  FlowCache(void);
  FlowCache(FlowCache const & p_norm_flow_cache) throw();


  // Operators

  FlowCache &  operator=(FlowCache const & p_norm_flow_cache);
  bool        operator==(FlowCache const & p_norm_flow_cache) const;
  bool        operator!=(FlowCache const & p_norm_flow_cache) const;


  // Variables

  frag_flow_multi_index_container frag_flow_cache;
  norm_flow_multi_index_container norm_flow_cache;
  u_int64_t                       frag_flow_count;
  u_int64_t                       merge_count;
  u_int64_t                       norm_flow_count;
  u_int64_t                       summary_count;
  SummaryAddEvent *               summary_add_event;
  ObjectPool<Flow> &              flow_pool;
  int unsigned                    max_frag_flowcache_size;
  int unsigned                    max_norm_flowcache_size;
  u_int32_t                       virtual_time;


  // Encapsulated Behavior

  bool add(sharedFlow const & p_flow) throw();
  bool real_add(sharedFlow const & p_flow) throw();
  bool real_frag_add(sharedFlow const & p_flow) throw();
  void flushAndZero(norm_flow_multi_index_container::index<NormFlowKey>::type::iterator & p_itor) throw();
  void reset(int unsigned const p_level = 0) throw();

  template<typename Tag, typename MultiIndexContainer>
  void printBy(MultiIndexContainer const & p_flow_cache, Tag * = 0) throw();
  
  template<typename MultiIndexContainer> 
  void flush(MultiIndexContainer & p_flow_cache, bool const p_is_normal_cache, u_int32_t const p_virtual_time, bool const p_shutdown = false) throw();


public:

  // Getters/Setters

  inline int unsigned getMaxSize(void) const throw()
  {
    return(getMaxNormFlowCacheSize());
  }


  inline int unsigned size(void) const throw()
  {
    return(getNormFlowCacheSize());
  }


  inline u_int64_t getFragFlowCount(void) const throw()
  {
    return(this->frag_flow_count);
  }


  inline void resetFragFlowCount(void) throw()
  {
    this->frag_flow_count = 0;
    return;
  }


  inline void updateFragFlowCount(void) throw()
  {
    if (getFragFlowCacheSize() > getFragFlowCount())
    {
      this->frag_flow_count = getFragFlowCacheSize();
    }
    return;
  }


  inline u_int64_t getMergeCount(void) const throw()
  {
    return(this->merge_count);
  }


  inline void incrMergeCount(void) throw()
  {
    ++this->merge_count;
    return;
  }


  inline void resetMergeCount(void) throw()
  {
    this->merge_count = 0;
    return;
  }


  inline u_int64_t getNormFlowCount(void) const throw()
  {
    return(this->norm_flow_count);
  }


  inline void resetNormFlowCount(void) throw()
  {
    this->norm_flow_count = 0;
    return;
  }


  inline void updateNormFlowCount(void) throw()
  {
    if (getNormFlowCacheSize() > getNormFlowCount())
    {
      this->norm_flow_count = getNormFlowCacheSize();
    }
    return;
  }


  inline u_int64_t getSummaryCount(void) const throw()
  {
    return(this->summary_count);
  }


  inline void incrSummaryCount(void) throw()
  {
    ++this->summary_count;
    return;
  }


  inline void resetSummaryCount(void) throw()
  {
    this->summary_count = 0;
    return;
  }


  // Constructors

  FlowCache(SummaryAddEvent * const p_summary_add_event, ObjectPool<Flow> & p_flow_pool, int unsigned const p_max_frag_flowcache_size, int unsigned const p_max_norm_flowcache_size) throw();


  // Destructor

  ~FlowCache(void) throw()
  {
    return;
  }


  // Iterators

  typedef norm_flow_multi_index_container::index<NormFlowKey>::type::iterator norm_flow_cache_flowkey_itor;
  typedef frag_flow_multi_index_container::index<FragFlowKey>::type::iterator frag_flow_cache_flowkey_itor;


  // Public Functions

  bool findByNormFlowKey(sharedFlow const & p_flow, norm_flow_cache_flowkey_itor & p_itor) throw();
  bool findByFragFlowKey(sharedFlow const & p_flow, frag_flow_cache_flowkey_itor & p_itor) throw();
  void printByNormFlowKey(void) throw();


  // Event-based behavior interface Functions

  void   onAddEvent(sharedFlow const p_flow) throw();
  string onMetricsEvent(int unsigned const p_level) throw();
  void   onNewInputEvent(int unsigned const p_level) throw();
  void   onNewOutputEvent(int unsigned const p_level) throw();
  void   onShutdownSystemEvent(int unsigned const p_level) throw();

};

#endif // FLOWCACHE_HPP
