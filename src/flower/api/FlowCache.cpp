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
#include <locale>
// External Includes
// Internal Includes
// Application Includes
#include "osUtil.hpp"
#include "FlowCache.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "FlowCache"


// Namespaces used


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


// ======================================================================
// Constructors
// ======================================================================


FlowCache::FlowCache(SummaryAddEvent * const p_summary_add_event, ObjectPool<Flow> & p_flow_pool, int unsigned const p_max_frag_flowcache_size, int unsigned const p_max_norm_flowcache_size) throw() :
  frag_flow_count(0),
  merge_count(0),
  norm_flow_count(0),
  summary_count(0),
  summary_add_event(p_summary_add_event),
  flow_pool(p_flow_pool),
  max_frag_flowcache_size(p_max_frag_flowcache_size),
  max_norm_flowcache_size(p_max_norm_flowcache_size),
  virtual_time(getEpoch())
{
  DEBUG(LOTS, ENTER);
  DEBUG(LOTS, LEAVE);
  return;
}


// ======================================================================
// Public Functions
// ======================================================================

bool FlowCache::findByNormFlowKey(sharedFlow const & p_flow, norm_flow_cache_flowkey_itor & p_itor) throw()
{
  DEBUG(TRACE, ENTER);
  
  bool result = true;
  
  p_itor = getNormFlowCache().get<NormFlowKey>().find(p_flow->getNormFlowKey());
  if (getNormFlowCache().get<NormFlowKey>().end() == p_itor)
  {
    result = false;
  }
  
  DEBUG(SCALAR, "Result:%d:", result);
  DEBUG(TRACE, LEAVE);
  return(result);
}


bool FlowCache::findByFragFlowKey(sharedFlow const & p_flow, frag_flow_cache_flowkey_itor & p_itor) throw()
{
  DEBUG(TRACE, ENTER);

  bool result = true;

  p_itor = getFragFlowCache().get<FragFlowKey>().find(p_flow->getFragFlowKey());
  if (getFragFlowCache().get<FragFlowKey>().end() == p_itor)
  {
    result = false;
  }

  DEBUG(SCALAR, "Result:%d:", result);
  DEBUG(TRACE, LEAVE);
  return(result);
}


void FlowCache::flushAndZero(norm_flow_cache_flowkey_itor & p_itor) throw()
{
  DEBUG(TRACE, ENTER); 
  (*p_itor)->setFragType(true);
  getSummaryAddEvent()->call(*p_itor); 
  incrSummaryCount();
  (*p_itor)->zero(); 
  (*p_itor)->setForceout(getVirtualTime()); 
  DEBUG(TRACE, LEAVE); 
  return;
}


