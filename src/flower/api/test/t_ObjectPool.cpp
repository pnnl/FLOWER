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


// System Includes
// External Includes
#define BOOST_TEST_MODULE ObjectPool
// Internal Includes
// Application Includes
#include "utility.hpp"
#include "Flow.hpp"
#include "ObjectPool.hpp"

// Namespaces
using namespace boost::posix_time;


struct ObjectPoolSuiteFixture
{
  ObjectPool<Flow> * flow_pool;


  ObjectPoolSuiteFixture()
  {
    BOOST_TEST_MESSAGE("ObjectPoolSuite setup fixture");
    utility_init();
    //int unsigned pool_size = 50;
    //TODO: Flow flow              = generateFlow(generateTimestamp(1), 1);
    //TODO: flow_pool              = new ObjectPool<Flow>(pool_size, flow);
    return;
  }


  ~ObjectPoolSuiteFixture()
  {
    BOOST_TEST_MESSAGE("ObjectPoolSuite teardown fixture");
    //TODO: delete(flow_pool);
    return;
  }

};


BOOST_FIXTURE_TEST_SUITE(ObjectPoolSuite, ObjectPoolSuiteFixture)


BOOST_AUTO_TEST_CASE(flow_pool_size)
{
  // Pre-condition:  Construct a new flow

  // Condition:      Hold N flows

  // Post-condition: The flow has the data right data
  //TODO: BOOST_CHECK_EQUAL(flow_pool->size(), size);
  BOOST_CHECK_EQUAL(1, 1);

  return;
}


BOOST_AUTO_TEST_SUITE_END()

/*
#include "flower.hpp"
#include "Exception.hpp"
#include "Flow.hpp"
#include "Field.hpp"
#include "ObjectPool.hpp"

using namespace std;


int main(int argc, char ** argv)
{
  Diagnostic diag(argc, argv, Exception::getErrorCodes());
  DEBUG(TRACE, ENTER);


  Fields                       fields;
  map<field_idx, int unsigned> field_map;
  flow_key_t                   flow_key(5, 0);
  flow_key_t                   frag_flow_key(5, 0);
  vector<char unsigned>        ip_address(16, 0xF0);


  // Create the first default Flow
  //Flow::buildFlowKeys(flow_key, frag_flow_key, true, 0, 0, 0, 0, '\001', ip_address, ip_address);
  //initializeFields(fields, field_map, 123456789, 123456789, 0, 0, ip_address, ip_address);
  //Flow flow_default(false, flow_key, frag_flow_key, fields, field_map, 123456789, 120, 900, true, false, false, false);

  Flow::buildFlowKeys(flow_key, frag_flow_key);
  initializeFields(fields, true);
  Flow default_flow(::g_REAL, flow_key, frag_flow_key, fields, field_map);

  ObjectPool<Flow> flow_pool(10, default_flow);

  cout << "Size initial:      " << flow_pool.size() << endl;
  sharedFlow flow1 = flow_pool.acquire();

  cout << "UnModified Flow1:" << flow1 << endl;
  cout << "Size after acquire:" << flow_pool.size() << endl;

  assignFields(flow1->fields, flow1->field_map, 345678912, 345678912, 0, 0, ip_address, ip_address);
  cout << "Modified   Flow1:" << flow1 << endl;

  flow_pool.release(flow1);
  cout << "Size after release:" << flow_pool.size() << endl;

  sharedFlow flow2 = flow_pool.acquire();
  sharedFlow flow3 = flow_pool.acquire();

  cout << "UnModified Flow2:" << flow2 << endl;
  cout << "UnModified Flow3:" << flow3 << endl;
  cout << "Flow2 == Flow3:  " << (flow2 == flow3) << endl;
  cout << "Size after acquire:" << flow_pool.size() << endl;

  flow_pool.release(flow2);
  flow_pool.release(flow3);
  cout << "Size after release:" << flow_pool.size() << endl;
  cout << "Exiting" << endl;

  DEBUG(TRACE, LEAVE);
  return(0);
}
*/
