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
#define BOOST_TEST_MODULE FlowCache
// Internal Includes
// Application Includes
#include "utility.hpp"
#include "FlowCache.hpp"


// Global Variables

int unsigned g_max_cache_size = 10;


struct FlowCacheSuiteFixture
{
  SummaryAddEvent    summary_add_event;
  FlowCache      *   flow_cache;


  FlowCacheSuiteFixture()
  {
    BOOST_TEST_MESSAGE("FlowCacheSuite setup fixture");
    // This block of code is called for every test case

    utility_init();
    setCacheTimeout(::g_timeout);

    sharedFlow flow = generateFlow(true, generateTimestamp(1), 1);

    ::g_flow_pool  = new ObjectPool<Flow>(::g_max_cache_size, *flow);
    flow_cache = new FlowCache(&summary_add_event, *::g_flow_pool, ::g_max_cache_size, ::g_max_cache_size);

    // Wire up the event to simulate Message passing to summary_collection
    summary_add_event += new SummaryAddEvent::Static("add_event", &testOnAddEvent);

    return;
  }


  ~FlowCacheSuiteFixture()
  {
    BOOST_TEST_MESSAGE("FlowCacheSuite teardown fixture");

    delete(flow_cache);
    delete(::g_flow_pool);
    ::g_flow_pool = NULL;

    ::g_add_event_called = false;

    return;
  }
};


void addFakeFlow(FlowCache * const p_flow_cache)
{
  FlowCache::norm_flow_cache_flowkey_itor itor;
  sharedFlow                              flow = generateFlow(true, generateTimestamp(getCacheTimeout() + ::g_time_step), 1, ::g_FAKE);

  p_flow_cache->onAddEvent(flow);
  BOOST_CHECK_EQUAL(p_flow_cache->findByNormFlowKey(flow, itor), false);
}


bool addFlow(FlowCache * const p_flow_cache, time_t const p_time, int unsigned const p_key, bool const p_check = true)
{
  // Create a new flow with a time, mac addresses, and an ether type
  FlowCache::norm_flow_cache_flowkey_itor itor;

  // Create a new message with the flow and add it to the flow_cache
  // through the onAvailEvent member function of FlowCache.
  sharedFlow flow = generateFlow(false, from_time_t(p_time), p_key);

  //DEBUG: Flow::getFlowKeyString(flow->getFlowKey());
  p_flow_cache->onAddEvent(flow);

  // Post-conditions: True for any test that calls this function
  //   The FlowAddEvent should have been called
  //   The flow_cache should have the flow we added
  bool result = p_flow_cache->findByNormFlowKey(flow, itor);
  BOOST_CHECK_EQUAL(result, p_check);

  return(result);
}


void addFlows(FlowCache * const p_flow_cache, int unsigned const p_nbr_flows)
{
  // Just a short cut routine to add multiple flows
  for (int unsigned idx = 1; idx <= p_nbr_flows; ++idx)
  {
    addFlow(p_flow_cache, generateTimestamp(), idx + 10);
  }

  return;
}


void flushFlows(FlowCache * const p_flow_cache, int unsigned const p_time_limit)
{
  int unsigned id        = 1;
  int unsigned nbr_flows = 1;
  time_t ts1             = generateTimestamp(1);
  time_t ts2             = generateTimestamp(p_time_limit + 5);

  // Pre-condition: Add a single flow
  addFlow(p_flow_cache, ts1, id);

  BOOST_CHECK_EQUAL(wasEventCalled(::g_add_event_called), false);

  // Condition:: Add another unique flow causing the first flow to
  //             timeout and be flushed
  addFlow(p_flow_cache, ts2, id+1);


  // Post-condition
  BOOST_CHECK_EQUAL(wasEventCalled(::g_add_event_called), true);
  BOOST_CHECK_EQUAL(p_flow_cache->size(), (int unsigned) nbr_flows);

  return;
}


//  Start Test Cases

BOOST_FIXTURE_TEST_SUITE(FlowCacheSuite, FlowCacheSuiteFixture)


BOOST_AUTO_TEST_CASE(add_unique_incoming_flows)
{
  BOOST_TEST_MESSAGE("add_unique_incoming_flows");
  // Pre-condition:
  //   The flow_cache is empty
  // Conditions:
  //   Add a single flow
  //   Add another single flow
  //   Add many unique flows
  // Post-conditions:
  //   The flow_cache should have only N flow objects (indicated by size())

  FlowCache::norm_flow_cache_flowkey_itor itor;
  int unsigned id        = 0;
  int unsigned nbr_flows = 0;

  // Pre-condition
  BOOST_CHECK_EQUAL(::g_flow_pool->size(), ::g_max_cache_size + 1);
  BOOST_CHECK_EQUAL(flow_cache->size(), ::g_zero);

  // Condition: Add a single flow
  id        = 1;
  nbr_flows = 1;
  addFlow(flow_cache, generateTimestamp(), id);
  BOOST_CHECK_EQUAL(::g_flow_pool->size(), ::g_max_cache_size - 0);

  // Post-condition
  BOOST_CHECK_EQUAL(flow_cache->size(), nbr_flows);

  // Condition: Add another single flow
  id        = 2;
  nbr_flows = 2;
  addFlow(flow_cache, generateTimestamp(), id);
  BOOST_CHECK_EQUAL(::g_flow_pool->size(), ::g_max_cache_size - 1);

  // Post-condition
  BOOST_CHECK_EQUAL(flow_cache->size(), nbr_flows);

  // Condition: Add N flows
  nbr_flows = ::g_max_cache_size - 2;
  addFlows(flow_cache, nbr_flows);
  BOOST_CHECK_EQUAL(::g_flow_pool->size(), 1);

  // Post-condition
  BOOST_CHECK_EQUAL(flow_cache->size(), ::g_max_cache_size);

  return;
}