bool FlowCache::add(sharedFlow const & p_flow) throw() 
{
  DEBUG(TRACE, ENTER); 
 
  bool result = false;

  // DEVELOPER NOTE: For every new flow we check 
  //                    - flush the cache of any idle flows that have timed out 
  //                    - If there is a matching flow_key then 
  //                    -   If the existing flow in the cache has been active
  //                        too long (forceout condition) then 
  //                    -     set continuation and more flags 
  //                    -     flush the flow from the cache but don't delete it 
  //                    -     zero out the values in the existing flow 
  //                    -   merge the new flow with the existing flow 
  //                    - Else 
  //                    -   add the new flow to the cache 
 
  //  Need to update the cache's virtual time
  setVirtualTime(max(getVirtualTime(), p_flow->getVirtualTime()));
  flush(getNormFlowCache(), true,  getVirtualTime());
  flush(getFragFlowCache(), false, getVirtualTime());

  if (p_flow->isFake())
  {
#ifndef NDEBUG
    CAUTION(FakeData, "FlowCache", "Received FAKE FLOW");
#endif
    getSummaryAddEvent()->call(p_flow);
    DEBUG(TRACE, LEAVE);
    return(true);
  }

  if (p_flow->isIpFrag()) 
  { 
    // DEVELOPER NOTE:
    //   IP Fragments are a particularly difficult problem and we have observed
    //   behaviors such as out of order fragments and request/response Flows
    //   that have the same flow key and fragment id. We contend that we handle
    //   IP fragmentation correctly without causing undue performance problems.
    //   There are 6 cases in particular that characterize handling IP fragments.
    //   Definitions:
    //     - Top Frag = The IP fragment that has the source and destination
    //                  port numbers and an IP Ident number.
    //   Case 1: p_flow is the Top Frag, there is an existing frag in the frag
    //           cache (*frag_itor), and the matching frag is also a Top Frag.
    //           This implies that p_flow is an exact duplicate packet or that
    //           a frag packet from system A to B had a src/dst pair and ip ident 
    //           and a frag packet from system B to A had the same src/dst/pair
    //           and ip ident. We have observed both of these conditions.
    //   Case 2: p_flow is the Top Frag, there is an existing frag in the frag
    //           cache (*frag_itor), but the matching frag is NOT a Top Frag.
    //           This indicates that the fragments were received out of order.
    //   Case 3: p_flow is the Top Frag, there is NOT an existing frag in the
    //           frag cache (*frag_itor)
    //           This indicates the frag is the first frag => NORMAL CASE
    //   Case 4: p_flow is the NOT the Top Frag, there is an existing frag in
    //           the frag cache (*frag_itor), and the matching frag is a Top
    //           Frag.
    //           This indicates the frag is a middle or end frag => NORMAL CASE
    //   Case 5: p_flow is the NOT the Top Frag, there is an existing frag in
    //           the frag cache (*frag_itor), and the matching frag is NOT a
    //           Top Frag.
    //           This indicates the frag is a middle or end frag and the TOP
    //           frag has not been seen yet.
    //   Case 6: p_flow is the NOT the Top Frag, there is NOT an existing frag
    //           in the frag cache (*frag_itor)
    //           This indicates the frag is a middle or end frag and the TOP
    //           frag has not been seen yet.
    //           This indicates the frag is a middle or end frag and the TOP
    //           frag has not been seen yet.
   
//cout << "KEY  :" << p_flow->getFlowKey() << endl;
    frag_flow_cache_flowkey_itor frag_itor; 
    if (findByFragFlowKey(p_flow, frag_itor)) // Do I match any other Frags in the FragCache
    {
      // I MATCHED SOMETHING IN THE CACHE
      if (p_flow->isFirstIpFrag())
      {
        if (! (*frag_itor)->isFirstIpFrag())
        {
          //cout << "Case 2" << endl;
          p_flow->fragment_pre_merge(*frag_itor);
          p_flow->merge(*frag_itor);
          (*frag_itor)->copyFlow(*p_flow);
          // p_flow => Fall Through
        }
        else
        {
          //cout << "Case 1 - Duplicate/Recycled Ident" << endl;
          //cout << "PFLOW:" << p_flow->getFlowKey() << endl;
          //cout << "FITOR:" << (*frag_itor)->getFlowKey() << endl;
          // p_flow => Fall Through
        }
      }
      else
      {
        if ((*frag_itor)->isFirstIpFrag())
        {
          //cout << "Case 4" << endl;
          p_flow->setNormFlowKey((*frag_itor)->getNormFlowKey());
          // p_flow => Fall Through
        }
        else
        {
          //cout << "Case 5" << endl;
          // DEVELOPER NOTE: We have to make a copy of the matching flow in
          //                 the cache so that we can remove the match in the
          //                 cache, update the values in the match with the
          //                 merge, and add the merged flow back in the cache
          //                 so that all mulit_index_containter indices are
          //                 updated correctly using insertion sort.
          sharedFlow temp = (*frag_itor);
          getFragFlowCache().get<FragFlowKey>().erase(frag_itor);
          if (temp->merge(p_flow))
          {
            result = real_frag_add(temp);
          }
          else
          {
            EERROR(e_error_fc_cant_merge_frag_flow, IllegalValue, "FlowKey (" + temp->getFlowKey().createFlowKeyString() + ")", "Flows cannot be merged");
          }
          this->flow_pool.release(p_flow);
          return(result);
        }
      }
    }
    else
    {
      // I DID NOT MATCHED ANYTHING IN THE CACHE
      if (! p_flow->isFirstIpFrag())
      {
        //cout << "Case 6" << endl;
        // Add new flow to frag cache
        return(real_frag_add(p_flow));
      }
      else
      {
        //cout << "Case 3" << endl;
        sharedFlow temp_flow;
        if (this->flow_pool.acquire(temp_flow))
        {
          temp_flow->copyFlow(*p_flow);
          // Add new flow to frag cache
          real_frag_add(temp_flow);
        }
        // p_flow => Fall Through
      }
    }
    //cout << "IDENT:" << p_flow->getFlowKey().frag_flow_key.ident << endl;
  }


  norm_flow_cache_flowkey_itor norm_itor; 
  if (findByNormFlowKey(p_flow, norm_itor)) 
  { 
    bool zeroed = false;
    if (getVirtualTime() > (*norm_itor)->getForceout()) 
    { 
      flushAndZero(norm_itor); 
      zeroed = true;
    } 
    // DEVELOPER NOTE: We have to make a copy of the matching flow in
    //                 the cache so that we can remove the match in the
    //                 cache, update the values in the match with the
    //                 merge, and add the merged flow back in the cache
    //                 so that all mulit_index_containter indices are
    //                 updated correctly using insertion sort.
    sharedFlow temp = (*norm_itor);
    getNormFlowCache().get<NormFlowKey>().erase(norm_itor);
    if (temp->merge(p_flow))
    {
      incrMergeCount();
      result = real_add(temp);
    }
    else
    {
      EERROR(e_error_fc_cant_merge_norm_flow, IllegalValue, "FlowKey (" + temp->getFlowKey().createFlowKeyString() + ")", "Flows cannot be merged");
    }
    this->flow_pool.release(p_flow);
    return(result);
  }
  
  DEBUG(TRACE, LEAVE); 
  return(real_add(p_flow));
}


