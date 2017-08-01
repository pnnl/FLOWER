///////////////////////////////////////////////////////////////////////////////
// Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
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
#include <exception>
#include <memory>
// External Includes
#define BOOST_TEST_MODULE Event
// Internal Includes
// Application Includes
#include "utility.hpp"
#include "Event.hpp"

using namespace std;

struct myexception : public std::exception
{
  virtual const char * what() const throw()
  {
    return("EXCEPTION_MESSAGE");
  }
};


struct Obj
{

  int unsigned number;
  string       name;

  Obj(int unsigned const p_number, string const & p_name) :
    number(p_number),
    name(p_name)
  {
    return;
  }

};


class TestClass
{

public:

  TestClass(void) throw() :
    counter(0),
    value(0)
  {
    return;
  }


  ~TestClass(void) throw()
  {
    return;
  }


  bool returnBoolNotification(int unsigned const p_value) throw()
  {
    this->counter++;
    this->value = p_value;
    return(true);
  }


  inline int unsigned getValue(void) const throw()
  {
    return(this->value);
  }


  void intNotification(int unsigned const p_value) throw()
  {
    this->counter++;
    this->value = p_value;
    return;
  }


  inline string getObjName(void) const throw()
  {
    return(this->obj->name);
  }


  inline int unsigned getObjNumber(void) const throw()
  {
    return(this->obj->number);
  }


  void objNotification(std::shared_ptr<Obj> const p_obj) throw()
  {
    this->counter++;
    this->obj = p_obj;
    return;
  }


  void throwNotification(int unsigned const p_value) throw(myexception)
  {
    this->counter++;
    this->value = p_value;
    throw(g_exception);
    return;
  }


private:

  int unsigned         counter;
  int unsigned         value;
  std::shared_ptr<Obj> obj;
  myexception          g_exception;

};


typedef Event< void, int unsigned >         TestIntEvent;
typedef Event< void, std::shared_ptr<Obj> > TestObjEvent;
typedef Event< bool, int unsigned >         TestBoolEvent;

struct EventSuiteFixture
{
  TestIntEvent  test_int_event;
  TestIntEvent  test_thr_event;
  TestObjEvent  test_obj_event;
  TestBoolEvent test_bool_event;
  TestClass     test_class1;
  TestClass     test_class2;
  TestClass     test_class3;
  TestClass     test_class4;
  TestClass     test_class5;
  myexception   g_exception;


  EventSuiteFixture() throw()
  {
    BOOST_TEST_MESSAGE("EventSuite setup fixture");
    return;
  }


  ~EventSuiteFixture() throw()
  {
    BOOST_TEST_MESSAGE("EventSuite teardown fixture");
    return;
  }

};


BOOST_FIXTURE_TEST_SUITE(EventSuite, EventSuiteFixture)


BOOST_AUTO_TEST_CASE(accept_registration_from_observers)
{
  // Pre-condition: NONE
  BOOST_CHECK_EQUAL(test_int_event.size(), ::g_zero);

  // Condition: Register observer
  int unsigned expected = 1;
  test_int_event += new TestIntEvent::New<TestClass>("test_class1", &test_class1, &TestClass::intNotification);

  // Post-condition: Verify the observer is registered
  BOOST_CHECK_EQUAL(test_int_event.size(), expected);

  return;
}


BOOST_AUTO_TEST_CASE(supply_observers_when_event_occurs)
{
  // Pre-condition: Register observers
  test_int_event  += new TestIntEvent::New<TestClass>("test_class1", &test_class1, &TestClass::intNotification);
  test_int_event  += new TestIntEvent::New<TestClass>("test_class2", &test_class2, &TestClass::intNotification);
  test_obj_event  += new TestObjEvent::New<TestClass>("test_class3", &test_class3, &TestClass::objNotification);

  // Condition: Trigger an event
  int unsigned    value = 12;
  string          name  = "anything";
  std::shared_ptr<Obj> obj(new Obj(value, name));

  test_int_event.call(value);
  test_obj_event.call(obj);

  // Post-condition: Verify both observers get the supplied value.
  BOOST_CHECK_EQUAL(test_class1.getValue(),     value);
  BOOST_CHECK_EQUAL(test_class2.getValue(),     value);
  BOOST_CHECK_EQUAL(test_class3.getObjNumber(), value);
  BOOST_CHECK_EQUAL(test_class3.getObjName(),   name);

  return;
}


BOOST_AUTO_TEST_CASE(supply_observers_when_event_occurs_and_returns)
{
  // Pre-condition: Register observers
  test_bool_event += new TestBoolEvent::New<TestClass>("test_class5", &test_class5, &TestClass::returnBoolNotification);

  // Condition: Trigger an event
  int unsigned    value = 12;

  BOOST_CHECK_EQUAL(test_bool_event.call(value), true);

  return;
}


// TODO: FIX: DEVELOPER NOTE: The following test worked with Boost 1_35-0
//                            but fails with 1_38-0 by doing a SIGABRT.
//                            THIS IS NOT USED IN OUR CODE ANYWHERE BUT
//                            does exist in the Event class so testing it
//                            was a "nice" thing to do.
//BOOST_AUTO_TEST_CASE(supply_observers_when_event_occurs_and_throw)
//{
//  // Pre-condition: Register observers
//  test_thr_event  += new TestIntEvent::New<TestClass>("test_class4", &test_class4, &TestClass::throwNotification);
//
//  // Condition: Trigger an event
//  int unsigned    value = 12;
//
//  cout << "DEVELOPER NOTE: There should be an EXCEPTION_MESSAGE ERROR message that follows" << endl;
//  try
//  {
//    BOOST_CHECK_THROW(test_thr_event.call(value), myexception);
//  }
//  catch (...)
//  {
//    // Do nothing
//  }
//
//  return;
//}


BOOST_AUTO_TEST_CASE(track_number_of_observers)
{
  // Pre-condition: NONE
  BOOST_CHECK_EQUAL(test_int_event.size(), ::g_zero);

  // Condition: Register observers
  int unsigned expected = 2;
  test_int_event += new TestIntEvent::New<TestClass>("test_class1", &test_class1, &TestClass::intNotification);
  test_int_event += new TestIntEvent::New<TestClass>("test_class2", &test_class2, &TestClass::intNotification);

  // Post-condition: Verify both delegates are registered
  BOOST_CHECK_EQUAL(test_int_event.size(), expected);

  return;
}


BOOST_AUTO_TEST_SUITE_END()