BOOST_AUTO_TEST_CASE(merge_existing_incoming_flow)
{
  BOOST_TEST_MESSAGE("merge_existing_incoming_flow");
  // Pre-condition:
  //   The flow_cache has 1 flow in it
  // Conditions:
  //   Add the same flow
  // Post-conditions:
  //   The flow_cache should have only 1 flow object (indicated by size())

  int unsigned id        = 1;
  int unsigned nbr_flows = 1;

  // Pre-condition: Add a single flow
  addFlow(flow_cache, generateTimestamp(), id);

  // Condition: Add the same flow
  addFlow(flow_cache, generateTimestamp(), id);

  // Post-condition
  BOOST_CHECK_EQUAL(flow_cache->size(), (int unsigned) nbr_flows);

  return;
}


BOOST_AUTO_TEST_CASE(flush_on_shutdown)
{
  BOOST_TEST_MESSAGE("flush_on_shutdown");
  // Pre-condition:
  //   The flow_cache has 1 flow in it
  // Conditions:
  //   Add another single flow
  // Post-conditions:
  //   The flow_cache should have only 1 flow object (indicated by size())

  int unsigned id        = 1;
  int unsigned nbr_flows = 1;

  // Pre-condition: Add a single flow
  addFlow(flow_cache, generateTimestamp(), id);
  BOOST_CHECK_EQUAL(flow_cache->size(), (int unsigned) nbr_flows);

  // Condition
  flow_cache->onShutdownSystemEvent(1);

  // Post-condition
  BOOST_CHECK_EQUAL(wasEventCalled(::g_add_event_called), true);
  BOOST_CHECK_EQUAL(flow_cache->size(), (int unsigned) 0);

  return;
}


BOOST_AUTO_TEST_CASE(flush_inactive_flows)
{
  BOOST_TEST_MESSAGE("flush_inactive_flows");
  // Pre-condition:
  //   The flow_cache has 1 flow in it
  // Conditions:
  //   Add another unique flow that has a time that is cause the flow in
  //   the cache to appear old enough to be idle/inactive and timeout
  // Post-conditions:
  //   The flow_cache should have only 1 flow object (indicated by size())

  flushFlows(flow_cache, ::g_timeout);

  return;
}


BOOST_AUTO_TEST_CASE(flush_expired_flows)
{
  BOOST_TEST_MESSAGE("flush_expired_flows");
  // Pre-condition:
  //   The flow_cache has 1 flow in it
  // Conditions:
  //   Add another unique flow that has a time that is cause the flow in
  //   the cache to appear to have exceeded its time limit and be expired
  // Post-conditions:
  //   The flow_cache should have only 1 flow object (indicated by size())

  flushFlows(flow_cache, getCacheForceout());

  return;
}


BOOST_AUTO_TEST_CASE(flush_on_fake_flow)
{
  BOOST_TEST_MESSAGE("flush_on_fake_flow");
  // Pre-condition:
  //   The flow_cache has 5 inactive flows in it
  // Conditions:
  //   Add a fake flow
  // Post-conditions:
  //   The flow_cache should have 0 flow object (indicated by size())

  int unsigned nbr_flows = 5;

  // Pre-condition: Add 5 inactive flows
  addFlows(flow_cache, nbr_flows);
  BOOST_CHECK_EQUAL(flow_cache->size(), nbr_flows);

  // Condition: Add a fake flow
  cout << "DEVELOPER NOTE: There should be an CAUTION message that follows" << endl;
  addFakeFlow(flow_cache);

  // Post-condition: The flow_cache is empty
  BOOST_CHECK_EQUAL(flow_cache->size(), ::g_zero);

  return;
}


BOOST_AUTO_TEST_CASE(only_handle_up_to_a_specified_number_of_flows)
{
  BOOST_TEST_MESSAGE("only_handle_up_to_a_specified_number_of_flows");
  // Pre-condition: Add MAX flows
  addFlows(flow_cache, ::g_max_cache_size);
  BOOST_CHECK_EQUAL(flow_cache->size(), ::g_max_cache_size);

  // Condition: Try to add one more flow
  cout << "DEVELOPER NOTE: There should be an ERROR message that follows" << endl;
  sharedFlow flow = generateFlow(false, from_time_t(generateTimestamp()), 50000);
flow->getFlowKey().createFlowKeyString();
  flow_cache->onAddEvent(flow);

  // Post-conditions: True for any test that calls this function
  //   The FlowAddEvent should have been called
  //   The flow_cache should have the flow we added
  FlowCache::norm_flow_cache_flowkey_itor itor;
  bool result = flow_cache->findByNormFlowKey(flow, itor);
  BOOST_CHECK_EQUAL(result, false);

  // Post-condition: Should still be MAX flows
  BOOST_CHECK_EQUAL(flow_cache->size(), ::g_max_cache_size);

  return;
}


BOOST_AUTO_TEST_SUITE_END()