bool FlowCache::real_frag_add(sharedFlow const & p_flow) throw()
{
  if (getMaxFragFlowCacheSize() > getFragFlowCacheSize())
  {
    // Add new flow to cache
    pair<frag_flow_cache_flowkey_itor, bool> result = getFragFlowCache().insert(p_flow);
    if (result.second)
    {
      updateFragFlowCount();
      return(true);
    }
    // DEVELOPER NOTE:  Add code to test the number of times the insert
    //                  fails and exit if N inserts fail. This could be
    //                  done with N insert failures in a row or N insert
    //                  failures over time, or percentage of failures.
    EERROR(e_error_fc_cant_add_frag_flow, IllegalValue, "Add Flow to FragFlowCache", ("FragFlowCache::add: Multi Index Container:insert failed for flow(" + p_flow->createFlowString() + "): because of: (" + (*result.first)->createFlowString() + ")").c_str());
  }
  else
  {
    EERROR(e_error_fc_frag_cache_full, RangeError, "Trying to add new flow to frag flow cache", "Cannot add any more flows to the flow_cache because it is already full");
  }
  this->flow_pool.release(p_flow);
  return(false);
}


bool FlowCache::real_add(sharedFlow const & p_flow) throw() 
{
  if (getMaxNormFlowCacheSize() > getNormFlowCacheSize())
  {
    // Add new flow to cache
    pair<norm_flow_cache_flowkey_itor, bool> result = getNormFlowCache().insert(p_flow);
    if (result.second)
    {
      updateNormFlowCount();
      return(true);
    }
    // DEVELOPER NOTE:  Add code to test the number of times the insert
    //                  fails and exit if N inserts fail. This could be
    //                  done with N insert failures in a row or N insert
    //                  failures over time, or percentage of failures.
    EERROR(e_error_fc_cant_add_norm_flow, IllegalValue, "Add Flow to FlowCache", ("FlowCache::add: Multi Index Container:insert failed for flow(" + p_flow->createFlowString() + "): because of: (" + (*result.first)->createFlowString() + ")").c_str());
  }
  else
  {
    EERROR(e_error_fc_norm_cache_full, RangeError, "Trying to add new flow to flow cache", "Cannot add any more flows to the flow_cache because it is already full");
  }
  this->flow_pool.release(p_flow);
  return(false);
}


template<typename MultiIndexContainer>
void FlowCache::flush(MultiIndexContainer & p_flow_cache, bool const p_is_normal_cache, u_int32_t const p_virtual_time, bool const p_shutdown) throw()
{
  DEBUG(TRACE, ENTER);

  const    typename boost::multi_index::index<MultiIndexContainer, Timeout>::type & idx = get<Timeout>(p_flow_cache);
  typedef  typename boost::multi_index::index<MultiIndexContainer, Timeout>::type::iterator itor_type;
  typename boost::multi_index::index<MultiIndexContainer, Timeout>::type::iterator idx_end(idx.end());
  typename boost::multi_index::index<MultiIndexContainer, Timeout>::type::iterator idx_itor;
  vector<itor_type> itors;

  // URL:  http://www.boost.org/doc/libs/1_34_1/libs/multi_index/doc/tutorial/basics.html#tagging

  DEBUG(LOTS, "LOWER BOUND:%s:", uitoa10(p_virtual_time).c_str());

  // DEVELOPER NOTES:
  //   The timeout time values used in the index in the flow cache already have
  //   their timeout values calculated before they are added to the cache. For
  //   example, if the timeout value is 120 and a new flow is added to the
  //   cache, the value for the timeout in the Timeout will be the time
  //   the flow was generated plus 120. That allows us to do simple
  //   calculations like less than and lower_bound.

  idx_itor = idx.lower_bound(p_virtual_time);

  DEBUG(LOTS, "VIRTUAL TIME:%s:", uitoa10(p_virtual_time).c_str());
  while (idx_end != idx_itor)
  {
    DEBUG(LOTS, "FLUSHBYTIMEOUT:%s:%s:", uitoa10(p_virtual_time).c_str(), (*idx_itor)->createFlowString().c_str());
    (*(idx_itor))->setFragType(false, p_shutdown);
    itors.push_back(idx_itor);

    // DEVELOPER NOTE:
    //  If we are processing the normal cache we always send the flow
    //  If we are processing the frag cache we only send the non-Top flows
    //  because the Top flows get copied to the normal cache so they we
    //  already have been sent. BUT, we still remove all fragmented Top
    //  and non-Top flows in the loop below

    if (p_is_normal_cache || (! (*(idx_itor))->isFirstIpFrag()))
    {
      getSummaryAddEvent()->call(*(idx_itor));
      incrSummaryCount();
    }
    ++idx_itor;
  }

  // DEVELOPER NOTE:
  //   The erase call has to take place outside the above while loop because it
  //   crashed on Windows because the idx_itor is no longer valid and can't be
  //   incremented
  for (int j = itors.size(); j;)
  {
    --j;
    this->flow_pool.release(*(itors.at(j)));
    p_flow_cache.template get<Timeout>().erase(itors.at(j));
  }

  DEBUG(TRACE, LEAVE);
  return;
}


template<typename Tag, typename MultiIndexContainer>
void FlowCache::printBy(MultiIndexContainer const & p_flow_cache, Tag *) throw()
{
  DEBUG(TRACE, ENTER);
  // DEVELOPER NOTE: obtain a reference to the index tagged by Tag
  const   typename boost::multi_index::index<MultiIndexContainer, Tag>::type & idx = get<Tag>(p_flow_cache);
  typedef typename MultiIndexContainer::value_type value_type;
    
  copy(idx.rbegin(), idx.rend(), ostream_iterator<value_type, char>(cout, "\n"));
  DEBUG(TRACE, LEAVE);
  return;
}


void FlowCache::printByNormFlowKey(void) throw()
{
  DEBUG(LOTS, ENTER);
  printBy<NormFlowKey>(getNormFlowCache());
  DEBUG(LOTS, LEAVE);
  return;
}


void FlowCache::onAddEvent(sharedFlow const p_flow) throw()
{
  DEBUG(LOTS, ENTER);
  add(p_flow);
  DEBUG(LOTS, LEAVE);
  return;
}


void FlowCache::reset(int unsigned const p_level) throw()
{
  DEBUG(TRACE, ENTER);
  setVirtualTime(getFuture());
  flush(getNormFlowCache(), true,  getFuture(), p_level);
  flush(getFragFlowCache(), false, getFuture());
  DEBUG(TRACE, LEAVE);
  return;
}


string FlowCache::onMetricsEvent(int unsigned const p_level) throw()
{
  string result  = "FC";
  result        += ":mff#" + uitoa10(getFragFlowCount());
  result        += ":mnf#" + uitoa10(getNormFlowCount());
  result        += ":mfc#" + uitoa10(getMergeCount());
  result        += ":sfc#" + uitoa10(getSummaryCount());
  output("      Max Frag Flows:     " + uitoa10(getFragFlowCount()));
  output("      Max Norm Flows:     " + uitoa10(getNormFlowCount()));
  output("      Merged Flow Count:  " + uitoa10(getMergeCount()));
  output("      Summary Flow Count: " + uitoa10(getSummaryCount()));
  resetFragFlowCount();
  resetMergeCount();
  resetNormFlowCount();
  resetSummaryCount();
  return(result);
}


void FlowCache::onNewInputEvent(int unsigned const p_level) throw()
{
  DEBUG(TRACE, ENTER);
  reset(p_level);
  setVirtualTime(getEpoch());
  DEBUG(TRACE, LEAVE);
  return;
}


void FlowCache::onShutdownSystemEvent(int unsigned const p_level) throw()
{
  DEBUG(TRACE, ENTER);
  reset(p_level);
  DEBUG(TRACE, LEAVE);
  return;
}
